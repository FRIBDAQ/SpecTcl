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
#include "TclRuns.h"
#include "TclCreate.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <Exception.h>

using namespace std;

/*----------------------------- Canonicals -----------------------------------------*/

/**
 ** Construction is just a matter of passing the interp and our name to the
 ** base class constructor
 ** @param interp - reference to the interpreter on which this command will
 **                 be registered.
 */
CTclRuns::CTclRuns(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "run", true)
{
}
/**
 ** Destruction is taken care of b the base class thanks to our virtuousity.
 **/
CTclRuns::~CTclRuns() {}

/*--------------------------- Command implementation ----------------------------*/

/**
 ** Process the command.
 ** see the class comments for details.
 ** @param interp - Interpreter on which we are registered.
 ** @param objv   - vector of encapsulated Tcl_Obj*'s that make up the command.
 ** @return int
 ** @retval TCL_OK - Ok completion.
 ** @retval TCL_ERROR - some error.
 **
 ** @note - typically errors leave information in the result that describe the problem.
 ** @note - see the class level comments for the command's syntax/form/semantics.
 */
int
CTclRuns::operator()(CTCLInterpreter& interp, 
		 std::vector<CTCLObject>& objv)
{
  try {
    string subcommand = getParameter<string>(interp, objv, 1);
    if (subcommand == "create") {
      return create(interp, objv);
    }
    else if (subcommand == "end") {
      return end(interp, objv);
    }
    else if (subcommand == "list") {
      return list(interp, objv);
    }
    else {
      string msg = "::spectcl::run  invalid subcommand: ";
      msg       += subcommand;
      throw(msg);
    }
  }
  catch (string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }
  catch (CException& e) {
    interp.setResult(e.ReasonText());
    return TCL_ERROR;
  }
  catch (...) {
    return TCL_ERROR;
  }

  return TCL_OK;
}
/**
 ** Process the end command.
 ** @param interp - Interpreter on which we are registered.
 ** @param objv   - vector of encapsulated Tcl_Obj*'s that make up the command.
 ** @return int
 ** @retval TCL_OK - Ok completion.
 ** @retval TCL_ERROR - some error.
 */
int 
CTclRuns::end(CTCLInterpreter& interp, 
		 std::vector<CTCLObject>& objv) throw(string)
{
  spectcl_experiment handle = getDatabaseHandle(interp, objv, 2, "spectcl::run end");
  int run = getParameter<int>(interp, objv, 3);
  time_t  endTime = time(NULL);	// Default time is now.
  try {
    endTime = getParameter<int>(interp, objv, 4);
  }
  catch (...) {
    // If there was a parameter it must be invalid.
    if (objv.size() > 4) {
      string msg("::spectcl::run end - end time was supplied but invalid: ");
      msg  += string(objv[4]);
      throw msg;
    }
  }

  int status = spectcl_run_end(handle, run, &endTime);
  if (status != SPEXP_OK) {
    string msg("::spectcl::run end - failed: ");
    msg += spectcl_experiment_error_msg(status);
    throw msg;
  }

  return TCL_OK;
}

/**
 ** Process the create command
 ** @param interp - Interpreter on which we are registered.
 ** @param objv   - vector of encapsulated Tcl_Obj*'s that make up the command.
 ** @return int
 ** @retval TCL_OK - Ok completion.
 ** @retval TCL_ERROR - some error.
 */
int 
CTclRuns::create(CTCLInterpreter& interp, 
		 std::vector<CTCLObject>& objv) throw(string)
{
  spectcl_experiment handle = getDatabaseHandle(interp, objv, 2,
						"spectcl::run create");
  int                run    = getParameter<int>(interp, objv, 3);
  string             title  = getParameter<string>(interp, objv, 4);

  // Parse remaining parameters:

  parsedOptions options;
  parseOptions(options, interp, objv, 5);
  if (options.s_haveUnrecognized) {
    string msg = "::spectcl::run create - unrecognized command option";
    msg       += options.s_unrecognized;
    throw msg;
  }
  // Create the run.

  int status = spectcl_run_create(handle, run, title.c_str(), 
				  options.s_haveStart ? &(options.s_start) : NULL);


  if (status != SPEXP_OK) {
    string msg(spectcl_experiment_error_msg(status));
    throw msg;
  }
  if (options.s_haveEnd) {
    status = spectcl_run_end(handle, run, &(options.s_end));
  }

  interp.setResult(objv[3]);
  return TCL_OK;
}
/**
 ** process the list command.
** @param interp - Interpreter on which we are registered.
 ** @param objv   - vector of encapsulated Tcl_Obj*'s that make up the command.
 ** @return int
 ** @retval TCL_OK - Ok completion.
 ** @retval TCL_ERROR - some error.
 */
int 
CTclRuns::list(CTCLInterpreter& interp, 
		 std::vector<CTCLObject>& objv) throw(string)
{
  spectcl_experiment handle = getDatabaseHandle(interp, objv, 2,
						"spectcl::run list");
  run_list pRuns = spectcl_run_list(handle);
  if (!pRuns) {
    throw string("::spectcl::run list - null run list returned some big bug");
  }

  // Marshall the run list into the return list.

  run_list p = pRuns;
  CTCLObject result;
  result.Bind(interp);

  while(*p) {
    pRunInfo pInfo = *p;
    CTCLObject element; element.Bind(interp);
    CTCLObject info;    info.Bind(interp);
   
    element += (int)pInfo->s_id;

    info    += pInfo->s_pTitle;
    info    += string(pInfo->s_pStartTime  ? pInfo->s_pStartTime : "");
 
    info    += string(pInfo->s_pEndTime    ? pInfo->s_pEndTime   : "");

    element += info;
    result  += element;
    p++;
  }
  spectcl_free_run_list(pRuns);

  interp.setResult(result);
  return TCL_OK;
}
/*---------------------------------- Utilities -------------------------- */
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
CTclRuns::parseOptions(parsedOptions&           result,
			    CTCLInterpreter&         interp, 
			    std::vector<CTCLObject>& objv,
			    unsigned                 startIndex) throw(std::string)
{
  // Initialie the result.

  result.s_haveStart       = false;
  result.s_haveEnd         = false;
  result.s_haveUnrecognized= false;


  // process the command line parameters:
  
  for (int i = startIndex; i < objv.size(); i+=2) {
    string option = getParameter<string>(interp, objv, i);
    if (option == "-start") {
      result.s_haveStart   = true;
      result.s_start = getParameter<int>(interp, objv, i+1);
    } 
    else if (option == "-end") {
      result.s_haveEnd     = true;
      result.s_end         = getParameter<int>(interp, objv, i+1);
    }
    else {
      result.s_haveUnrecognized = true;
      result.s_unrecognized     = option;
      break;
    }
  }
}
