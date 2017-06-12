/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


static const char* Copyright = "(C) Copyright Michigan State University 2008, All rights reserved";
//  CUnBindCommand.cpp
// Parses and dispatches the unbind command.
// The unbind command removes bindings between
//  histograms and the display subsystem.
//  The form of the unbnid command is:
//
//   unbind     namelist
//       Unbind a set of spectra by name.
//   unbind  -id idlist
//       Unbind a set of spectra by id.
//   unbind -xid xidlist
//       Unbind a set of spectra by bindings id.
//   unbind -all
//       Unbind all spectra.
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

#include <config.h>
#include "UnbindCommand.h"                               
#include "TCLInterpreter.h"
#include "TCLCommandPackage.h"
#include "TCLResult.h"
#include "SpectrumPackage.h"

#include <histotypes.h>
#include <vector>
#include <string>
#include <string.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


// Static storage.

struct SwitchTableEntry {
  const char*  pSwitch;		// Text of switch.
  CUnbindCommand::Switch eValue;		// Value associated with switch.
};

static const SwitchTableEntry SwitchTable[] = {
  { "-id",   CUnbindCommand::keId },
  { "-all",  CUnbindCommand::keAll}//,
//  { "-xid",  CUnbindCommand::keXid}
};
static const UInt_t SwitchTableSize =
               sizeof(SwitchTable)/sizeof(SwitchTableEntry);

