///////////////////////////////////////////////////////////
//  CTreeVariableCommand.h
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



#if !defined(__CTREEVARIABLECOMMAND_H)
#define __CTREEVARIABLECOMMAND_H

#ifndef __TCLPROCESSOR_H
#include <TCLProcessor.h>
#endif



#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

// Forward definitions:

class CTCLInterpreter;
class CTCLResult;
class CTreeVariableProperties;

/**
 * CTCLProcessor for the treevariable command.  The treevariable command is
 * an ensemble of subcommands that manipulate and inspect tree variables.
 * Tree variables, in turn, are C++ objects that encapsulate a hierachical
 * naming scheme for TCL Variables.  The concept is that these objects can
 * be embedded into C++ structures or classes where they provide a simulated data
 * structure that's actually a set of TCL variable values.
 *
 * The ensemble of commands includes:
 * - -list   Lists the characteristics of a matching set of tree variables.
 * - -set    Sets the value and properties of a tree variable.  Note that the
 *           TCL set command can basically do the same thing for the value,
 *           treevariables also have units and it's necessary to use this command
 *           to access the units field.
 * - -check  Determines if the variable has been modified since it was created.
 * - -setchanged - forces subsequent checks to return true.
 * - -firetraces - If the variable has been modified since construction or the last
 *                 firetraces, fires any pending traces that are set on the
 *                 variable.
 * @author Ron Fox
 * @version 1.0
 * @created 30-Mar-2005 11:03:53 AM
 */
class CTreeVariableCommand : public CTCLProcessor
{
  
public:
  virtual ~CTreeVariableCommand();
  CTreeVariableCommand(CTCLInterpreter* pInterp = 0);
  int operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
		 int argc, char** argv);
  
protected:
  friend class TreeVarCommandTest;	// For testability.
  int SetChanged(CTCLInterpreter& rInterp, CTCLResult& rResult, 
		 int argc, char** argv);
  int List(CTCLInterpreter& rInterp, CTCLResult& rResult, 
	   int argc, char** argv);
  int SetProperties(CTCLInterpreter& rInterp, CTCLResult& rResult, 
		    int argc, char** argv);
  int CheckChanged(CTCLInterpreter& rInterp, CTCLResult& rResult, 
		   int argc, char** argv);
  int FireTraces(CTCLInterpreter& rInterp, CTCLResult& rResult, 
		 int argc, char** argv);

  STD(string) FormatVariable(CTreeVariableProperties* pProperties);
  STD(string) Usage();
  
};




#endif
