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
#include "TCLObject.h"
#include "SpectrumPackage.h"

#include <histotypes.h>
#include <vector>
#include <string>
#include <string.h>

#include "BindTraceSingleton.h"
#include <TCLObject.h>

using namespace std;


// Static storage.

struct SwitchTableEntry {
  const char*  pSwitch;		// Text of switch.
  CUnbindCommand::Switch eValue;		// Value associated with switch.
};

static const SwitchTableEntry SwitchTable[] = {
  { "-id",   CUnbindCommand::keId },
  { "-all",  CUnbindCommand::keAll},
  {"-trace", CUnbindCommand::keTrace},
	{"-untrace", CUnbindCommand::keUntrace}
};
static const TCLPLUS::UInt_t SwitchTableSize =
               sizeof(SwitchTable)/sizeof(SwitchTableEntry);

// Functions for class CUnbindCommand

/** 
 * constructor
 *     @param pInterp - Pointer to the interpreter on which this command will be registered.
 * 
*/
CUnbindCommand::CUnbindCommand(CTCLInterpreter* pInterp) :
  CTCLPackagedObjectProcessor(*pInterp, "unbind", true) 
  {}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int operator() ( CTCLInterpreter& rInterp, CTCLResult& rResult,
//                     int nArgs, char* pArgs[] )
//  Operation Type:
//     Command processor
//
int 
CUnbindCommand::operator()(CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv) 
{
// Processes the Unbind command.
// This involves parsing the command options,
//  building an appropriate set of spectra to Unbind and
//  then calling the appropriate Unbind function in the package.
//
// Formal Paramters:
//    CTCLInterpreter&  rInterp:
//        References the interpreter on which the command runs.
//    std::vector<CTCLObject>& objv 
//        References the vector of object encapsulated command words.
// Returns:
//     TCL_OK         - All spectra were unbound.
//     TCL_ERROR - Some or all spectra could not be unbound.
//

  // Reconstruct pArgs and nArgs in order to reduce the port coding.
  // Note that we need a loop to construct the args and a second loop
  // to construct the const char pointers because of lifetie issues and c_str
  // now rust would not have let me make an implicit lifetime error :-P

  std::vector<std::string> words;
  std::vector<const char*> pWords;
  int nArgs = objv.size();
  for (auto& word: objv) {
    words.push_back(std::string(word));
  }
  for (auto& word: words) {
    pWords.push_back(word.c_str());
  }
  auto pArgs = pWords.data();

  nArgs--;
  pArgs++;			// Skip the command name.

  // Our job is to farm out the command to the appropriate UnbindByxxxx
  // member or to unbind all. This is done by processing the first
  // parameter which must either be a switch or a spectrum name.

  
  if(nArgs <= 0) {
    Usage(rInterp);
    return TCL_ERROR;
  }
  CSpectrumPackage& rPack(*(CSpectrumPackage*)getPackage());

  switch(MatchSwitch(pArgs[0])) {
  case keId:			// -id switch:
    nArgs--;
    pArgs++;
    if(nArgs <= 0) {		// Must unbind at least one id.
      Usage(rInterp);
      return TCL_ERROR;
    }
    return UnbindById(rInterp,  nArgs, pArgs);

  case keAll:			// -all switch.
    rPack.UnbindAll();
    return TCL_OK;
	case keTrace:
		return Trace(rInterp, nArgs, pArgs);
	case keUntrace:
		return Untrace(rInterp, nArgs, pArgs);
  case keNotSwitch:		// Must be a spectrum name..
    return UnbindByName(rInterp, nArgs, pArgs);
    
  default:
    Usage(rInterp);
    return TCL_ERROR;
  }
}
/////////////////////////////////////////////////////////////////////////
//
// Function:
//    Int_t UnbindByName(CTCLInterpreter& rInterp, 
//		         int nArgs, char* pArgs[])
// Operation Type:
//    Utility Function
//
TCLPLUS::Int_t
CUnbindCommand::UnbindByName(CTCLInterpreter& rInterp, 
			     int nArgs, const char* pArgs[])
{
  // Unbinds a set of spectra from the display given their names.
  // 
  // Formal Paramters:
  //    CTCLInterpreter&  rInterp:
  //        References the interpreter on which the command runs.
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

  CSpectrumPackage& rPack(*(CSpectrumPackage*)getMyPackage());
  return rPack.UnbindList(rResult, vNames);

}
////////////////////////////////////////////////////////////////////////
//
// Function:
//      Int_t UnbindById(CTCLInterpreter& rInerp,
//		          int nArgs, char* pArgs[])
// Operation Type:
//    Utility.
//
TCLPLUS::Int_t
CUnbindCommand::UnbindById(CTCLInterpreter& rInterp, int nArgs, const char* pArgs[])
{
  // Unbinds a set of spectra from the display given their ids.
  //
  // Formal Paramters:
  //    CTCLInterpreter&  rInterp:
  //        References the interpreter on which the command runs.
  //      int nArgs:
  //        Number of command line parameters.
  //     char* pArgs[]:
  //        Array of pointers to command line parameters.
  //        Must be an array of spectrum id numbers.
  // Returns:
  //     TCL_OK         - All spectra were unbound.
  //     TCL_ERROR - Some or all spectra could not be unbound.
  //
  std::vector<TCLPLUS::UInt_t> vIds;
  CSpectrumPackage& rPack(*(CSpectrumPackage*)getPackage());

  if(rPack.GetNumberList(rInterp, vIds, nArgs, pArgs)) {
    return TCL_ERROR;
  }

  return rPack.UnbindList(rInterp, vIds);
}
/**
 * Trace
 *    Adds a trace to the unbind trace list.
 *    - Ensures we have the right number of parameters.
 *    - Object wraps the script.
 *    - Locates the BindTraceSingletonand adds the script as an unbind trace.
 *  @return Int_t TCL_OK on success, TCL_ERROR on failure.
 */
TCLPLUS::Int_t
CUnbindCommand::Trace(
	CTCLInterpreter& rInterp, int nArgs, const char* pArgs[]
)
{
	// Check the parameter count.
	
	if (nArgs != 2) {
		Usage(rInterp);
		return TCL_ERROR;
	}
	// Object wrap the script.
	
	CTCLObject scriptStem;
	scriptStem.Bind(rInterp);
	scriptStem = pArgs[1];
	
	// Establish the trace:
	
	BindTraceSingleton& wrapper(BindTraceSingleton::getInstance());
	wrapper.addUnbindTrace(rInterp, scriptStem);
	
	return TCL_OK;
}
/**
 * Untrace
 *   Process unbind -untrace.
 *   - Ensure we have the correct number of  parameters.
 *   - Object wrap the script
 *   - LOcate the wrappgin singleton and attempt to remove the script.
 *  @return Int_t - TCL_OK - success, TCL_ERROR on failure.
 *  @note the remove method throws an exception of the script is not already
 *    in the trace list.  We convert that to a TCL_ERROR return.
 */
TCLPLUS::Int_t
CUnbindCommand::Untrace(
	CTCLInterpreter& rInterp, int nArgs, const char* pArgs[]
)
{
	// Check the parameter count:
	
	if (nArgs != 2) {
		Usage(rInterp);
		return TCL_ERROR;
	}
	// Object wrap the script:
	
	CTCLObject scriptStem;
	scriptStem.Bind(rInterp);
	scriptStem = pArgs[1];
	
	// Attempt the removal:
	
	try {
		BindTraceSingleton& wrapper(BindTraceSingleton::getInstance());
		wrapper.removeUnbindTrace(scriptStem);
	}
	catch (std::exception& e) {
		rInterp.setResult(e.what());
		return TCL_ERROR;
	}
	return TCL_OK;
}
///////////////////////////////////////////////////////////////////////////
//
//  Function:
//      void   Usage(CTCLResult& rResult)
//  Operation Type:
//      Protected Utility.
//
void
CUnbindCommand::Usage(CTCLInterpreter& rInerp)
{
  std::string rResult;
  rResult += "Usage: \n";
  rResult += "   unbind name1 [name2...]\n";
  rResult += "   unbind -id id1 [id2...]\n";
  rResult += "   unbind -all\n";
  rResult += "\n unbind removes an association between a spectrum and a \n";
  rResult += " displayer slot.";
  rInterp.setResult(rResult);
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
  for(TCLPLUS::UInt_t i = 0; i < SwitchTableSize; i++) {
    if(strcmp(pSwitch, SwitchTable[i].pSwitch) == 0) {
      return SwitchTable[i].eValue;
    }
  }
  return CUnbindCommand::keNotSwitch;
}
