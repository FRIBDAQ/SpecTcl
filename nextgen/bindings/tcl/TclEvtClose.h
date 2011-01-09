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

#ifndef _TCLEXPCLOSE_H
#define _TCLEXPCLOSE_H


#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif




/**
 ** Implement the ::spectcl::evtclose command
 ** This command is responsible for creating an events database that 
 ** holds event data for a specific run in an experiment.
 ** the syntax is:
 ** \verbatim
::spectcl::evtclose exphandle run path
    \endverbatim
 **
 ** Where:
 **    - exphandle is an experiment handle open on an experiment database.
 **    - run is the number of a run in that database.
 **    - path is the path to a file that will be the newly closed events database.
 ** @note we can assume at this point that the API works.  We only need to ensure that we are
 **       exercising it properly.
 */

class CTclEvtClose : public CTclDBCommand
{
  /* Canonicals: */

public:
  CTclEvtClose(CTCLInterpreter& interp);
  virtual  ~CTclEvtClose();
private:
  CTclEvtClose& operator=(const CTclEvtClose&);
  int operator==(const CTclEvtClose&) const;
  int operator!=(const CTclEvtClose&) const;

  /* Command entry point */

public:
  int operator()(CTCLInterpreter& interp,
		 std::vector<CTCLObject>& objv);
};


#endif
