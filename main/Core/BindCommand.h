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
#include "TCLPackagedObjectProcessor.h"


// Forward class definitions:

class CTCLCommandPackage;
class CTCLInterpreter;
                               
class CBindCommand  : public CTCLPackagedObjectProcessor
{
public:			// Internal class definitions:
  enum eSwitches {
    keNew,
    keId,
    keAll,
    keList,
    keTrace,
    keUntrace,
    keNotSwitch
  };

public:

			//Constructor with arguments
  CBindCommand (CTCLInterpreter* pInterp, const char* cmd = "sbind");
   ~ CBindCommand ( ) { }       //Destructor
	
			//Copy constructor - illegal

private:
  CBindCommand (const CBindCommand& aCBindCommand ) ;
private:
  CBindCommand& operator= (const CBindCommand& aCBindCommand);
  int operator==(const CBindCommand& rhs) const;
  int operator!=(const CBindCommand& rhs) const;
public:

  // Operations:

public:                       
  virtual   int operator() (CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv);

  TCLPLUS::Int_t BindAll(CTCLInterpreter& rInterp);
  TCLPLUS::Int_t BindByName(CTCLInterpreter& rInterp, int nArgs, const char* pArgs[]);
  TCLPLUS::Int_t ListBindings (CTCLInterpreter& rInterp, int nArgs, const char* pArgs[])  ;

  TCLPLUS::Int_t ListAll(CTCLInterpreter& rInterp, const char* pattern = "*");
  TCLPLUS::Int_t ListByName(CTCLInterpreter& rInterp, int nArgs, const char* pArgs[]);
  TCLPLUS::Int_t Trace(CTCLInterpreter& rInterp, int nArgs, const char* pArgs[]);
  TCLPLUS::Int_t Untrace(CTCLInterpreter& rInterp, int nArgs, const char* pArgs[]);
  
  // Protected member functions:

protected:

  static eSwitches MatchSwitch(const char* pSwitch);
  static void      Usage(CTCLInterpreter& rInterp);

};

#endif

