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
#include "TclAugment.h"
#include <TCLObject.h>
#include <TCLInterpreter.h>
#include <Exception.h>
#ifdef bool
#undef bool
#endif
#include <map>
#include <string>

#include <iostream>

/**
 ** Constructor; all the real work is going to be done by the 
 ** base class constructor.
 ** @param interp - CTCLInterpreter that wraps the Tcl interpreter on which we will
 **                 register the command.
 */
CTclAugment::CTclAugment(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "augment", true)
{}

/**
 ** Destructor; all the work will alsob e done by the base class destructor...
 ** and in any event usually objects of this sort live for the lifetime of the
 ** program.
 */
CTclAugment::~CTclAugment()
{}

/**
 ** Gets control when the augment command is executed:
 ** @param interp  - Tcl interpreter (object wrapped) that is executing the command.
 ** @param objv    - vector of command line words wrapped in objectivied Tcl_Objs.
 ** @return int
 ** @retval TCL_OK    - The command worked correctly.
 ** @retval TCL_ERROR - The command failed.
 **
 ** @note The result is only set on an error and in that case it is the 
 **       human readable error message.
 */
int
CTclAugment::operator()(CTCLInterpreter& interp,
			std::vector<CTCLObject>& objv)
{
  try {
    if (objv.size() != 4) {
      throw std::string("spectcl::augment - incorrect command line parameter count");
    }
    // Pull the handles out of the command list:

    spectcl_experiment pExperiment = getDatabaseHandle(interp, objv, 1, 
						       "::spectcl::augment");
    spectcl_events     pEvents     = getDatabaseHandle(interp, objv, 2,
						       "::spectcl::augment");
    std::string        script      = getParameter<std::string>(interp, objv, 3); // callback script.
    validateExpEvtHandles(pExperiment, pEvents);

    
    std::map<std::string, int> parameters = getParameters(pExperiment);
    std::vector<std::string>   parameterNames = invertMap(parameters);

    CallbackData context = {
      this, 
      pExperiment, pEvents, 
      interp, script,
      parameters, parameterNames
    };

    m_errorMessages.clear();
    int status = spectcl_events_augment(pEvents,
					toScriptAndBack,
					&context);
    if (status != SPEXP_OK) {
      throw std::string(spectcl_experiment_error_msg(status));
    }
    if (m_errorMessages.size()) {
      throw m_errorMessages;
    }

  }
  catch (std::string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }

  return TCL_OK;
}
/*------------------------------ Utilities ----------------------------------*/

/*
 * Make a parameter name -> parameter id translation table.
 * this is used to allow the user's callback script to return named parameters
 * rather than have to worry about parameter is.
 * @param pExperient - handle to the Experiment database which will be queried for
 *                     the parameter names.
 * @return std::map<std::string, id>
 * @retval map indexed by parameter name that contains the corresponding parameter id.
 */
std::map<std::string, int>
CTclAugment::getParameters(void* pExperiment)
{
  spectcl_experiment pHandle = reinterpret_cast<spectcl_experiment>(pExperiment);

  parameter_list plist       = spectcl_parameter_list(pHandle, "*");

  parameter_list p           = plist;
  std::map<std::string, int> result;
  while (*p) {
    pParameterInfo pinfo = *p;
    std::string    name  = pinfo->s_pName;
    int            id    = pinfo->s_id;
    result[name]         = id;
    p++;
  }
  spectcl_free_parameter_list(plist);
  return result;
}
/**
 * Invert a string->id map, returning a vector that translates ids -> strings.
 * If there are ids that don't have corresponding names, they will translate to an
 * empty string.
 * @param theMap - Map to invert.
 * @return std::vector<std::string> 
 * @retval The inverted map as a lookup table.
 */
std::vector<std::string>
CTclAugment::invertMap(const std::map<std::string, int>& theMap)
{
  std::vector<std::string> result;
  std::map<std::string, int>::const_iterator p = theMap.begin();
  while(p != theMap.end()) {
    std::string name = p->first;
    int         id   = p->second;
    while(result.size() <= id) {
      result.push_back(std::string(""));
    }
    result[id] = name;

    p++;
  }
  return result;
}
/**
 ** Bridge between the spectcl_events_augment callback and the
 ** tcl script callback.
 ** @param nParams   - Number of parameters in the event.
 ** @param pParams   - Pointer to the parameter data for the event.
 **                    this is assumed to be sorted by trigger number.
 ** @param context   - This is a void pointer that actually points to a
 **                    CallbackData struct.
 **
 ** @return pAugmentResult 
 ** @retval The stuff to add to the event.  This will be dynamically allocated.
 **         our caller will be expected to destroy the associated storage.
 */
