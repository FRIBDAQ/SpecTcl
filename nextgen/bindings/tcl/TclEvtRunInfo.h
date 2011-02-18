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
#ifndef _TCLEVTRUNINFO_H
#define _TCLEVTRUNINFO_H


#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif

#ifndef __SPECTCL_EXPERIMENT_H
#include <spectcl_experiment.h>
#endif

#ifdef bool
#undef bool
#endif

class CTCLInterpreter;
class CTCLObject;

/**
 ** This command allows one to determine information about the run attached to
 ** the database.  Note that if no run is attached this is not an error but
 ** instead empty information is returned.  This allows this call to probe for
 ** the existence of an attachedm event file.
 **
 ** Format of the command:
 ** \verbatim
 **  ::spectcl::evtruninfo exphandle ?attach-point?
 ** \endverbatim
 **
 ** Where:
 **   - exphandle is the handle to an experiment database
 **   - attach-point specifies the attachpoint on which to look for the event database.
 **     If not supplied a default attach point; 'EVENTS' is used.
 */
class CTclEvtRunInfo : public CTclDBCommand
{
  // canonicals:

public:
  CTclEvtRunInfo(CTCLInterpreter& interp);
  virtual ~CTclEvtRunInfo();

  // methods that are externally callable:

public:
  int operator()(CTCLInterpreter& interp,
		 std::vector<CTCLObject>& objv);

};

#endif
