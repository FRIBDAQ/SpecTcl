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

#ifndef _TCLEVTUUID_H
#define _TCLEVTUUID_H

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
 ** This command returns the textualized UUID of an events database.
 ** Format is:
 ** \verbatim
 **  evtuuid handle
 ** \endverbatim
 */
class CTclEvtUUID : public CTclDBCommand
{
public:
  CTclEvtUUID(CTCLInterpreter& interp);
  virtual ~CTclEvtUUID();

  // methods that are externally callable:

public:
  int operator()(CTCLInterpreter& interp,
		 std::vector<CTCLObject>& objv);

};

#endif
