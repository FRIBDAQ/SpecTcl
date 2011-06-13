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

#ifndef __TCLWSCLOSE_H
#define __TCLWSCLOSE_H

#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif


/**  This class implements the wsClose command.  
 *   wsClose closes a SpecTcl workspace given a handle returned from
 *   wsOpen. The format of the command is:'
 * \verbatim
::spectcl::wsClose workspace-handle
\endverbatim
 * where workspace-handle was a handle returned froma  call to 
 * ::spectcl::wsOpen.
 */ 
class CTclWsClose : public CTclDBCommand
{

  // Canonicals
public:
  CTclWsClose(CTCLInterpreter& interp);
  virtual ~CTclWsClose();

  // Interface for CTCLObject Processor
public:
  int operator()(CTCLInterpreter& interp, 
		 std::vector<CTCLObject>& objv);

};

#endif
