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
#include "TclEvtCreate.h"
#include "handleManager.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <spectcl_experiment.h>

/**
 ** Constructor uses the base class to get the real work done.
 ** @param interp - referencde to the TCL Intpereter on which the command is registered.
 */
CTclEvtCreate::CTclEvtCreate(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "evtcreate", true)
{
}
/**
 ** The destructor is also handled by the base class.
 */
CTclEvtCreate::~CTclEvtCreate()
{}

/**
 ** The command is dispatched here.
 ** See the class level comments for information about command syntax.
 ** @param interp - interpreter running the command.
 ** @param objv   - encapsluated Tcl_Obj*s that represent this command.
 ** @return int
 ** @retval TCL_OK - command worked. Result is a handle to the database.
 ** @retval TCL_ERROR - command failed; result is an error message.
 */
int
CTclEvtCreate::operator()(CTCLInterpreter& interp, 
			  std::vector<CTCLObject>& objv)
{
  // Require an experiment handle:

  try {
    spectcl_experiment expHandle = getDatabaseHandle(interp, objv, 1, "spectcl::evtcreate");
    int                run       = getParameter<int>(interp, objv, 2);
    std::string        path      = getParameter<std::string>(interp, objv, 3);

    spectcl_events     pEvents   = spectcl_events_create(expHandle,
							 run,
							 path.c_str());
    if (!pEvents) {
      interp.setResult(spectcl_experiment_error_msg(spectcl_experiment_errno));
      return TCL_ERROR;
    }
    /*  Add the handle to the handle manager */

    CHandleManager* pManager = getHandleManager();
    std::string     handle   = pManager->add(pEvents);
    interp.setResult(handle);

  }
  catch (std::string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }


  return TCL_OK;
}
