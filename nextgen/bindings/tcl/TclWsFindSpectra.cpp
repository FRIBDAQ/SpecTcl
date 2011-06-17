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
#include "TclWsFindSpectra.h"
#include "handleManager.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <spectcl_experiment.h>
#include <uuid/uuid.h>
#include <stdlib.h>
#include <uuid/uuid.h>
#include <Exception.h>




/**
 * The constructor uses the bas class constructor
 * to do all the real work.
 * @param interp - Reference to the interpreter on which this 
 *                 command (wsAttache) will be registered.
 */
CTclWsFindSpectra::CTclWsFindSpectra(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "wsFindSpectra", true)
{}

/**
 * Simlarly the destructor uses the base class destructor to do
 * all the real work:
 */
CTclWsFindSpectra::~CTclWsFindSpectra()
{
}
/**
 * Processes the wsFindSpectra command.  See the .h file for the
 * syntax of this command.
 * @param interp - Reference to the Tcl Interpter that is executing this command.
 * @param objv   - vector of encapsulated Tcl_Obj pointers that define the
 *                 command words.
 * @return int
 * @retval TCL_OK - Successful completion, returns the id of the spectrum.
 *                  The id can be used to look up useful information about the
 *                  spectrumm in other commands.
 * @retval TCL_ERROR - Command failed, the result is a human readable error
 *                  message that describes the failure.
 */
int
CTclWsFindSpectra::operator()(CTCLInterpreter& interp,
				 std::vector<CTCLObject>& objv)
{

  try {
    // Get the database handle and verify this is an experiment handle.
    //
    spectcl_experiment exp = getDatabaseHandle(interp, objv, 1, "::spectcl::wsFindSpectra");

    throwIfNotExpHandle(exp);
  } 
  catch (std::string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }
  interp.setResult("Unimplemented");
  return TCL_ERROR;
}
