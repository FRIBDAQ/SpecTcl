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

//  CBindCommand.cpp
//  This class implements the bind command.
//   The bind command manipulates and examines
//   the display bindings associated with spectra.
//   The command takes the following forms:
//
//        bind namelist
//                 binds the named spectra to the display.  Spectra
//                 already bound are not effected.
//        bind -all
//                  binds all spectra to the display.  Already bound spectra
//                  are not affected.
//        bind -list
//                   List all bindings.
//        bind -list namelist
//                   List bindings for named spectra.
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
#include "BindCommand.h"
#include "TCLInterpreter.h"
#include "TCLString.h"
#include "TCLObject.h"
#include "SpectrumPackage.h"

#include <histotypes.h>                               
#include <string>
#include <vector>
#include <string.h>

#include "BindTraceSingleton.h"
#include <TCLObject.h>
#include <stdexcept>


using namespace std;

// Static Data:


struct SwitchTableEntry {
  const char*      pSwitchText;
  CBindCommand::eSwitches Switch;
};
static const SwitchTableEntry Switches[] = {
  { "-new",  CBindCommand::keNew },
  { "-id",   CBindCommand::keId },
  { "-all",  CBindCommand::keAll },
    { "-list", CBindCommand::keList },
    { "-trace", CBindCommand::keTrace},
    {"-untrace", CBindCommand::keUntrace}
};

static const TCLPLUS::UInt_t nSwitches = sizeof(Switches)/sizeof(SwitchTableEntry);

// Functions for class CBindCommand

/**
 *  Constructor.
 *    @param pInterp - pointer to the interpreter on which this command will be
 *      registered
*/
CBindCommand::CBindCommand(CTCLInterpreter* pInterp) :
  CTCLPackagedObjectProcessor(*pInterp, "sbind", true)
{}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int operator() ( CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv )
//  Operation Type:
//     Command Processor
//
int 
CBindCommand::operator()(CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv)
{
// Called whenever TCL executes the bind
// command.  The command is parsed and 
// dispatched to the appropriate handler.
// 
// Formal Parameters:
//     CTCLInterpreter&  rInterp:
//         The interpreter on which this command is running.
//      std::vector<CTCLObject>& objv - the command words.
// Returns:
//      TCL_OK           - If the command worked.
//      TCL_ERROR   - if not.
//

  // This function must mostly distinguish between actual binding requests
  // and list requests:

  // marshall into arg , argv form to make the porting minimal work:

  int nArgs = objv.size();
  std::vector<std::string> words;
  std::vector<const char*> pWords;

  // Two loops due to lifetime issues and c_str().

  for (auto& word: objv) {
    words.push_back(std::string(word));
  }
  for (auto& word: words) {
    pWords.push_back(word.c_str());
  }
  auto pArgs = pWords.data();

  nArgs--; pArgs++;		// Don't care about command name.
  
  if(nArgs <= 0) {		// Need at least on parameter.
    Usage(rInterp);
    return TCL_ERROR;
  }
  // Parse out the switch:

  switch(MatchSwitch(pArgs[0]) ){
  case keList:			// List bindings.
    nArgs--;
    pArgs++;
    return ListBindings(rInterp,  nArgs, pArgs);

  case keAll:			// New bindings with all.
    nArgs--;
    pArgs++;
    if(nArgs) {			// No arguments permitted on -all switch.
      Usage(rInterp);
      return TCL_ERROR;
    }
    return BindAll(rInterp);
    
  case keNew:			// New by name (explicit).
    nArgs--;			// Skip over the -new switch
    pArgs++;			// and fall through.
  case keNotSwitch:		// New by name (implied).
    if(nArgs <= 0) {		// Must be at least on binding:
      Usage(rInterp);
      return TCL_ERROR;
    }
    return BindByName(rInterp, nArgs, pArgs);
  case keTrace:
    return Trace(rInterp, nArgs, pArgs);
  case keUntrace:
    return Untrace(rInterp, nArgs, pArgs);
  default:			// Switch not allowed or unrecognized.
    Usage(rInterp);
    return TCL_ERROR;
  }

}
//////////////////////////////////////////////////////////////////////////
// 
//  Function:
//    Int_t BindAll(CTCLInterpreter& rInterp)
// Operation type:
//    Utility
//
TCLPLUS::Int_t 
CBindCommand::BindAll(CTCLInterpreter& rInterp)
{
  try {
    CSpectrumPackage &rPack(*(CSpectrumPackage*)(getPackage()));
  
    return rPack.BindAll(rInterp);
  }
  catch (std::exception& e) {
    rInterp.setResult(e.what());
    return TCL_ERROR;
  }
}



