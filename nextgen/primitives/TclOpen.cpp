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
#include "TclOpen.h"
#include "TclCreate.h"
#include "spectcl_experiment.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <string>
#include "handleManager.h"

using namespace std;


/**
 ** Construction lets the base class do its work:
 ** @param interp - reference to the interpreter that we are going to be registered on.
 */

CTclOpen::CTclOpen(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "expopen", true)
{

}

/**
 ** Destruction also is handled primarily by the base class.
 */
CTclOpen::~CTclOpen() {}

/**
 ** Implement the command processor.
 ** @param interp - interpreter running the command.
 ** @param objv   - encapsluated Tcl_Obj*s that represent this command.
 ** @return int
 ** @retval TCL_OK - command worked. Result is a handle to the database.
 ** @retval TCL_ERROR - command failed; result is an error message.
 */
int
CTclOpen::operator()(CTCLInterpreter& interp, 
		     std::vector<CTCLObject>& objv)
{
  // Require a database name:

  string path;
  try {
    path = getParameter<string>(interp, objv, 1);
    path = normalizePath(path);
  }
  catch (string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }

  // Try the open:

  spectcl_experiment pHandle = spectcl_experiment_open(path.c_str());
  if (!pHandle) {
    interp.setResult(spectcl_experiment_error_msg(spectcl_experiment_errno));
    return TCL_ERROR;
  }
  CHandleManager* pManager = getHandleManager();
  string          handle   = pManager->add(pHandle);

  interp.setResult(handle);

  return TCL_OK;
}
