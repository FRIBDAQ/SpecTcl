//  CTapeCommand.cpp
// Implements Tcl commands which operate on tape data sources
// the syntax of the tape command is:
//
//     tape  -open  filename
//     tape  -open  -next
//     tape  -close
//     Tape -rewind

//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//


#include "TapeCommand.h"                               
#include "TCLInterpreter.h"
#include "TCLCommandPackage.h"
#include "TCLResult.h"
#include "DataSourcePackage.h"

#include <histotypes.h>
#include <string.h>

static const char* Copyright = 
"TapeCommand.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Static data:

struct SwitchEntry {
  char*                   pName;
  CTapeCommand::Switch_t  Value;
};
static const SwitchEntry SwitchTable[] = {
  { "-open",    CTapeCommand::keOpen},
  { "-close",   CTapeCommand::keClose},
  { "-rewind",  CTapeCommand::keRewind},
  { "-next",    CTapeCommand::keNext}
};
static const UInt_t nSwitches = sizeof(SwitchTable)/sizeof(SwitchEntry);

// Functions for class CTapeCommand

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int operator() ( CTCLInterpreter& rInterp, CTCLResult& rResult, 
//                     int nArgs, char* pArgs[] )
//  Operation Type:
//     Functionalizer
//
int 
CTapeCommand::operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			 int nArgs, char* pArgs[]) 
{
// This member is called to implement the tape command
// by the Tcl interpreter.  
//
// Formal Parameters:
//     CTCLInterpreter& rInterp:
//             Interpreter which is running this command.
//     CTCLResult& rResult:
//            Resuilt string associated with that interpreter.
//     int nArgs:
//              Number of command line elements.
//     char* pArgs[]:
//           Array of pointers to the command line elements.
// Returns:
//     TCL_OK                 - if success.
//     TCL_ERROR         - if falied with reason in rResult.


  nArgs--;
  pArgs++;
  if(nArgs < 1) {
    Usage(rResult);
    return TCL_ERROR;
  }
  // First off, the data source must be a tape:

  CDataSourcePackage& rPack = (CDataSourcePackage&)getMyPackage();
  if(!rPack.isTape()) {
    rResult = "The data source is not currently a tape data source";
    return TCL_ERROR;
  }

  // The next parameter must be a switch in {-open, -close, -rewind}

  char* pSwitch = pArgs[0];
  nArgs--;
  pArgs++;

  switch(ParseSwitch(pSwitch)) {
  case keOpen:
    return OpenFile(rResult, nArgs, pArgs);
  case keClose:
    return CloseFile(rResult, nArgs, pArgs);
  case keRewind:
    return RewindTape(rResult, nArgs, pArgs);
  default:			// Not a switch or not ok in context.
    Usage(rResult);
    return TCL_ERROR;
  }

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int OpenFile ( CTCLResult& rResult, int nArgs, char* pArgs[]) 
//  Operation Type:
//     Subfunction.
//
int 
CTapeCommand::OpenFile(CTCLResult& rResult, int nArgs, char* pArgs[]) 
{
// Opens a specific file on tape, given the filename.
//  
// Formal Parameters:
//   CTCLResult&        rResult:
//        The TCL result string.
//   int nArgs, char* pArgs[]:
//        The parameters remaining on the line
// Returns:
//     TCL_OK          if ok, with result set to tape:device:filename
//     TCL_ERROR  if unable to open with result set to the reason.
//
  CDataSourcePackage& rPackage = (CDataSourcePackage&)getMyPackage();

  // There must be a parameter which is either the -next switch or
  // the name of the file to open:

  if(nArgs != 1) {
    Usage(rResult);
    return TCL_ERROR;
  }
  switch(ParseSwitch(pArgs[0])) {
  case keNext:
    return rPackage.OpenNextTapeFile(rResult);

  case keNotSwitch:
    return rPackage.OpenSource(rResult, pArgs[0],    // NOTE: Tape sources, 
			       knDefaultBufferSize); // ignor buffersize.
  default:
    rResult = "Invalid switch in context: ";
    rResult += pArgs[0];
    rResult += "\n";
    Usage(rResult);
    return TCL_ERROR;
  }

}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int CloseFile ( CTCLResult& rResult, int nArgs, char* pArgs[] )
//  Operation Type:
//     SubCommand.
//
int 
CTapeCommand::CloseFile(CTCLResult& rResult, int nArgs, char* pArgs[]) 
{
// Closes the current tape file.
//
// Formal parameters:
//     CTCLResult& rResult:
//        Result string for TCL interpreter.
//     int nArgs, char* pArgs[]
//        Remaining command line parameters.
// Returns:
//       TCL_OK         - Everything worked.
//       TCL_ERROR - Failure
//

  if(nArgs) {
    Usage(rResult);
    return TCL_ERROR;
  }
  CDataSourcePackage& rPack = (CDataSourcePackage&)getMyPackage();
  return rPack.CloseSource(rResult);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int RewindTape ( CTCLResult& rResult, int nArgs, char* pArgs[] )
//  Operation Type:
//     Tape manipulation
//
int 
CTapeCommand::RewindTape(CTCLResult& rResult, int nArgs, char* pArgs[]) 
{
// Rewinds a tape back to the BOT.  Note that this is intended to 
// be used just before searching for a file prior to the current tape
// position
//
// Formal Parameters:
//   CTCLResult& rResult:
//           Reference to TCL result string
//    int nArgs, char* pArgs[]:
//           Remaining command line parameters.
//  Returns:
//    TCL_OK         - All ok.
//    TCL_ERROR - On error.
  if(nArgs) {
    Usage(rResult);
    return TCL_ERROR;
  }
  CDataSourcePackage& rPack = (CDataSourcePackage&)getMyPackage();
  return rPack.RewindTape(rResult);


}
/////////////////////////////////////////////////////////////////////////
//
//  Function:
//     void     Usage(CTCLResult& rResult)
//  Operation Type:
//     Protected utility.
//
void
CTapeCommand::Usage(CTCLResult& rResult)
{
  // Appends the correct usage to the current result string.
  //

  rResult += "Usage:\n";
  rResult += "   tape -open file\n";
  rResult += "   tape -open -next\n";
  rResult += "   tape -close\n";
  rResult += "   tape -rewind\n";
  rResult += "\n  The tape command manipulates tape data sources containing\n";
  rResult += "Multiple event files";
}
/////////////////////////////////////////////////////////////////////////
//
//   Function:
//       Switch_t ParseSwitch(const char* pSwitch)
//   Operation Type:
//       Protected utility
//
CTapeCommand::Switch_t
CTapeCommand::ParseSwitch(const char* pSwitch)
{
  for(UInt_t i = 0; i < nSwitches; i++) {
    if(strcmp(SwitchTable[i].pName, pSwitch) == 0) {
      return SwitchTable[i].Value;
    }
  }
  return keNotSwitch;
}