pAugmentResult
CTclAugment::toScriptAndBack(size_t         nParams,
			     pParameterData pEvent,
			     void*          context)
{
  pCallbackData pContext = reinterpret_cast<pCallbackData>(context);
  CTclAugment*  pObj     = pContext->pObject;
  static AugmentResult emptyResult = {
        0, st_static, 0};
  
  // We must append to the script, the list of parameters in the form
  // [list trigger [list param_name param_value]]
  // The data from a single event are assumed to come from a single trigger.
  // as that's the definition of an event.

  CTCLObject event;
  CTCLObject parameters;
  event.Bind(pContext->interp);
  parameters.Bind(pContext->interp);
  event = static_cast<int>(pEvent->s_trigger); // Trigger number from the first parameter.
  for (int i =0; i < nParams; i++) {
    int    id    = pEvent[i].s_parameter;
    double value = pEvent[i].s_value;
    

    std::string pName = pContext->parametersById[id];
    
    parameters       += pName;
    parameters       += value;

  }
  event += parameters;

  // The string representation of event is the paramter to the script

  CTCLObject scriptobj;
  scriptobj.Bind(pContext->interp);
  scriptobj = pContext->script;
  scriptobj += event;

  std::string script = scriptobj;


  std::string result;
  try {
    result = pContext->interp.GlobalEval(script);
  }
  catch (CException& e) {
    // Script error:

    std::string errormsg = e.ReasonText();
    std::string exmsg    = "Augment Callback script error: ";
    exmsg               += errormsg;
    pObj->m_errorMessages += exmsg;
    return &emptyResult;
  }


  // Process the result string..this should be a list of the form
  // [list trigger [list param-name value...]]

  CTCLObject resultList;
  resultList.Bind(pContext->interp);
  resultList   = result;

  // Error if not a two element list:

  if (resultList.llength() != 2) {
    std::string msg = "Augment callback script did not return a 2 element list";
    pObj->m_errorMessages += msg;
    return &emptyResult;
  }

  int trigger = resultList.lindex(0); // Trigger number for everything.


  CTCLObject resultParams = resultList.lindex(1);
  resultParams.Bind(pContext->interp);
  int nElements = resultParams.llength();

  // The parameter result list must be even as they consist of param-name/param-value pairs:

  if (nElements %2) {
    std::string msg = "Augment Callback script parameter list is malformed, odd number of elements";
    pObj->m_errorMessages += msg;
    return &emptyResult;
  }
  // If there are no elements  return a static, empty augment struct.

  if (!nElements) {
    return &emptyResult;
  }
  //  Now allocate the AugmentationResult and figure out how to fill it in.

  int nResult = nElements/2;
 
  pAugmentResult pResult       = new AugmentResult;
  pResult->s_pData             = new ParameterData[nResult];
  pResult->s_destructMechanism = st_dynamic;
  pResult->s_numParameters     = nResult;
  int param = 0;
  try {
    for (int i = 0; i < nElements; i+=2) {
      std::string name = resultParams.lindex(i);
      double      value= resultParams.lindex(i+1);

      // Convert the string to a parameter id.  If no match throw an error:
      //

      const std::map<std::string, int>::iterator p = pContext->parametersByName.find(name);
      if (p == pContext->parametersByName.end()) {
	std::string msg = "Augment callback script returned an undefined parameter: ";
	msg            += name;
	pObj->m_errorMessages += msg;
	return &emptyResult;
      }
      int paramId = p->second;
      pResult->s_pData[param].s_trigger   = trigger;
      pResult->s_pData[param].s_parameter = paramId;
      pResult->s_pData[param].s_value     = value;
      param++;
    }
  }
  catch (...) {
    //Error handling is to free the parameters and 
    // then rethrow.
    //
    delete []pResult->s_pData;
    delete pResult;
    return &emptyResult;
  }
  return pResult;
}

