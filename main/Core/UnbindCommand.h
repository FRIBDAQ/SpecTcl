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
//  CUnBindCommand.h:
//
//    This file defines the CUnBindCommand class.
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

#ifndef UNBINDCOMMAND_H  //Required for current class
#define UNBINDCOMMAND_H
                               //Required for base classes
#include "TCLPackagedCommand.h"

class CTCLInterpreter;
class CTCLCommandPackage;
class CTCLResult;                               
                               
class CUnbindCommand  : public CTCLPackagedCommand        
{
public:
  enum Switch {
    keId,
    keAll,
    keTrace,
    keUntrace,
    keNotSwitch
  };
public:
			//Constructor with arguments
  CUnbindCommand (CTCLInterpreter* pInterp, CTCLCommandPackage& rPackage) : 
    CTCLPackagedCommand("unbind", pInterp, rPackage)
  { }        
  virtual ~ CUnbindCommand ( ) { }       //Destructor
  
  //Copy constructor   -- Illegal
private:
  CUnbindCommand (const CUnbindCommand& aCUnbindCommand );
public:

			//Operator= Assignment Operator -- Illegal
private:
  CUnbindCommand& operator= (const CUnbindCommand& aCUnbindCommand);
  int operator==(const CUnbindCommand& rhs) const;
  int operator!=(const CUnbindCommand& rhs) const;
public:

  // Operations:

public:             
  virtual   int operator() (CTCLInterpreter& rInterp, CTCLResult& rResult, 
			    int nArgs, char* pArgs[])  ;
  Int_t UnbindByName(CTCLInterpreter& rInterp, CTCLResult& rResult, 
		     int nArgs, char* pArgs[]);
  Int_t UnbindById(CTCLInterpreter& rInterp, CTCLResult& rResult, 
		   int nArgs, char* pArgs[])  ;
  Int_t Trace(
    CTCLInterpreter& rInterp, CTCLResult& rResult, 
		int nArgs, char* pArgs[]
  );
  Int_t Untrace(
    CTCLInterpreter& rInterp, CTCLResult& rResult, 
		int nArgs, char* pArgs[]
  );
protected:
  static void   Usage(CTCLResult& rResult);
  static Switch MatchSwitch(const char* pSwitch);
 
};

#endif
