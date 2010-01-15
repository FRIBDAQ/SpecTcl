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
#include "ParamMapCommand.h"
#include <TCLInterpreter.h>
#include <TreeParameter.h>
#include <TCLObject.h>

using namespace std;

// Static member data:

CParamMapCommand*   CParamMapCommand::m_pTheInstance(0);

/////////////////////////////////////////////////////////////////////////////
/*
   Constructor is private.  It is really just an ordinary
   constructor for a command 
*/
CParamMapCommand::CParamMapCommand(CTCLInterpreter& interp, string command) :
  CTCLObjectProcessor(interp, command)
{
}
/*!
   The destructor is implemented and it just nulls out the instance pointer.
   The vectors are member data and will therefore take care of themselves.
*/
CParamMapCommand::~CParamMapCommand()
{
  m_pTheInstance = reinterpret_cast<CParamMapCommand*>(0);
}


/////////////////////////////////////////////////////////////////////////////

/*!
  The create member is the way the singleton instance is created.  If it already
  exists a string exception is thrown.  We do this sort of odd thing so that
  we can deal with the parameterization of the constructor.
  \param interp - the interpreter on which the command will be defined.
  \param command - the command string (defaults to "parammap").

*/
void
CParamMapCommand::create(CTCLInterpreter& interp, string command)
{
  if (m_pTheInstance) {
    throw string("Attempt to create the CParamMapCommand singelton when it already exists");
  }
  else {
    m_pTheInstance = new CParamMapCommand(interp, command);
  }
}
/*!
   Return a pointer to the instance of the singleton. Note that if the
   object has not yet been created, null is returned.

   \return CParamMapCommand*
   \retval 0  - The singleton has not been created.
   \retval other - pointer to the singletong
*/
CParamMapCommand*
CParamMapCommand::getInstance()
{
  return m_pTheInstance;
}
////////////////////////////////////////////////////////////////////////////


