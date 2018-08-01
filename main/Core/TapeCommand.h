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

#ifndef TAPECOMMAND_H  //Required for current class
#define TAPECOMMAND_H
                               //Required for base classes
#include "TCLPackagedCommand.h"


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
  int operator==(const CTapeCommand& rhs) const;
  int operator!=(const CTapeCommand& rhs) const;
public:

                       
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

