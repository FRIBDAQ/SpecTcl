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

#ifndef FALSE
#define FALSE 0
#endif




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
  spectrum_definition** pDefinitions(0);
  try {
    // Get the database handle and verify this is an experiment handle.
    //
    spectcl_experiment exp = getDatabaseHandle(interp, objv, 1, "::spectcl::wsFindSpectra");

    throwIfNotExpHandle(exp);

    // For the optional parameters that are just strings,
    // we're going to use a pattern where we just ignore exceptions:

    // Default pattern:

    std::string pattern("*");	// Default pattern.
    try {
      pattern = getParameter<std::string>(interp, objv, 2);
    }
    catch(...) {}

    // All versions flag:

    int allVersions(0);
    if (objv.size() >= 4) {
      std::string allVersionsString = getParameter<std::string>(interp, objv, 3);
      int status = Tcl_GetBoolean(interp.getInterpreter(), allVersionsString.c_str(),
				  &allVersions);
      if (status != TCL_OK) {
	return TCL_ERROR;
      }
    }
    // Workspace attach point:

    const char* pAttachPoint(0);
    std::string attachPoint;
    try {
      attachPoint = getParameter<std::string>(interp, objv, 4);
      pAttachPoint = attachPoint.c_str();
    }
    catch(...) {
    }
    const char* pmsg = pAttachPoint ? pAttachPoint : "NULL";

    // Get the spectra definitions, convert them into Tcl List form,
    // set the result etc.

    pDefinitions = spectcl_workspace_find_spectra(exp, pattern.c_str(), allVersions, pAttachPoint);
    if (!pDefinitions) {
      throw std::string(spectcl_experiment_error_msg(spectcl_experiment_errno));
    }
    std::string definitions = definitionsToList(interp, pDefinitions);
    spectcl_ws_free_specdefs(pDefinitions);

    interp.setResult(definitions);

  } 
  catch (std::string msg) {
    spectcl_ws_free_specdefs(pDefinitions); // harmless if NULL.
    interp.setResult(msg);
    return TCL_ERROR;
  }

  return TCL_OK;
}


/*--------------------------------------------------------------------*/
/*  Private utility functions:                                       
 */

/** 
 * Convert a spectrum definition list to a Tcl List string
 * @param interp       - Tcl interpreter (used to bind CTCL Objects.
 * @param ppDefinition - Pointer to an array of pointers to spectrum definitions.
 *                       the array is terminated by a null pointer.
 * @return std::string 
 * @retval String that is a properly formatted Tcl list
 *         that contains the spectrum definition.
 */
std::string
CTclWsFindSpectra::definitionsToList(CTCLInterpreter& interp, spectrum_definition** ppDefinitions)
{
  CTCLObject resultList;
  resultList.Bind(interp);

  while (*ppDefinitions) {
    resultList += spectrumDefToList(interp, *ppDefinitions);
    ppDefinitions++;
  }

  return (std::string)(resultList);
}
/**
 * Convert a single spectrum definition to list representation:
 * @param interp - Tcl interpreter, needed to bind CTCLObjects for 
 *                'higher functions' on those objects.
 * @param pDefinition - pointer to a single spectrum definition.
 * @return TCLObject
 * @retval see the header for information on the form of this list.
 */
CTCLObject
CTclWsFindSpectra::spectrumDefToList(CTCLInterpreter& interp, spectrum_definition*  pDefinition)
{
  CTCLObject result;
  result.Bind(interp);

  // Build up the list:

  result += pDefinition->s_id;
  result += pDefinition->s_name;
  result += pDefinition->s_type;
  result += pDefinition->s_version;
  result += parameterDefsToList(interp,pDefinition->s_parameters);


  return result;
}

/**
 * Convert a parameter list into a tcl list.
 * @param interp - CTCLInterpreter that is bound to  CTCLObject objects so
 *                that higher functions can be performed.
 * @param spectrum_parameter** Null terminated list of parameter definitions.
 * @return CTCLObject
 * @retval A list of parameter definitions as described in the
 *         class header comments.
 */
CTCLObject
CTclWsFindSpectra::parameterDefsToList(CTCLInterpreter& interp, spectrum_parameter** ppParams)
{
  CTCLObject resultList;
  resultList.Bind(interp);

  while (*ppParams) {
    CTCLObject parameter;
    parameter.Bind(interp);
    spectrum_parameter* pParam = *ppParams;

    parameter += pParam->s_dimension;
    parameter += pParam->s_name;

    resultList += parameter;

    ppParams++;
  }

  return resultList;
}
