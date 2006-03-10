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
//        bind -id idlist
//                  binds the set of spectra specified by id to the display.
//        bind -all
//                  binds all spectra to the display.  Already bound spectra
//                  are not affected.
//        bind -list
//                   List all bindings.
//        bind -list namelist
//                   List bindings for named spectra.
//        bind -list -id idlist
//                   List bindings for id'd spectra.
//        bind -list -xid  idlist
//                   List bindings for spectra given their
//                   display bindings.
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
#include "TCLString.h"
#include "TCLResult.h"
#include "SpectrumPackage.h"

#include <histotypes.h>                               
#include <string>
#include <vector>
#include <string.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Static Data:


struct SwitchTableEntry {
  char*      pSwitchText;
  CBindCommand::eSwitches Switch;
};
static const SwitchTableEntry Switches[] = {
  { "-new",  CBindCommand::keNew },
  { "-id",   CBindCommand::keId },
  { "-all",  CBindCommand::keAll },
  { "-list", CBindCommand::keList },
  { "-xid",  CBindCommand::keXid }
};

static const UInt_t nSwitches = sizeof(Switches)/sizeof(SwitchTableEntry);

// Functions for class CBindCommand

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int operator() ( CTCLInterpreter& rInterp, CTCLResult& rResult, 
//                    int nArgs, char* pArgs[] )
//  Operation Type:
//     Command Processor
//
int 
CBindCommand::operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			 int nArgs, char* pArgs[]) 
{
// Called whenever TCL executes the bind
// command.  The command is parsed and 
// dispatched to the appropriate handler.
// 
// Formal Parameters:
//     CTCLInterpreter&  rInterp:
//         The interpreter on which this command is running.
//     CTCLResult&        rResult:
//          A result string for this command.
//     int nArgs:
//           Number of command line parameters.
//     char* pArgs[]:
//           Set of pointers to arguments.
// Returns:
//      TCL_OK           - If the command worked.
//      TCL_ERROR   - if not.
//

  // This function must mostly distinguish between actual binding requests
  // and list requests:

  nArgs--; pArgs++;		// Don't care about command name.
  
  if(nArgs <= 0) {		// Need at least on parameter.
    Usage(rResult);
    return TCL_ERROR;
  }
  // Parse out the switch:

  switch(MatchSwitch(pArgs[0]) ){
  case keList:			// List bindings.
    nArgs--;
    pArgs++;
    return ListBindings(rInterp, rResult, nArgs, pArgs);

  case keAll:			// New bindings with all.
    nArgs--;
    pArgs++;
    if(nArgs) {			// No arguments permitted on -all switch.
      Usage(rResult);
      return TCL_ERROR;
    }
    return BindAll(rInterp, rResult);

  case keId:			// New by ident.
    nArgs--;
    pArgs++;			// Skip the -id keyword.
    if(nArgs <= 0) {
      Usage(rResult);
      return TCL_ERROR;
    }
    return BindByIdent(rInterp, rResult, nArgs, pArgs);
    
  case keNew:			// New by name (explicit).
    nArgs--;			// Skip over the -new switch
    pArgs++;			// and fall through.
  case keNotSwitch:		// New by name (implied).
    if(nArgs <= 0) {		// Must be at least on binding:
      Usage(rResult);
      return TCL_ERROR;
    }
    return BindByName(rInterp, rResult, nArgs, pArgs);

  default:			// Switch not allowed or unrecognized.
    Usage(rResult);
    return TCL_ERROR;
  }

}
//////////////////////////////////////////////////////////////////////////
// 
//  Function:
//    Int_t BindAll(CTCLInterpreter& rInterp, CTCLResult& rResult)
// Operation type:
//    Utility
//
Int_t 
CBindCommand::BindAll(CTCLInterpreter& rInterp, CTCLResult& rResult)
{
  CSpectrumPackage &rPack = (CSpectrumPackage&)(getMyPackage());

  return rPack.BindAll(rResult);
}



