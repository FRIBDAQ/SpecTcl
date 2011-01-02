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


#ifndef __TCLCREATE_H
#define __TCLCREATE_H

#ifndef _TCLDBCOMMAND_H
#include "TclDBCommand.h"
#endif


/**
 ** This class implements the expcreate command.  expcreate creates a 
 ** new SpecTcl experiment database.  The command returns a handle to
 ** the newly created database.
 ** We are also going to include a static function that gets and maintains
 ** the handle manager for experimental database handles.
 **
 */
class CTclCreate : public CTclDBCommand
{
  // Class level storage:

  static CHandleManager* m_pHandles;

  // Canonicals
public:
  CTclCreate(CTCLInterpreter& interp);
  virtual ~CTclCreate();

  // Interface for CTCLObject Processor
public:
  int operator()(CTCLInterpreter& interp, 
		 std::vector<CTCLObject>& objv);


  
};



#endif 
