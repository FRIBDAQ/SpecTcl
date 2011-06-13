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
#ifndef __TCLWSOPEN_H
#define __TCLWSOPEN_H

#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif

/**
 ** This class implements the expWsOpen command.  expWsOpen
 ** WsOpens an existing database.
 ** The format of this command is:
 ** \verbatim
 ::spectcl::wsOpen path
\enverbatim
 **
 ** where path is the path to the workstation file.
 ** the command, when successful returns a handle to the
 ** database which can be used in other Tcl commands that
 ** require a Workspace handle.
 ** 
 */
class CTclWsOpen : public CTclDBCommand
{

  // Canonicals
public:
  CTclWsOpen(CTCLInterpreter& interp);
  virtual ~CTclWsOpen();

  // Interface for CTCLObject Processor
public:
  int operator()(CTCLInterpreter& interp, 
		 std::vector<CTCLObject>& objv);

};


#endif
