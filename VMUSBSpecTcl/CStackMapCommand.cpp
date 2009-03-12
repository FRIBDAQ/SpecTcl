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

#include<config.h>
#include "CStackMapCommand.h"
#include "CParamMapCommand.h"

#include <TCLObject.h>
#include <TCLInterpreter.h>
#include <RangeError.h>

using namespace std;


CStackMapCommand::stackMap CStackMapCommand::m_stacks[8];
 
//////////////////////////////////////////////////////////////////////////////
/* Constructors and other canonical functions:


/*!
  Construction creates the command 
  \param interp - The interpreter on which the command will be registered.
  \param name   - The command name.
*/
CStackMapCommand::CStackMapCommand(CTCLInterpreter& interp,
		 string           name) :
  CTCLObjectProcessor(interp, name, true)
{}


/*!
   Destructor.. the base class does all the heavy lifting here too:
*/
CStackMapCommand::~CStackMapCommand()
{

}

/////////////////////////////////////////////////////////////////////////////
//   Static selectors.
/*!
   Return a reference to the stack map for a specific stack.  

  \return CStackMapCommand::stackMap
  \throw CRangeException - if number isn't valid.
*/
const  CStackMapCommand::stackMap&
CStackMapCommand::getMap(int number)
{
  if ((number < 0) || (number > 7)) {
    throw CRangeError(0, 7, number, 
		      "Retrieving a stack map");
  }

  return m_stacks[number];
}

////////////////////////////////////////////////////////////////////////
//   Virtual function overrides.

/*!
   Implement the command.  
  \param interp - The interpreter that is executing this command.
  \param objv   - The objects that represent the Tcl_Obj's that are the
                  command words.
   \return int
   \retval TCL_OK  - Success
   \retval TCL_ERROR - failure.
*/

int
CStackMapCommand::operator()(CTCLInterpreter& interp,
			     STD(vector)<CTCLObject>& objv)
{
  // We need exactly 3 command words including the command:

  if (objv.size() != 3) {
    string error = "Incorrect number of command parameters\n";
    error       += Usage();
    interp.setResult(error);
    return TCL_ERROR;
  }

  // Bind the objects so that the higher level function sin CTCLObject work:

  objv[1].Bind(interp);
  objv[2].Bind(interp);

  // Pull out the stack number and the list of modules:

  int                stackNumber;
  vector<CTCLObject> modules;
  stackMap           newMap;

  try {
    stackNumber = objv[1];
    modules     = objv[2].getListElements();
  }
  catch(...) {
    string error = "stackMap invalid parameter type\n";
    error       += Usage();
    interp.setResult(error);
    return TCL_ERROR;
  }
  // Validate the stack number:

  if ((stackNumber < 0) || (stackNumber > 7)) {
    string error = "stackMap - stack number must be in the range [0..7]\n";
    error       += Usage();
    interp.setResult(error);
    return TCL_ERROR;
  }
  // Now we can start building the stack map.  If there modules that don't have a map,
  // that's an error.


  CParamMapCommand::ParameterMap& parameterMap(CParamMapCommand::getMap());

  for (int i=0; i < modules.size(); i++) {
    modules[i].Bind(interp);
    string module = modules[i];

    // Ensure the parmeter has a map:

    CParamMapCommand::ParameterMapIterator p = parameterMap.find(module);
    if (p == parameterMap.end()) {
      string error = module;
      error       += " does not have a parameter map associated with it\n";
      error       += Usage();
      interp.setResult(error);
      clearMap(newMap);
      return TCL_ERROR;
    }

    // create a copy of the map and save a pointer to it:

    CParamMapCommand::AdcMapping* pModuleMap = new CParamMapCommand::AdcMapping(p->second);
    newMap.push_back(pModuleMap);

  }
  // The new map has been constructed and can replace the old map.

  clearMap(m_stacks[stackNumber]);
  m_stacks[stackNumber] = newMap;

  return TCL_OK;
}

///////////////////////////////////////////////////////////////////////////////
//    Utilities.

/*
   Return a usage string.
*/
string
CStackMapCommand::Usage()
{
  string usage = "Usage:\n";
  usage       += "  stackMap stackNumber modules\n";
  usage       += "Where:\n";
  usage       += " stackNumber is a VM-USB stack number\n";
  usage       += " modules     is a list of module names in the stack\n";
  usage       += "             in the order in which they are read\n";

  return usage;
}
/*
   Clear a map... delete the map copies for all elements in the map.
*/
void
CStackMapCommand::clearMap(stackMap& map)
{
  for (int i =0; i < map.size(); i++) {
    delete map[i];
  }
}
