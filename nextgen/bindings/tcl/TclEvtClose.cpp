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
#include "TclEvtClose.h"
#include "handleManager.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <spectcl_experiment.h>

/**
 ** Constructor uses the base class to get the real work done.
 ** @param interp - referencde to the TCL Intpereter on which the command is registered.
 */
CTclEvtClose::CTclEvtClose(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "evtclose", true)
{
}
/**
 ** The destructor is also handled by the base class.
 */
CTclEvtClose::~CTclEvtClose()
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
CTclEvtClose::operator()(CTCLInterpreter& interp, 
			  std::vector<CTCLObject>& objv)
{
  try {
    spectcl_events pHandle = getDatabaseHandle(interp, objv, 1, 
					       "spectcl::evtclose");
    int status = spectcl_events_close(pHandle);
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
