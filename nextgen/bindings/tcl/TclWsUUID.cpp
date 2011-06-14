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
#include "TclWsUUID.h"

#include "handleManager.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <spectcl_experiment.h>
#include <string>
#include <uuid/uuid.h>
#include <iostream>
#include <stdlib.h>

/**
 ** Constructor uses the base class to get the real work done.
 ** @param interp - referencde to the TCL Intpereter on which the command is registered.
 */
CTclWsUUID::CTclWsUUID(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "wsUUID", true)
{
}
/**
 ** The destructor is also handled by the base class.
 */
CTclWsUUID::~CTclWsUUID()
{}
/**
 * Command handler that is invoked by the Tcl interpreter when the wsUUID command is 
 * used.  For syntax, see the header file.
 * @param objv   - Vector of encapsulated Tcl_Obj* that make up the command.
 * @return int
 * @retval TCL_OK - Everything worked, result is  the UUID of the workspace.
 * @retval TCL_ERROR - Failure, result is an error message.
 */
int
CTclWsUUID::operator()(CTCLInterpreter& interp, 
		       std::vector<CTCLObject>& objv)
{
  try {
    // Get the sqlite3 handle and ensure it's for a workspace:

    spectcl_workspace ws = getDatabaseHandle(interp, objv, 1, "::spectcl::wsUUID");
    if (!spectcl_workspace_isWorkspace(ws)) {
      throw std::string(spectcl_experiment_error_msg(SPEXP_NOT_WORKSPACE));
    }
    // Now we can fetch the UUID and convert it to text:


    uuid_t* uuid = spectcl_workspace_uuid(ws);
    if (!uuid) {
      throw std::string(spectcl_experiment_error_msg(spectcl_experiment_errno));
    }
    char uuidText[1000];
    uuid_unparse(*uuid, uuidText);
    interp.setResult(uuidText);
    free(uuid);
 
    
  }
  catch (std::string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }

  return TCL_OK;
}
