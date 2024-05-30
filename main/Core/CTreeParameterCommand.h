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
// This implementation of TreeParameter is based on the ideas and original code of:
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//


#ifndef CTREEPARAMETERCOMMAND_H
#define CTREEPARAMETERCOMMAND_H

#include <TCLObjectProcessor.h>  // Modernize.
#include <MPITclCommandAll.h>
#include <string>
#include <map>

// Forward Definitions:

class CTCLInterpreter;
class CTCLObject;


/* For MPI SpecTcl, we do the trick of changing the class name
  to CTreeVariableCommandActual and then encapsulating it in a
  CMPITclCommandAll.  In order to maintain some sense of
  order, only rank 0 (in parallel mode) will do perform the
  -list operation. Alll other ranks, including rank 0 will
  do everything else so that tree parameters can be interrogated everywhere
  It's really critical these be available to the worker ranks so user
  code can access metadata.
*/

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
 * - -create    - Dynamically create a new parameter - e.g. prior to 
 *                loading a new event processor, or defniing a pseudo.
 * - -listnew  -  List the tree parameters that were created this session via
 *                the -create operation.
 */
class CTreeParameterCommandActual : public CTCLObjectProcessor
{
private:
  static std::map<std::string, std::string> m_createdParameters;

public:
  virtual ~CTreeParameterCommandActual();
  CTreeParameterCommandActual(CTCLInterpreter* pInterp = 0);
  virtual int operator()(CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv);
protected:
  friend class TreeCommandTest;
  std::string Usage();
  int List(CTCLInterpreter& rInterp, char argc, const char** argv);
  int SetDefinition(CTCLInterpreter& rInterp, int argc, const char** argv);
  int SetIncrement(CTCLInterpreter& rInterpreter, int argc, const char** argv);
  int SetChannelCount(CTCLInterpreter& rInterp, int argc, const char** argv);
  int SetUnit(CTCLInterpreter& rInterp, int argc, const char** argv);
  int SetLimits(CTCLInterpreter& rInterp, int argc, const char** argv);
  int Check(CTCLInterpreter& rInterp, int argc, const char** argv);
  int UnCheck(CTCLInterpreter& rInterp, int argc, const char** argv);
  int Version(CTCLInterpreter& rInterp, int argc, const char** argv);
  int Create(CTCLInterpreter& rInterp, int argc, const char** argv);
  
  int listNew(CTCLInterpreter& rInterp, int argc, const char** argv);
	// Utility functions.
private:
	int TypeSafeParseFailed(CTCLInterpreter& rInterp,
				std::string parameter, std::string expectedType);

};


// The mpi wrapper class:

class CTreeParameterCommand : public CMPITclCommandAll {
public:
  CTreeParameterCommand(CTCLInterpreter* pInterp);
  ~CTreeParameterCommand() {}
};

#endif