/*!
   Process the command.  This top level processor is just a dispatcher.  We will
   check that there is a switch, and if it's known dispatch to the appropriate
   member that knows how to do the requested action.  If there are not enough
   parameter or if the switch is not known, a usage message is set in the result
   and the command errors.  Otherwise, we are at the mercy of the specific processor
   as to the command value and return value.

   \param interp - The interpreter executing this command.
   \param objv   - The vector of objects that make up the command words.

   \return int
   \retval TCL_OK - The command executed properly.
   \retval TCL_ERROR - The command detected an error.

   \note The interpreter result will be set with some string that depends on the
         success or failure of the command and the details of the command itself.
	 For more information, see the descriptions of the actual detailed processors.
*/
int
CParamMapCommand::operator()(CTCLInterpreter& interp,
			     std::vector<CTCLObject>& objv)
{
  // Validate the number of command parameters:

  if (objv.size() < 2) {
    string result = "Incorrect parameter count: \n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }
  // Do a favor to all the processors, bind the command word objects.

  for (int i=0; i < objv.size(); i++) {
    objv[i].Bind(interp);
  }

  // Extract the switch and dispatch:


  string action(objv[1]);


  if (action == string("-add")) {
    return add(interp, objv);
  }
  else if (action == string("-delete")) {
    return remove(interp, objv);                  // sorry but delete is a reserved keyword.
  }
  else if (action == string("-list")) {
    return list(interp, objv);
  }
  else {
    string result = "Unrecognized command action: ";
    result       += action;
    result       += "\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }
  // should not get here.

}
/*!
   Processor that adds a parameter map to the current map set.
   If a module already has a parameter map, it is replaced.
   Syntax:
\verbatim

parammap -add module-number module-type [list name1 ...]

   module-number   - The module number starting from zero 
   name1 ..        - Names of the parameters.  

\endverbatim

  Each non-blank parameter name has a tree parameter created in its honor.
  The id of the parameter is saved in a vector of parameters for the module.
  -1 is used for parameters that are not defined (e.g. blank).  The size of the
  vector indicates the last parameter name provided.

  If there is already a map for this module, it is silently ovewritten, but the
  prior parameters are not deleted.

  \param interp - the interpreter that is running this command
  \param objv   - The objects containing the words that make up the command.
  \return int
  \retval TCL_OK - The map was parsed and created.
  \retval TCL_ERROR - Some sort of error occured.

  \note  On success the result will be empty.  On failure it will contain an error message and 
         helpful usage text.
*/
int
CParamMapCommand::add(CTCLInterpreter& interp, 
		      vector<CTCLObject>& objv)
{
  // need parammap -add number type id parameters 
  // or 6 parameters:

  if (objv.size() != 6) {
    string result = "Incorrect number of parameters for an -add subcommand: \n";
    result      += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }
  
  // Note in the stuff below that objv has already been bound.

  int moduleNumber;
  try {
    moduleNumber = objv[2];	    // Throws if this is not an integer.
    if (moduleNumber < 0) throw 0; // To get to common error code in the catch block.
  }
  catch (...) {
    string result = "Module number parameter for -add was : ";
    result       += (string)(objv[2]);
    result       += " must be a positive integer\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }

  int moduleType;
  try {
    moduleType = objv[3];         // Throws if this is not an integer.
    if (moduleType < 0) throw 0; // To get to common error code in the catch block.
  }
  catch (...) {
    string result = "Module type parameter for -add was : ";
    result       += (string)(objv[3]);
    result       += " must be a positive integer\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }
 
  int moduleId;

  try {
    moduleId = objv[4];         // Throws if this is not an integer.
    if (moduleId < 0) throw 0;  // To get to common error handling code if value no good.
  }
  catch (...) {
    string result = "Module type parameter for -add was : ";
    result       += (string)(objv[4]);
    result       += " must be a positive integer\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }	

  // Get the list of parameter names now:

  vector<CTCLObject> parameterList;
  try {
    parameterList = objv[5].getListElements();
  }
  catch(...) {
    string result;
    result   += "Module parameters are not a valid list: ";
    result   += string(objv[5]);
    result   += "\n";
    result   += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }

  // Now loop through the parameters.  For each non blank parameter name, create
  // a tree parameter and bind it (yes we leak tree parameters).
  // Add either the parameter's id or -1 if the parameter was "" to the parameter id
  // vector.

  ParameterMap  map;
  
  map.s_moduleType = moduleType;
  map.s_id         = moduleId;
  
  for (int i=0; i < parameterList.size(); i++) {
    string name = parameterList[i]; // The parameter name.
    if (name != string("")) {
      CTreeParameter* pParam = new CTreeParameter(name, 4096, 0.0, 4095.0, string("channels"));
      pParam->Bind();		// Bind to a spectcl parameter, creating if needed.
      map.s_parameterIds.push_back(pParam->getId());
    }
    else {
      map.s_parameterIds.push_back(-1);	// no parameter.
    }

  }


  // Now add the parameter id vector to the map...add elements to the map as needed:

  while(moduleNumber >= m_modules.size()) {
    ParameterMap empty;
    m_modules.push_back(empty);
  }
  m_modules[moduleNumber] = map;

  // Done..

  return TCL_OK;
	    

}
/*!
   Remove a mapping for a module.  Removal is all or nothing.  The parameters are not
   destroyed, just the mappings.
   Syntax:
\verbatim

parammap -delete module_number

\endverbatim

   It is an error to attempt to delete a nonexistent mapping.

   \param interp - Interpreter running this command.
   \param obvj   - Vector of objects that make up the command words.
   \return int
   \retval TCL_OK
   \retval TCL_ERROR

   \note The result is empty in the event of success, and is a helpful error message in the
         event of an error message.
*/
int
CParamMapCommand::remove(CTCLInterpreter& interp, vector<CTCLObject>& objv)
{
  // Ensure the right number of parameters are supplied:

  if (objv.size() != 3) {
    string result = "Incorrect number of parameters\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }
  // Decode the module number... note that all objv elements have already been bound.
  // by the caller:

  int moduleNumber;
  try {
    moduleNumber = objv[2];
    if(moduleNumber < 0) throw 0; // Common error code is in the catch block.
  }
  catch(...) {
    string result = "Invalid module number: ";
    result       += (string)(objv[2]);
    result       += " must be a positive integer\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }
  // If we don't have this module that's an error too:

//  if((moduleNumber < m_modules.size()) && (m_modules[moduleNumber].size())) {
  if((moduleNumber < m_modules.size()) && (m_modules[moduleNumber].s_parameterIds.size())) {
    ParameterMap empty;
    m_modules[moduleNumber] = empty; // clear the map
  }
  else {
    string result = "Attempted to delete parameter map for module ";
    result       += (string)(objv[2]);
    result       += ": module does not exist\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }
  // If we get here, everything worked.

  return TCL_OK;
}

/*!
   List the module maps.  The module maps are listed as a list of pairs.  Each pair consists
   of the module number followed by the module's map as list of integer parameter ids.
   Syntax:

\verbatim
parammap -lsit ?module-number?
\endverbatim

   If module-number  is supplied, only the list for that parameter is returned.
   The description lists are returned in the interpreter result.

   It is an error to provide a module number that does not have a map or has an empty map.
   In the full listing, empty maps are suppressed.

   \param interp - The interpreter running this command.
   \param objv   - Vector of command word objects.
   \return int
   \retval TCL_ERROR -  error of some sort.
   \retval TCL_Ok    -  correct return.

   \note On success, the interpreter result is the module map description as defined above.
         On faiulre, the interpreter result will contain an error message with usage help.

*/
int
CParamMapCommand::list(CTCLInterpreter& interp,
		       vector<CTCLObject>& objv)
{
  // Ensure we have the right number of parameters:

  if ((objv.size() != 2) && (objv.size() != 3)) {
    string result = "Incorrect number of command parameters:\n";
    result       += Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }
  // We're going to iterate over the map. The only question is
  // if the supplied module number will constrain the iteration to
  // a single element ;-).
 

  int first  = 0;
  int end    = m_modules.size(); //  These are the values if no module-number is supplied.

  if (objv.size() == 3) {
    int moduleNumber;
    try {
      moduleNumber = objv[2];
      if (moduleNumber < 0) throw 0;
      if (!(moduleNumber < m_modules.size())) throw 1;
  //    if (m_modules[moduleNumber].size() == 0) throw 2;
      if (m_modules[moduleNumber].s_parameterIds.size() == 0) throw 2;

      // Ok so we have a non empty map for moduleNumber:
      
      first = moduleNumber;
      end   = moduleNumber+1;	// So the listing loop will only 'iterate' over that module.
    } 
    catch (...) {
      string result = "Module number ";
      result       += (string)(objv[2]);
      result       += " must be a positive integer for which a map exists\n";
      result       += Usage();
      interp.setResult(result);
      return TCL_ERROR;
    }
  }

  // Generate the list:

  CTCLObject result;		//  The list result
  result.Bind(getInterpreter());

  for (int i = first; i < end; i++) {
  //  if (m_modules[i].size()) {
    if (m_modules[i].s_parameterIds.size()) {

      CTCLObject item;
      item.Bind(getInterpreter());

      item   += i;
      item   += listModule(i);
      result += item;
    }
  }
  interp.setResult(result);
  return TCL_OK;
}
//////////////////////////////////////////////////////////////////////////////////////////
/*!
    Get a module map for a specific module number.  
    This is usually called when analyzing data.

    \param moduleNumber - the module number we want a map for.
    \return std::vector<int>*
    \retval NULL        - There is no map (or the map is empty).
    \retval other       - Pointer to the map.
*/

CParamMapCommand::ParameterMap*
CParamMapCommand::getModuleMap(unsigned int moduleNumber)
{
  if (moduleNumber < m_modules.size()) {
    if (m_modules[moduleNumber].s_parameterIds.size()) {
      return &(m_modules[moduleNumber]);
    }
    else {
      return reinterpret_cast<ParameterMap*>(0);
    }
  }
  else {
    return reinterpret_cast<ParameterMap*>(0);

  }
}

size_t
CParamMapCommand::getMapSize() const
{
  return m_modules.size();
}
///////////////////////////////////////////////////////////////////////////////////////////
/*
   Creates a list that describes a module. The module entry exist and has a list size >0.
   We are going to return a CTCLObject that contains a list made up of the module number,
   and a list of the map entries that are defined. 

   Parameters:
      moduleNumber - The number of the module we will describe

*/
CTCLObject 
CParamMapCommand::listModule(unsigned int moduleNumber)
{
  CTCLObject result;
  result.Bind(getInterpreter());
  result += (int)(moduleNumber);	// First list element is the module number.

  CTCLObject map;
  map.Bind(getInterpreter());
//  for (int i=0; i < m_modules[moduleNumber].size(); i++) {
  for (int i=0; i < m_modules[moduleNumber].s_parameterIds.size(); i++) {
//    map += (int)(m_modules[moduleNumber][i]);
    map += (int)(m_modules[moduleNumber].s_parameterIds[i]);
  }
  result += map;
  return map;
}
//////////////////////////////////////////////////////////////////////////////////////////
/*
    Return a string that describes the usage of this command.

*/
string
CParamMapCommand::Usage() const
{
  string result;
  string command = getName();

  result  = "Usage:\n";

  result += "    ";
  result += command;
  result += " -add module_number module_type module-id [list name1 ... ]\n";

  result += "    ";
  result += command;
  result += " -delete module_number\n";

  result += "    ";
  result += command;
  result += " -list ?module_number\n";


  return result;
}
 
