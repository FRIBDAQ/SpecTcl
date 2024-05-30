///////////////////////////////////////////////////////////
//  CTreeVariableCommand.cpp
//  Implementation of the Class CTreeVariableCommand
//  Created on:      30-Mar-2005 11:03:53 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////
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
// This implementation of TreeParameter is based on the ideas and original code of::
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//

#include <config.h>
#include "CTreeVariableCommand.h"
#include "CTreeVariable.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <TCLString.h>
#include <SpecTcl.h>
#include <Globals.h>
#include <TclPump.h>
#include "CTreeVariableProperties.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


CTreeVariableCommandActual::~CTreeVariableCommandActual()
{

}


/**
 * Constructs/registers the treevariable command with SpecTcl's interpreter.  If
 * the interpreter pointer is NULL, then it is gotten from the SpecTcl API.
 * @param pInterp
 *        The interpreter on which the "treevariable" command will be
 *        installed.
 * 
 */
CTreeVariableCommandActual::CTreeVariableCommandActual(CTCLInterpreter* pInterp) :
  CTCLObjectProcessor(*pInterp, "treevariable", false)
{
  // Allow a default interpreter to be the SpecTcl interpreter.

  if(!pInterp) {
    SpecTcl& api(*(SpecTcl::getInstance()));
    Bind(*(api.getInterpreter()));
  }
  Register();


}


/**
 * Implements an ensemble of commands for "treevariable"  The ensemble dispatches
 * as follows:
 * - -list   - List
 * - -set  - SetProperties
 * - -check - CheckChanged Determines if the item has changed definition since
 * last time.
 * - -setchanged - mark the object as modified.
 * - -firetraces - FireTraces Fires any pending traces.
 * 
 * Any other subcommands are errors and result in a message, a Usage string and an
 * error return.
 *
 * @param rInterp
 *        Interpreter that is executing this command.
 * @param objv - object encapsulated command words.
 *
 * \return int
 * \retval TCL_OK   - The function completed normally.
 * \retval TCL_ERROR - some error occured.
 * 
 */
int 
CTreeVariableCommandActual::operator()(
  CTCLInterpreter& rInterp, 
  std::vector<CTCLObject>& objv)
{
  // Marshall objv -> argc, argv to make port simpler.

  std::vector<std::string> words;
  std::vector<const char*> pWords;

  int argc = objv.size();
  for (auto& word : objv) {
    words.push_back(std::string(word));
  }
  for (auto& word : words) {
    pWords.push_back(word.c_str());
  }
  auto argv = pWords.data();

  // There must be a subcommand:
  
  argc--;
  argv++;
  std::string rResult;
  if(!argc) {
    
    rResult = "Missing the treevariable subcommand.\n";
    rResult += Usage();
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }
  
  // Pull out the ensemble subcommand.
  
  string subcommand(*argv);
  argv++;
  argc--;
  
  // Dispatch based on the subcommand or error if there's no matching subcommand.
  
  
  if(subcommand == "-list") {
    if (gMPIParallel && (myRank() != MPI_ROOT_RANK)) {
      return TCL_OK;                // Let root rank take care of it
    }
    // Serial or MPI but root rank.
    return List(rInterp, argc, argv);
  }
  else if (subcommand == "-set") {
    return SetProperties(rInterp, argc, argv);
  }
  else if (subcommand == "-check") {
    return CheckChanged(rInterp, argc, argv);
  }
  else if (subcommand == "-setchanged") {
    return SetChanged(rInterp, argc, argv);
  }
  else if (subcommand == "-firetraces") {
    return FireTraces(rInterp, argc, argv);
  }
  else {
    rResult   = "Unrecognized subcommand";
    rResult += subcommand;
    rResult += "\n";
    rResult += Usage();
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }
  
  
}


/**
 * Processes the -list subcommand of the treevariable ensemble.   lists all tree
 * variables that match the pattern.  Note that if no pattern is given it is
 * assumed to be * which matches all variable names.  For each matching variable a
 * list element is appended to the result. The list element is itself a list
 * containing:
 * - The treevariable name,
 * - The value of the treevariable
 * - The units of the treevariable.
 * @param rInterp
 *        The interpreter on which this command is executing.
 * @param argc
 *        Number of command line parameters that are remaining in the
 *        command line.  This will be either empty or a glob pattern that specifies 
 *        which tree parameters to list.  For example  s800.fp.crdc1
 *        will list all tree parameters whose names  start with that string.
 * @param argv
 *        Array of pointers to the command line parameters.
 * 
 */
