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
#include "TclUUID.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include "spectcl_experiment.h"
#include <stdlib.h>
#include <errno.h>

using namespace std;

/*--------------------------- canonicals -----------------------------*/

/**
 ** consruction is done by the base class:
 ** @param interp - reference to the interpreter that will be running our
 **                 command.
 */
CTclUUID::CTclUUID(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "expuuid", true)
{}
/**
 ** Similarly for the destructor.
 */
CTclUUID::~CTclUUID()
{
}

/*------------------------ Command implementation ------------------*/

/**
 ** The command is dispatched here by the object framework.
 ** All we are going to do is figure out which subcommand
 ** is being requested and dispatch accordingly.
 ** @param interp - Reference to to the interpreter that is running this 
 **                 command.
 ** @param objv   - Vector of encapsulated Tcl_Obj's that make up this command.
 ** @return int
 ** @retval TCL_OK - Everything worked.
 ** @retval TCL_ERROR - Failure.
 */
int
CTclUUID::operator()(CTCLInterpreter& interp,
		     std::vector<CTCLObject>& objv)
{
  try {
    string subcommand = getParameter<string>(interp, objv, 1);
    if (subcommand == "check") {
      return check(interp, objv);
    }
    else if (subcommand == "get")  {
      return get(interp, objv);
    }
    else {
      string msg("::spectcl::expuuid - invalid subcommand : ");
      msg += subcommand;
      throw msg;
    }
  }
  catch (string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }
  return TCL_OK;
}

/*------------------------ Subcommand processors -------------------------------*/

/**
 ** Process the get subcommand.  See the class comments for the full 
 ** description of this command.
 **
 ** @param rInterp - Reference to to the interpreter that is running this 
 **                 command.
 ** @param objv   - Vector of encapsulated Tcl_Obj's that make up this command.
 ** @return int
 ** @retval TCL_OK - Everything worked.
 ** @retval TCL_ERROR - Failure.
 */
int
CTclUUID::get(CTCLInterpreter& rInterp,
		std::vector<CTCLObject>& objv) throw(std::string)
{
  // We must have a database handle:

  spectcl_experiment handle = getDatabaseHandle(rInterp, objv, 2, "::spectcl::expuuid get");
  uuid_t* uuid = spectcl_experiment_uuid(handle);
  if (!uuid) {
    string msg("::spectcl::expuuid get - failed: ");
    msg  += spectcl_experiment_error_msg(spectcl_experiment_errno);
    throw msg;
  }
  char uuidText[1000];
  uuid_unparse(*uuid, uuidText);
  
  rInterp.setResult(uuidText);
  free(uuid);

  return TCL_OK;
}
/**
 ** Determine if a specific UUID matches the uuid of an experiment
 ** database.
 ** See the class comments for a full description of this subcommand.
 **
 ** @param objv   - Vector of encapsulated Tcl_Obj's that make up this command.
 ** @return int
 ** @retval TCL_OK - Everything worked.
 ** @retval TCL_ERROR - Failure.
 */
int
CTclUUID::check(CTCLInterpreter& interp,
		std::vector<CTCLObject>& objv) throw(std::string)
{
  spectcl_experiment handle = getDatabaseHandle(interp, objv, 2, "::spectcl::expuid check");
  string uuid_text          = getParameter<string>(interp, objv, 3);
  uuid_t uuid;
  
  int status = uuid_parse(uuid_text.c_str(), uuid);
  if (status == -1) {
    int e = errno;
    string msg("::spectcl::expuuid check - invalid UUID: ");
    msg += uuid_text;
    throw(msg);
  }
  bool result = spectcl_correct_experiment(handle, &uuid);
  if (result) {
    interp.setResult(1);
  } 
  else {
    interp.setResult(0);
  }

  return TCL_OK;
}
