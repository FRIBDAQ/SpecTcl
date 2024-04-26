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

#include <config.h>

#include "ParameterCommand.h"                               
#include "ParameterPackage.h"
#include "CParameterDictionarySingleton.h"
#include "TCLInterpreter.h"
#include "TCLResult.h"
#include "TCLCommandPackage.h"
#include "TCLList.h"
#include "TCLString.h"
#include "SpecTcl.h"
#include "TCLVariable.h"
#include "TCLObject.h"
#include "TCLObject.h"
#include <Exception.h>

#include <tcl.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <map>
#include <stdio.h>
#include <iostream>
#include <string>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


// Local Data types:

struct LookupTableEntry {
  const char* pSwitch;
  CParameterCommand::SwitchValue_t Value;
};

static LookupTableEntry LookupTable[] = {
  { "-new",     CParameterCommand::NewSw    },
  { "-list",    CParameterCommand::ListSw   },
  { "-delete",  CParameterCommand::DeleteSw },
  { "-id",      CParameterCommand::IdSw     },
  { "-byid",    CParameterCommand::ByIdSw   },
  { "-trace",   CParameterCommand::AddTrace },
  { "-untrace", CParameterCommand::RmTrace  }
};
static UInt_t nLookupTableSize = sizeof(LookupTable)/sizeof(LookupTableEntry);

// Functions for class CParameterCommand


/**
 * Constructor
 *
 * @param pInterp - pointer to the interpreter.
 * @param rPackage - reference to our enclosing package.
 */
CParameterCommand::CParameterCommand(
    CTCLInterpreter* pInterp, CTCLCommandPackage& rPackage
)  :
     CTCLPackagedObjectProcessor(*pInterp, "parameter", true),
     m_Observer(pInterp)
{
    CParameterDictionarySingleton::getInstance()->addObserver(&m_Observer);
}

/**
 * destructor
 *    Remove the observer.
 */
