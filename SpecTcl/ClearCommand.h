

//  CClearCommand.h:
//
//    This file defines the CClearCommand class.
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

#ifndef __CLEARCOMMAND_H  //Required for current class
#define __CLEARCOMMAND_H
                               //Required for base classes
#ifndef __TCLPACKAGEDCOMMAND_H
#include "TCLPackagedCommand.h"
#endif

class CTCLInterpreter;
class CTCLResult;                               
                               
class CClearCommand  : public CTCLPackagedCommand        
{
public:
  // Switch set:
  enum Switch {
    keAll,
    keId,
    keNotSwitch
  };


public:
  CClearCommand (CTCLInterpreter* pInterp,
		 CTCLCommandPackage& rPack) :
    CTCLPackagedCommand("clear", pInterp, rPack)
  { }        
  virtual ~CClearCommand ( ) 
  { }				//Destructor	
			//Copy constructor - illegal
private:
  CClearCommand (const CClearCommand& aCClearCommand );
public:
			//Operator= Assignment Operator - illegal

private:
  CClearCommand& operator= (const CClearCommand& aCClearCommand);
public:

			//Operator== Equality Operator -- allowed but...

  int operator== (const CClearCommand& aCClearCommand)
  { 
    return ((CTCLPackagedCommand::operator== (aCClearCommand)) );
  }                             

  // Operations.

public:
  virtual   int operator() (CTCLInterpreter& rInterp, CTCLResult& rResult, 
			    int nArgs, char* pArgs[])  ;
  Int_t  ClearNameList(CTCLResult& rResult, 
		       int nArgs, char* pArgs[]);
  Int_t  ClearIdList(CTCLResult& rResult,
		     int nArgs, char* pArgs[]);
protected:
  static void   Usage(CTCLResult& rResult);
  static Switch MatchSwitch(const char* pSwitchText);
};

#endif
