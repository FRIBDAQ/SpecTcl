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
#include "TclWsCreate.h"
#include "handleManager.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <spectcl_experiment.h>
#include <uuid/uuid.h>
#include <stdlib.h>


/**
 * Constructor uses the base class to get the real work done.
 * Our command name is "wsCreate"
 *   @param interp   - The interpreter on which the command will be registered.
 */
CTclWsCreate::CTclWsCreate(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "wsCreate", true)
{
}

/**
 * The destructor also is taken care of by the base class.
 */
CTclWsCreate::~CTclWsCreate() 
{}

/** The command is dispatched here.
 ** See the class level comments for information about command syntax.
 ** @param interp - interpreter running the command.
 ** @param objv   - encapsluated Tcl_Obj*s that represent this command.
 ** @return int
 ** @retval TCL_OK - command worked. There is no result.
 **                  you can do a wsOpen to get a handle to the database.
 ** @retval TCL_ERROR - command failed; result is an error message.
 */
int
CTclWsCreate::operator()(CTCLInterpreter& interp, 
			  std::vector<CTCLObject>& objv)
{
  try {
    spectcl_experiment expHandle = getDatabaseHandle(interp, objv, 1, "spectcl::wsCreate");

    throwIfNotExpHandle(expHandle);

    /* Get the new database path and create the workspace. */

    std::string path = getParameter<std::string>(interp, objv, 2);

    int status = spectcl_workspace_create(expHandle, path.c_str());
    if (status != SPEXP_OK) {
      throw std::string(spectcl_experiment_error_msg(status));
    }
  }
  catch (std::string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }

  return TCL_OK;
}
