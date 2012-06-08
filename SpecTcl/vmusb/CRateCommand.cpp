/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

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
#include "CRateCommand.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <SpecTcl.h>

#include "CRateList.h"
#include "CRateProcessor.h"

#include <stdint.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/*!
   Construct the rate command.  This is just constructing a command with the 
   name rate:

*/
CRateCommand::CRateCommand(CTCLInterpreter& interp) :
  CTCLObjectProcessor(interp, "rate")
{}
CRateCommand::~CRateCommand() {}

/*!
  Process the command.  In this case we just figure out the 
  subcommand and dispatch it:
  -   create   -to createRate
  -   delete   -to deleteRate
  -   list     -to listRates.

  \param interp : CTCLInterpreter&
      Interpreter that is executing this command.
  \param objv   : std::vector<CTCLObject>& objv
      Reference to the Tcl_Obj items that are the command words.
      Each Tcl_Obj* is encapsulated in a CTCLObject.

   \return int
   \retval TCL_OK    - Command completed ok.
   \retval TCL_ERROR - Command completed in error.

*/
int
CRateCommand::operator()(CTCLInterpreter& interp, vector<CTCLObject>& objv)
{
  // We must have at least a single parameter, the subcommand keyword.
  
  if (objv.size() < 2) {
    string result = "Need to have at least a subcommand keyword\n";
    result       +=  Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }
  // Branch out depending on the subcommand keyword:

  string subcommand = objv[1];

  if (subcommand == "create") {
    return createRate(interp, objv);
  }
  if (subcommand == "delete") {
    return deleteRate(interp, objv);
  }
  if (subcommand == "list") {
    return listRates(interp, objv);
  }
  string result = "Invalid command keyword: ";
  result       += subcommand;
  result       += "\n";
  result       += Usage();

  interp.setResult(result);
  return TCL_ERROR;
}

/*!
   create a rate object.  The rate is created, added to the 
   beginning of the event processing pipeline (so that update triggers
   are after it), and entered into the rate list.
   The syntax of the command is:
\verbatim
   rate create spectrum
\verbatim
   The spectrum must already exist.

   Parameters are the same as for operator() as is the return value.
   The result will either be empty for success or a descriptive error message.

*/
int
CRateCommand::createRate(CTCLInterpreter& interp, 
			 vector<CTCLObject>& objv)
{
  if (objv.size() != 3) {
    string result = "Incorrect number of parameters for rate create\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }

  string     spectrumName = objv[2];
  SpecTcl*   api          = SpecTcl::getInstance();
  CSpectrum *pSpec        = api->FindSpectrum(spectrumName);

  if(!pSpec) {
    string result = "Spectrum ";
    result       += spectrumName;
    result       += " does not exist in rate create command\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }
 
  // Now nothing is allowed to go wrong:
  // duplications are not fatal and our script front end will prevent them
  // in almost all cases.
  //

  // prepend to the pipeline.

  CRateProcessor* processor = new CRateProcessor(*pSpec);
  api->InsertEventProcessor(*processor,
			    api->ProcessingPipelineBegin(),
			    spectrumName.c_str());
  // Add to the rate list.

  CRateList* pList = CRateList::getInstance();
  pList->addProcessor(*processor);

  return TCL_OK;
 
}
/*!
   Delete a rate processor.  This 
   - removes the rate processor from the event processing pipeline
   - removes the rate processor from the rate list.
   - deletes the rate processor.

   syntax is:
\verbatim
  rate delete spectrum
\endverbatim

   no action is taken unless the event processor can be found in both the
   event pipeline and in the rate list..to prevent coincidental removal
   of event processors named erroneously.

   parameters are for operator() as is the resturn value.  The result
   is empty unless an error is detected in which case the error message
   is set as the result.
*/
int
CRateCommand::deleteRate(CTCLInterpreter& interp,
			 vector<CTCLObject>& objv)
{
  if (objv.size() != 3) {
    string result = "Incorrect number of parameters for rate delete\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }

  string spectrumName = objv[2];

  // The following must be true:
  // - This must be the name of an event processor that is a
  //   CRateProcessor.
  // - The rate processor must exist in the rate list.
  //

  SpecTcl* api = SpecTcl::getInstance();
  CTclAnalyzer::EventProcessorIterator i = api->FindEventProcessor(spectrumName);
  if (i == api->ProcessingPipelineEnd()) {
    string result = "Spectrum ";
    result       += spectrumName;
    result       += " does not have a rate event processor\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }
  CEventProcessor* pProcessor = i->second;
  CRateProcessor*  pRate      = dynamic_cast<CRateProcessor*>(pProcessor);
  if (!pRate) {
    string result = "While an event processor named ";
    result       +=  spectrumName;
    result       += " exists, it is not a rate processor";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }
  
  CRateList* pRates = CRateList::getInstance();
  CRateList::RateListIterator rle = pRates->find(*pRate);
  if (rle == pRates->end()) {
    string result = "While a rates processor named ";
    result       += spectrumName;
    result       += " exists, it is not in the rates list!!\n";
    interp.setResult(result);
    return TCL_ERROR;
  }

  // Now evertying has worked out:

  api->RemoveEventProcessor(i);
  pRates->deleteProcessor(*pRate);
  delete pRate;

  return TCL_OK;
}

/*!
   List the rates processors.
   The following information is listed for each rate (list of sublists)
   spectrum name.
   - Spectrum name
   - Delete pending flag (set by processor when the parameter was yanked out
     from underneath it).
   - Totals
   - Last increments.
*/
int
CRateCommand::listRates(CTCLInterpreter& interp,
			vector<CTCLObject>& objv)
{
  if (objv.size() != 2) {
    string result   = "Invalid number of arguments for rate list\n";
    result         += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }

  CRateList* pList = CRateList::getInstance();
  CRateList::RateListIterator i = pList->begin();
  CTCLObject result;
  result.Bind(interp);
  
  while (i != pList->end()) {
    CTCLObject element;
    element.Bind(interp);

    CSpectrum* pSpec = i->first->getSpectrum();
    element += pSpec->getName();
    element += (int)i->second;
    element += (double)i->first->getTotals();
    element += (double)i->first->getIncrements();

    result  += element;

    i++;
  }
  interp.setResult(result);
  return TCL_OK;
}


////// Create the usage string:

string
CRateCommand::Usage()
{
  string result;
  result  = "Usage\n";
  result += "  rate create spectrum-name\n";
  result += "  rate delete spectrum-name\n";
  result += "  rate list\n";

  
  return result;
}
