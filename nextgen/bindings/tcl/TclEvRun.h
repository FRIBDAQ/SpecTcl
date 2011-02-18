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
#ifndef _TCLEVRUN_H
#define _TCLEVRUN_H

#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif

#ifndef __SPECTCL_EXPERIMENT_H
#include <spectcl_experiment.h>
#endif

#ifdef bool
#undef bool
#endif


/**
 ** This command allows you to determine the run number associated with a specific
 ** event database given a handle open on it
 ** The form of the command is:
 ** \verbatim
 ** ::spectcl::evrun  evthandle
 ** \endverbatim
 ** Where:
 **    - evthandle is a handle gotten when the database was opened.
 */
class CTclEvRun : public CTclDBCommand
{
  // Canonicals

public:
  CTclEvRun(CTCLInterpreter& interp);
  virtual ~CTclEvRun();


  // Entry point 
public:
  int operator()(CTCLInterpreter& interp,
		 std::vector<CTCLObject>& objv);

};

#endif
