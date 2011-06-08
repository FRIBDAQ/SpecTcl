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

#ifndef _TCLWSATTACH_H
#define _TCLWSATTACH_H

#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif

/**
 * Implement the spectcl::wsAttach command.  This attaches a workstation database
 * to an experiment database.
 * The syntax of the command is:
 *\verbatim

::spectcl::wsAttach exphandle path ?attach-point?

\endverbatim

 * Where:
 *   - exphandle    - is a tcl handle to the experiment database to which the
 *                    workspace will be attached.
 *   - path         - is the filesystem path to the workspace to attach to the experiment.
 *   - attach-point - is an optional database name to give the attached database name.
 *                    IF not supplied a default attach point is used.
 */
class CTclWsAttach : public CTclDBCommand
{
  /* Canonicals */
public:
  CTclWsAttach(CTCLInterpreter& interp);
  virtual ~CTclWsAttach();

private:
  CTclWsAttach& operator=(const CTclWsAttach&);
  int operator==(const CTclWsAttach&) const;
  int operator!=(const CTclWsAttach&) const;

  /* Command entry point: */

public:
  int  operator()(CTCLInterpreter& interp,
		  std::vector<CTCLObject>& objv);
};


#endif
