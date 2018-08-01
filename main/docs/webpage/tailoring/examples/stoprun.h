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
#include <string>

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
public:

			//Operator== Equality Operator [strange but allowed]
  int operator== (const CStopRun& aCStopRun)
  { return (
	    (CTCLPackagedCommand::operator== (aCStopRun)) 
	    );
  }                             
  // Operations:

public:
  virtual   int operator() (CTCLInterpreter& rInterpreter, 
			    CTCLResult& rRestul, 
			    int nArguments, char* pArguments[])  ;
 
};

#endif
