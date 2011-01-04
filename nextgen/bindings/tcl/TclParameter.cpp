/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#include <config.h>
#include "TclParameter.h"
#include "TclCreate.h"
#include "handleManager.h"

#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <Exception.h>



using namespace std;

/*------------------------ Canonicals -----------------------------------------*/

/**
 ** Constructor.. leverages the functinoality of the base class to register
 ** this parameter as spectcl::parameter
 ** @param interp - Reference to the Tcl intpereter on which this command will be registered.
 */
CTclParameter::CTclParameter(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "parameter", true) {}

/**
 ** Similarly the destructor of the base classes takes care of unregistering us.
 */
CTclParameter::~CTclParameter() {}

/*---------------------- CTCLObjectProcessor interface implementation --------*/

/**
 ** Command is dispatched to this method.  About all this method does is
 ** - Ensure there's a subcommand present
 ** - Dispatch to the appropriate subcommand handler
 ** - Return the status returned by the subcommand handler, trusting in it to 
 **   set the interpreter status.
 ** @param interp - Ineterpreter that's running this command.
 ** @param objv   - Vector of encapsulate Tcl_Obj*'s that make up the command.
 ** @return int
 ** @retval TCL_OK - Everything worked.
 ** @retval TCL_ERROR - failure.
 */
int
CTclParameter::operator()(CTCLInterpreter& interp,
			  std::vector<CTCLObject>& objv)
{
  // ensure we have a subcommand and fish it out:
  try {
    string subcommand = getParameter<string>(interp, objv, 1);

    // All of the subcommands can throw error strings.

    
    if (subcommand == "create") {
      return create(interp,objv);
    }
    else if (subcommand == "list") {
      return list(interp, objv);
    }
    else if (subcommand == "alter") {
      return alter(interp, objv);
    }
    else {
      string message = "spectcl::parameter - unrecognized subcommand: ";
      message       += subcommand;
      message       += " must be 'create', 'list' or 'alter'";
      interp.setResult(message);
      return TCL_ERROR;
    }
  }
  catch (string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }

  return TCL_OK;
}

/*--------------------------- CTCLObjectProcessor utility functions -------*/

/**
 ** Process the list subcommand.  This looks like:
 ** \verbatim
 **   spectcl::parameter list expdb ?pattern?
 ** \endverbatim
 ** @param interp - Ineterpreter that's running this command.
 ** @param objv   - Vector of encapsulate Tcl_Obj*'s that make up the command.
 ** @return int
 ** @retval TCL_OK - Everything worked.
 ** @retval TCL_ERROR - failure
 */
int
CTclParameter::list(CTCLInterpreter& interp,
		    std::vector<CTCLObject>& objv) throw(string)
{

  // Get the database handle -- or throw.

  spectcl_experiment pHandle = getDatabaseHandle(interp, objv, 2, "list");

  // If a pattern has been supplied it overrides the default search pattern of *

  string pattern ="*";
  try {
    pattern = getParameter<string>(interp, objv, 3);
  }
  catch(...) {}

  // Fetch the parameter definitions from the database:

  parameter_list pList = spectcl_parameter_list(pHandle, pattern.c_str());
  if (!pList) {
    string message = "::spectcl::parameter list - failed: ";
    message       += spectcl_experiment_error_msg(spectcl_experiment_errno);
    throw message;
  }
  // Marshall the list items into a TCL list:

  CTCLObject result;
  result.Bind(interp);
  for (int i = 0; pList[i]; i++) {
    pParameterInfo pInfo = pList[i];
    CTCLObject     item;
    item.Bind(interp);
    
    item += pInfo->s_pName;

    if (pInfo->s_pUnits) {
      item += pInfo->s_pUnits;
    }
    else {
      item += "";
    }
    if (pInfo->s_haveLowLimit) {
      item += pInfo->s_lowLimit;
    }
    else {
      item += "";
    }
    if (pInfo->s_haveHiLimit) {
      item += pInfo->s_hiLimit;
    }
    else {
      item += "";
    }
    result += item;
  }
  spectcl_free_parameter_list(pList);

    
  interp.setResult(result);
  return TCL_OK;
}

/** 
 ** Process the create command.  This looks like
 ** \verbatim
 **    ::spectcl::parameter add handle name ?options?
 ** \endverbatim
 **  See the class comments for a description of the supported options.
 ** @param interp - Ineterpreter that's running this command.
 ** @param objv   - Vector of encapsulate Tcl_Obj*'s that make up the command.
 ** @return int
 ** @retval TCL_OK - Everything worked.
 ** @retval TCL_ERROR - failure
 */
