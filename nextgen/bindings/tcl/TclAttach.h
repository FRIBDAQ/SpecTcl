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

#ifndef _TCLATTACH_H
#define _TCLATTACH_H


#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif


/**
 ** Thid command attaches an existing events database to an already open experiment
 ** database.  The form of the command is:
 **
 \verbatim
  spectcl::evtattach handle path ?binding-name?
\endverbatim
**
** Where:
**   - handle is a handle already open on an experiment database.
**   - path is the path to the events database to attach.
**   - If supplied, binding-name provides the 'bind point' for the attach.
**     If not supplied a default bind point is used.
**     Note that if an existing attach on that bind point has been issued,
**     that database must be detached first.
**
** To understand bind points we must examine the SQL command this executes:
**
\verbatim
  ATTACH DATABASE path AS binding-name
\endverbatim
**
** The bind point is the binding-name after the AS keyword.
*/
class CTclAttach : public CTclDBCommand
{
  /* Canonicals */

public:
  CTclAttach(CTCLInterpreter& interp);
  virtual ~CTclAttach();
private:
  CTclAttach& operator=(const CTclAttach& rhs);
  int operator==(const CTclAttach& rhs) const;
  int operator!=(const CTclAttach& rhs) const;

  /* Command entry point */
public:
  int operator()(CTCLInterpreter& interp,
		 std::vector<CTCLObject>& objv);
};


#endif
