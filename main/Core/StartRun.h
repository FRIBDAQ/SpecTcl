/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


//  CStartRun.h:
//
//    This file defines the CStartRun class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef STARTRUN_H  //Required for current class
#define STARTRUN_H
                               //Required for base classes
#include "TCLPackagedCommand.h"

// Forward Definitions:

class CTCLInterpreter;
class CTCLCommandPackage;
class CTCLResult;
                               
                               
class CStartRun  : public CTCLPackagedCommand        
{
  
public:
			// Constructor
  CStartRun (CTCLInterpreter* pInterp, CTCLCommandPackage& rPackage) :  
    CTCLPackagedCommand("start", pInterp, rPackage)
  { } 
  virtual ~CStartRun ( ) { }       //Destructor

			//Copy constructor [illegal]
private:
  CStartRun (const CStartRun& aCStartRun );
public:
			//Operator= Assignment Operator [illegal]
private:
  CStartRun& operator= (const CStartRun& aCStartRun);
  int operator==(const CStartRun& rhs) const;
  int operator!=(const CStartRun& rhs) const;
public:

			//Operator== Equality Operator [strange but legal]

  //

public:
  virtual   int operator() (CTCLInterpreter& rInterpreter, 
			    CTCLResult& rResult, 
			    int nArguments, char* pArguments[])  ;
  
};

#endif
