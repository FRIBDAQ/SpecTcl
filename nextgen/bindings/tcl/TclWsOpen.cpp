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
#include "TclWsOpen.h"

#include "handleManager.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <spectcl_experiment.h>
#include <string>
#include <uuid/uuid.h>
#include <iostream>

/**
 ** Constructor uses the base class to get the real work done.
 ** @param interp - referencde to the TCL Intpereter on which the command is registered.
 */
CTclWsOpen::CTclWsOpen(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "wsOpen", true)
{
}
/**
 ** The destructor is also handled by the base class.
 */
CTclWsOpen::~CTclWsOpen()
{}

/**
 * Command dispatch handler for the wsOpen command.
 * Seethe header for program usage.
 * @param interp - Interpreter that is executing this command.
 * @param objv   - Vector of references to encapsulated Tcl_Obj's that
 *                 represent the words of the command.
 * @return int
 * @retval TCL_OK - the command succeeded its value is the Tcl handle to the
 *                  database.
 * @retval TCL_ERROR - the command failed in some way, it's value is the
 *                  human readable reason for failure.
 *
 */
int
CTclWsOpen::operator()(CTCLInterpreter& interp, 
		       std::vector<CTCLObject>& objv)
{
  try {

    std::string       path = getParameter<std::string>(interp, objv, 1);
    spectcl_workspace ws   = spectcl_workspace_open(path.c_str());
    if (!ws) {
      throw std::string(spectcl_experiment_error_msg(spectcl_experiment_errno));
    }
    // Assign the spectcl handle a Tcl handle:

    CHandleManager* pMgr = getHandleManager();
    interp.setResult(pMgr->add(ws));
    
  } 
  catch (std::string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }
		     
  return TCL_OK;
}

