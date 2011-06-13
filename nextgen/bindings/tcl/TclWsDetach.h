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

#ifndef _TCLWSDETACH_H
#define _TCLWSDETACH_H

#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif

/**
 * provides the ::spectcl::wsDetach command.  This command
 * detaches a workspace from an existing experiment.
 * The form of this command is:
 *
 *\verbatim
::spectcl::detach exphandle ?attach-point?
\endverbatim
 * Where:
 *   - exphandle - is an experiment database handle from e.g. spectcl::expopen
 *   - attach-point - is an optional attach point that indicates where the workspace
 *                    was attached.  If omitted, the default value of "WORKSPACE" is used.
 */
class CTclWsDetach : public CTclDBCommand
{
  /* Canonicals */
public:
  CTclWsDetach(CTCLInterpreter& interp);
  virtual ~CTclWsDetach();

private:
  CTclWsDetach& operator=(const CTclWsDetach&);
  int operator==(const CTclWsDetach&) const;
  int operator!=(const CTclWsDetach&) const;

  /* Command entry point: */

public:
  int  operator()(CTCLInterpreter& interp,
		  std::vector<CTCLObject>& objv);
};


#endif
