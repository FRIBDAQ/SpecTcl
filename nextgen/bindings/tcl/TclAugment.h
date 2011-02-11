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
#ifndef __TCLAUGMENT_H
#define __TCLAUGMENT_H



#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif

#ifndef __SPECTCL_EXPERIMENT_H
#include <spectcl_experiment.h>
#endif

#ifdef bool
#undef bool
#endif

#ifndef __STL_MAP
#include <map>
#ifndef __STL_MAP
#define __STL_MAP
#endif
#endif

/**
 ** This command allows you to augment existing events in a database by either modifying
 ** existing parameter or by adding new parameters (that are defined in the experiment but
 ** not in the event.
 ** Syntax:
\verbatim
  ::spectcl::augment exphandle evthandle script
\endverbatim
** Where:
**  - exphandle - is an experiment database handle.
**  - evthandle - is an event database handle.
**  - script is a script that is called for each event.  The event is handed to the
**    script in the form [list trigger [list param_name value ...]
**     the script is supposed to return a possibly empty list defining the set off paramters
**    modified or added to the event in the form:
**      [list trigger [list  param-name value...]]
*/
class CTclAugment : public CTclDBCommand
{
  // Data Structures:

  typedef struct _CallbackData_ {
    CTclAugment*                 pObject;
    void*                        pExperiment;
    void*                        pEvents;
    CTCLInterpreter&             interp;
    std::string                  script;
    std::map<std::string, int>&  parametersByName; // name -> id
    std::vector<std::string>&    parametersById;   // id   -> name

  } CallbackData, *pCallbackData;


  // Member data:

  std::string    m_errorMessages;
  // Canonicals

public:
  CTclAugment(CTCLInterpreter& interp);
  virtual ~CTclAugment();

  // methods:

  int operator()(CTCLInterpreter& interp,
		 std::vector<CTCLObject>& objv);


  // Utility methods:

private:
  std::map<std::string, int> getParameters(void* pExperiment);
  std::vector<std::string>   invertMap(const std::map<std::string, int>& parameters);
  static  AugmentResult* toScriptAndBack(size_t nParams,
				 pParameterData pEvent,
				 void* context);
};

#endif
