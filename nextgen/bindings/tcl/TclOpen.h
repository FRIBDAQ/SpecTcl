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
#ifndef __TCLOPEN_H
#define __TCLOPEN_H

#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif

/**
 ** This class implements the expopen command.  expopen
 ** opens an existing database.
 */
class CTclOpen : public CTclDBCommand
{

  // Canonicals
public:
  CTclOpen(CTCLInterpreter& interp);
  virtual ~CTclOpen();

  // Interface for CTCLObject Processor
public:
  int operator()(CTCLInterpreter& interp, 
		 std::vector<CTCLObject>& objv);

};


#endif
