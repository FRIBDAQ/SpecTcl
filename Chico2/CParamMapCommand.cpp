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
#include "CParamMapCommand.h"
#include <TCLObject.h>
#include <TCLInterpreter.h>
#include <Exception.h>

#include <tcl.h>
#include <SpecTcl.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// static data declarations:

CParamMapCommand::ParameterMap  CParamMapCommand::m_theMap;

/*!
   Create the command.. All the real work is done by the base class
   constructor which registers us on the interpreter.
*/
CParamMapCommand::CParamMapCommand(CTCLInterpreter& interp) :
  CTCLObjectProcessor(interp, "paramMap")
{}
/*!
  Destruction is also taken care of by the base class:

*/
CParamMapCommand::~CParamMapCommand()
{}

/*!
  Return a const reference to the parameter map so that the
  unpacker can figure out mappings.
*/
CParamMapCommand::ParameterMap&
CParamMapCommand::getMap()
{
  return m_theMap;
}
/*!
   Process the paramMap command.
*/
int
CParamMapCommand::operator()(CTCLInterpreter& interp,
			     vector<CTCLObject>& objv)
{
  // We need to have all the parameters (5 counting the command):

  if (objv.size() != 5) {
    string error = "paramMap - incorrect number of parameters.\n";
    error       += Usage();
    interp.setResult(error);
    return TCL_ERROR;
  }
  // Bind the parameters to the interpreter prior to doing any conversions.

  for (int i=1; i < 5; i++) {
    objv[i].Bind(interp);
  }
  string moduleName;
  int type;
  int slot;
  vector<CTCLObject> parameterList;

  try {
    moduleName = string(objv[1]);
    type       = objv[2];
    slot       = objv[3];
    parameterList = objv[4].getListElements();
  }
  catch(...) {
    string error = "Invalid parameter type\n";
    error       += Usage();
    interp.setResult(error);
    return TCL_ERROR;
  }
  // The only errors we know are that parameter names may not be defined.

  SpecTcl* api = SpecTcl::getInstance();
  AdcMapping mapping;
  mapping.name      = moduleName;
  mapping.vsn       = slot;
  mapping.type      = type;
  mapping.extraData = NULL;

  for (int i =0; i < parameterList.size(); i++) {
    parameterList[i].Bind(interp);
    string name = string(parameterList[i]);
    if (name != string("")) {
      CParameter* pParam  = api->FindParameter(name);
      if (!pParam) {
	string error = string(parameterList[i]);
	error       += " is not a defined parameter";
	error       += Usage();
	interp.setResult(error);
	return TCL_ERROR;
      }
      mapping.map[i] = pParam->getNumber();
    }
    else {
      mapping.map[i] = -1;
    }
  }
  // If we survived this far, we have a mapping.  create/replace the 
  // mapping for moduleName

  m_theMap[moduleName] = mapping;

  interp.setResult(moduleName);
  return TCL_OK;

}


/*
    Return a usage string.
*/
string
CParamMapCommand::Usage() 
{
  string result = "Usage: \n";
  result       += "   paramMap moduleName moduleType slot channels\n";
  result       += "     moduleName  - The name of a digitizer module\n";
  result       += "     moduleType  - The integer module type that selects an unpacker\n";
  result       += "     slot        - Virtual slot number (may no always be meaningful\n";
  result       += "     channels    - List of names of parameters for each channel\n";
    
  return result;
}
