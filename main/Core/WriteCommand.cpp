/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

static const char* Copyright = "(C) Copyright Michigan State University 2008, All rights reserved";

 
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

#include <config.h>
#include "WriteCommand.h"    				
#include <TCLResult.h>
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
#include <iostream>
#include <fstream>
#include <tclstreams.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif



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

  const char* pFormat = "nsclascii";	// Default format.
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

  // Before we go and create a new file, we want to respond to 
  // Bug #111 by ensuring there's at lease one valid spectrum in 
  // the list we've been handed.

  char** pSpectra = pArgs;
  int    nSpectra = nArgs;
  pSpectra++;                     // Don't check the file itself.
  nSpectra--;

  int nValid = CountValidSpectra(pSpectra, nSpectra);
  
  if(nValid <= 0) {		// No valid spectra!!
    rResult = "None of the spectra in this swrite command exist\n";
    Usage(rResult);
    return TCL_ERROR;
  }

  //  Then next parameter can be one of two things, either a 
  //  Channel name or a filename.
  //  In the end we want to have an ostream derived object 
  //  to pass to the formatter.  The code below decides whether or not
  //  to create a  tclostream or a ofstream.   Regardless,
  //  by the time we pass out of this block of code, pOut is 
  //  pointing to that stream.
  //  Playing this kind of game removes the need to do all the fd caching
  //  we used to do in the 2.x and earlier versions of SpectTcl.
  //  The cost of this is that the output file permissions, when the
  //  file parameter is a true filename are determined by the run time library
  //  and umask rather than us.... this may cause users's some problems.
  //  but for now tough.

  ostream*     pOut;
  Tcl_Channel  pChannel(rInterp.GetChannel(pArgs[0]));
  
  if(pChannel) {
    pOut = new tclostream(pChannel);
  }
  else {
    pOut = new ofstream(pArgs[0], ios::trunc | ios::out);
    rResult="";
  }
  // If the file could not be created/mapped then pOut is false:
  // kill it off, and return the error:

  if(!(*pOut)) {
    rResult = "Unable to create or open a stream on ";
    rResult += pArgs[0];
    delete pOut;
    return TCL_ERROR;
  }

  // Now we can write out the spectra:

  nArgs--;			
  pArgs++;
  UInt_t nFailed = 0;
  vector<string> Failures;
  vector<string> FailedNames;
  CSpectrumPackage& rPack((CSpectrumPackage&)getMyPackage());
  while(nArgs) {
    string thisResult;
    if(rPack.Write(thisResult, string(*pArgs), *pOut, pFormatter) != TCL_OK) {
      nFailed++;
      Failures.push_back(thisResult);
      FailedNames.push_back(*pArgs);
    }
    nArgs--;
    pArgs++;
  }
  //  Kill off the stream:  If this is a tclostream, that won't close the
  //  channel.

  delete pOut;

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
  rResult = "BUG Report that: Control fell through to the end of CWriteCommand::operator()";
  return TCL_ERROR;
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
/*!
   Return the number of spectra in a list that actually exist.
   \param pSpectra (char** [in]) list of spectra to check for.
   \param nSpectra (int [in]):   Number of spectra in the list

   \retval int - number of spectra that exist.
*/
int
CWriteCommand::CountValidSpectra(char** pSpectra,
				 int    nSpectra) 
{
  // Locate the histogrammer:

  CSpectrumPackage& rPack((CSpectrumPackage&)getMyPackage());
  CHistogrammer*   pHist = (CHistogrammer*)rPack.getHistogrammer();

  // Figure out the set of valid spectra.

  int result(0);
  while (nSpectra) {
    if(pHist->FindSpectrum(string(*pSpectra))) {
      result++;
    }
    nSpectra--;
    pSpectra++;
  }
  
  
  return result;
}