CParameterCommand::~CParameterCommand()
{
    CParameterDictionarySingleton::getInstance()->removeObserver(&m_Observer);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int operator() ( CTCLInterpreter& rInterp, CTCLResult& rResult, 
//                     int nArguments, char* pArguments )
//  Operation Type:
//     Execution
//
Int_t 
CParameterCommand::operator()(CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv) 
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
//         std::vector<CTCLObject>& objv - the command words encapsulated as CTCLObjects.
// Returns:
//      TCL_OK         - The command succeeded.
//      TCL_ERROR - The command failed.
// 
  size_t nArguments = objv.size();
  if(nArguments < 2) {		// Minimal command is "parameter list"
    Usage(rInterp);
    return TCL_ERROR;
  }

  // The second parameter can be a switch or if it isn't this signals
  // a desire to create a new parameter:

  
  bindAll(rInterp, objv);
  unsigned firstCreateParam(1);
  switch(ParseSwitch(std::string(objv[1]).c_str())) {
  case CParameterCommand::ListSw: // List the parameters:
    return List(rInterp, objv);
  case CParameterCommand::DeleteSw: // Delete a parameter:
    return Delete(rInterp,objv);
  case CParameterCommand::NewSw:	// Create a new parameter
    firstCreateParam++;
  case CParameterCommand::NotSwitch:
    return Create(rInterp, firstCreateParam, objv);
  case CParameterCommand::AddTrace:
    return addTrace(rInterp, objv);
 case CParameterCommand::RmTrace:
    return removeTrace(rInterp, objv);

  default:			// Some other switch invalid in context:
    Usage(rInterp);
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
CParameterCommand::Create(CTCLInterpreter& rInterp, int firstCreateParam, std::vector<CTCLObject>& objv) 
{
// Creates a new parameter.
// This is the default, if there is a subcommand selector switch,
//  and is also the action taken on the -new subcommand selector switch.
//  
//  If successful, the result string is the name of the created parameter.
//  If failed, then the result string is filled appropriately and thrown.

// Formal Parameters:
//     CTCLInterpreter& rInterp:
//        Refers to the interpreter under which we are running
//     unsigned firstCreateParam - where the tail starts This can vary since the
//        lack of a switch defaults to -create.
//    std::vector<CTCLObject>& objv - enacpsulated command words.

// The tail of the command can be any of the folowing:
// The parameter name
// A parameter id that defines the index within the event array that
//    contains the parameter data.
// An optional full scale value that represents the number of bits the
//    raw parameter has (leave this off for 'real units' parameters).
// An optional full scale value  as above and a three element list containing
//    information about the meaningful range of values the parameter takes:
//    low value in real units, high value in real units and units.
//    (intended for automatically calibrating integer parameters).
// A single optional units value: for real parameters describing the units
//    of the parameter.
// No additional parameters for a unitless real parameter.
//  Examples:
//       name id bits       ;# Integer parameter with bits resolution.
//       name id bits {low hi units} ;# Integer scalable parameter with units
//       name id units      ;# Real parameter with units.
//       name id            ;# Real parameter without units.


  // Simplest is really to re-create nArg and pArg.

  std::vector<std::string>  argStrings;   // Stringified args.
  std::vector<const char*>  args;
  unsigned nArg = objv.size() - firstCreateParam;
  for (int i = firstCreateParam; i <  objv.size(); i++) {
    argStrings.push_back(std::string(objv[i]));
    args.push_back(argStrings.back().c_str());
  }
  auto pArg = args.data();

  if((nArg != 2) && (nArg != 3) && (nArg != 4)) {
    Usage(rInterp);
    return TCL_ERROR;
  }

  
  const char* pName = pArg[0];
  Int_t nId;
  Int_t nResolution;

  Int_t  nListElements;
  char**  ppListElements;
  Float_t nLow;
  Float_t nHi;
  const char*   pUnits = (char*)kpNULL;
  
  std::string rResult;

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

    if(nArg > 2) {
      if (string(pArg[2]) == "") {
      rResult = "Units/resolution parameter cannot be blank: ";
      rResult += pArg[2];
      rResult += "\n";
      throw rResult;
    }
      
  
      if(ParseInt(pArg[2], &nResolution) != TCL_OK) { // pArg[2] are units.
	      pUnits = pArg[2];

      }
      else {
	//  BUGBUGBUG - Really need to know how big a word is to do this right
	//              for now assume a 32 bit system.
	
        if((nResolution <= 0) ||
          (nResolution >  31)) {	// Must be in range of allowable bits
          rResult = "Parameter resolution must be in the range (0,32). ";
          rResult += pArg[2];
          rResult += "\n";
          throw rResult;
        }
      }
    }
  }
  catch (std::string msg) {
    Usage(rInterp, msg.c_str());
    return TCL_ERROR;
  }



  // If there is range information, then the parameter has some sort
  // of transformation mapping applied to it.

  if(nArg == 4) {
    CTCLList lRangeList(&rInterp, pArg[3]);
    lRangeList.Split(nListElements, &ppListElements);
    if(nListElements < 2) {
      Usage(rInterp, "Too few elements in range list\n");
      return TCL_ERROR;
    }
    nLow   = atof(ppListElements[0]);
    nHi    = atof(ppListElements[1]);
    pUnits = "";
    if(nListElements == 3) {
      pUnits = ppListElements[2];
    }
  }



  // At this point, the command parameters are all checked and we
  // can call back to our package manager to attemp the creation.
  // The Package manager will return the appropriate result string and
  // error code.
  //
  CParameterPackage& rPackage(*(CParameterPackage*)getPackage());

  // What we do now depends on how we parsed the parameter.
  // nArg == 2 : Real unitless parameter.
  // nArg == 3 &&  pUnits == kpNULL Integer parameter with bit resolution.
  // nArg == 3 &&  pUnits != kpNULL Real parameter with units.
  // nArg == 4 : Integer parameter with scaling information.
  Int_t added;
  if(nArg == 2) {		// Unitless real.
    added = rPackage.AddParameter(rInterp, pName, nId);
  }
  else if ( (nArg == 3) && (pUnits == kpNULL)) { // Integer with resolution.

    added = rPackage.AddParameter(rInterp, 
				  pName, nId, nResolution);
    
  }
  else if ( (nArg == 3) && (pUnits != kpNULL )) { // Real with units.
    added = rPackage.AddParameter(rInterp,
				  pName, nId, pUnits);
  }
  else if (( nArg == 4)) {	                  // Integer with scale/units
    added = rPackage.AddParameter(rInterp, pName, nId, nResolution,
				  nLow, nHi, pUnits);
  }
  else {			// Error bugcheck error message and continue.
    rResult  = "BUG detected in CParameterCommand::Create - can't figure out";
    rResult += " how to create the parameter";
    added = TCL_ERROR;
  }
  
  return added;
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
CParameterCommand::List(CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv) 
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
//      std::vector<CTCLObject>& objv - the encapsulated command words.
// Returns:
//      TCL_OK                - List succeeded.
//      TCL_ERROR        - List failed for a reason which didn't demand
//                                     an exception.

  std::string rResult;
  CParameterPackage& rPackage(*(CParameterPackage*)getPackage());;
  int  nPars = objv.size() - 2;      // For the parameter and -list opetion.
  if(nPars == 0) {		// List all parameters:
    CTCLList ParamList(rPackage.CreateTclParameterList(rInterp, "*"));
    CTCLObject* result = makeListObject(ParamList);
    rInterp.setResult(*result);
    delete result;
    return TCL_OK;
  }
  // Reconstruct the nPars and the pPars from the objv:

  std::vector<std::string>  argStrings;   // Stringified args.
  std::vector<const char*>  args;
  
  for (int i = 2; i <  objv.size(); i++) {
    argStrings.push_back(std::string(objv[i]));
    args.push_back(argStrings.back().c_str());
  }
  auto pPars = args.data();

  const char* pattern = "*";
  // The next parameter is either a switch (-byid or -id) or it is
  // the name of a parameter to list:
  //

  int status;
  switch(ParseSwitch(pPars[0])) {
  case CParameterCommand::ByIdSw:	// List all by id.
    if (nPars>1) {
	    pattern = pPars[1];
    } 
    return ListParametersById(pattern);
  case CParameterCommand::IdSw:	// List one given an id.
    if(nPars != 2) {
      Usage(rInterp, "Need an id");
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
    catch (std::string msg) {
      Usage(rInterp, msg.c_str());
      return TCL_ERROR;
    }
    status = rPackage.ListParameter(rInterp, nId);
    return status;
  case CParameterCommand::NotSwitch: // List using pattern
    CTCLList ParamList(rPackage.CreateTclParameterList(rInterp, pPars[0]));
    CTCLObject* pList = makeListObject(ParamList);
    rInterp.setResult(*pList);
    delete pList;
    return TCL_OK;
    break;
  }
  //default:			// Some switch invalid in context.
    Usage(rInterp);
    return TCL_ERROR;
  

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Uint_t Delete ( CTCLInterpreter& rInterp, CTCLResult& rResult, UInt_t nPars, Char_t* pPars[] )
//  Operation Type:
//     mutator.
//
UInt_t 
CParameterCommand::Delete(CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv) 
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

  // Reconstruct nPars, pPars:
  // Starting with the stuff post "parameter -delete"

  std::vector<std::string> stringArgs;
  std::vector<const char*>  charArgs;
  std::string rResult;

  for (int i =2; i < objv.size(); i++) {
    stringArgs.push_back(objv[i]);
    charArgs.push_back(stringArgs.back().c_str());
  }
  int nPars = charArgs.size();
  auto pPars = charArgs.data();

  if(nPars < 1) {		// Must have at leat one parameter.
    Usage(rInterp, "Missing command parameters");
    return TCL_ERROR;
  }
  CParameterPackage& rPackage(*(CParameterPackage*)getPackage());
  Int_t              nId;
  // The next parameter must either be an -id switch or a
  // parameter name:

  switch(ParseSwitch(pPars[0])) {
  case CParameterCommand::IdSw:	// Delete given parameter id.
    if(nPars != 2) {
      Usage(rInterp, "-id needs an id");
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
    catch (std::string msg) {
      Usage(rInterp, msg.c_str());
      return TCL_ERROR;
    }
    return rPackage.DeleteParameter(rInterp, nId);
  case CParameterCommand::NotSwitch: // Delete given parameter name.
    if(nPars != 1) {
      Usage(rInterp, "MIsssing a parameter name");
      return TCL_ERROR;
    }
    return rPackage.DeleteParameter(rInterp, pPars[0]);

  default:			// Invalid switch in this context.
    Usage(rInterp, "Invalid option for -delete");
    return TCL_ERROR;
  }
}
/**
 * addTrace
 *    Add a new trace script to the parameter dictionary.  There should only
 *    be one parameter present, the script.
 *
 * @param interp - the interpreter.
 * @param result - the result.
 * @param npars  - Number of remaining command line parameters.
 * @param pars   - array of pointers to those parameters.
 * @return UInt_t - TCL_OK for success and TCL_ERROR for failures.
 * */
UInt_t
CParameterCommand::addTrace(
    CTCLInterpreter& rInterp, CTCLResult& rResult,
    UInt_t nParas, Char_t* pPars[]
)
{
    if (nParas > 1) {
        rResult = "Incorrect number of command parameters";
        Usage(rInterp, rResult);
        return TCL_ERROR;
    }
    
    m_Observer.addTrace(pPars[0]);
    return TCL_OK;
}
/**
 * removeTrace
 *    Remove an existing(?) trace from the parameter dict.
 *    The only parameter is the script.
 * @param interp - the interpreter.
 * @param result - the result.
 * @param npars  - Number of remaining command line parameters.
 * @param pars   - array of pointers to those parameters.
 * @return UInt_t - TCL_OK for success and TCL_ERROR for failures.
 * */
UInt_t
CParameterCommand::removeTrace(
    CTCLInterpreter& rInterp, CTCLResult& rResult,
    UInt_t nParas, Char_t* pPars[]
)
{
    if (nParas > 1) {
        rResult = "Incorrect number of command parameters";
        Usage(rInterp, rResult);
        return TCL_ERROR;
    }
    m_Observer.removeTrace(pPars[0]);
    return TCL_OK;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Usage ( CTCLInterpreter& rInterp, CTCLResult& rResult )
//  Operation Type:
//     Utility
//
void 
CParameterCommand::Usage(CTCLInterpreter& rInterp, const char* pMsg)
{
// Appends the proper command usage to the result string.
// 
// Formal Parameters:
//     CTCLInterpeter&  rInterp:
//        Interpreter on which command is running.
//     const char* pMsg
//        If not null an additiona message.
//
// Exceptions:  

  std::string rResult;
  if (pMsg) {
    result ++ pMsg;
  }
  rResult += "Usage:\n";
  rResult += "   parameter [-new] name id bits\n";
  rResult += "   parameter [-new] name id bits {low hi units}\n";
  rResult += "   parameter [-new] name id units\n";
  rResult += "   parameter [-new] name id\n";
  rResult += "   parameter -list [-byid]\n";
  rResult += "   parameter -list  [pattern]\n";
  rResult += "   parameter -list  -id nId\n";
  rResult += "   parameter -delete name\n";
  rResult += "   parameter -delete -id \n";
  rResult += "   parameter -trace script \n";
  rResult += "   parameter -untrace script \n\n";
   
  rInterp.setResult(rResult);
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
CParameterCommand::ListParametersById(const char* pattern)
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
  CParameterPackage& rPackage(*(CParameterPackage*)getPackage());


  CTCLList ParameterList(rPackage.CreateTclParameterList(*(getInterpreter()),pattern));
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
    
    // break out the parameter description. It must be a list with 
    // At least a parameter and an id:

    if(Parameter.Split(nItems, &pItems) != TCL_OK) {
      rResult = "Unable to split parameter description: ";
      rResult += pParameters[i];
      rResult += "\n";
      Tcl_Free((char*)pParameters);
      return TCL_ERROR;
    }
    if(nItems < 2) {
      rResult = "Incorrectly formatted parameter description: ";
      rResult += pParameters[i];
      rResult += "\n";
      Tcl_Free((char*)pParameters);
      return TCL_ERROR;
    }

    // Parse the id...
    
    if(ParseInt(pItems[1], &nId) != TCL_OK) {
      Tcl_Free((char*)pParameters);
      rResult = "Could not parse parameter id from description ";
      rResult += pItems[1];
      rResult += "\n";
      Tcl_Free((char*)pItems);
      return TCL_ERROR;
    }
    if(nId < 0) {
      rResult = "Parameter id in description was less than zero ";
      rResult += pItems[1];
      rResult += "\n";
      Tcl_Free((char*)pItems);
      Tcl_Free((char*)pParameters);
      return TCL_ERROR;
    }
    aMap[(UInt_t)nId] = Parameter.getList();
    Tcl_Free((char*)pItems);
  }
  Tcl_Free((char*)pParameters);		// Done with that storage.

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

  getIntepreter()->setResult(std::string(SortedList));
  return TCL_OK;
}
/*-----------------------------------------------------------------------------
 *  Implement the nested TraceDispatcher observer class:
 */

/**
 * constructor
 *    @param pInterp  - pointer to the interpreter to use to execute the scripts.
 */
CParameterCommand::TraceDispatcher::TraceDispatcher(CTCLInterpreter* pInterp) :
    m_pInterp(pInterp)
{}

/**
 * addTrace
 *    Adds a new trace script to the observer, the observer is a
 *    container of Tcl scripts to execute when parameters are added/deleted.
 *
 * @param script - pointer to the script text.
 */
void
CParameterCommand::TraceDispatcher::addTrace(const char* script)
{
    m_scripts.push_back(script);
}
/**
 * removeTrace
 *    Remove the first (if any) trace that matches the script.
 *
 *  @param script - script to remove from tracing.
 */
void
CParameterCommand::TraceDispatcher::removeTrace(const char* script)
{
    for (auto p = m_scripts.begin(); p != m_scripts.end(); p++) {
        if (*p == script) {
            m_scripts.erase(p);
            break;                   // Only kill off one.
        }
    }
}
/**
 * onAdd
 *    Invoked when a parameter has been added to the dict.  We'll dispatch
 *    (at the global level) the scrips with the text "add" and the name of
 *    the parameter appended as parameters.
 *
 *  @param name - the name of the parameter added.
 *  @param param - Reference to the CParameter object for that parameter.
 */
void
CParameterCommand::TraceDispatcher::onAdd(std::string name, CParameter& param)
{
    for (auto p = m_scripts.begin() ; p != m_scripts.end(); p++) {
        CTCLObject command;
        command.Bind(*m_pInterp);
        command += *p;
        command += "add";
        command += name;
        
        try {
            command();            // Evals at global level.
        }
        catch (CException& e) {
            Tcl_BackgroundError(m_pInterp->getInterpreter());
        }
    }
}
/**
 * onRemove
 *    Invoked when a parameter has been deleted from the dict.  Dispatch
 *    the trace script with delete and the name of the parameter added.
 *
 *  @param name - the name of the parameter removed.
 *  @param param - References the CParameter object about to be removed.
 */
void
CParameterCommand::TraceDispatcher::onRemove(std::string name, CParameter& param)
{
    for (auto p = m_scripts.begin() ; p != m_scripts.end(); p++) {
        CTCLObject command;
        command.Bind(*m_pInterp);
        command += *p;
        command += "delete";
        command += name;
        
        try {
            command();            // Evals at global level.
        }
        catch (CException& e) {
            Tcl_BackgroundError(m_pInterp->getInterpreter());

        }
    }
}
/**
 *  ParseInt 
 *     This is used to parse integers in the same way that CTCLCommand used to.
 *    
 * @param pString - pointer to the string.
 * @param pValue - Pointer to the value to fill on success.
 * @return int - TCL_OK on success.
*/
int
CParameterCommand::(const char* pString, int* pValue) {
  return Tcl_GetInt(getInterpreter()->getInterpreter(), pString, pValue);
}
/**
 *  makeListResult
 *    Create a CTCLObject that contains a list of strings.
 * 
 *   @param list - references a CTCLList object.
 *   @return CTCLObject* - dynamically allocated object containing the list.
*/
CTCLObject*
CParamterCommand::makeListObject(CTCLList& list) {
  int argc;
  char** argv;
  list.Split(argc, &argv);
  CTCLObject* pResult = new CTCLObject();
  pResult->Bind(getInterpreter());
  for (int i =0; i < argc; i++) {
    (*pResult) += argv[i];
  }
  return pResult;
}