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
 ** This command detaches an attached  events database from an already open experiment
 ** database.  The form of the command is:
 **
 \verbatim
  spectcl::detach handle ?binding-name?
\endverbatim
**
** Where:
**   - handle is a handle already open on an experiment database.
**   - If supplied, binding-name provides the 'bind point' for the detach.
**     If not supplied a default bind point is used.
**     Note that if an existing detach on that bind point has been issued,
**     that database must be detached first.
**

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
