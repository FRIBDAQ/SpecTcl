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
#include "TclClose.h"
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
CTclClose::CTclClose(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "expclose", true)
{}
/**
 ** Destruction - again base class does the work.
 */
CTclClose::~CTclClose() {}

/**
 ** Implement the command.
 ** @param interp - interpreter executing the command.
 ** @param objv   - Vector of encapsulated Tcl_Obj* that make up the command.
 ** @return int
 ** @retval TCL_OK - Everything worked, result is null.
 ** @retval TCL_ERROR - Failure, result is an error message.
 */
int
CTclClose::operator()(CTCLInterpreter& interp, 
		     std::vector<CTCLObject>& objv)
{
  // There should be exactly one command line parameter:

  try {

    // Marshall the handle

    spectcl_experiment pExperiment = getDatabaseHandle(interp, objv, 1, "::spectcl::expclose");
    objv[1].Bind(interp);						       
    string handle = objv[1];	// Must exist.

    // Remove from handle database and close:
    // 
    getHandleManager()->unregister(handle);
    
    int status = spectcl_experiment_close(pExperiment);
    if(status) {
      interp.setResult(spectcl_experiment_error_msg(status));
      return TCL_ERROR;
    }
  }
  catch (string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }


  return TCL_OK;

}