int 
CTreeVariableCommandActual::List(CTCLInterpreter& rInterp,
			   int argc, const char** argv)
{
  std::string rResult;
  
  // Figure out the search pattern and whether or not there are any errors.
  
  string pattern("*");               // Default pattern lists everythining.
  
  if(argc) {
    pattern = argv[0];
    argc--;
    argv++;
  }
  
  if(argc) {
    rResult  = "Extra parameters on command: treevariable -list ";
    rResult += pattern;
    rResult += " ";
    rResult += argv[0];
    rResult += " ...\n";
    rResult += Usage();
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }

  // Now visit each node and add those that match to the list.
  // Since we're only doing one visitation in this class, we don't bother
  // with the extra work of building a visitor object and just iterate here:
  //
  CTCLObject result;
  result.Bind(rInterp);
  map<string, CTreeVariableProperties*>::iterator i = CTreeVariable::begin();
  while(i != CTreeVariable::end()) {
      if(Tcl_StringMatch(i->first.c_str(),pattern.c_str())) {
	result += (FormatVariable(i->second));
      }
      
      i++;
  }
  rInterp.setResult(result);
  return TCL_OK;
  
  
}


/**
 * Processes the -set subcommand of the treevariable ensemble. Set the properties
 * of the associated treevariable.  Expects the following command line elements:
 * - name of the tree parameter
 * - Value of the parameter
 * - Unit [optional] - if omitted, units will not be modified.
 * 
 * @param rInterp
 *        Interpreter object representing the Tcl/Tk interpreter that
 *        is executing this command.
 * @param rResult
 *        The result object that will contain the string this command
 *        returns to the invoking script.
 * @param argc
 *        The count of remaining command line arguments.  These are
 *        expected to be:
 *        - A single tree variable name that is to be modified.
 *        - The new value to give the parameter
 *        - The new units string to associate with the paramter.
 * 
 * @param argv
 *        Array of pointers to the command line parameters.
 * 
 */
int 
CTreeVariableCommandActual::SetProperties(CTCLInterpreter& rInterp,
				    int argc, const char** argv)
{
  std::string rResult;

  // Check the number of parameters is ok...
  
  if (argc < 2) {
    rResult = "Incorrect number of command line parameters in treevariable -set\n";
    rResult += Usage();
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }

  // Now pull the parameters out of the line and, where necessary, parse them  
  // as expressions.
  
  string name(argv[0]);
  double value;
  
  try {
    value = rInterp.ExprDouble(argv[1]);
  }
  catch(...) {
     rResult = "Failed to parse value for treevariable -set needed a double and got : ";
     rResult += argv[1];
     rResult += "\n";
     rResult += Usage();
     rInterp.setResult(rResult);
     return TCL_ERROR;
  }
  // Get the units ("" if not supplied).

  string units("");
  argv += 2;
  argc -= 2;
  if(argc) {
    units = *argv;
    argc--;
    argv++;
  }
  if(argc) {
    rResult = "Incorrect number of command line parameters in treevariable -set\n";
    rResult += Usage();
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }



  //   Locate the variable to modify:
  
  map<string, CTreeVariableProperties*>::iterator i = CTreeVariable::find(name);
  if (i == CTreeVariable::end()) {
    rResult = "treevariable -set : unable to find variable : ";
    rResult += name;
    rResult += "\n";
    rResult += Usage();
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }
  //   Now modify the property object associated with that name:
  
  CTreeVariableProperties* pProps = i->second;
  (*pProps)  = value;
  pProps->setUnits(units);
  pProps->fireTraces();
  rInterp.setResult(rResult);   // Probalby an empty string.
  return TCL_OK;
  
}


/**
 * processess the -check subcommand of the ensemble.  Sets the result to nonzero
 * if the variable has changed definition or value.
 * @param rInterp
 *        The interpreter object on which this command is executing.
 * @param rResult
 *        The result string that is supposed to hold the result of the
 *        command.
 * @param argc
 *        Number of command line parameters that remain.  This should just
 *        be a single parameter name.
 * @param argv
 *        Pointers to the remaining commandl ine parameters.
 * 
 */
