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
#include "TclWsSpecTypes.h"
#include "handleManager.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <spectcl_experiment.h>
#include <uuid/uuid.h>
#include <stdlib.h>
#include <uuid/uuid.h>

/**
 * The constructor uses the bas class constructor
 * to do all the real work.
 * @param interp - Reference to the interpreter on which this 
 *                 command (wsSpecTypese) will be registered.
 */
CTclWsSpecTypes::CTclWsSpecTypes(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "wsSpecTypes", true)
{}

/**
 * Simlarly the destructor uses the base class destructor to do
 * all the real work:
 */
CTclWsSpecTypes::~CTclWsSpecTypes()
{
}
/**
 * This function is dispatched to when the wsSpecTypes command is actually
 * executed.  See the header file for a description of the syntax of the
 * command.
 * @param interp - Reference to the interpreter that is executing this command.
 * @param objv   - std::vector of object encapsulated Tcl_Obj*'s that make
 *                 up the command keywords.
 * @return int
 * @retval TCL_OK - the command successfully returned information about
 *                  the supported spectrum types.  The format of this
 *                  returned value is described in the header.
 * @retval TCL_ERROR - the command failed for some reason.
 *                    the command result is the human readable error message
 *                    that describes why the command failed
 */
int
CTclWsSpecTypes::operator()(CTCLInterpreter& interp,
		  std::vector<CTCLObject>& objv)
{
  try {

    // Check for too many command line parameters:
    // the code below automatically checks for too few:

    if (objv.size() > 3) {
      throw std::string("Incorrect number of command line parameters");
    }

    // Get the database handle from the command, convert it to a spectcl handle 
   

    spectcl_experiment expHandle = getDatabaseHandle(interp, objv, 1, "spectcl::wsSpecTypes");

    /// Figure out the attach point:

    const char* pAttachPoint(0);
    std::string sAttachPoint;

    if (objv.size() == 3) {
      sAttachPoint = getParameter<std::string>(interp, objv, 2);
      pAttachPoint = sAttachPoint.c_str();

    }
   
    // Get the spectrum types, throwing on error.

    spectcl_spectrum_type** types = spectcl_experiment_spectrumTypes(expHandle, pAttachPoint);
    if (!types) {
      throw std::string(spectcl_experiment_error_msg(spectcl_experiment_errno));
    }
    // Success, marshall the result string, release the types storage and 
    // fall through to the success return:

    interp.setResult(marshallList(interp, types));
    spectcl_workspace_free_typelist(types);

  } 
  catch (std::string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }

  return TCL_OK;
}
/*----------------- Utilities -------------------------- */

/**
 * Marshall the type list to a result list.
 * The form of the result list is documented in 
 * the header.  The format of the data structure in spectcl_experiment.h
 *
 * @param interp -reference to the interpreter we are running.
 * @param types - The result struct of froim spectcl_experiment_spectrumTypes.
 * @return std::string - stringified result list.
 */
std::string
CTclWsSpecTypes::marshallList(CTCLInterpreter& interp, spectcl_spectrum_type** types)
{
  CTCLObject resultObj;
  resultObj.Bind(interp);

  // Iterate over the defined types:

  while (*types) {
    spectcl_spectrum_type* item = *types;
    CTCLObject element;
    element.Bind(interp);

    // Build the sublist:

    element += item->s_type;
    element += item->s_description;

    // Append it to the result list.

    resultObj += element;

    types++;
  }
  // Conver the result objec to a string:

  return std::string(resultObj);
}
