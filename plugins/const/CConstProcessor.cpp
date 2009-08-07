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
#include "CConstProcessor.h"
#include "CConstData.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <stdlib.h>



using namespace std;

///////////////////////////////////////////////////////////////////////////////

/*!
    Construction - the name of the command is hardwired to 'const' and the
    command is registered.

    @param interp - Reference to the interpreter on which the command will be
                    registered.
*/
CConstProcessor::CConstProcessor(CTCLInterpreter& interp) :
  CTCLObjectProcessor(interp, string("const"), true)
{}

/*!
   Destructor just passes up the inheritance chain as we have no object specific 
   dynamic data:
*/
CConstProcessor::~CConstProcessor()
{}

///////////////////////////////////////////////////////////////////////////////

/*!

    Dispatch the command to create, destroy or usage in the case of errors.
    - Commands must have at least 3 words, else usage() is called to get the
      error message which is then returned as the command value along with TCL_ERROR
    - If the second command word is -delete, control is passed to the destroy()
      method, else to the create() method.  These methods can set the interpreter
      result and the status which are passed back to the caller without further 
      interpretation.
    - Any exception processing must either be done in the detailed processors 
      or in the caller chain, as none is done at this level.

      @param interp   (CTCLInterpreter&)  Reference to the interpreter that is running
                      this command.
      @param objv     (vector<CTCLObject>&) Reference to a vector of encapsulated
                      Tcl_Obj's that make up the command.  objv[0] is the command
                      word ("const").
      @return int
      @retval TCL_OK    - The command executed without error.  The result is the
                          name of the new parameter.
      @retval TCL_ERROR - The command failed and the reason for the failure is
                          the interpreter result (error message).
*/
int
CConstProcessor::operator()(CTCLInterpreter& interp,
			    std::vector<CTCLObject>& objv)
{
  // Check for the minimum number of command words:

  if (objv.size() < 3) {
    string result = "Too few command parameters\n";
    result       += usage();
    interp.setResult(result);
    return TCL_ERROR;
  }

  // Bind all the command words to this interpreter enabling more advanced functionality
  // of each CTCTLObject:

  for(int i=0; i < objv.size(); i++) {
    objv[i].Bind(interp);
  }

  // Dispatch the command:

  string keyword(objv[1]);	// is this -delete?

  if (keyword == string("-delete")) {
    return destroy(interp, objv);
  }
  else {
    return create(interp, objv);
  }
}


//////////////////////////////////////////////////////////////////////////////

/*
 * Create a new variable.  
 * - There can be only 4 or 5 parameters.
 * - If there are 5 parameters, the second word must be "-and" and we will create an and
 *   processor.
 * - If there are 4 parameters we're creating an or processor.
 * - The outvalue must parse as a double.
 * - The call to the appropriate add function in CConstData must not throw
 *   an exception (the exception string will be turned into the result and result in a
 *   TCL_ERROR
 * Parameters:
 *    interp - Intepreter that's running this command.
 *    objv   - Array of encapsulated objects (Tcl_Obj) that define the command.
 * Return:
 *   TCL_OK  - A new parameter was created successfully.  Result string is the name
 *             of the parameter.
 *   TCL_ERROR- The parameter creation failed and the result string is the
 *             reason for the failure.
 */
int
CConstProcessor::create(CTCLInterpreter& interp, vector<CTCLObject>& objv) const
{
  // All errors are turned into string exceptions:

  try {
    size_t wordCount = objv.size();
    int    nameIndex = 1;
    bool   and(false);

    // Must be 4 or 5 words:

    if (wordCount != 5 && wordCount != 4) {
      throw string("Incorrect number of command paramters");
    }
    // If 5 first parameter must be '-and'.

    if (wordCount == 5) {
      if (objv[1] != string("-and")) {
	throw string("If there are 5 command words the second must be '-and'");
      }
      and = true;
      nameIndex = 2;
    }
    // at this point, and is true if we are doing an and and outparam is at
    // the index in nameIndex.  The rest is nice common code until the
    // actual attempt to create:

    string name(objv[nameIndex]);

    // validate the value:

    string value(objv[nameIndex+1]);
    char*  endPointer;
    double fValue = strotod(value.c_str(), &endPointer);
    if (endPointer == value.c_str()) {
      throw string("The value parameter is not evaluating to a floating point number");
    }
    // Pull the input params into a list of strings:

    try {
      vector<CTCLObject> inputParameters = objv[nameIndex + 2].getListElments();
    }
    catch (...) {
      throw string("The input parameters are not a correctly formatted TCL list");
    }

    vector<string> inputParameterNames;
    for( int i=0; i < inputParameters.size(); i++) {
      inputParameters[i].Bind(interp);
      inputParameterNames.push_back(string(inputParameters[i]));
    }
    
    CConstData& data(CConstData::getInstance());
    if (and) {
      data.addAndParameter(name, fValue, inputParameterNames);
    }
    else {
      data.addOrParameter(name, fValue, inputParameterNames);
    }

    // We survived so we can set the result and return TCL_OK.

    interp.setResult(name);
    return TCL_OK;

  }

  // All errors are turned into exceptions:

  catch(string msg) {
    mst += '\n';
    msg += usage();
    interp.setResult(msg);
    return TCL_ERROR;
  }
}


/*
 * Delete an existing variable. 
 * - There must be exactly three command words.
 * - The last command word is passed to the data manager's
 *   deleteParameter function
 * Parameters:
 *    interp - Intepreter that's running this command.
 *    objv   - Array of encapsulated objects (Tcl_Obj) that define the command.
 * Return:
 *   TCL_OK    - The parameter was deleted.  No result is set.
 *   TCL_ERROR - Parameter deletion failed and the result is an error message.
 */
int
CConstProcessor::destroy(CTCLInterpreter& interp, vector<CTCLObject>& objv) const
{
  // All errors get mapped to string exceptions:

  try {
    if (objv.size() != 3) {
      throw string("const -delete has the wrong number of command words");
    }
    string name = objv[2];
    CConstData& data(CConstData::getInstance());

    data.deleteParameter(name);
    
    return TCL_OK;

  }
  catch (string msg) {
    msg += "\n";
    msg += usage();
    interp.setResult(msg);
    return TCL_ERROR;
  }


}
/*
 *  Provides information about  how to use the const command:
 */
string
CConstProcessor::usage() const
{
  string result   = "Usage:\n";
  result         += "  const ?-and? outname outvalue [list innames]\n";
  result         += "  const -delete outname\n";
  result         += "Where:\n";
  result         += "  The first form creates a new parameter:\n";
  result         += "     -and if present indicates all input parameters must be defined\n";
  result         += "     outname is the name of the new parameter\n";
  result         += "     outvalue is the value the outname parameter will be given\n";
  result         += "     innames are the input parameter names.  If -and was not\n";
  result         += "     present, outname will be  assigned outvalue if any of the\n";
  result         += "     innames were assigned values in the event.\n";
  result         += "     If -and was present, all innames must have been assigned values\n";
  result         += "     for outname to be assigned outvalue\n";
  result         += " The second form deletes an existing const parameter named outname\n";
  result         += "      it is an error to attempt to use this to delete anything but a\n";
  result         += "      const param\n";
  result         += "WARNING:\n";
  result         += "  Deleting a const parameter via 'parameter -delete' is dangerous as\n";
  result         += "  the const event processor will continue to try to compute the parameter";

  return result;
}