//////////////////////////////////////////////////////////////////////////
//
// Function
//    Int_t BindByIdent(CTCLInterpreter& rInterp, CTCLResult& rResult,
//                      Int_t nArgs, char* pArgs[])
//  Operation Type:
//      Utility:
//
Int_t
CBindCommand::BindByIdent(CTCLInterpreter& rInterp, CTCLResult& rResult,
                          Int_t nArgs, char* pArgs[])
{
  //
  // Binds a list of spectra to Displayer slots given the ids of the spectra.
  //
  // Formal Parameters:
  //     CTCLInterpreter& rInterp:
  //        TCL Interpreter executing the command.
  //     CTCLResult& rResult:
  //        Result string associated with this interpreter.
  //     int nArgs, char* p Args[]:
  //        Command line parameters.  Note that the parameters must be
  //        unsigned integer values or else the usage message will be printed
  //
  // Returns:
  //    TCL_OK      if bound.
  //    TCL_ERROR   if some could not be bound.

  vector<UInt_t>  vIdents;
  CSpectrumPackage& rPack = (CSpectrumPackage&)getMyPackage();
  if(rPack.GetNumberList(rResult, vIdents, nArgs, pArgs)) {
    return TCL_ERROR;
  }


  return rPack.BindList(rResult, vIdents);

}

