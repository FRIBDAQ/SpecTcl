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

#ifndef _TCLEXPOPEN_H
#define _TCLEXPOPEN_H


#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif



class CTclEvtOpen : public CTclDBCommand
{
  /* Canonicals: */

public:
  CTclEvtOpen(CTCLInterpreter& interp);
  virtual  ~CTclEvtOpen();
private:
  CTclEvtOpen& operator=(const CTclEvtOpen&);
  int operator==(const CTclEvtOpen&) const;
  int operator!=(const CTclEvtOpen&) const;

  /* Command entry point */

public:
  int operator()(CTCLInterpreter& interp,
		 std::vector<CTCLObject>& objv);
};




#endif
