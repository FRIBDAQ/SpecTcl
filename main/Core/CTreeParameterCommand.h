///////////////////////////////////////////////////////////
//  CTreeParameterCommand.h
//  Implementation of the Class CTreeParameterCommand
//  Created on:      30-Mar-2005 11:03:51 AM
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


#ifndef CTREEPARAMETERCOMMAND_H
#define CTREEPARAMETERCOMMAND_H

#include <TCLProcessor.h>
#include <string>
#include <map>

// Forward Definitions:

class CTCLInterpreter;
class CTCLResult;


/**
 * Provides a command that interrogates the tree parameter set.  This only
 * operates on tree parameters. Tree-arrays are just not seen by it.   The command
 * "treeparameter" is an ensemble of the following subcommands:
 * - -list     - Lists the characteristics of matching tree parameters.
 * - -set      - Sets all properities of matching tree parameter(s)
 * - -setinc   - Sets the channel width of a matching tree parameter(s)
 * - -setbins  - Sets the default channel count of matching tree parameter(s).
 * - -setunit  - For now a no-op -- claims to set the units on a parameter.
 * - -setlimits - Sets the default channel limits on a parameter.
 * - -check     - true if the tree parameter has changed definition.
 * - -uncheck   - unsets the parameter definition change flag.
 * - -version   - returns the version number string of the package.
 * @author Ron Fox
 * @version 1.0
 * @created 30-Mar-2005 11:03:51 AM
 */
class CTreeParameterCommand : public CTCLProcessor
{
private:
  static std::map<std::string, std::string> m_createdParameters;

public:
  virtual ~CTreeParameterCommand();
  CTreeParameterCommand(CTCLInterpreter* pInterp = 0);
  virtual int operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			 int argc, char** argv);
protected:
  friend class TreeCommandTest;
  std::string Usage();
  int List(CTCLInterpreter& rInterp, CTCLResult& rResult, 
	   char argc, char** argv);
  int SetDefinition(CTCLInterpreter& rInterp, CTCLResult& rResult, 
		    int argc, char** argv);
  int SetIncrement(CTCLInterpreter& rInterpreter, CTCLResult& rResult, 
		   int argc, char** argv);
  int SetChannelCount(CTCLInterpreter& rInterp, CTCLResult& rResult, 
		      int argc, char** argv);
  int SetUnit(CTCLInterpreter& rInterp, CTCLResult& rResult, 
	      int argc, char** argv);
  int SetLimits(CTCLInterpreter& rInterp, CTCLResult& rResult, 
		int argc, char** argv);
  int Check(CTCLInterpreter& rInterp, CTCLResult& rResult, 
	    int argc, char** argv);
  int UnCheck(CTCLInterpreter& rInterp, CTCLResult& rResult, 
	      int argc, char** argv);
  int Version(CTCLInterpreter& rInterp, CTCLResult& rResult, 
	      int argc, char** argv);
  int Create(CTCLInterpreter& rInterp, CTCLResult& rResult,
	     int argc, char** argv);
  
  int listNew(CTCLInterpreter& rInterp, CTCLResult& rResult,
	      int argc, char** argv);
	// Utility functions.
private:
	int TypeSafeParseFailed(CTCLResult& rResult,
				std::string parameter, std::string expectedType);

};




#endif
