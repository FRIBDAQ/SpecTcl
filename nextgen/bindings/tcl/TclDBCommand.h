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
#ifndef _TCLDBCOMMAND_H
#define _TCLDBCOMMAND_H



#ifndef _TCLOBJECTPROCESSOR_H
#include <TCLObjectProcessor.h>
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef _SPECTL_EXPERIMENT_H
#include "spectcl_experiment.h"
#endif

class CHandleManager;
class CTCLInterpreter;

/**
 ** This abstract base class provides utilities for database using
 ** commands in SpecTcl.
 */
class CTclDBCommand : public CTCLObjectProcessor 
{
  static CHandleManager* m_pHandles;
public:
  CTclDBCommand(CTCLInterpreter& interp, std::string command, bool doRegister);
  virtual ~CTclDBCommand();
  
  // utilities:
  
public:
  static spectcl_experiment getDatabaseHandle(CTCLInterpreter& interp,
					      std::vector<CTCLObject>& objv,
					      int which,
					      std::string subcommand) throw(std::string);
  
  static CHandleManager* getHandleManager();
protected:
  std::string     normalizePath(std::string path) throw(std::string);
  static void     validateExpEvtHandles(spectcl_experiment pExp, spectcl_events pEvts);
  static void     throwIfNotExpHandle(spectcl_experiment expHandle);

};


#endif
