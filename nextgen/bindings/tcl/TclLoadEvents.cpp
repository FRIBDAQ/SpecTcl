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
#include "TclLoadEvents.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <spectcl_experiment.h>

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <string>
#ifdef bool
#undef bool			// messes with std::map
#endif
#include <map>
#include <algorithm>
#include <iostream>

using namespace std;



/**
 * Constructor.  Creates the command and registers it on the interprter.
 * @param interp - interpreter on which the command should be registered.
 */
CTclLoadEvents::CTclLoadEvents(CTCLInterpreter& interp) :
  CTclDBCommand(interp, "loadevents", true)
{}
/**
 ** Destructor:
 */
CTclLoadEvents::~CTclLoadEvents()
{
}


/**
 ** Gets control when the command is dispatched.   See the the header for the details of the command
 ** syntax.
 ** @param inter - interpreter running the command.
 ** @param objv  - Encapsulated array of command line words.
 ** @return int
 ** @retval TCL_OK - Correct comletion.. number of events loaded is the result.
 ** @retval TCL_ERROR error completion... error message is the result.
 */
int
CTclLoadEvents::operator()(CTCLInterpreter& interp,
			   std::vector<CTCLObject>& objv)
{
  try {
    if (objv.size() != 4) {
      throw std::string("::spectcl::loadevents - wrong number of command parameters");
    }
    // Ensure we have both handles:

    spectcl_experiment pExperiment = getDatabaseHandle(interp, objv, 1, "::spectcl::loadevents");
    spectcl_experiment pEvents     = getDatabaseHandle(interp, objv, 2, "::spectcl::loadevents");

    // That the handles are of the correct type and related:

    validateExpEvtHandles(pExperiment, pEvents);

    // start dealing with the event list

    std::string eventList = getParameter<std::string>(interp, objv, 3);
    CTCLObject events;
    events.Bind(interp);
    events = eventList;
   
    // If the events list is empty we short cut here:

    int numEvents = events.llength();
    if (numEvents == 0) {
      interp.setResult(0);
    }

    // Marshall the events into a pParameterData struct for submission to the loader
    // the call is a bit funny as it allows us to get the event count.
    // note that errors throw strings which is compatible with our structure.

    pParameterData  pData;
    int nParams = marshallEvents(pExperiment, events, &pData);
    spectcl_events_load(pEvents, nParams, pData);

    free(pData);
    interp.setResult(numEvents);
  }
  catch (std::string msg) {
    interp.setResult(msg);
    return TCL_ERROR;
  }
  return TCL_OK;
}
/* --------------------------- Utilities --------------------------*/

/**
 ** This functionm marshalls the Tcl list of events into a pParameterData
 ** object. This requires a few passes:
 ** - first pass determines how many parameter values will be inserted, as that's the
 **   insertion unit from the point of view of the API.
 ** - Second pass actually marshals the data into the dynamically allocated ParameterData array.
 ** Any errors result in a string throw which is caught to allow cleanup  of dynamic memory before 
 ** rethrow
 ** @param pExpermient - Experiment datababse.  
 ** @param events    - Tcl list representation of the events.
 ** @param ppData    - Pointer to a pParameterData to hold the resulting stuff (out).
 ** @return int 
 ** @retval number of events in the list.
 ** @throws std::string human readable error message.
 */
int
CTclLoadEvents::marshallEvents(void* pExperiment, CTCLObject& eventList, pParameterData* ppData)
{
  int nEvents = eventList.llength(); // Number of events.

  // First pass.. count up the parameters... each event is a list of the form
  // trigger {param value} ....
  // so the number of parameters supplied is llenght -1.

  int nParams = 0;
  for (int i =0; i < nEvents; i++) {
    CTCLObject event = eventList.lindex(i);
    nParams += event.llength() - 1;
  }

  // Allocate storage for this event.. note that a future refinement might be to 
  // allow storage allocation failure to bust the load up into several pieces but for now we'll
  // leave that to our invoking script.
  //

  pParameterData pOutput = reinterpret_cast<pParameterData>(malloc(nParams * sizeof(ParameterData)));
  if (!pOutput) {
    throw std::string((strerror(errno)));
  }
  *ppData = pOutput;

  // interlude: make a indexed by parameter name that provides the associated parameter id.


  parameter_list definedParameters = spectcl_parameter_list(pExperiment, "*");
  std::map<std::string, unsigned int> parameters;

  parameter_list p = definedParameters;
  while (*p) {
    pParameterInfo pInfo =*p;					\
    std::string sname(pInfo->s_pName);
    parameters[sname] = (int)pInfo->s_id;
    p++;
  }
  spectcl_free_parameter_list(definedParameters);

  // Pass 2 - Marshall each event into a set of parameters packaged with its trigger.
  //

  try {
    for (int i =0; i < nEvents; i++) {
      CTCLObject event    = eventList.lindex(i);
      int        len      = event.llength();
      CTCLObject trigger  = event.lindex(0);
      int triggerNo       = trigger;
      for (int param = 1; param < len; param++) {
	CTCLObject parameter = event.lindex(param);
	CTCLObject name      = parameter.lindex(0);
	CTCLObject value     = parameter.lindex(1);


	// Translate the name to an index:f

	std::map<std::string, unsigned int>::iterator pIndex = parameters.find(std::string(name));
	if (pIndex == parameters.end()) {
	  std::string msg = "Parameter: ";
	  msg            += std::string(name);
	  msg            += " is not defined";
	  throw msg;
	}
	unsigned int index = pIndex->second;
	pOutput->s_trigger   = (int)trigger;
	pOutput->s_parameter = index;
	pOutput->s_value     = value;
	pOutput++;
	
      }
    }
    return nParams;
  }
  catch (CException& e)	 {	// turn this into a string exception.
    std::string message = e.ReasonText();
    message            += " while: ";
    message            += e.WasDoing();
    free(pOutput);
    throw message;
  }
  catch (...) {
    free(pOutput);
    throw;			// rethrow
  }


}
