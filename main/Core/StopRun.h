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


//  CStopRun.h:
//
//    This file defines the CStopRun class.
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

#ifndef STOPRUN_H  //Required for current class
#define STOPRUN_H
                               //Required for base classes
#include "TCLPackagedCommand.h"

          

// Forward definitions:
//

class CTCLInterpreter;
class CTCLCommandPackage;
class CTCLResult;
                               
class CStopRun  : public CTCLPackagedCommand        
{
  
public:
     // Constructor:


  CStopRun (CTCLInterpreter* pInterp, CTCLCommandPackage& rPackage) :  
    CTCLPackagedCommand("stop", pInterp, rPackage) 
  { } 
  virtual ~ CStopRun ( ) { }       //Destructor

			//Copy constructor [illegal]
private:
  CStopRun (const CStopRun& aCStopRun );
public:
                        // Assignment operator [illegal]
private:
  CStopRun& operator= (const CStopRun& aCStopRun);
  int operator==(const CStopRun& rhs) const;
  int operator!=(const CStopRun& rhs) const;
public:

  // Operations:

public:
  virtual   int operator() (CTCLInterpreter& rInterpreter, 
			    CTCLResult& rRestul, 
			    int nArguments, char* pArguments[])  ;
 
};

#endif
