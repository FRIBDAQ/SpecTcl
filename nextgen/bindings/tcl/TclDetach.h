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

#ifndef _TCLDETACH_H
#define _TCLDETACH_H


#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif


/**
 ** Thid command detaches an existing events database to an already open experiment
 ** database.  The form of the command is:
 **
 \verbatim
  spectcl::evtdetach handle path ?binding-name?
\endverbatim
**
** Where:
**   - handle is a handle already open on an experiment database.
**   - path is the path to the events database to detach.
**   - If supplied, binding-name provides the 'bind point' for the detach.
**     If not supplied a default bind point is used.
**     Note that if an existing detach on that bind point has been issued,
**     that database must be detached first.
**
** To understand bind points we must examine the SQL command this executes:
**
\verbatim
  DETACH DATABASE path AS binding-name
\endverbatim
**
** The bind point is the binding-name after the AS keyword.
*/
class CTclDetach : public CTclDBCommand
{
  /* Canonicals */

public:
  CTclDetach(CTCLInterpreter& interp);
  virtual ~CTclDetach();
private:
  CTclDetach& operator=(const CTclDetach& rhs);
  int operator==(const CTclDetach& rhs) const;
  int operator!=(const CTclDetach& rhs) const;

  /* Command entry point */
public:
  int operator()(CTCLInterpreter& interp,
		 std::vector<CTCLObject>& objv);
};


#endif
