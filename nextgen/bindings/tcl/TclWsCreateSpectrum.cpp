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
#include "TclWsCreateSpectrum.h"
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
CTclWsCreateSpectrum::CTclWsCreateSpectrum(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "wsCreateSpectrum", true)
{}

/**
 * Simlarly the destructor uses the base class destructor to do
 * all the real work:
 */
CTclWsCreateSpectrum::~CTclWsCreateSpectrum()
{
}
/**
 * Processes the wsCreateSpectrum command.  See the .h file for the
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
CTclWsCreateSpectrum::operator()(CTCLInterpreter& interp,
				 std::vector<CTCLObject>& objv)
{

  try {
    // If there are too many parameters throw that...
    // otherwise the getParamters will throw if there are too few:

    if (objv.size() > 6) {
      throw "Incorrect number of command line parameters";
    }

    // Get the experiment database handle and validate it:

    spectcl_experiment expHandle = getDatabaseHandle(interp, objv, 1, "::spectcl::wsCreateSpectrum");
    throwIfNotExpHandle(expHandle);

    std::string spectrumName = getParameter<std::string>(interp, objv, 2);
    std::string spectrumType = getParameter<std::string>(interp, objv, 3);
    std::string parameterList= getParameter<std::string>(interp, objv, 4);

    spectrum_parameter** pParameters = parameterListToStructs(interp, parameterList);

    // Figure out the attach point

    const char* pAttach(0);
    std::string sAttach;
    if (objv.size() == 6) {
      sAttach = getParameter<std::string>(interp, objv, 5);
      pAttach = sAttach.c_str();
    }

    // Try to make the spectrum:

    int id = spectcl_workspace_create_spectrum(expHandle, spectrumType.c_str(), spectrumName.c_str(),
					       const_cast<const spectrum_parameter**>(pParameters), 
					       pAttach);
    spectcl_ws_free_spec_pars(pParameters);

    if (id < 0) {
      throw std::string(spectcl_experiment_error_msg(spectcl_experiment_errno));
    }
    interp.setResult(id);
  }
  catch (std::string errorMsg) {
    interp.setResult(errorMsg);
    return TCL_ERROR;
  }

  return TCL_OK;
}

/*--------------------------------------------------------------------------
** Private utilities:

/**
* Convert a string from something that is supposed to look like a valid
* Tcl list of pairs to a set of spectrum_parameters.  These are dynamically
* allocated and null terminated.  Once used the results should be 
* freed via spectcl_ws_free_spec_parameters
*
* @param interp   - Interpreter that will be used to bind CTCLObjects used to
*                   process the string.
* @param tclList  - String that is supposed to be a list of pairs.
* @return spectrum_parameter** '
* @retval Pointer to a null terminated array of spectrum_parameter*
*         each of which has been pulled from successive elements of
*         tclList
* @throw std::string for many cases including:
*    - tclList is not a valid TclList.
*    - tclList has an element that is not a two element list.
*    - tclList has an element whose first element is not an integer.
*    - Some CTCLObject operation throws an exception.
*/
spectrum_parameter**
CTclWsCreateSpectrum::parameterListToStructs(CTCLInterpreter& interp, std::string tclList)
{
  spectrum_parameter** ppResult(0);

  // BUGBUG: Need to check for malloc failures and report those.

  try {
    // Convert tclList to an object and figure out how many elements it has as a list:
    
    CTCLObject parameterList;
    parameterList.Bind(interp);
    parameterList      = tclList;
    int parameterCount = parameterList.llength();

    ppResult = (spectrum_parameter**)calloc(parameterCount+1, 
					    sizeof(spectrum_parameter)); // +1 for the null terminator:

    // Iterate over the list elements of parameterList.
    // - Each must be a pair.
    // - The first element must be an integer.
    // - Allocate a new spectcl_paramter struct, fill it in and poke it in an element
    //   of ppResult.

    for (int i = 0; i < parameterCount; i++) {
      CTCLObject element = parameterList.lindex(i);
      element.Bind(interp);
      int l = element.llength();
      if (element.llength() != 2) {
	throw std::string("Invalid format for wsCreateSpectrum parameter list");
      }
      CTCLObject dimension = element.lindex(0); // Throws if doesn't exist.
      CTCLObject name      = element.lindex(1);
      dimension.Bind(interp);
      name.Bind(interp);

      int         iDimension = dimension; // throws if not int 
      std::string sName      = name;

      spectrum_parameter* pParameter = (spectrum_parameter*)malloc(sizeof(spectrum_parameter));
      pParameter->s_name             = (char*)malloc(sName.size() + 1); // +1 terminating zero.
      pParameter->s_dimension        = iDimension;
      strcpy(pParameter->s_name, sName.c_str());
      
      ppResult[i] =  pParameter;
 
    }

  }
  catch (std::string msg) {	// Need to free ppResult and rethrow:
    spectcl_ws_free_spec_pars(ppResult);
    throw;
  }
  catch (CException& e) {	// free ppResult convert to string and throw that.
    spectcl_ws_free_spec_pars(ppResult);
    std::string msg(e.ReasonText());
    throw msg;
  }
  catch (...) {			// what the hell just in case:
    spectcl_ws_free_spec_pars(ppResult);
    throw std::string("CTclWsCreateSpectrum::paramterListToStructs - unexpected exception caught");
  }

  return ppResult;
   
}
