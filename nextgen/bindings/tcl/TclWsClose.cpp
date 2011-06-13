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
#include "TclWsClose.h"
#include "TclCreate.h"
#include "spectcl_experiment.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <string>
#include "handleManager.h"

using namespace std;


/**
 **  Construction - base class does all the heavy lifting.
 ** @param interp - Interpreter on which the command is being registered.
 */
CTclWsClose::CTclWsClose(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "wsClose", true)
{}
/**
 ** Destruction - again base class does the work.
 */
CTclWsClose::~CTclWsClose() {}
/**
 * This handler is called by the Tcl intperpreter when the wsClose command is
 * dispatched.  For command syntax see the  header file.
 * @param interp  The Tcl interpreter that is running this command.
 * @param objv    The set of encapsulated Tcl_Obj* that make up the command words.
 * @return int
 * @retval TCL_OK - Successful completion.  The command does not return a value.
 * @retval TCL_ERROR - Failure, the command return value is a human readable error message
 */
int
CTclWsClose::operator()(CTCLInterpreter& interp, 
		     std::vector<CTCLObject>& objv)
{
  try {
    spectcl_workspace ws = getDatabaseHandle(interp,
					     objv,
					     1,
					     "::spectcl::wsClose ");
    int status = spectcl_workspace_close(ws);
    if (status != SPEXP_OK) {
      throw std::string(spectcl_experiment_error_msg(status));
    }
    getHandleManager()->unregister(objv[1]);
  }
  catch (std::string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }
  return TCL_OK;
}