int
CTclParameter::create(CTCLInterpreter& interp,
		      std::vector<CTCLObject>& objv) throw(string)
{
  // get thedatabase handle or throw

  spectcl_experiment pHandle = getDatabaseHandle(interp, objv, 2, "create");

  // Require a parameter name

  string name = getParameter<string>(interp, objv, 3);

  // Create the parameter, and throw on error.

  const char*  units(0);
  double*      low(0);
  double*      hi(0);

  parsedOptions options;
  parseOptions(options, interp, objv, 4);
  throwBadOption(options, "create");

  if(options.s_haveUnits)   units = options.s_unitsString.c_str();
  if(options.s_haveHigh)    hi    = &(options.s_high);
  if(options.s_haveLow)     low   = &(options.s_low);



  int status = spectcl_parameter_create(pHandle, name.c_str(), 
					units, low, hi);
  if (status != SPEXP_OK) {
    throw string(spectcl_experiment_error_msg(status));
  }

  // Return the parameter name as the result.

  interp.setResult(name);
  return TCL_OK;
}

/**
 ** Alter a parameter.
 ** \verbatim
 **    :spectcl::parameter alter handle name ?limitoptions?
 ** \endverbatim
 ** See the class comments for a description of 'limitoptions'
 **
 ** @param interp - Ineterpreter that's running this command.
 ** @param objv   - Vector of encapsulate Tcl_Obj*'s that make up the command.
 ** @return int
 ** @retval TCL_OK - Everything worked.
 ** @retval TCL_ERROR - failure.
 */
int
CTclParameter::alter(CTCLInterpreter& interp,
		     std::vector<CTCLObject>& objv) throw(std::string)
{
  // Get the database hnadle

  spectcl_experiment pHandle = getDatabaseHandle(interp, objv, 2, "alter");

  // Require a parameter name:

  string name = getParameter<string>(interp, objv, 3);

  // Process the options:

  parsedOptions options;
  parseOptions(options, interp, objv, 4);
  throwBadOption(options, "alter");


  if (options.s_haveUnits) {
    throw string("::spectcl::parameter alter - May not change units");
  }
  double* low(0);
  double* hi(0);

  if (options.s_haveLow) low = &(options.s_low);
  if (options.s_haveHigh) hi = &(options.s_high);

  int status  = spectcl_parameter_alter(pHandle, name.c_str(), low, hi);
  if (status != SPEXP_OK) {
    throw string(spectcl_experiment_error_msg(status));
  }

  return TCL_OK;
}
/*---------------------- Utility functions --------------------------------*/


/**
 ** Parse the options that are known about:
 ** - -units - supplies a units string.
 ** - -hi    - Supplies a high limit.
 ** - -low   - Supplies a low limit.
 ** These should therefore come paired with a value.
 ** @param result - reference to the parsedOptions& result struct we are going to fill in.
 ** @param interp - TCl interpeter running this command.
 ** @param objv   - Vector of encapsulated Tcl_Obj*'s that make up the command.
 ** @param startIndex - Index in objv where we can start looking for options.
 **
 */
void
CTclParameter::parseOptions(parsedOptions&           result,
			    CTCLInterpreter&         interp, 
			    std::vector<CTCLObject>& objv,
			    unsigned                 startIndex) throw(std::string)
{
  // Initialie the result.

  result.s_haveUnits        = false;
  result.s_haveHigh         = false;
  result.s_haveLow          = false;
  result.s_haveUnrecognized = false;

  // process the command line parameters:
  
  for (int i = startIndex; i < objv.size(); i+=2) {
    string option = getParameter<string>(interp, objv, i);
    if (option == "-units") {
      result.s_haveUnits   = true;
      result.s_unitsString = getParameter<string>(interp, objv, i+1);
    } 
    else if (option == "-hi") {
      result.s_haveHigh    = true;
      result.s_high        = getParameter<double>(interp, objv, i+1);
    }
    else if (option == "-low") {
      result.s_haveLow     = true;
      result.s_low         = getParameter<double>(interp, objv, i+1);
    }
    else {
      result.s_haveUnrecognized = true;
      result.s_unrecognized     = option;
      break;
    }
  }
}
/**
 ** Throw if an option parse resulted in a bad option:
 ** @param options - the structure defining the options.
 ** @param subcommand - subcommand tht cares about this (formatted into error message).
 ** @throws std::string if options.s_havUnrecognized is true.
 **/
void
CTclParameter::throwBadOption(CTclParameter::parsedOptions options, string subcommand) throw(std::string)
{
  if (options.s_haveUnrecognized) {
    string msg = "::spectcl::parameter ";
    msg       += subcommand;
    msg       += " unrecognizes option: ";
    msg       += options.s_unrecognized;
    throw msg;
  }

}
