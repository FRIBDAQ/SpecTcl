//  CTapeCommand.h:
//
//    This file defines the CTapeCommand class.
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

#ifndef __TAPECOMMAND_H  //Required for current class
#define __TAPECOMMAND_H
                               //Required for base classes
#ifndef __TCLPACKAGEDCOMMAND_H
#include "TCLPackagedCommand.h"
#endif                               

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif


// Foward definitions:


class CTCLIntpreter;
class CTCLCommandPackage;
class CTCLResult;

// Class definition:
                               
class CTapeCommand  : public CTCLPackagedCommand        
{
  // Local data types:
public:
  enum Switch_t {
    keOpen,
    keClose,
    keRewind,
    keNext,
    keNotSwitch
  };
  // Members;
public:

			//Constructor with arguments
  CTapeCommand (CTCLInterpreter* pInterp, CTCLCommandPackage& rPackage ) :
    CTCLPackagedCommand("tape", pInterp, rPackage)
  {}
  virtual ~ CTapeCommand ( ) { }       //Destructor
	
			//Copy constructor [illegal]
private:
  CTapeCommand (const CTapeCommand& aCTapeCommand );
public:
			//Operator= Assignment Operator [illegal]
private:
  CTapeCommand& operator= (const CTapeCommand& aCTapeCommand);
public:


			//Operator== Equality Operator legal but...

  int operator== (const CTapeCommand& aCTapeCommand)
  { 
    return ((CTCLPackagedCommand::operator== (aCTapeCommand)) );
  }                             
                       
  // Operations on the class:

public:
  virtual   int operator() (CTCLInterpreter& rInterp, CTCLResult& rResult, 
			    int nArgs, char* pArgs[])  ;

  int OpenFile (CTCLResult& rResult, int nArgs, char* pArgs[])  ;
  int CloseFile (CTCLResult& rResult, int nArgs, char* pArgs[])  ;
  int RewindTape (CTCLResult& rResult, int nArgs, char* pArgs[])  ;

protected:
  static void     Usage(CTCLResult& rResult);
  static Switch_t ParseSwitch(const char* pSwitch);
};

#endif