Int_t
CBindCommand::BindByName(CTCLInterpreter& rInterp, CTCLResult& rResult,
			 int nArgs, char* pArgs[])
{
  // Binds a list of spectrum names to Displayer slots. 
  //
  // Formal Parameters:
  //     CTCLInterpreter& rInterp:
  //        TCL Interpreter executing the command.
  //     CTCLResult& rResult:
  //        Result string associated with this interpreter.
  //     int nArgs, char* p Args[]:
  //        Command line parameters.
  // Returns:
  //    TCL_OK      if bound.
  //    TCL_ERROR   if some could not be bound.
  //


  std::vector<std::string> vNames;
  CSpectrumPackage::GetNameList(vNames, nArgs, pArgs);

  CSpectrumPackage& rPack = (CSpectrumPackage&)getMyPackage();

  return rPack.BindList(rResult, vNames);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t ListBindings ( CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[] )
//  Operation Type:
//     Utility
//
Int_t 
CBindCommand::ListBindings(CTCLInterpreter& rInterp, CTCLResult& rResult, int nArgs, char* pArgs[]) 
{
// Processes the bind commands which
//  list bindings.
//   
// Formal parameters:
//     CTCLInterpreter&  rInterp:
//            TCL Interpreter.
//     CTCLResult&       rResult:
//           Command result string.
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
    case keId:			// List given idents.
      nArgs--;
      pArgs++;			// Skip over the -id switch.
      if(nArgs <= 0) {		// Ensure they actually provided > 0 ids.
	Usage(rResult);
	return TCL_ERROR;
      }
      return ListById(rInterp, rResult, nArgs, pArgs);

    case keXid:			// List given Xid list.
      nArgs--;
      pArgs++;			// Skip over -xid switch.
      if(nArgs <= 0) {		// Ensure at least 1 xid is provided.
	Usage(rResult);
	return TCL_ERROR;
      }
      return ListByXid(rInterp, rResult, nArgs, pArgs);

    case keNotSwitch:		// List given names.
      //return ListByName(rInterp, rResult, nArgs, pArgs);
      return ListAll(rInterp,rResult, pArgs[0]);

    default:			// Invalid switch in this context...
      return ListAll(rInterp,rResult, pArgs[0]);

    }
  }
  else {
    return ListAll(rInterp, rResult, "*");
  }

}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//   Int_t ListAll(CTCLInterpreter& rInterp, CTCLResult& rResult)
// Operation Type:
//   Utility:
//
Int_t
CBindCommand::ListAll(CTCLInterpreter& rInterp, CTCLResult& rResult, const char* pattern)
{
  // List all spectrum bindings.
  //

  CSpectrumPackage& rPack = (CSpectrumPackage&)getMyPackage();
  rPack.ListAllBindings(rResult, pattern);
  return TCL_OK;

}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//   Int_t ListByName(CTCLINterpreter& rInterp, CTCLResult& rResult,
//                    int nArgs, char* pArgs[])
// Operation Type:
//   Utility
//
Int_t
CBindCommand::ListByName(CTCLInterpreter& rInterp, CTCLResult& rResult,
			 int nArgs, char* pArgs[])
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

  CSpectrumPackage& rPack = (CSpectrumPackage&)getMyPackage();
  return rPack.ListBindings(rResult, vNames);
}
////////////////////////////////////////////////////////////////////////////
//
//  Function:
//     Int_t ListById(CTCLInterpreter& rInterp, CTCLResult& rResult,
//		      int nArgs, char* pArgs[])
//  Operation Type:
//     Utility:
//
Int_t
CBindCommand::ListById(CTCLInterpreter& rInterp, CTCLResult& rResult,
		       int nArgs, char* pArgs[])
{
  // Lists the bindings of a set of spectra given their ids.
  //
  // Formal Parameters:
  //    CTCLInterpreter& rInterp:
  //       TCL Interpreter running the command.
  //    CTCLResult& rResult:
  //       Result string.
  //    int nArgs, char* pArgs[]:
  //       Number of parameters and the parameter string pointers.
  //       In this case, all of the parameters must be unsigned integers.
  //
  // Returns:
  //   TCL_ERROR - if error.
  //   TCL_OK    - if all spectrum bindings fetched.
  //
  vector<UInt_t> vIds;
  CSpectrumPackage& rPack = (CSpectrumPackage&)getMyPackage();

  if(rPack.GetNumberList(rResult, vIds, nArgs, pArgs)) { 
                                                    // Failed to parse uints.
    return TCL_ERROR;
  }

  return rPack.ListBindings(rResult, vIds);
  
}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//   Int_t ListByXid(CTCLInterpreter& rInterp, CTCLResult& rResult,
//		     int nArgs, char* pArgs[]);
// Operation Type:
//   Utility 
//
Int_t
CBindCommand::ListByXid(CTCLInterpreter& rInterp, CTCLResult& rResult,
			int nArgs, char* pArgs[])
{
  // List the bindings of a set of spectra given the bindings slot numbers.
  //
  // Formal Parameters:
  //    CTCLInterpreter& rInterp:
  //       TCL Interpreter running the command.
  //    CTCLResult& rResult:
  //       Result string.
  //    int nArgs, char* pArgs[]:
  //       Number of parameters and the parameter string pointers.
  //       In this case, all of the parameters must be unsigned integers.
  //
  // Returns:
  //   TCL_ERROR - if error.
  //   TCL_OK    - if all spectrum bindings fetched.
  //
 
  vector<UInt_t> vIds;
  CSpectrumPackage& rPack = (CSpectrumPackage&)getMyPackage();

  if(rPack.GetNumberList(rResult, vIds, nArgs, pArgs)) {
    return TCL_ERROR;		// Not all args parsed as UInt_t
  }
  return rPack.ListXidBindings(rResult, vIds);
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
  for(UInt_t i = 0; i < nSwitches; i++) {
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
CBindCommand::Usage(CTCLResult& rResult)
{
  // Fills rResult with the correct command usage for the
  // bind Tcl Command:

  rResult  = "Usage: \n";
  rResult += "   sbind [-new] name1 [name2...]\n";
  rResult += "   sbind -id id1 [id2...]\n"; 
  rResult += "   sbind -all\n";
  rResult += "   sbind -list\n";
  rResult += "   sbind -list name1 [name2 ...]\n";
  rResult += "   sbind -list -id  id1 [id2...]\n";
  rResult += "   sbind -list -xid slot1 [slot2...]\n";
  rResult += "\n sbind allocates a display slot for a spectrum or lists\n";
  rResult += " the correspondence between display slots and spectra.\n";
  rResult += "NOTE: The bind command is a Tk command that binds gui events\n";
  rResult += "      to tcl procedures\n";
}
