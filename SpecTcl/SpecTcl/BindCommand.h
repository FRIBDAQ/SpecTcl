

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

#ifndef __BINDCOMMAND_H  //Required for current class
#define __BINDCOMMAND_H
                               //Required for base classes
#ifndef __TCLPACKAGEDCOMMAND_H
#include "TCLPackagedCommand.h"
#endif                               

#ifndef __STL_VECTOR_H
#include <vector>
#define __STL_VECTOR_H
#endif

#ifndef __STL_STRING_H
#include <string>
#define __STL_STRING_H
#endif

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
    keXid,
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
public:


			//Operator== Equality Operator - legal but weird.

  int operator== (const CBindCommand& aCBindCommand)
  { 
    return (
	    (CTCLPackagedCommand::operator== (aCBindCommand)) 

	    );
  }                             
  // Operations:

public:                       
  virtual   int operator() (CTCLInterpreter& rInterp, CTCLResult& rResult,
			    int nArgs, char* pArgs[])  ;

  Int_t BindAll(CTCLInterpreter& rInterp, CTCLResult& rResult);
  Int_t BindByName(CTCLInterpreter& rInterp, CTCLResult& rResult,
		   int nArgs, char* pArgs[]);
  Int_t BindByIdent(CTCLInterpreter& rInterp, CTCLResult& rResult,
		    int nArgs, char* pArgs[]);
  Int_t ListBindings (CTCLInterpreter& rInterp, CTCLResult& rResult, 
			int nArgs, char* pArgs[])  ;

  Int_t ListAll(CTCLInterpreter& rInterp, CTCLResult& rResult);
  Int_t ListByName(CTCLInterpreter& rInterp, CTCLResult& rResult,
		   int nArgs, char* pArgs[]);
  Int_t ListById(CTCLInterpreter& rInterp, CTCLResult& rResult,
		 int nArgs, char* pArgs[]);
  Int_t ListByXid(CTCLInterpreter& rInterp, CTCLResult& rResult,
		  int nArgs, char* pArgs[]);
  // Protected member functions:

protected:

  static eSwitches MatchSwitch(const char* pSwitch);
  static void      Usage(CTCLResult& rResult);

};

#endif

