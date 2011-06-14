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
#ifndef __TCLWSUUID_H
#define __TCLWSUUID_H

#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif

/**
 ** wsWsUUID is a command that reads the WsUUID out of a workspace database.
 ** WsUUID's are used to identify which databases can legitimately be
 ** attached to other databases.  The root is an experiment database which
 ** establishs a WsUUID for a set of related databases that are created relative
 ** to the experiment databas and therefore inherit its WsUUID.
 **
 ** Command syntax:
 ** \verbatim
::spectcl::wsWsUUID workspace-handle
\endverbatim
 ** 
 ** Where workspace-handle is the  handle gotten from a ::spectcl::wsOpen
 ** On success, the command result is the textualized UUID of the
 ** workspace.
 */
class CTclWsUUID : public CTclDBCommand
{

  // Canonicals
public:
  CTclWsUUID(CTCLInterpreter& interp);
  virtual ~CTclWsUUID();

  // Interface for CTCLObject Processor
public:
  int operator()(CTCLInterpreter& interp, 
		 std::vector<CTCLObject>& objv);

};


#endif
