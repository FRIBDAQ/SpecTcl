
 
// Class: CWriteCommand
// Implements the swrite command.
//  swrite writes a spectrum to a file.
//     CSpectrumFormatters and the
//     CSpectrumFormatterFactory allow the
//     format of spectra written to be extended
//     easily by the user.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
//
//////////////////////////.cpp file/////////////////////////////////////////////////////
#include "WriteCommand.h"    				

#include <SpectrumPackage.h>
#include <SpectrumFormatter.h>
#include <SpectrumFormatterFactory.h>
#include <ErrnoException.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <stdio.h>
#include <unistd.h>
#include <iostream.h>
#include <fstream.h>


static char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved WriteCommand.cpp \n";

// Functions for class CWriteCommand

//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
//                int nArgs, char* pArgs[])
//  Operation Type: 
//     evaluator.
int CWriteCommand::operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			      int nArgs, char* pArgs[])  
{
  // Implements the swrite command.
  // swrite writes spectra to a file.
  // Syntax is:
  //     swrite -format fmtname   file
  //        where:
  //              -format fmtname  - selects the output format.
  //                                           type.
  //              file                      - is either something returned
  //                                          from the tcl open command or
  //                                          the path of the file to write to.
  // Formal Paramters:
  //   CTCLInterpreter&   rInterp:
  //        Interpreter running the command.
  //   CTCLResult&         rResult:
  //        Result string being created.
  //    int nArgs:
  //         Number of command parameters.
  //    char* pArgs[] :
  //         Command arguments.
  //
  nArgs--;
  pArgs++;
  if(nArgs < 2) {		// Need at least a file and 1 spectrum.
    Usage(rResult);
    return TCL_ERROR;
  }
  // The next parameter is either a file descriptor or a 
  // -format selector pair.
  // The appropriate format is selected.

  char* pFormat = "nsclascii";	// Default format.
  if(strcmp(pArgs[0], "-format") == 0) {	// Format override.
    pFormat = pArgs[1];
    nArgs -= 2;
    pArgs += 2;
  }
  // There must still be 2 parameters at least (fd and spectrum).

  if(nArgs < 2) {
    Usage(rResult);
    return TCL_ERROR;
  }
  CSpectrumFormatter* pFormatter = GetFormatter(pFormat);
  if(!pFormatter) {
    Usage(rResult);
    return TCL_ERROR;
  }
  //  Then next parameter can be one of two things, either a 
  //  Tcl File descriptor or a file name.  
  //  The next section of code determines what it is we are trying
  //  write to and returns a file id to it if possible, or errors out if not.
  //

  Int_t  fid;
  Bool_t MustClose = kfFALSE;
  if(IsFileId(pArgs[0])) {
    fid = GetFileId(pArgs[0]);
  }
  else {
    fid = open(pArgs[0], O_WRONLY |O_CREAT, S_IRWXU | S_IRGRP | S_IROTH);
    if(fid < 0) {
      rResult += strerror(errno);
      return TCL_ERROR;
    }
    MustClose = kfTRUE;
  }
  //
  //   Now we can associate a stream with the file id and
  //   ask that the spectra be written out:
  //
  FILE* junk;
  ofstream out(fid);

  nArgs--;
  pArgs++;
  UInt_t nFailed = 0;
  vector<string> Failures;
  vector<string> FailedNames;
  CSpectrumPackage& rPack((CSpectrumPackage&)getMyPackage());
  while(nArgs) {
    string thisResult;
    if(rPack.Write(thisResult, string(*pArgs), out, pFormatter) != TCL_OK) {
      nFailed++;
      Failures.push_back(thisResult);
      FailedNames.push_back(*pArgs);
    }
    nArgs--;
    pArgs++;
  }

  //
  // If we opened the file we should close it.
  //

  if(MustClose) {
    out.flush();
    close(fid);
  }
  // Now based on nFailed, figure out the result and the return value:

  if(nFailed) {
    for(UInt_t i = 0; i < nFailed; i++ ) {
      CTCLString subresult;
      subresult.StartSublist();
      subresult.AppendElement(FailedNames[i]);
      subresult.AppendElement(Failures[i]);
      subresult.EndSublist();
      rResult += (const char *)subresult;			 
    }
    return TCL_ERROR;
  }
  else {
    return TCL_OK;
  }
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//    Bool_t               IsFileId(const char* pConnectionString);
// Operation type:
//    Utility.
//  
Bool_t
CWriteCommand::IsFileId(const char* pConnectionString)
{
  //  Uses GetFileId to determine if a connection string is a valid 
  //  Tcl/TK file id
  // Formal Parameters:
  //     const char* pConnection String:
  //        Points to the connection string (e.g. file9 or /usr/local/test
  // Returns:
  //    kfTRUE - pConnection string represents a file id connecion.
  //    kfFALSE- pConnection does not represent a file id connection.
  //
  
  try {
    Int_t nId = GetFileId(pConnectionString);
    return kfTRUE;		// No throw indicates valid file id.
  }
  catch(...) {
    return kfFALSE;		// throws mean that not a file id.
  }

}
//////////////////////////////////////////////////////////////////////////
//
//   Function:
//     Int_t                GetFileId(const char* pConnectionString)
//   Operation Type:
//     Utility:
//
Int_t
CWriteCommand::GetFileId(const char* pConnectionString)
{
  // Takes a connection string which represents a Tcl file id and
  // either returns the fid corresponding to it or throws an CErrnoException:
  //  Determines if a file name string represents a Tcl internal
  //  file variable.  If it does, it will be of the form:  file%d  where %d is
  //  the file id of the file it represents, and %d will be a valid fd.
  //  Therefore our strategy is to first parse the string for validity, and
  //  if valid, to stat the id to see if it's an open fid.
  //
  // Formal Parameters:
  //     const char* pConnection String:
  //        Points to the connection string (e.g. file9 or /usr/local/test
  // Returns:
  //    value of the file id or an errno exception otherwise.
  //
  
  Int_t  nId;
  UInt_t Status = sscanf(pConnectionString, "file%d", &nId);
  if(Status == 0) {
    errno = ENOENT;
    throw 
      CErrnoException("Parsing connection string in CWriteCommand::GetFileId");
  }

  // nId could be a valid/open file id.. If so, then fstat should work.

  struct stat statbuf;
  if(fstat(nId, &statbuf) == -1) {
    errno = EBADF;
    throw
      CErrnoException("Invoking stat() on fid in CWriteCommand::GetFileId");
  }

  return nId;

}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//   CSpectrumFormatter*  GetFormatter(const char* pFormatter)
// Operation Type:
//   Utility.
//
CSpectrumFormatter*
CWriteCommand::GetFormatter(const char* pFormatter)
{
  //  Returns the formatter associated with the formatter ident string,
  //  or returns a null pointer if there is no associated formatter.
  // 
  CSpectrumFormatterFactory Factory;// Create the lookup factory..
  return Factory.MatchFormatter(string(pFormatter));

}
/////////////////////////////////////////////////////////////////////////
//
// Function:
//   void                 Usage(CTCLResult& rResult)
// Operation Type:
//   protected utility.
//
void
CWriteCommand::Usage(CTCLResult& rResult)
{
  rResult += "Usage:\n";
  rResult += "  swrite -format fmtname file spectrum1 [spectrum2 ... ]\n";
  rResult += "Where:\n";
  rResult += "   fmtname selects a format, \n";
  rResult += "   file    is either a file returned from Tcl's open or a \n";
  rResult += "           path to a Unix file to which spectra will be written\n";
  rResult += "   spectrumn are names of spectra to be written.\n";
  rResult += "Supported Formats are:\n";
  
  //  Add a list of formatters and their descriptions to the result:

  CSpectrumFormatterFactory Fact;
  FormatterIterator         i    = Fact.FormatterBegin();
  while(i != Fact.FormatterEnd()) {
    CSpectrumFormatter* pFmt = (*i).second;
    rResult += "   "; rResult += (*i).first; 
    rResult += "\t" ; rResult += pFmt->getDescription();
    rResult += "\n";

    i++;
  }
}
