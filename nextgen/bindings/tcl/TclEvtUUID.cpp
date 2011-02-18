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
#include "TclEvtUUID.h"

#include "TCLInterpreter.h"
#include "TCLObject.h"
#include <spectcl_experiment.h>
#include <string>
#include <uuid/uuid.h>
#include <stdlib.h>

/**
 ** Construction just uses the base class to register the command
 ** @param interp  Encapsulated interpreter
 */
CTclEvtUUID::CTclEvtUUID(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "evtuuid", true) 
{
}
/**
 ** Destructor -- base class does the real work
 */
CTclEvtUUID::~CTclEvtUUID()
{}

/**
 ** Implement the command.  Thsi is called when the spectcl::evtuuid command is
 ** invoked.
 ** @param interp   - Interpreter that will be executing the command.
 ** @param objv     - encapsulated Tcl_Obj's that make up the command words.
 ** @return int
 ** @retval TCL_OK   - Command completed normally.  UUID is result.
 ** @retval TCL_ERROR- command failed.  result/msg is the failure reason.
 */
int
CTclEvtUUID::operator()(CTCLInterpreter& interp,
			std::vector<CTCLObject>& objv)
{
  try {
    spectcl_experiment handle = getDatabaseHandle(interp, objv,
						  1, "");
    uuid_t* pBinaryUUID = spectcl_events_uuid(handle);
    if (!pBinaryUUID) {
      throw std::string(spectcl_experiment_error_msg(spectcl_experiment_errno));
    }
    char uuidText[40];
    uuid_unparse(*pBinaryUUID, uuidText);
    interp.setResult(std::string(uuidText));

    free(pBinaryUUID);
  } 
  catch (std::string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }
  return TCL_OK;
}
