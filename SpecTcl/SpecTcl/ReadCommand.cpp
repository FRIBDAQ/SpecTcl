// Class: CReadCommand
// Implements the sread command.  sread reads spectra 
// from a file.
//   CSpectrumFormatters and
//   the CSpectrumFormatterFactory
//  allows the format of the file to be
//  easily extended by the user.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
//
// Modified:
//    Chase Bolen (added stream map functions CheckIn, CheckOut, etc.) 4/14/2000 
//////////////////////////.cpp file/////////////////////////////////////////////////////

#include "ReadCommand.h"    				
#include "WriteCommand.h"

#include <histotypes.h>
#include <SpectrumPackage.h>
#include <SpectrumFormatter.h>
#include <SpectrumFormatterFactory.h>
#include <TCLInterpreter.h>
#include <TCLString.h>
#include <TCLResult.h>
#include <iostream.h>
#include <fstream.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <string>
#include <ErrnoException.h>

static char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved ReadCommand.cpp \n";

// Static data.

struct SwitchEntry {
  const char*            pSwitchText;
  CReadCommand::Switch_t Switch;
};

struct StreamEntry {
  fstream *pstreamloc;
  off_t pos;
  dev_t device;
  ino_t inode;
};
static map<int,CReadCommand::StreamEntry> StreamMap;
static
SwitchEntry Switches[] = {
  {"-format",     CReadCommand::keFormat},
  {"-snapshot",   CReadCommand::keSnapshot},
  {"-nosnapshot", CReadCommand::keNoSnapshot},
  {"-replace",    CReadCommand::keReplace},
  {"-noreplace",  CReadCommand::keNoReplace},
  {"-bind",       CReadCommand::keBind},
  {"-nobind",     CReadCommand::keNoBind}
};

static UInt_t nTableSize = sizeof(Switches)/sizeof(SwitchEntry);

// Functions for class CReadCommand

