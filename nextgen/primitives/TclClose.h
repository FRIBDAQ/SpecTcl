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

#ifndef __TCLCLOSE_H
#define __TCLCLOSE_H

#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif


/**  This class implements the expclose command.  The 
 **  expclose command will take an existing database handle open on a
 **  database and close it.  The databas handle is then removed from the set of experiment handles.
 **
 */
class CTclClose : public CTclDBCommand
{

  // Canonicals
public:
  CTclClose(CTCLInterpreter& interp);
  virtual ~CTclClose();

  // Interface for CTCLObject Processor
public:
  int operator()(CTCLInterpreter& interp, 
		 std::vector<CTCLObject>& objv);

};

#endif