int 
CTreeVariableCommandActual::CheckChanged(CTCLInterpreter& rInterp,
				   int argc, const char** argv)
{
  // Check for the right number of  parameters.

  std::string rResult;
  if (argc != 1) {
    rResult  = "Incorrect number of arguments in treevariable -check command\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  // Pull out the name and find the variable's property object.
  
  string name(argv[0]);
  map<string, CTreeVariableProperties*>::iterator i = CTreeVariable::find(name);
  if(i == CTreeVariable::end()) {
    rResult = "treevariable -check :  unable to find variable: ";
    rResult += argv[0];
    rResult += "\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  // Figure out what we are going to return:
  
  CTreeVariableProperties* p = i->second;
  if(p->definitionChanged()) {
    rResult = "1";
  }
  else {
    rResult = "0";
  }
  rInterp.setResult(rResult);
  return TCL_OK;
}


/**
 * @param rInterp
 *        Reference to the interpreter that is running this command.
 * @param rResult
 *        The result string of the command.
 * @param argc
 *        Number of remaning parameters... this is 1: the name of the
 *        variable to set as modified.
 * @param argv
 *        Pointer to command line parameters.
 * 
 */
int 
CTreeVariableCommandActual::SetChanged(CTCLInterpreter& rInterp,
				 int argc, const char** argv)
{

  //  Check the number of command line parameters:
  std::string rResult;
  if (argc != 1) {
    rResult = "treevariable -setchanged - incorrect number of command line parameters\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  
  //  Pick out the name and locate the tree variable.
  
  string name(argv[0]);
  map<string, CTreeVariableProperties*>::iterator i = CTreeVariable::find(name);
  
  if(i  == CTreeVariable::end()) {
    rResult = "treevariable -setchanged ";
    rResult += name;
    rResult += " : variable not found\n";
    rResult += Usage();
    return TCL_ERROR;
  }
  
  // We set the changed flag by setting the variable to itself:
  //
  CTreeVariableProperties* p = i->second;
  (*p) = (double)(*p);
  
  return TCL_OK;
}


/**
 * For all matching tree parameters (pattern defaults to *), fires any pending
 * traces.
 * @param rInterp
 *        Reference to the interpreter that is executing this command.
 * @param rResult
 *        The result string that must be filled in by this command to
 *        return a result to the caller.
 * @param argc
 *        Number of command line parameters.  This can be
 *        - 0 Pending traces on all known tree variables are fired.
 *        - 1 there is a pattern on the command line and only those variables whose name
 *          matches that pattern under globbing rules will have their traces fired.
 * @param argv
 *        Array of pointers to command parameters.
 * 
 */
int 
CTreeVariableCommandActual::FireTraces(CTCLInterpreter& rInterp, 
				 int argc, const char** argv)
{

  // Figure out what the name match pattern should be
  // and if there an appropriate number of command line parameters.
  
  string pattern("*");       // The default pattern * matches all.
  std::string rResult;
  if(argc) {
    pattern = argv[0];
    argc--;
    argv++;
  }

  if(argc) {
    rResult   = "Incorrect number of command line parameters on command";
    rResult  += "treevariable -firetraces";
    rResult  += pattern;
    rResult  += "\n";
    rResult  += Usage();
    rInterp.setResult(rResult);
    return TCL_ERROR;
  }
 
  // Now we can iterate through the variables firing traces on matching names.
  
  map<string, CTreeVariableProperties*>::iterator i = CTreeVariable::begin();
  while (i != CTreeVariable::end()) {
    
    if( Tcl_StringMatch(i->first.c_str(), pattern.c_str())) {
      CTreeVariableProperties* p  = i->second;
      p->fireTraces();
    }
    
    i++;
  }
  
  // Return ok:
  rInterp.setResult(rResult);   // probably empty.
  return TCL_OK;
}


/**
 * Provide a command usage string that can be appended to any error messages
 * produced by the command executors.
 */
string 
CTreeVariableCommandActual::Usage()
{

  string result;
  
  result   = "Usage\n";
  result += "    treevariable -list ?pattern?\n";
  result += "    treevariable -set name value ?units?\n";
  result += "    treevariable -check name\n";
  result += "    treevariable -setchanged name\n";
  result += "    treevariable -firetraces ?pattern?\n";
  
  return result;

}


/**
 * Returns the formatted version of a parameter.
 * @param pProperties
 *        Pointer to the properties of the variable we will list.
 * 
 */
string 
CTreeVariableCommandActual::FormatVariable(CTreeVariableProperties* pProperties)
{
  
  char        buffer[100];
  string      result;
  CTCLString  answerlist;
  
  answerlist.AppendElement(pProperties->getName());
  
  snprintf(buffer, sizeof(buffer), "%g",(double)(*pProperties));
  answerlist.AppendElement(buffer);
  
  answerlist.AppendElement(pProperties->getUnits());
  
  return (const char*)(answerlist);
  
  
}



/// The MPI wrapper class implementation:
// There is a bit of wonkiness since for reasons I can't fathom,
// CTreeVariableCommand(Actual) allows pInterp ==nullptr.

CTreeVariableCommand::CTreeVariableCommand(CTCLInterpreter* pInterp) :
  CMPITclCommandAll(
    pInterp ? *pInterp : *SpecTcl::getInstance()->getInterpreter(),
     "treevariable", 
     new CTreeVariableCommandActual(pInterp))
{
}