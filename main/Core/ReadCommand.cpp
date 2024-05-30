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

#include <config.h>
#include "ReadCommand.h"    				
#include "WriteCommand.h"

#include <histotypes.h>
#include <SpectrumPackage.h>
#include <SpectrumFormatter.h>
#include <SpectrumFormatterFactory.h>
#include <TCLInterpreter.h>
#include <TCLString.h>
#include <TCLResult.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <string>
#include <ErrnoException.h>
#include <tclstreams.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


// Static data.

struct SwitchEntry {
  const char*            pSwitchText;
  CReadCommand::Switch_t Switch;
};


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

static TCLPLUS::UInt_t nTableSize = sizeof(Switches)/sizeof(SwitchEntry);

// Functions for class CReadCommand

//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     operator()(CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv)
//  Operation Type: 
//     Evaluator.
TCLPLUS::Int_t CReadCommand::operator()(CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv)
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
  //      std::vector<CTCLObject>& objv
  //          object encapsulated commnand words.
  // Returns:
  //   TCL_OK on success.
  //   TCL_ERROR on failure.
  //

  // Convert objj -> nArgs, pArgs for compatibility:

  std::vector<std::string> words;
  std::vector<const char*> pWords;
  for (auto& word: objv) {
    words.push_back(std::string(word));
  }
  for (auto& word : words) {
    pWords.push_back(word.c_str());
  }
  int nArgs = words.size();
  auto pArgs = pWords.data();
  std::string rResult;     // Make port syntacitcally simpler

  string format("nsclascii");	// Default value for format.
  string file;			// no default file.
  TCLPLUS::Bool_t fSnapshot=TCLPLUS::kfTRUE;	// Default is a snapshot spectrum.
  TCLPLUS::Bool_t fReplace =TCLPLUS::kfFALSE;	// Which does not replace existing specs.
  TCLPLUS::Bool_t fBind    =TCLPLUS::kfTRUE;      // Which is bound to the displayer.

  nArgs--; pArgs++;		// Skip over the command name.
  if(nArgs < 1) {		// Must be at least a file specifier.
    Usage(rInterp);
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
	  Usage(rInterp);
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
      break;
    default:
      Usage(rInterp);
      return TCL_ERROR;
    }
    nArgs--; pArgs++;
  }
  // We need to:
  // 1. Translate the format specifier into a CSpectrumFormatter*
  // 2. get the file specifier and translate it into an istream derived
  //    object.
  //

  file = string(*pArgs);
  CSpectrumFormatter* pFormatter = CWriteCommand::GetFormatter(format.c_str());
  if(pFormatter == (CSpectrumFormatter*)kpNULL) {
    Usage(rInterp);
    return TCL_ERROR;
  }

  // The file parameter can either be a file name or a TCL channel name.
  // we give priority to the channel name.  The block of code 
  // below will result in pIn being filled in with a pointer to
  // either a tclistream or an ifstream depending on which is which.
  // This change was forced a bit by  the port to g++ 3.x where fid's can no longer
  // construct fstreams. In any event, this is better because:
  //   - We're no longer tied to the internals assumption that the tcl channel
  //     name is of the form file%d  where the number is the fid.
  //   - Since we're well synchronized with the Tcl_Channel, and since
  //     tcl[io]stream  does not close the underlying channel on destruction,
  //     we no longer have to play the games where we cache the file ids.
  //
  // 
  istream*     pIn;
  Tcl_Channel  pChannel(rInterp.GetChannel(pArgs[0]));

  if(pChannel) {
    pIn = new tclistream(pChannel);
  }
  else {
    pIn = new ifstream(pArgs[0]);
  }
  // If the open failed, *pIn is false:

  
  if(!(*pIn)) {
    rResult = "Unable to open a stream on ";
    rResult = pArgs[0];
    rInterp.setResult(rResult);
    delete pIn;
    return TCL_ERROR;
  }

  // Now we can read the spectrum from file/channel.

  CSpectrumPackage& rPack(*(CSpectrumPackage*)getPackage());
  TCLPLUS::UInt_t ReadFlags = 0;
  if(!fSnapshot) ReadFlags |= CSpectrumPackage::fLive;
  if(fReplace)   ReadFlags |= CSpectrumPackage::fReplace;
  if(fBind)      ReadFlags |= CSpectrumPackage::fBind;

  string Result;
  TCLPLUS::Int_t  status = rPack.Read(Result, *pIn, pFormatter, ReadFlags);

  rInterp.setResult(Result);
  return status;
}
////////////////////////////////////////////////////////////////////////////////
//
// Function:
//   void Usage(CTCLInterpreter& rInterp)
// Operation Type:
//    Static utility
//
void
CReadCommand::Usage(CTCLInterpreter& rInterp)
{
  //  Appends the command's usage string to rResult.
  //  Note that the usage consists of a fixed part which describes the general
  //  command usage, and a variable part describing the formatters availble
  //  by iterating the formatter list.
  //

  std::string rResult = rInterp.GetResultString();
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
  rInterp.setResult(rResult);
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
  for(TCLPLUS::UInt_t i = 0; i < nTableSize; i++,pTable++) {
    if(strcmp(pTable->pSwitchText, pText) == 0) {
      return pTable->Switch;
    }
  }
  return keNotSwitch;
}
