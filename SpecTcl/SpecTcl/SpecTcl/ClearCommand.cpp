//  CClearCommand.cpp
// Parses and dispatches the TCL clear
// command.  This command clears selected
// spectra.  The form of this command is:
// 
//    clear -all
//          All spectra are cleared.
//    clear namelist
//         The named spectra are cleared.
//    clear -id idlist
//         The spectra given by ID are cleared.
//
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

#include "ClearCommand.h"                               
#include "SpectrumPackage.h"
#include "TCLInterpreter.h"
#include "TCLResult.h"

#include <vector>
#include <string>
#include <string.h>

static const char* Copyright = 
"ClearCommand.cpp: Copyright 1999 NSCL, All rights reserved\n";

struct SwitchEntry {
  char*                  pSwitchText;
  CClearCommand::Switch  eSwitchValue;
};

static const SwitchEntry SwitchTable[] = {
  { "-all", CClearCommand::keAll },
  { "-id" , CClearCommand::keId }
};
static const UInt_t SwitchTableSize = sizeof(SwitchTable)/sizeof(SwitchEntry);


// Functions for class ClearCommand

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int operator() ( CTCLInterpreter& rInterp, CTCLResult& rResult, 
//                     int nArgs, char* pArgs[] )
//  Operation Type:
//     Command Processor.
//
int 
CClearCommand::operator()(CTCLInterpreter& rInterp, CTCLResult& rResult,
			  int nArgs, char* pArgs[]) 
{
// Called by the TCL application framework
// when the clear command is encountered.
//  The remainder of the command is parsed
//  and dispatched to the appropriate function(s)
//   in the CSpectrumPackage interface class.
//
// Formal parameters:
//     CTCLInterpreter& rInterp:
//          Reference to the interpreter executing
//          this command.
//     CTCLResult&  rResult:
//           References the command's result string.
//     int nArgs:
//           Number of command parameters..
//    char* pArgs[]:
//           Pointers to the command parameters.
// Returns:
//    TCL_OK         - if all spectra are cleared.
//    TCL_ERROR - if one or more spectra could not be cleared.
//

  nArgs--;			// Don't pay attention to command name.
  pArgs++;

  // There must be at least one parameter, else pop the usage.


    

  if(nArgs <= 0) {
    Usage(rResult);
    return TCL_ERROR;
  }
  CSpectrumPackage& rPack = (CSpectrumPackage&)getMyPackage();

  switch(MatchSwitch(pArgs[0])) {
  case keAll:
    rPack.ClearAll();
    return TCL_OK;

  case keId:
    nArgs--;
    pArgs++;
    return ClearIdList(rResult, nArgs, pArgs);
  case keNotSwitch:
    return ClearNameList(rResult, nArgs, pArgs);
  
  default:
    Usage(rResult);
    return TCL_ERROR;
  }
}
///////////////////////////////////////////////////////////////////////////
//
//  Function:
//    Int_t  ClearNameList(CTCLResult& rResult, int nArgs, char* pArgs[])
//  Operation Type:
//    Utility.
//
Int_t
CClearCommand::ClearNameList(CTCLResult& rResult, int nArgs, char* pArgs[])
{
  // Clear a named list of spectra.
  //
  // Formal parameters:
  //     CTCLInterpreter& rInterp:
  //          Reference to the interpreter executing
  //          this command.
  //     CTCLResult&  rResult:
  //           References the command's result string.
  //     int nArgs:
  //           Number of command parameters..
  //    char* pArgs[]:
  //           Pointers to the command parameters.
  //           Which must be a list of spectrum names.
  // Returns:
  //    TCL_OK         - if all spectra are cleared.
  //    TCL_ERROR - if one or more spectra could not be cleared.
  //

  std::vector<std::string> vNames;
  CSpectrumPackage::GetNameList(vNames, nArgs, pArgs);
  CSpectrumPackage& rPack = (CSpectrumPackage&)getMyPackage();

  return rPack.ClearSubset(rResult, vNames);
}
//////////////////////////////////////////////////////////////////////
// 
// Function:
//   Int_t  ClearIdList(CTCLResult& rResult, int nArgs, char* pArgs[])
// Operation Type:
//   Utility function.
//
Int_t
CClearCommand::ClearIdList(CTCLResult& rResult, int nArgs, char* pArgs[])
{
  // Clear a list of spectra given their ids.
  // Formal parameters:
  //     CTCLInterpreter& rInterp:
  //          Reference to the interpreter executing
  //          this command.
  //     CTCLResult&  rResult:
  //           References the command's result string.
  //     int nArgs:
  //           Number of command parameters..
  //    char* pArgs[]:
  //           Pointers to the command parameters.
  //           Which must be a list of spectrum ids.
  // Returns:
  //    TCL_OK         - if all spectra are cleared.
  //    TCL_ERROR - if one or more spectra could not be cleared.
  //

  std::vector<UInt_t>  vIds;
  CSpectrumPackage& rPack = (CSpectrumPackage&)getMyPackage();
  if(rPack.GetNumberList(rResult, vIds, nArgs, pArgs)) {
    return TCL_ERROR;
  }
  return rPack.ClearSubset(rResult, vIds);

}
/////////////////////////////////////////////////////////////////////
//
//  Function:
//     void   Usage(CTLResult& rResult)
//  Operation type:
//     Protected Utility.
//
void
CClearCommand::Usage(CTCLResult& rResult)
{
  rResult += "Usage: \n";
  rResult += "   clear -all\n";
  rResult += "   clear name1 [name2 ...]\n";
  rResult += "   clear -id id1 [id2...]\n";
  rResult += "\n  Clears all or a selected set of spectra\n";
}
////////////////////////////////////////////////////////////////////
//
//  Function:
//    Switch MatchSwitch(const char* pSwitchText)
//  Operation type:
//    Protected utility (parsing).
//
CClearCommand::Switch
CClearCommand::MatchSwitch(const char* pSwitchText)
{
  for(int i = 0; i < SwitchTableSize; i++) {
    if(strcmp(pSwitchText, SwitchTable[i].pSwitchText) == 0) {
      return SwitchTable[i].eSwitchValue;
    }
  }
  return CClearCommand::keNotSwitch;
}
