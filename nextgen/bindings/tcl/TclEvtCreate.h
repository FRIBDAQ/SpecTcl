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

#ifndef _TCLEXPCREATE_H
#define _TCLEXPCREATE_H


#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif




/**
 ** Implement the ::spectcl::evtcreate command
 ** This command is responsible for creating an events database that 
 ** holds event data for a specific run in an experiment.
 ** the syntax is:
 ** \verbatim
::spectcl::evtcreate exphandle run path
    \endverbatim
 **
 ** Where:
 **    - exphandle is an experiment handle open on an experiment database.
 **    - run is the number of a run in that database.
 **    - path is the path to a file that will be the newly created events database.
 ** @note we can assume at this point that the API works.  We only need to ensure that we are
 **       exercising it properly.
 */

class CTclEvtCreate : public CTclDBCommand
{
  /* Canonicals: */

public:
  CTclEvtCreate(CTCLInterpreter& interp);
  virtual  ~CTclEvtCreate();
private:
  CTclEvtCreate& operator=(const CTclEvtCreate&);
  int operator==(const CTclEvtCreate&) const;
  int operator!=(const CTclEvtCreate&) const;

  /* Command entry point */

public:
  int operator()(CTCLInterpreter& interp,
		 std::vector<CTCLObject>& objv);
};


#endif
