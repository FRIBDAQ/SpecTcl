/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef _CTREEPARAMTERCOMMAND_H
#define _CTREEPARAMTERCOMMAND_H

/**
 * @file CTreeParameterCommand.h
 * @brief Define class that executes the spectcl::treeparam command ensemble.
 */


#ifndef __TCLOBJECTPROCESSOR_H
#include <TCLObjectProcessor.h>	/* base class. */
#endif

#ifndef _CPARAMETERDICTIONARY_H
#include "CParameterDictionary.h"
#endif

#ifndef _STD_STRING
#include <string>
#ifndef _STD_STRING
#define _STD_STRING
#endif
#endif

#ifndef _STL_LIST
#include <list>
#ifndef _STL_LIST
#define _STL_LIST
#endif
#endif

class CTCLInterpreter;
class CTCLObject;

/**
 * @class CTreeParameterCommand
 * 
 *  Provides the ::spectcl::treeparameter command ensemble.  While this is closely
 *  modeled after the 3.x and earlier version of that command, the existence of 
 *  observers makes possible the use of tree parameter traces where before this
 *   awful -check/-uncheck scheme had to be done in the past.
 *
 *  Here are the subcommands for the ensemble (full syntax is given in the
 *  processor for each subcommand):
 *
 *  * create - Creates a tree parameter.  These tree parameters cannot really be 
 *    destroyed the purpose of the create subcommand is to pre-build parameters
 *    so that spectra can be defined before e.g. event processors can initialize
 *    and define them.
 *  * list      - lists tree parametrers and their properties.
 *  * modify    - Modifies the shape of a parameter.
 *  * setbins   - Changes the suggested binning.
 *  * setlimits - Sets the parameter limits.
 *  * setunits  - Sets the parameter units.
 *  * trace add - Adds a trace to the tree parameter dictionary.  Traces are scripts
 *                that get called as a result of significant events in the dictionary.
 *  * trace remove - Removes an existing trace from the tree parameter dictionary.
 *  * trace show   - Show the set of trace scripts that are defined on the
 *                   tree parameter dictionary.
 *
 * @note This is a singleton object.
 */
class CTreeParameterCommand : public CTCLObjectProcessor
{
private:
  // private data types:

  typedef struct _ParameterInfo {
    double      s_low;
    double      s_high;
    unsigned    s_bins;
    std::string s_units;
    
  } ParameterInfo, *pParameterInfo;


  class CScriptObserver : public CObserver<CParameterDictionary>
  {
    
  public:
    virtual void operator()(CObservable<CParameterDictionary>* pDict);
  };
private:
  static CTreeParameterCommand* m_pInstance;

private:
  CScriptObserver*        m_pObserver;
  std::list<CTCLObject*>  m_traceScripts;

  // canonicals
private:
  CTreeParameterCommand(); 
  ~CTreeParameterCommand();



public:
  static CTreeParameterCommand* instance();

  // command interface:

public:
  int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);

  // Execute the subcommands errors are signalled via throw.

private:
  void create(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void list(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void modify(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void setbins(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void setlimits(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void setunits(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void trace(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void traceAdd(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void traceRemove(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
  void traceShow(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);

  // utilities:

private:
  void getProperties(std::vector<CTCLObject>& objv, 
		     pParameterInfo pInfo, unsigned startingAt = 3 );
  CParameterDictionary::DictionaryIterator findParameter(CTCLObject& obj, 
							 const char* pSubcommand);
  void runTraces(const CParameterDictionary::pOperationInfo pInfo);

};
#endif
