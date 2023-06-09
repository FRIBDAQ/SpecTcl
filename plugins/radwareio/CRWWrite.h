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
#ifndef CRWWRITE_H
#define CRWWRITE_H

#include <TCLObjectProcessor.h>
#include <TCLObject.h>
#include <vector>


// forward class defs:

class CTCLInterpreter;



/*!
   This class implements a command processor to write a SpecTcl 1-d spectrum
   in radware .spe form.  It makes use of elements of the radware package
   that are redistributed by permission.
*/
class CRWWrite : public CTCLObjectProcessor
{
 public:
  CRWWrite(CTCLInterpreter& interp);
  virtual ~CRWWrite();


  // Probably can't do the private unimplemented trick on loadable extensions.

public:
  virtual int operator()(CTCLInterpreter& interp,
			 STD(vector)<CTCLObject>& objv);


};


#endif
