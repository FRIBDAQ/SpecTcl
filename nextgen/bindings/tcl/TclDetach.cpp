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
#include "TclDetach.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <spectcl_experiment.h>
#include <iostream>


/**
 * Constructor: -- all the real work gets done by the base class.
 *
 *  @param interp - TCL Interpreter on which this command will be registerd.
 */
CTclDetach::CTclDetach(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "detach", true)
{}
/**
 * Similarly the base class does all the destruction work, though typically
 * the destructor will not be called for the lifetime of the program.
 */
CTclDetach::~CTclDetach()
{
}

/** 
 * Called in response to the detach command.
 * @param interp - Encapsulated interpreter that is running the command.
 * @param objv   - Vector of encapsulated objects that define the command
 *                 words.
 */
int
CTclDetach::operator()(CTCLInterpreter& interp,
		       std::vector<CTCLObject>& objv)
{
  if ((objv.size() < 2) || (objv.size() > 3)) {
    interp.setResult("::spectcl::detach - incorrect number of command parameters");
    return TCL_ERROR;
  }
  try {
    spectcl_experiment handle = getDatabaseHandle(interp, objv, 1, "::spectcl::detach");

    // Use a null attachment point unless the user has supplied one.

    const char* pAttachPointer = NULL;
    std::string      userAttachPoint;
    try {
      userAttachPoint = getParameter<std::string>(interp, objv, 2); // throws if not 3 cmd words.
      pAttachPointer  = userAttachPoint.c_str();
    }
    catch (...) {
    }

    // Ready to do the actual detach.

    int status = spectcl_events_detach(handle, pAttachPointer);
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
