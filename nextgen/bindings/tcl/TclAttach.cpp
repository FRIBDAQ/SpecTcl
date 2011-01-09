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
#include "TclAttach.h"

#include "handleManager.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <spectcl_experiment.h>

/**
 ** Constructor uses the base class to get the real work done.
 ** @param interp - referencde to the TCL Intpereter on which the command is registered.
 */
CTclAttach::CTclAttach(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "attach", true)
{
}
/**
 ** The destructor is also handled by the base class.
 */
CTclAttach::~CTclAttach()
{}
/**
 ** The command is dispatched here.
 ** See class level comments for information about the command syntax.
 ** 
 ** @param interp - interpreter running the command.
 ** @param objv   - encapsluated Tcl_Obj*s that represent this command.
 ** @return int
 ** @retval TCL_OK - command worked. Result is a handle to the database.
 ** @retval TCL_ERROR - command failed; result is an error message.
 */
int
CTclAttach::operator()(CTCLInterpreter& interp, 
			  std::vector<CTCLObject>& objv)
{
  const char* attachPoint = NULL;

  try {
    spectcl_experiment expHandle = getDatabaseHandle(interp, objv, 1, "::spectcl::attach");
    std::string        evtPath   = getParameter<std::string>(interp, objv, 2);
    
    // If an attach point was supplied we need to honor it:

    std::string        attachHere;
    if (objv.size() > 3) {
      attachHere  = getParameter<std::string>(interp, objv, 3);
      attachPoint = attachHere.c_str();
    }

    // Attempt the attach:

    int                status    = spectcl_events_attach(expHandle, evtPath.c_str(), attachPoint);


    if (status != SPEXP_OK) {
      std::string msg = spectcl_experiment_error_msg(status);
      throw msg;
    }

  }
  catch (std::string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }
  return TCL_OK;
}
