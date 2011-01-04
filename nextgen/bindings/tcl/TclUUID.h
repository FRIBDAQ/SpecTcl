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
#ifndef _TCLUUID_H
#define _TCLUUID_H

#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif



/**
 ** This class provides tcl bindings for the uiud part of the experiment
 ** database API.   The expuuid command is a command ensemble:
 ** \verbatim
 **   expuuid check handle uuid_text
 **   expuuid get handle
 ** \verbatim
 ** - The check subcommand returns 1 if the uuid_text matches the uuid of the
 **   experiment database open on 'handle'.
 ** - The get subcommand gets the textual representation of the uuid of the
 **   experiment database open on handle.
 ** - Parameters:
 **   - handle - handle open on an experiment database via expcreate or expopen.
 **   - uuid_text - the textual representation of a uuid.  E.g. the output of
 **     uuid_unparse.
 */
class CTclUUID : public CTclDBCommand
{
  // Canonicals:
public:
  CTclUUID(CTCLInterpreter& rInterp);
  virtual ~CTclUUID();

  // Operations at object level:

  int operator()(CTCLInterpreter& rInterp,
		 std::vector<CTCLObject>& objv);
private:
  // Subcommand functions:

  int check(CTCLInterpreter& rInterp,
	    std::vector<CTCLObject>& objv) throw(std::string);
  int get(CTCLInterpreter& rInterp,
	  std::vector<CTCLObject>& objv) throw(std::string);
};
#endif
