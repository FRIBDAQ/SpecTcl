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

#include "config.h"
#include "CMapValueCommand.h"
#include "CMapValueProcessor.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <SpecTcl.h>
#include <TreeParameter.h>

using namespace std;

static unsigned int serialNumber(0);

/*!
   Contructs the command and registers it.
*/
CMapValueCommand::CMapValueCommand(CTCLInterpreter& interp) :
  CTCLObjectProcessor(interp, string("mapvalue")) {}

CMapValueCommand::~CMapValueCommand() {}

/*!
  The command processor.  There must be exactly three parameters 
  (in addition to the command).  The first one must be an existing 
  parameter. The second will be a new or existing parameter.
  The final parameter is a list of pairs that define the map.
 \param interp - The interpreter that will be executing the command
 \param objv   - The command words as a vector of CTCLObject wrappers around
                 Tcl_Obj's.
  \return int
  \retval TCL_OK    - Indicates success.
  \retval TCL_ERROR - Indicates some problem.

  \note on success, the name of the new event processor will be 
        returned as the result of the command.
  \note on failure, some textual description of the reason for 
        failure will be emitted.
*/
int
CMapValueCommand::operator()(CTCLInterpreter&         interp,
			     std::vector<CTCLObject>& objv)
{
  // Validate the number of parameters:

  if (objv.size() != 4) {
    string message = "Incorrect number of command line arguments\n";
    message       += usage();
    interp.setResult(message);
    return TCL_ERROR;
			 
  }

  // bind the parameters we care about to the interpreter:

  objv[1].Bind(interp);
  objv[2].Bind(interp);
  objv[3].Bind(interp);

  // Pull out the parameter names:

  string inName  = objv[1];
  string outName = objv[2];


  SpecTcl* pApi = SpecTcl::getInstance();

  // The input parameter must exist else this is an error:

  CParameter* pIn = pApi->FindParameter(inName);
  if (!pIn) {
    string message = "Input parameter: ";
    message       += inName;
    message       += " does not exist. \n";
    message       += usage();
    interp.setResult(message);
    return TCL_ERROR;
  }

  // If the output parameter does not exist, it must be created.
  // it will be created with an empty units string.  If you want something
  // else, just create the parameter first as desired.
  //
  CParameter* pOut = pApi->FindParameter(outName);
  if (!pOut) {
    pOut = pApi->AddParameter(outName,
			      pApi->AssignParameterId(),
			      string(""));
  }

  // Create the mapping map; If there's an exception, we just return
  // TCL_ERROR trusting in createMap to have set interp.result:
  // 

  std::map<int, float> valueMap;
  try {
    valueMap = createMap(interp, objv[3]);
  }
  catch(...) {
    return TCL_ERROR;
  }
  // Create and register the event processor.

  char processorName[100];
  sprintf(processorName, "ValueMap_%d", serialNumber++);
  CMapValueProcessor* pProcessor = new CMapValueProcessor(pIn->getNumber(),
							  pOut->getNumber(),
							  valueMap);
  pApi->AddEventProcessor(*pProcessor, processorName);

  interp.setResult(processorName);
  return TCL_OK;
}

/*
**  return the map that connects the input parameter value to the output parameter value.
** Parameters:
**   interp  - the interpreter running the command.
**   mapList - the object that has the list of mapping entries.
** Returns:
**   The constructed map.
** Exceptions:
**   Throws TCL_ERROR if there's an error of some sort.
**   interp.setResult will have been called to set the appropriate error message.
**
*/

map<int, float>
CMapValueCommand::createMap(CTCLInterpreter& interp,
			    CTCLObject&      mapList)
{
  map<int, float>    theMap;

  vector<CTCLObject> theList;
  try {
    theList = mapList.getListElements();

    // Bind each element of the list to the interpreter:
    
    for (int i=0; i < theList.size(); i++) {
    theList[i].Bind(interp);
    }
    // Each list element must have a 
    for (int i=0; i < theList.size(); i++) {
      if (theList[i].llength() != 2) {
	throw TCL_ERROR;
      }
      else {
	int inValue    = theList[i].lindex(0);
	float outValue = (double)(theList[i].lindex(1));
	theMap[inValue] = outValue;
      }
    }
  }
  catch (...) {
    string message = "The mapping list is not a valid Tcl List of integer/real pairs.\n";
    message       += usage();
    interp.setResult(message);
    throw TCL_ERROR;
  }
  
  return theMap;
}



/*
** return a string that provides the comman usage:
*/
string
CMapValueCommand::usage()
{
  string message = "Usage:\n";
  message       += "  mapvalue inName outName map\n";
  message       += "Where:\n";
  message       += "   inName  - Is the name of the existing input parameter\n";
  message       += "   outName - Is the name of the output parameter (need not exist)\n";
  message       += "   map     - Is the listified map {{in1 out1} {in2 out2}....}\n";
  return message;

}
