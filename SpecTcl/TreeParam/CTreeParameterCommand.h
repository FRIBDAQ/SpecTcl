///////////////////////////////////////////////////////////
//  CTreeParameterCommand.h
//  Implementation of the Class CTreeParameterCommand
//  Created on:      30-Mar-2005 11:03:51 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#if !defined(__CTREEPARAMETERCOMMAND_H)
#define __CTREEPARAMETERCOMMAND_H

#ifndef __TCLPROCESSOR_H
#include <TCLProcessor.h>
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_MAP
#include <map>
#ifndef __STL_MAP
#define __STL_MAP
#endif
#endif

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
  static STD(map)<STD(string), STD(string)> m_createdParameters;

public:
  virtual ~CTreeParameterCommand();
  CTreeParameterCommand(CTCLInterpreter* pInterp = 0);
  virtual int operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, 
			 int argc, char** argv);
protected:
  friend class TreeCommandTest;
  STD(string) Usage();
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
				STD(string) parameter, STD(string) expectedType);

};




#endif
