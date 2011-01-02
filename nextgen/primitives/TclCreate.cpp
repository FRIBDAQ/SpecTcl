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
#include "TclCreate.h"
#include "handleManager.h"
#include "spectcl_experiment.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <handleManager.h>
using namespace std;




/*--------------------- object methods -------------------------------------------*/


/**
 ** Constructor... this is handled by the base class.
 ** @param interp - Reference to the interpreter on which we will be registered.
 */
CTclCreate::CTclCreate(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "expcreate", true)
{
}
/**
 ** Destructor - also handled by base class.
 */
CTclCreate::~CTclCreate() {}

/**
 ** Implement the create command:
 ** - Require a parameter
 ** - Pass the parameter to the experiment_create function.
 ** - handle-ize the return pointer.
 ** - Set the handle as the result.
 ** @param interp - interpreter executing the command.
 ** @param objv   - Vector of encapsulated Tcl_Objv* that are the command.
 ** @return int
 ** @retval TCL_OK - command worked, result is a handle to the database.
 ** @retval TCL_ERROR - command failed result is an error message.
 */
int
CTclCreate::operator()(CTCLInterpreter& interp, 
		       std::vector<CTCLObject>& objv)
{
  // Require a parameter:
  string path;
  
  try {
    path = getParameter<string>(interp, objv, 1);
    path = normalizePath(path);
  }
  catch (string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }


 
  // Open the databse:

  spectcl_experiment pHandle = spectcl_experiment_create(path.c_str());
  if (!pHandle) {
    interp.setResult(spectcl_experiment_error_msg(spectcl_experiment_errno));
    return TCL_ERROR;
  }
  CHandleManager* pManager = getHandleManager();
  string         handle   = pManager->add(pHandle);

  interp.setResult(handle);

  return TCL_OK;
}

