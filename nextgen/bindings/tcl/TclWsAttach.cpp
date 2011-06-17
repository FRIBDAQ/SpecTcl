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
#include "TclWsAttach.h"
#include "handleManager.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <spectcl_experiment.h>
#include <uuid/uuid.h>
#include <stdlib.h>
#include <uuid/uuid.h>

/**
 * The constructor uses the bas class constructor
 * to do all the real work.
 * @param interp - Reference to the interpreter on which this 
 *                 command (wsAttache) will be registered.
 */
CTclWsAttach::CTclWsAttach(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "wsAttach", true)
{}

/**
 * Simlarly the destructor uses the base class destructor to do
 * all the real work:
 */
CTclWsAttach::~CTclWsAttach()
{
}

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
CTclWsAttach::operator()(CTCLInterpreter& interp, 
			  std::vector<CTCLObject>& objv)
{
  try {
    if (objv.size() > 4) { 
      throw std::string("::spectcl::wsAttach - too many command parameters");
    }
    spectcl_experiment expdb = getDatabaseHandle(interp, objv, 1, "spectcl::wsAttach");
    
    throwIfNotExpHandle(expdb);

    std::string workspacePath = getParameter<std::string>(interp, objv, 2);

    const char* attachPoint = NULL;
    std::string sAttachPoint;
    if (objv.size() == 4) {
      sAttachPoint = getParameter<std::string>(interp, objv, 3);
      attachPoint = sAttachPoint.c_str();
    }

    int status = spectcl_workspace_attach(expdb, workspacePath.c_str(),
					  attachPoint);
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

