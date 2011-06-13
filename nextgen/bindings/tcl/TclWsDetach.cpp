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
#include "TclWsDetach.h"

#include "handleManager.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <spectcl_experiment.h>
#include <uuid/uuid.h>
#include <iostream>

/**
 ** Constructor uses the base class to get the real work done.
 ** @param interp - referencde to the TCL Intpereter on which the command is registered.
 */
CTclWsDetach::CTclWsDetach(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "wsDetach", true)
{
}
/**
 ** The destructor is also handled by the base class.
 */
CTclWsDetach::~CTclWsDetach()
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
CTclWsDetach::operator()(CTCLInterpreter& interp, 
			  std::vector<CTCLObject>& objv)
{
  // Catch the case where we have too many command parameters.
  // the try/catch below deals with when we have too few.

  if (objv.size() > 3) {
    interp.setResult("Incorrect number of command line parameters");
    return TCL_ERROR;
  }


  try {
    spectcl_experiment expDb = getDatabaseHandle(interp, objv, 1, "spectcl::wsDetach");

    // See if this is really an expdb handle:

    uuid_t*            puuid = spectcl_experiment_uuid(expDb);
    if (!puuid) {
      throw std::string(spectcl_experiment_error_msg(spectcl_experiment_errno));
    }
    // Figure out what th pass as the attach point:

    const char* pAttachPoint = NULL;
    std::string sAttachPoint;
    if (objv.size() == 3) {
      sAttachPoint = getParameter<std::string>(interp, objv, 2);
      pAttachPoint = sAttachPoint.c_str();
    }

    // Try the detach:

    int status = spectcl_workspace_detach(expDb, pAttachPoint);
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
