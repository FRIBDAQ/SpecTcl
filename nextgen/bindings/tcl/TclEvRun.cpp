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
#include "TclEvRun.h"

#include "TclCreate.h"
#include "spectcl_experiment.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <string>
#include "handleManager.h"


/**
 ** Construction, let the base class do its work. We just have to tell it
 ** our command name:
 ** @param interp  - Wrapped interpreter on which the command will be run.
 **/

CTclEvRun::CTclEvRun(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "evrun", true)
{}
/**
 ** Similarly the destructor takes care of everything in the base class.
 */
CTclEvRun::~CTclEvRun()
{
}

/**
 ** Process the command.  The form of the command is described in the header.
 ** this command just wraps the spectcl_events_run primitive.
 ** @param interp  - Wrapped interpreter that is executing this command
 ** @param objv    - Wrapped Tcl_Obj*'s that make up the words of the command.
 ** @return int
 ** @retval TCL_OK - The command succeded and the run number associated with the
 **                  event file is the command result.
 ** @retval TCL_ERROR - The command failed and the result (message) explains shy.
 */
int
CTclEvRun::operator()(CTCLInterpreter& interp,
		     std::vector<CTCLObject>& objv)
{
  try {
    spectcl_experiment handle = getDatabaseHandle(interp,
						  objv,
						  1,
						  "");
    spectcl_events evhandle = reinterpret_cast<spectcl_events>(handle);
    int run;
    int status              = spectcl_events_run(&run, evhandle);
    if (status != SPEXP_OK) {
      throw std::string(spectcl_experiment_error_msg(status));
    }

    interp.setResult(run);
  }
  catch (std::string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }
  return TCL_OK;
}
