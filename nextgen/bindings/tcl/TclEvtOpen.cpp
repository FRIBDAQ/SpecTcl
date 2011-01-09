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
#include "TclEvtOpen.h"
#include "handleManager.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <spectcl_experiment.h>

/**
 ** Constructor uses the base class to get the real work done.
 ** @param interp - referencde to the TCL Intpereter on which the command is registered.
 */
CTclEvtOpen::CTclEvtOpen(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "evtopen", true)
{
}
/**
 ** The destructor is also handled by the base class.
 */
CTclEvtOpen::~CTclEvtOpen()
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
CTclEvtOpen::operator()(CTCLInterpreter& interp, 
			  std::vector<CTCLObject>& objv)
{
  try {
    std::string path = getParameter<std::string>(interp, objv, 1);
    spectcl_events handle = spectcl_events_open(path.c_str());
    if (handle == NULL) {
      std::string msg = spectcl_experiment_error_msg(spectcl_experiment_errno);
      throw msg;
    }
    CHandleManager* pManager = getHandleManager();
    std::string strHandle = pManager->add(handle);
    interp.setResult(strHandle);
  }
  catch (std::string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }
  return TCL_OK;
}