//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
//                int nArgs, char* pArgs[])
//  Operation Type: 
//     Evaluator.
Int_t CReadCommand::operator()(CTCLInterpreter& rInterp, 
			       CTCLResult& rResult, int nArgs, char* pArgs[])  
{
  // Called to process the sread command. sread reads the next single
  // spectrum from a file.  If you have a file with multiple spectra,
  // you must open it with Tcl's open command and sread until eof.
  //
  // This member function parses the switches:
  //     -format {ascii}     - Format of data file.
  //     -snapshot           - Read spectrum into
  //                                 snapshot.
  //     -replace              - Ovewrite existing
  //                                  spectrum.
  //     -bind                 - bind spectrum to displayer.
  //
  //      file                     - A file returned from
  //                                 Tcl/TK's open command.
  //                                 or the name of a disk file.
  // Formal Parameters:
  //      CTCLInterpreter& rInterp:
  //          Interpreter running the command.
  //      CTCLResult&       rResult:
  //          Result string of the operation which is:
  //            Success: Name of spectrum created.
  //            Failure: Reason for error.
  //        int        nArgs:
  //          Number of parameters in the command.      
  //        char* pArgs[]:
  //           Command parameters.        
  // Returns:
  //   TCL_OK on success.
  //   TCL_ERROR on failure.
  //

  string format("nsclascii");	// Default value for format.
  string file;			// no default file.
  Bool_t fSnapshot=kfTRUE;	// Default is a snapshot spectrum.
  Bool_t fReplace =kfFALSE;	// Which does not replace existing specs.
  Bool_t fBind    =kfTRUE;      // Which is bound to the displayer.

  nArgs--; pArgs++;		// Skip over the command name.
  if(nArgs < 1) {		// Must be at least a file specifier.
    Usage(rResult);
    return TCL_ERROR;
  }
  CReadCommand::Switch_t Switch;
  // Next the switches get parsed:
  //
  while(nArgs && (Switch = NextSwitch(*pArgs)) != CReadCommand::keNotSwitch) {
    switch (Switch) {
    case keFormat:
      {
	nArgs--; pArgs++;
	if(nArgs < 2) {		// Need a format specifier as well as a file..
	  Usage(rResult);
	  return TCL_ERROR;
	}
	format = string(*pArgs);	// Fetch format specifier string.
      }
      break;
    case keSnapshot:
      fSnapshot = kfTRUE;
      break;
    case keNoSnapshot:
      fSnapshot = kfFALSE;
      break;
    case keReplace:
      fReplace = kfTRUE;
      break;
    case keNoReplace:
      fReplace = kfFALSE;
      break;
    case keBind:
      fBind = kfTRUE;
      break;
    case keNoBind:
      fBind = kfFALSE;
    default:
      Usage(rResult);
      return TCL_ERROR;
    }
    nArgs--; pArgs++;
  }
  // We need to:
  // 1. Translate the format specifier into a CSpectrumFormatter*
  // 2. get the file specifier and translate it into an ifstream.
  //

  file = string(*pArgs);
  CSpectrumFormatter* pFormatter = CWriteCommand::GetFormatter(format.c_str());
  if(pFormatter == (CSpectrumFormatter*)kpNULL) {
    Usage(rResult);
    return TCL_ERROR;
  }
  // The file could be either a file id or a file name.

  Bool_t MustClose = kfFALSE;	// Assume it's a TCL file id.
  Int_t fid;

  if(CWriteCommand::IsFileId(file.c_str())) { // Tcl file id...
    fid = CWriteCommand::GetFileId(file.c_str());
    
  }
  else {
    fid = open(file.c_str(), O_RDONLY );
    if(fid < 0) {
      rResult += strerror(errno);
      return TCL_ERROR;
    }
    MustClose = kfTRUE;
  }
  // if it is new dont do anything yet.
  // if its already in use check to see if its a differant spectrum.
  // if its the same spectrum continue with the cache of the old one.
  // if its a differant spectrum delete the old one and remove from map.
  // Now we can put it all together to call the package Read member.
  ifstream *in;
  if (InMap(fid)) {
    in = (ifstream*)CheckOut(fid);
  } else {
    in = new ifstream;
    if (in == NULL) {
      errno = ENOMEM;
      throw(CErrnoException("Creating input file stream."));
    }
    in->attach(fid);		// Attach to the requested fd.

  }
  CSpectrumPackage& rPack((CSpectrumPackage&)getMyPackage());
  UInt_t ReadFlags = 0;
  if(!fSnapshot) ReadFlags |= CSpectrumPackage::fLive;
  if(fReplace)   ReadFlags |= CSpectrumPackage::fReplace;
  if(fBind)      ReadFlags |= CSpectrumPackage::fBind;

  string Result;
  Int_t  status = rPack.Read(Result, *in,pFormatter, ReadFlags);
  if(MustClose) {
    close(fid);
    delete in;
  } else if(!(in->eof())) {
    // if the stream isnt at end of file put it in the map.
    bool end = in->eof();
    CheckIn((fstream*)in,fid);
  } else {
    delete in;
  }
  rResult = Result;
  return status;
}
////////////////////////////////////////////////////////////////////////////////
//
// Function:
//   void Usage(CTCLResult& rResult)
// Operation Type:
//    Static utility
//
void
CReadCommand::Usage(CTCLResult& rResult)
{
  //  Appends the command's usage string to rResult.
  //  Note that the usage consists of a fixed part which describes the general
  //  command usage, and a variable part describing the formatters availble
  //  by iterating the formatter list.
  //
  rResult += "Usage: \n";
  rResult += "   sread [-format fmtname] [-[no]snapshot] \\ \n";
  rResult += "         [-[no]replace]    [-[no]bind] file    \n\n";
  rResult += " file is either a file descriptor from the tcl open command\n";
  rResult += " or the path to a file containing the spectrum.\n\n";
  rResult += " fmtname is a formatter which can be any of: \n";

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
//////////////////////////////////////////////////////////////////////////////
//
// Function:
//   Switch_t NextSwitch(const char* pText)
// Operation type:
//   Static utility.
// 
CReadCommand::Switch_t
CReadCommand::NextSwitch(const char* pText)
{
  SwitchEntry* pTable = Switches;
  for(UInt_t i = 0; i < nTableSize; i++,pTable++) {
    if(strcmp(pTable->pSwitchText, pText) == 0) {
      return pTable->Switch;
    }
  }
  return keNotSwitch;
}

//CReadCommand::GetEntry(StreamEntry& temp, int fid)
//
//    fills in the StreamEntry struct temp with information
//    from the fstat command

void CReadCommand::GetEntry(StreamEntry& temp, int fid) {
  struct stat buffer;
  fstat(fid,&buffer);
  temp.pstreamloc = NULL;
  temp.pos = lseek(fid,0,SEEK_CUR);
  temp.device = buffer.st_dev;
  temp.inode = buffer.st_ino;
}
//CReadCommand::CheckOut(int fid)
//
//     called with the file id of a file in the stream map
//     CheckOut will return a pointer to a stream.  the streams 
//     are uniquely identified with the file position, the device, 
//     and the inode.  if the files are the same file then the stored
//     stream pointer is returned.  if they're differant files, the
//     stored stream is deleted, and the function returns a newed 
//     stream pointer.  in both cases the entry is removed from the map.

fstream *CReadCommand::CheckOut(int fid) {
  StreamEntry newfile, cachedfile;
  GetEntry(newfile,fid);
  cachedfile = StreamMap[fid];
  if ((newfile.pos == cachedfile.pos) &&
      (newfile.device == cachedfile.device) &&
      (newfile.inode == cachedfile.inode)) {
    // cached file is the same file
    newfile.pstreamloc = cachedfile.pstreamloc;
  } else {
    //cached file is differant but with same fid
    delete cachedfile.pstreamloc;
    newfile.pstreamloc = new fstream(fid);
    if (newfile.pstreamloc == NULL) {
      errno = ENOMEM;
      throw(CErrnoException("Creating input file stream."));
    }
  }
  StreamMap.erase(fid);
  return newfile.pstreamloc;
}

// CReadCommand::InMap(int fid)
//
//     returns a boolean value signifying
//     whether the file with id fid is in the map
//

bool CReadCommand::InMap(int fid) {
  bool res = false;
  if (StreamMap.find(fid) != StreamMap.end()) res = true;
  return res;
					  
}

// CReadCommand::CheckIn(fstream *in, int fid)
//
//     inserts the StreamEntry, made from the pointer
//     to the stream, the file position, the device, and the
//     inode, into the map keyed by the fid.

void CReadCommand::CheckIn(fstream *in, int fid) {
  StreamEntry newfile;
  GetEntry(newfile,fid);
  newfile.pstreamloc = in;
  StreamMap[fid]=newfile;
}
