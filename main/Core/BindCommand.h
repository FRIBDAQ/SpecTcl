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


//  CBindCommand.h:
//
//    This file defines the CBindCommand class.
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
// Change log:
//    July 15, 1999 Ron Fox
//      Changed command name to sbind in order to avoid conflict with TK
//      bind command.
//
/////////////////////////////////////////////////////////////

#ifndef BINDCOMMAND_H  //Required for current class
#define BINDCOMMAND_H
                               //Required for base classes
#include "TCLPackagedCommand.h"


// Forward class definitions:

class CTCLCommandPackage;
class CTCLInterpreter;
                               
class CBindCommand  : public CTCLPackagedCommand        
{
public:			// Internal class definitions:
  enum eSwitches {
    keNew,
    keId,
    keAll,
    keList,
//    keXid,
    keNotSwitch
  };

public:

			//Constructor with arguments
  CBindCommand (CTCLInterpreter* pInterp,  CTCLCommandPackage& rPack) :
    CTCLPackagedCommand("sbind", pInterp, rPack)
  { }        
   ~ CBindCommand ( ) { }       //Destructor
	
			//Copy constructor - illegal

private:
  CBindCommand (const CBindCommand& aCBindCommand ) ;
public:
			//Operator= Assignment Operator - illegal
private:
  CBindCommand& operator= (const CBindCommand& aCBindCommand);
  int operator==(const CBindCommand& rhs) const;
  int operator!=(const CBindCommand& rhs) const;
public:

  // Operations:

public:                       
  virtual   int operator() (CTCLInterpreter& rInterp, CTCLResult& rResult,
			    int nArgs, char* pArgs[])  ;

  Int_t BindAll(CTCLInterpreter& rInterp, CTCLResult& rResult);
  Int_t BindByName(CTCLInterpreter& rInterp, CTCLResult& rResult,
		   int nArgs, char* pArgs[]);
  Int_t ListBindings (CTCLInterpreter& rInterp, CTCLResult& rResult, 
			int nArgs, char* pArgs[])  ;

  Int_t ListAll(CTCLInterpreter& rInterp, CTCLResult& rResult, 
		const char* pattern);
  Int_t ListByName(CTCLInterpreter& rInterp, CTCLResult& rResult,
		   int nArgs, char* pArgs[]);
  // Protected member functions:

protected:

  static eSwitches MatchSwitch(const char* pSwitch);
  static void      Usage(CTCLResult& rResult);

};

#endif