// Functions for class CUnbindCommand

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int operator() ( CTCLInterpreter& rInterp, CTCLResult& rResult,
//                     int nArgs, char* pArgs[] )
//  Operation Type:
//     Command processor
//
int 
CUnbindCommand::operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			   int nArgs, char* pArgs[]) 
{
// Processes the Unbind command.
// This involves parsing the command options,
//  building an appropriate set of spectra to Unbind and
//  then calling the appropriate Unbind function in the package.
//
// Formal Paramters:
//    CTCLInterpreter&  rInterp:
//        References the interpreter on which the command runs.
//     CTCLResult&  rResult:
//        References the resutl associated with rInterp.
//      int nArgs:
//        Number of command line parameters.
//     char* pArgs[]:
//        Array of pointers to command line parameters.
// Returns:
//     TCL_OK         - All spectra were unbound.
//     TCL_ERROR - Some or all spectra could not be unbound.
//

  nArgs--;
  pArgs++;			// Skip the command name.

  // Our job is to farm out the command to the appropriate UnbindByxxxx
  // member or to unbind all. This is done by processing the first
  // parameter which must either be a switch or a spectrum name.

  
  if(nArgs <= 0) {
    Usage(rResult);
    return TCL_ERROR;
  }
  CSpectrumPackage& rPack = (CSpectrumPackage&)getMyPackage();

  switch(MatchSwitch(pArgs[0])) {
  case keId:			// -id switch:
    nArgs--;
    pArgs++;
    if(nArgs <= 0) {		// Must unbind at least one id.
      Usage(rResult);
      return TCL_ERROR;
    }
    return UnbindById(rInterp, rResult, nArgs, pArgs);

  case keAll:			// -all switch.
    rPack.UnbindAll();
    return TCL_OK;

//  case keXid:			// -xid switch.
//    nArgs--;
//    pArgs++;
//    if(nArgs <= 0) {		// Must supply at least one xid.
//      Usage(rResult);
//      return TCL_ERROR;
//    }
//    return UnbindByXid(rInterp, rResult, nArgs, pArgs);

  case keNotSwitch:		// Must be a spectrum name..
    return UnbindByName(rInterp, rResult, nArgs, pArgs);
    
  default:
    Usage(rResult);
    return TCL_ERROR;
  }
}
/////////////////////////////////////////////////////////////////////////
//
// Function:
//    Int_t UnbindByName(CTCLInterpreter& rInterp, CTCLResult& rResult, 
//		         int nArgs, char* pArgs[])
// Operation Type:
//    Utility Function
//
Int_t
CUnbindCommand::UnbindByName(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			     int nArgs, char* pArgs[])
{
  // Unbinds a set of spectra from the display given their names.
  // 
  // Formal Paramters:
  //    CTCLInterpreter&  rInterp:
  //        References the interpreter on which the command runs.
  //     CTCLResult&  rResult:
  //        References the resutl associated with rInterp.
  //      int nArgs:
  //        Number of command line parameters.
  //     char* pArgs[]:
  //        Array of pointers to command line parameters.
  //        Must be an array of spectrum names.
  // Returns:
  //     TCL_OK         - All spectra were unbound.
  //     TCL_ERROR - Some or all spectra could not be unbound.
  //
  std::vector<std::string> vNames;
  CSpectrumPackage::GetNameList(vNames, nArgs, pArgs);

  CSpectrumPackage& rPack = (CSpectrumPackage&)getMyPackage();
  return rPack.UnbindList(rResult, vNames);

}
////////////////////////////////////////////////////////////////////////
//
// Function:
//      Int_t UnbindById(CTCLInterpreter& rInterp, CTCLResult& rResult, 
//		          int nArgs, char* pArgs[])
// Operation Type:
//    Utility.
//
Int_t
CUnbindCommand::UnbindById(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			   int nArgs, char* pArgs[])
{
  // Unbinds a set of spectra from the display given their ids.
  //
  // Formal Paramters:
  //    CTCLInterpreter&  rInterp:
  //        References the interpreter on which the command runs.
  //     CTCLResult&  rResult:
  //        References the resutl associated with rInterp.
  //      int nArgs:
  //        Number of command line parameters.
  //     char* pArgs[]:
  //        Array of pointers to command line parameters.
  //        Must be an array of spectrum id numbers.
  // Returns:
  //     TCL_OK         - All spectra were unbound.
  //     TCL_ERROR - Some or all spectra could not be unbound.
  //
  std::vector<UInt_t> vIds;
  CSpectrumPackage& rPack = (CSpectrumPackage&)getMyPackage();

  if(rPack.GetNumberList(rResult, vIds, nArgs, pArgs)) {
    return TCL_ERROR;
  }

  return rPack.UnbindList(rResult, vIds);
}
///////////////////////////////////////////////////////////////////////////////
////
//// Function:
////   Int_t UnbindByXid(CTCLInterpreter& rInterp, CTCLResult& rResult,
////		       int nArgs, char* pArgs[])
//// Operation Type:
////   Utility.
////
//Int_t
//CUnbindCommand::UnbindByXid(CTCLInterpreter& rInterp, CTCLResult& rResult,
//			    int nArgs, char* pArgs[])
//{
//  //  Unbind a set of spectra from the display given their display slot ids.
//  //
//  // Formal Paramters:
//  //    CTCLInterpreter&  rInterp:
//  //        References the interpreter on which the command runs.
//  //     CTCLResult&  rResult:
//  //        References the resutl associated with rInterp.
//  //      int nArgs:
//  //        Number of command line parameters.
//  //     char* pArgs[]:
//  //        Array of pointers to command line parameters.
//  //        Must be an array of display slot ids.
//  // Returns:
//  //     TCL_OK         - All spectra were unbound.
//  //     TCL_ERROR - Some or all spectra could not be unbound.
//  //
//  std::vector<UInt_t> vIds;
//  CSpectrumPackage& rPack = (CSpectrumPackage&)getMyPackage();

//  if(rPack.GetNumberList(rResult, vIds, nArgs, pArgs)) {
//    return TCL_ERROR;
//  }


//  return  rPack.UnbindXidList(rResult, vIds);

//}
///////////////////////////////////////////////////////////////////////////
//
//  Function:
//      void   Usage(CTCLResult& rResult)
//  Operation Type:
//      Protected Utility.
//
void
CUnbindCommand::Usage(CTCLResult& rResult)
{
  rResult += "Usage: \n";
  rResult += "   unbind name1 [name2...]\n";
  rResult += "   unbind -id id1 [id2...]\n";
  rResult += "   unbind -all\n";
  rResult += "\n unbind removes an association between a spectrum and a \n";
  rResult += " displayer slot.";
}
/////////////////////////////////////////////////////////////////////////
//
// Function:
//   Switch MatchSwitch(const char* pSwitch)
// Operation Type:
//   Protected Utility.
//
CUnbindCommand::Switch
CUnbindCommand::MatchSwitch(const char* pSwitch)
{
  for(UInt_t i = 0; i < SwitchTableSize; i++) {
    if(strcmp(pSwitch, SwitchTable[i].pSwitch) == 0) {
      return SwitchTable[i].eValue;
    }
  }
  return CUnbindCommand::keNotSwitch;
}
