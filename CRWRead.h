/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef __CRWREAD_H
#define __CRWREAD_H



#ifndef __TCLOBJECTPROCESSOR_H
#include <TCLObjectProcessor.h>
#endif

#ifndef __TCLOBJECT_H
#include <TCLObject.h>
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif


// forward class defs:

class CTCLInterpreter;

/*!
   Implements a command processor to read a radware .spe file into
    a 1-d snapshot spectrum.  This makes use of pieces of the radware
    package that are redistributed by permission.
*/
class CRWRead : public CTCLObjectProcessor
{
public:
  CRWRead(CTCLInterpreter& interp);
  virtual ~CRWRead();

  //

public:
  virtual int operator()(CTCLInterpreter& interp,
			 STD(vector)<CTCLObject>& objv);


};

#endif
