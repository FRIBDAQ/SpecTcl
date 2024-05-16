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
#include "CFirstofCommand.h"
#include "CFirstofEventProcessor.h"
#include "CBiggestOfEventProcessor.h" 


#include <TCLInterpreter.h>
#include <TCLInterpreterObject.h>
#include <Parameter.h>
#include <SpecTcl.h>
#include <tcl.h>

using namespace std;

//////////////////////////////////////////////////////////////////////////
// Constructors and other canonicals.


/*!
   Constructor registers the command with the interpreter
   \param interp - The interpreter on which the command will be registered.
   \param name   - The command name (defaults to "firstof" naturally).
*/
CFirstofCommand::CFirstofCommand(CTCLInterpreter& interp, string name) :
  CTCLObjectProcessor(interp, name)
{}

/*!
  Destructor .. the base class desctructor will remove us from the 
  interpreter.
*/
CFirstofCommand::~CFirstofCommand()
{}

//////////////////////////////////////////////////////////////////////////
// Command processor
/*!
   Process the firstof command. Minus error handling which 
   is just setting the result to a message and the usage before
   returning TCL_ERROR:

   - Ensure we have 4 parameters.
   - Decode the two target  parameters, these should not
     exist.
   - Ensure that the last parameter is a valid TCL list and
     that elements of the list are all names of existing parameters
     and get their parameter ids.
   - Create the target parameters .. get their ids.
   - Generate the Event processor and register it at the end
     of the current set of event processors.

*/
int
CFirstofCommand::operator()(CTCLInterpreter&     interp,
			    vector<CTCLObject>&  objv)
{
  string  command = objv[0];

  if (objv.size() != 4) {
    string result;
    result += "Incorrect number of command parameters\n";
    result += Usage(command);
    interp.setResult(result);
    return TCL_ERROR;
  }
  for (int i =0; i < objv.size(); i++) {
    objv[i].Bind(interp);
  }
  string valueParam = objv[1];
  string hitnumParam= objv[2];

  // The target parameters must be new.
  
  SpecTcl* pApi = SpecTcl::getInstance();
  CParameter* pValue = pApi->FindParameter(valueParam);
  CParameter* pHit   = pApi->FindParameter(hitnumParam);
  if (pValue || pHit) {
    string result;
    result += "The target parameters of firstof must not be";
    result += "defined\n";
    result += Usage(command);
    interp.setResult(result);
    return TCL_ERROR;
  }
  // The final parameter must be a valid Tcl list and each
  // element must be an existing parameter.
  //

  vector<CTCLObject>   paramList;
  vector<CParameter*>  sourceParameters;

  try {
    paramList = objv[3].getListElements();
  }
  catch (...) {
    string result;
    result  += "Final parameter of firstof must be a valid Tcl list\n";
    result  += Usage(command);
    interp.setResult(result);
    return TCL_ERROR;
  }
  for (int i=0; i < paramList.size(); i++) {
    CParameter* pParameter = pApi->FindParameter((string)(paramList[i]));
    if (!pParameter) {
      string result;
      result  += "The source parameters must already exist and ";
      result  += (string)(paramList[i]);
      result  += " does not\n";
      interp.setResult(result);
      return TCL_ERROR;
    }
    sourceParameters.push_back(pParameter);
  }
  // At this point everything must succeed.
  // Create the new output parameters, 
  // construct the new event processor, add it to the SpecTcl event processors
  // and return TCL_OK to signal success.
  // we leave the result empty.


  pValue = pApi->AddParameter(valueParam, 
			      pApi->AssignParameterId(), string(""));
  pHit   = pApi->AddParameter(hitnumParam,
			      pApi->AssignParameterId(), string(""));

  CEventProcessor* pProcessor;
  if (command == string("firstof")) {
    pProcessor   = new CFirstofEventProcessor(pValue, pHit, 
					       sourceParameters);
  }
  else if (command == string("biggestof")) {
    pProcessor = new CBiggestofEventProcessor(pValue, pHit, 
					      sourceParameters);
  }
  else {
    string result;
    result = "Unrecognized command keyword!";
    interp.setResult(result);
    return TCL_ERROR;
  }



  pApi->AddEventProcessor(*pProcessor);

  return TCL_OK;

}

/////////////////////////////////////////////////////////////////////
// Private utilities.

/*
  Return the command usage help string.
*/
string
CFirstofCommand::Usage(string commandName) const
{
  string usage;
  usage    += "Usage\n";
  usage    += "  ";
  usage    += commandName;
  usage    += " valueparam hitnumparam input-list\n";
  usage    += "valueparam  - output parameter with first value\n";
  usage    += "hitnumparam - output paramter with first index\n";
  usage    += "input-list  - Tcl List of input parameters.";

  return usage;
}
