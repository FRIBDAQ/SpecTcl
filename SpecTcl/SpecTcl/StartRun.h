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

#ifndef __STARTRUN_H  //Required for current class
#define __STARTRUN_H
                               //Required for base classes
#ifndef __TCLPACKAGEDCOMMAND_H
#include "TCLPackagedCommand.h"
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

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
public:

			//Operator== Equality Operator [strange but legal]

  int operator== (const CStartRun& aCStartRun)
  { 
    return (
	    (CTCLPackagedCommand::operator== (aCStartRun))

	    );
  }                             
  // class operations:

public:
  virtual   int operator() (CTCLInterpreter& rInterpreter, 
			    CTCLResult& rResult, 
			    int nArguments, char* pArguments[])  ;
  
};

#endif
