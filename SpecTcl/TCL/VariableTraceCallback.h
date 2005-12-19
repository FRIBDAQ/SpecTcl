/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/

#ifndef __VARIABLETRACECALLBACK_H
#define __VARIABLETRACECALLBACK_H

class CTCLInterpreter;

/*!
   This is an abstract base class that defines the interface into a
   variable trace callback object.  Variable trace callback objects are used 
   along with the CTracedVariable class to support a simplified interface to 
   callbacks.
*/
class CVariableTraceCallback 
{
 public:
  virtual char* operator()(CTCLInterpreter* pInterp,
			   char* pVariable, 
			   char* pElement,
			   int   flags) = 0;
};

#endif