TCLPLUS::Int_t
CBindCommand::BindByName(CTCLInterpreter& rInterp, int nArgs, const char* pArgs[])
{
  // Binds a list of spectrum names to Displayer slots. 
  //
  // Formal Parameters:
  //     CTCLInterpreter& rInterp:
  //        TCL Interpreter executing the command.
  //     int nArgs, char* p Args[]:
  //        Command line parameters.
  // Returns:
  //    TCL_OK      if bound.
  //    TCL_ERROR   if some could not be bound.
  //

  try {
    std::vector<std::string> vNames;
    CSpectrumPackage::GetNameList(vNames, nArgs, pArgs);
  
    CSpectrumPackage& rPack(*(CSpectrumPackage*)getPackage());
  
    return rPack.BindList(rInterp, vNames);
  }
  catch (std::exception & e) {
    rInterp.setResult(e.what());
    return TCL_ERROR;
  }
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t ListBindings ( CTCLInterpreter& rInterp,  int nArgs, char* pArgs[] )
//  Operation Type:
//     Utility
//
TCLPLUS::Int_t 
CBindCommand::ListBindings(CTCLInterpreter& rInterp, int nArgs, const char* pArgs[]) 
{
// Processes the bind commands which
//  list bindings.
//   
// Formal parameters:
//     CTCLInterpreter&  rInterp:
//            TCL Interpreter.
//     int nArgs:
//           Number of parameters past the
//           -list swtich.
//     char* pArgs[]:
//          pointers to those parameters.

  if(nArgs) {			// If there are more params; partial list.
    //
    //  The next parameter determines what the lookup list is:
    //
    switch(MatchSwitch(pArgs[0])) {

    case keNotSwitch:		// List given names.
      //return ListByName(rInterp, rResult, nArgs, pArgs);
      return ListAll(rInterp, pArgs[0]);

    default:			// Invalid switch in this context...
      return ListAll(rInterp, pArgs[0]);

    }
  }
  else {
    return ListAll(rInterp, "*");
  }

}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//   Int_t ListAll(CTCLInterpreter& rInterp, const char* pattern)
// Operation Type:
//   Utility:
//
TCLPLUS::Int_t
CBindCommand::ListAll(CTCLInterpreter& rInterp, const char* pattern)
{
  // List all spectrum bindings.
  //

  CSpectrumPackage& rPack(*(CSpectrumPackage*)getPackage());
  rPack.ListAllBindings(rInterp, pattern);
  return TCL_OK;

}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//   Int_t ListByName(CTCLINterpreter& rInterp, int nArgs, char* pArgs[])
// Operation Type:
//   Utility
//
TCLPLUS::Int_t
CBindCommand::ListByName(CTCLInterpreter& rInterp, int nArgs, const char* pArgs[])
{
  // List the bindings of a set of spectra given their names.
  // The set of parameters in nArgs/pArgs is assumed to be a set of
  // spectrum names.
  //
  // Formal Parameters:
  //    CTCLInterpreter& rInterp:
  //       TCL Interpreter running the command.
  //    CTCLResult& rResult:
  //       Result string.
  //    int nArgs, char* pArgs[]:
  //       Number of parameters and the parameter string pointers.
  //
  // Returns:
  //   TCL_ERROR - if error.
  //   TCL_OK    - if all spectrum bindings fetched.
  //

  vector<string> vNames;
  CSpectrumPackage::GetNameList(vNames, nArgs, pArgs);

  CSpectrumPackage& rPack(*(CSpectrumPackage*)getPackage());
  return rPack.ListBindings(rInterp, vNames);
}
/**
 * Trace
 *    Add a new trace to the sbind traces.
 *    - There must be exactly 3 arguments, sbind, -trace, and the script stem.
 *    - Locate the bind trace singleton.
 *    - Encapsulate the script stem in a CTCLOBject and
 *    - Add it as a sbinding trace.
 *  @return Int_t - TCL_OK On success, TCL_ERROR on failure.
 */
TCLPLUS::Int_t
CBindCommand::Trace(
  CTCLInterpreter& rInterp, int nArgs, const char* pArgs[]
)
{
  // Validate the argument count:
  
  if (nArgs != 2) {
    Usage(rInterp);
    return TCL_ERROR;
  }
  CTCLObject scriptStem;
  scriptStem.Bind(rInterp);
  scriptStem = pArgs[1];
  
  // Get the trace singleton and add the script stem:
  
  BindTraceSingleton& traceContainer(BindTraceSingleton::getInstance());
  traceContainer.addSbindTrace(rInterp, scriptStem);
  
  // Return success:
  
  return TCL_OK;
}
/**
 * Untrace
 *    Remove an sbindings trace:
 *    - Ensure there are three parameters: sbind, -untrace, script-stem.
 *    - Convert the script-stem into a CTCLObject.
 *    - Get the trace container singleton and remove the script object.
 *  @return  int TCL_OK on success, TCL_ERROR on failure
 *  @note The untrace operation in the singleton will report errors via
 *        an std::exception...we'll catch that and convert it to an
 *        interpreter result and TCL_ERROR return.
 */
TCLPLUS::Int_t
CBindCommand::Untrace(
  CTCLInterpreter& rInterp, int nArgs, const char* pArgs[]
)
{
  // Validate the argument count.
  
  if (nArgs != 2) {
    Usage(rInterp);
    return TCL_ERROR;
  }
  // Pull the script stem into an object:
  
  CTCLObject scriptStem;
  scriptStem.Bind(rInterp);
  scriptStem = pArgs[1];
  
  // Get the singleton and try to unregister this script:
  
  try {
    BindTraceSingleton& traceContainer(BindTraceSingleton::getInstance());
    traceContainer.removeSbindTrace(scriptStem);
  } catch (std::exception& e) {
    rInterp.setResult(e.what());
    return TCL_ERROR;
  }
  return TCL_OK;
}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//    static eSwitches MatchSwitch(const char* pSwitch)
// Operation type:
//    protected utility.
//
CBindCommand::eSwitches
CBindCommand::MatchSwitch(const char* pSwitch)
{
  for(TCLPLUS::UInt_t i = 0; i < nSwitches; i++) {
    if(strcmp(pSwitch, Switches[i].pSwitchText) == 0) {
      return Switches[i].Switch;
    }
  }
  return keNotSwitch;
}
/////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void      Usage(CTCLResult& rResult)
//  Operation Type:
//    Protected Utility
//
void 
CBindCommand::Usage(CTCLInterpreter& rInterp)
{
  // Fills rResult with the correct command usage for the
  // bind Tcl Command:

  std::string rResult;
  rResult  = "Usage: \n";
  rResult += "   sbind [-new] name1 [name2...]\n";
  rResult += "   sbind -all\n";
  rResult += "   sbind -list\n";
  rResult += "   sbind -list name1 [name2 ...]\n";
  rResult += "   sbind -trace script-stem\n";
  rResult += "   sbind -untrace script-stem\n";
  rResult += "\n sbind adds a spectrum or a list of spectra \n";
  rResult += " to the display. It also can be used to list bound\n";
  rResult += " spectra by name.";
  rResult += "  With SpecTcl 5.5, the ability to add and remove traces to sbind\n";
  rResult += "  has been added with the -trace and -untrace options.\n";
  rResult += "NOTE: The bind command is a Tk command that binds gui events\n";
  rResult += "      to tcl procedures\n";

  rInterp.setResult(rResult);
}
