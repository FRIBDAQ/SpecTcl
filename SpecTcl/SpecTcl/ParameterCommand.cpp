
//  CParameterCommand.cpp
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

#include "ParameterCommand.h"                               
#include "ParameterPackage.h"
#include "TCLInterpreter.h"
#include "TCLResult.h"
#include "TCLCommandPackage.h"
#include "TCLList.h"
#include "TCLString.h"

#include <tcl.h>
#include <string.h>
#include <malloc.h>
#include <map>
#include <stdio.h>

static const char* Copyright = 
"CParameterCommand.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Local Data types:

struct LookupTableEntry {
  char* pSwitch;
  CParameterCommand::SwitchValue_t Value;
};

static LookupTableEntry LookupTable[] = {
  { "-new",     CParameterCommand::NewSw    },
  { "-list",    CParameterCommand::ListSw   },
  { "-delete",  CParameterCommand::DeleteSw },
  { "-id",      CParameterCommand::IdSw     },
  { "-byid",    CParameterCommand::ByIdSw   }
};
static UInt_t nLookupTableSize = sizeof(LookupTable)/sizeof(LookupTableEntry);

// Functions for class CParameterCommand

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int operator() ( CTCLInterpreter& rInterp, CTCLResult& rResult, 
//                     int nArguments, char* pArguments )
//  Operation Type:
//     Execution
//
Int_t 
CParameterCommand::operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			      int nArguments, char* pArguments[]) 
{
//  Processes the parameter command.
//  This command has several subcommands
//  which are processed by Create(), List() and Delete()
//  respectively.  Internal exceptions can trigger
//  production of the Usage message by calling the
//  Usage() function.
//       Many commands use sets of switches  The total
//   set of switches which are acceptable are decoded by
//   the ParseSwitch() member.
//
//   Formal Parameters:
//         CTCLInterpreter& rInterpreter:
//                Refers to the interpreter on which this
//                command is running.
//          CTCLResutl&   rResult:
//                Represents the result string of that interpreter.
//          int nArguments:
//                Total Number of command arguments.
//          char* pArgv[]:
//                Array of pointers to the parameters.  Note that
//               pArgv[0] points to the command name itself.
// Returns:
//      TCL_OK         - The command succeeded.
//      TCL_ERROR - The command failed.
// 

  if(nArguments < 2) {		// Minimal command is "parameter list"
    Usage(rInterp, rResult);
    return TCL_ERROR;
  }
  // The second parameter can be a switch or if it isn't this signals
  // a desire to create a new parameter:

  UInt_t  nTail   = nArguments;
  char** pTail    = pArguments;
  switch(ParseSwitch(pArguments[1])) {
  case CParameterCommand::ListSw: // List the parameters:
    return List(rInterp, rResult,
		nArguments - 2, pArguments+2);
  case CParameterCommand::DeleteSw: // Delete a parameter:
    return Delete(rInterp, rResult,
		  nArguments-2, pArguments+2);
  case CParameterCommand::NewSw:	// Create a new parameter
    nTail--;			// Allow for the new switch.
    pTail++;
  case CParameterCommand::NotSwitch:
    nTail--;			// Allow for the command name.
    pTail++;			
    return Create(rInterp, rResult,
		  nTail, pTail);
  default:			// Some other switch invalid in context:
    Usage(rInterp, rResult);
    return TCL_ERROR;
  }
  
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Create ( CTCLInterpreter& rInterp, CTCLResult rResult,
//                  UInt_t nArg, char*  pArg[] )
//  Operation Type:
//     Creational
//
UInt_t
CParameterCommand::Create(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			  UInt_t nArg, char*  pArg[]) 
{
// Creates a new parameter.
// This is the default, if there is subcommand selector swtich,
//  and is also the action taken on the -new subcommand selector switch.
//  
//  If successful, the result string is the name of the created parameter.
//  If failed, then the result string is filled appropriately and thrown.

// Formal Parameters:
//     CTCLInterpreter& rInterp:
//        Refers to the interpreter under which we are running
//     CTCLResult&      rResult:
//        Refers to an object representation of the result
//        string for this interpreter.
//     UInt_t   nArgs:
//        count of parameters in the tail of the command.
//     char*     pArgs[]:
//        Array of pointers to the parameters in the tail of the command.
//

  // The tail of the command must contain a parameter and two integers:
  // the two integers are the parameter Id and the Parameter resolution in
  // bits.

  if(nArg != 3) {
    Usage(rInterp, rResult);
    return TCL_ERROR;
  }
  char* pName = pArg[0];
  Int_t nId;
  Int_t nResolution;
  
  //  To simplify error handling, the parsing of the id and resolution
  //  values is done within a try catch block with the CTCLResult reference
  //  thrown in event of error.
  //  The idea is to put the first part of the error message in result, and
  //  toss it to the exception handler where Usage is appended and
  //  TCL_ERROR is returned to the caller.

  try {
    if(ParseInt(pArg[1], &nId) != TCL_OK) {
      rResult = "Unable to parse parameter id as integer from ";
      rResult += pArg[1];
      rResult += "\n";
      throw rResult;
    }
    if(nId < 0) {		// Note that id's must be positive.
      rResult = "Parameter id must not be negative but is: ";
      rResult += pArg[1];
      rResult += "\n";
      throw rResult;
    }
    if(ParseInt(pArg[2], &nResolution) != TCL_OK) {
      rResult = "Unable to parse parameter resolution as integer from ";
      rResult += pArg[2];
      rResult += "\n";
      throw rResult;
    }
    if(nResolution <= 0) {	// Note that resolution > 0
      rResult = "Parameter resolution must be greater than zero but is: ";
      rResult += pArg[2];
      rResult += "\n";
      throw rResult;
    }
  }
  catch (CTCLResult& rExcept) {
    Usage(rInterp, rExcept);
    return TCL_ERROR;
  }
  // At this point, the command parameters are all checked and we
  // can call back to our package manager to attemp the creation.
  // The Package manager will return the appropriate result string and
  // error code.
  //
  CParameterPackage& rPackage = (CParameterPackage&)getMyPackage();
  return rPackage.AddParameter(rResult, 
			       pName, nId, nResolution);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t List ( CTCLInterpreter& rInterp, CTCLResult& rResult, 
//                  UInt_t nPars, char* pPars[] )
//  Operation Type:
//     Subcommand
//
UInt_t 
CParameterCommand::List(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			UInt_t nPars, char* pPars[]) 
{
// Lists the attributes of parameters.
//  The following tails are recognized:
//     {empty}
//        All parameters are listed ordered by parameter name.
//     name
//        The parameter whose name is given is listed.
//     -id n
//         The parameter whos id is n is listed.
//    -byid
//         All parameters are listed, ordered by parameter id.
//    Each parameter is listed as a TCL list of the form:
//       { name  id  resolutionbits }
//    The forms which list multiple parameters produce a list of sublists,
//     One sublist for each parameter.
//    Failures to create these lists (e.g. invalid parameter name) are 
//    cause for throwing the result class as an exceptoin with
//    the reason in the result string.
//  Formal Parameters:
//      CTCLInterpreter&  rInterpreter:
//         The interpreter running the command.
//      CTCLResult&       rResult:
//          Representation of the result string of the interpreter.
//      UInt_t      nPars:
//          Number of command parameters in the tail of the command.
//      char*      pPars[]:
//          array of pointers to the tail parameters.
// Returns:
//      TCL_OK                - List succeeded.
//      TCL_ERROR        - List failed for a reason which didn't demand
//                                     an exception.

  CParameterPackage& rPackage = (CParameterPackage&)getMyPackage();

  if(nPars == 0) {		// List all parameters:
    CTCLList ParamList(rPackage.CreateTclParameterList(rInterp));
    rResult = ParamList.getList();
    return TCL_OK;
  }

  // The next parameter is either a switch (-byid or -id) or it is
  // the name of a parameter to list:
  //

  switch(ParseSwitch(pPars[0])) {
  case CParameterCommand::ByIdSw:	// List all by id.
    return ListParametersById(rResult);
  case CParameterCommand::IdSw:	// List one given an id.
    if(nPars != 2) {
      Usage(rInterp, rResult);
      return TCL_ERROR;
    }
    Int_t nId;
    try {			// See previous fcn for notes on this try/catch
      if(ParseInt(pPars[1], &nId) != TCL_OK) {
	rResult = " Unable to parse the parameter Id as an integer : ";
	rResult += pPars[1];
	rResult += "\n";
	throw rResult;
      }
      if(nId < 0) {
	rResult = " Parameter ID's must be positive integers was: ";
	rResult+= pPars[1];
	rResult+= "\n";
	throw rResult;
      }
    }
    catch (CTCLResult& rException) {
      Usage(rInterp, rException);
      return TCL_ERROR;
    }
    return rPackage.ListParameter(rResult, nId);
  case CParameterCommand::NotSwitch: // List one given a name.

    return rPackage.ListParameter(rResult, pPars[0]);
  default:			// Some switch invalid in context.
    Usage(rInterp, rResult);
    return TCL_ERROR;
  }

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Uint_t Delete ( CTCLInterpreter& rInterp, CTCLResult& rResult, UInt_t nPars, Char_t* pPars[] )
//  Operation Type:
//     mutator.
//
UInt_t 
CParameterCommand::Delete(CTCLInterpreter& rInterp, CTCLResult& rResult,
			  UInt_t nPars, Char_t* pPars[]) 
{
// Deletes parameters which are in the current set.
//    This subcommand is triggered by the -delete subcommand
//    flag.   Legal tails are:
//       name
//                 Deletes the named parameter if it exists.
//       -id nid
//                 Deletes the parameter with id = nid, if it 
//                 exists.
//   Formal Parameters:
//       CTCLInterpreter& rInterpreter,  CTCLResult& rResult:
//             Interpreter and result context.
//       UInt_t   nPars:
//            Count of parameters which are in the tail.
//       Char_t*     pPars[]:
//             Set of tail parameters.
// Returns:
//     TCL_OK               - if the parameter is deleted.
//     TCL_ERROR       - if the parameter could not be 
//                                    deleted.
// NOTE:
//     Some problems result in throwing the result string.
//  

  if(nPars < 1) {		// Must have at leat one parameter.
    Usage(rInterp, rResult);
    return TCL_ERROR;
  }
  CParameterPackage& rPackage = (CParameterPackage&)getMyPackage();
  Int_t              nId;
  // The next parameter must either be an -id switch or a
  // parameter name:

  switch(ParseSwitch(pPars[0])) {
  case CParameterCommand::IdSw:	// Delete given parameter id.
    if(nPars != 2) {
      Usage(rInterp, rResult);
      return TCL_ERROR;
    } 
    try {			// See Create() for comments on this try/catch
      if(ParseInt(pPars[1], &nId) != TCL_OK) { // idiom.
	rResult = "Unable to parse parameter id as integer ";
	rResult += pPars[1];
	rResult += "\n";
	throw rResult;
      }
      if(nId < 0) {
	rResult = "Parameter ids must be positive or zero ";
	rResult += pPars[1];
	rResult += "\n";
	throw rResult;
      }
    }
    catch (CTCLResult& rResult) {
      Usage(rInterp, rResult);
      return TCL_ERROR;
    }
    return rPackage.DeleteParameter(rResult, nId);
  case CParameterCommand::NotSwitch: // Delete given parameter name.
    if(nPars != 1) {
      Usage(rInterp, rResult);
      return TCL_ERROR;
    }
    return rPackage.DeleteParameter(rResult, pPars[0]);

  default:			// Invalid switch in this context.
    Usage(rInterp, rResult);
    return TCL_ERROR;
  }
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Usage ( CTCLInterpreter& rInterp, CTCLResult& rResult )
//  Operation Type:
//     Utility
//
void 
CParameterCommand::Usage(CTCLInterpreter& rInterp, CTCLResult& rResult) 
{
// Appends the proper command usage to the result string.
// 
// Formal Parameters:
//     CTCLInterpeter&  rInterp:
//        Interpreter on which command is running.
//     CTCLResult&       rResult:
//        Result string associated with that interpreter.
//
// Exceptions:  

  rResult += "Usage:\n";
  rResult += "   parameter [-new] name id resolution\n";
  rResult += "   parameter -list [-byid]\n";
  rResult += "   parameter -list  name\n";
  rResult += "   parameter -list  -id nId\n";
  rResult += "   parameter -delete name\n";
  rResult += "   parameter -delete -id n\n";
   

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    SwitchValue_t ParseSwitch ( const char* pSwitch )
//  Operation Type:
//     Utility
//
CParameterCommand::SwitchValue_t 
CParameterCommand::ParseSwitch(const char* pSwitch) 
{
// Parses a potential switch value and returns 
// the name of the switch, or NotSwitch if no switch 
// matches.
//
//  Formal Parameters:
//     const char* pSwitch:
//         Pointer to the potential switch.
//  Returns:
//     The switch name from the SwitchValue_t enumerator 
//      or NotSwitch if there is no match with known switches.

  for(UInt_t i = 0; i < nLookupTableSize; i++) {
    if(strcmp(pSwitch, LookupTable[i].pSwitch) == 0)
      return LookupTable[i].Value;
  }
  return CParameterCommand::NotSwitch;
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//    UInt_t ListParametersById(CTCLResults& rResult)
//
// Operation:
//    Utility Function
UInt_t
CParameterCommand::ListParametersById(CTCLResult& rResult)
{
  // This function retrieves the list of parameter definitions,
  // sorts them by parameter id order and
  // returns the sorted list in rResult.
  // This function, therefore, implements the 
  //
  // paramter -list -byid
  //
  // Command Functionality.
  //
  // Formal Parameters:
  //    CTCLResult& rResult:
  //       Result string into which the final result is placed.
  // Returns:
  //    TCL_OK          - If everything worked else.
  //    TCL_ERROR
 
  typedef map<UInt_t, std::string> SortingMap;
  CParameterPackage& rPackage = (CParameterPackage&)getMyPackage();


  CTCLList ParameterList(rPackage.CreateTclParameterList(*(getInterpreter())));
  ParameterList.Bind(getInterpreter());

  // Strategy:
  //   Break the parameters descriptions out of the list.
  //   Each parameter is described in a sublist.  The
  //   second element of each list is the Id.. Each sublist
  //   is then broken out and inserted in a map<UInt_t, CTCLList>
  //   indexed on the id.  A new list is then built up by traversing
  //   the map with an iterator (gaurenteed I think to go in key order).
  //

  Int_t    nParameters;	// These hold the split list.
  Char_t**  pParameters;
  SortingMap aMap;

  if(ParameterList.Split(nParameters, &pParameters) != TCL_OK) {
    rResult = "Unable to split parameter list\n";
    return TCL_ERROR;
  }
  for(UInt_t i = 0; i < nParameters; i++) {
    CTCLList Parameter(getInterpreter(), pParameters[i]);
    Int_t   nItems;		// These will hold broken out items from
    Int_t   nId;
    Char_t** pItems;		// the parameter description.
    
    // break out the parameter description. It must be a list with 3 items.

    if(Parameter.Split(nItems, &pItems) != TCL_OK) {
      rResult = "Unable to split parameter description: ";
      rResult += pParameters[i];
      rResult += "\n";
      free(pParameters);
      return TCL_ERROR;
    }
    if(nItems != 3) {
      rResult = "Incorrectly formatted parameter description: ";
      rResult += pParameters[i];
      rResult += "\n";
      free(pParameters);
      return TCL_ERROR;
    }

    // Parse the id...
    
    if(ParseInt(pItems[1], &nId) != TCL_OK) {
      free(pParameters);
      rResult = "Could not parse parameter id from description ";
      rResult += pItems[1];
      rResult += "\n";
      free(pItems);
      return TCL_ERROR;
    }
    if(nId < 0) {
      rResult = "Parameter id in description was less than zero ";
      rResult += pItems[1];
      rResult += "\n";
      free(pItems);
      free(pParameters);
      return TCL_ERROR;
    }
    aMap[(UInt_t)nId] = Parameter.getList();
    free(pItems);
  }
  free(pParameters);		// Done with that storage.

  // Now we just need to reconstruct a list out of the
  // lists stored in the map.
  // This is done into a TCLString since that has better facilities for
  // adding on to stuff.
  //

  CTCLString SortedList;
  SortingMap::iterator Index;
  for(Index = aMap.begin(); Index != aMap.end(); Index++) {
    SortedList.AppendElement((*Index).second.c_str());
    SortedList.Append("\n");
  }

  rResult = (const char*)SortedList; 
  return TCL_OK;
}
