//  CAttachCommand.h:
//
//    This file defines the CAttachCommand class.
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
//  Change Log:
//      July 14, 1999 Ron Fox
//         Removed online connection stubs and replaced with 
//         pipe.
/////////////////////////////////////////////////////////////

#ifndef __ATTACHCOMMAND_H  //Required for current class
#define __ATTACHCOMMAND_H
                               //Required for base classes
#ifndef __TCLPACKAGEDCOMMAND_H
#include "TCLPackagedCommand.h"
#endif                               

          
// Forward class references:

class CTCLInterpreter;
class CTCLCommandPackage;
class CTCLResult;

//  Class definition:
                     
class CAttachCommand  : public CTCLPackagedCommand        
{
  // Data types:

public:
  enum Switch_t {		// The command switch set.
    keFile,
    keTape,
    kePipe,
    keBufferSize,
    keNotSwitch
  };
  
public:

			//Constructor with arguments
  CAttachCommand (CTCLInterpreter* pInterp, CTCLCommandPackage& rPackage ) :
    CTCLPackagedCommand("attach", pInterp, rPackage)
  {}
  virtual ~ CAttachCommand ( ) { }       //Destructor
	
			//Copy constructor [illegal]
private:
  CAttachCommand (const CAttachCommand& aCAttachCommand );
public:

			//Operator= Assignment Operator [illegal]
private:
  CAttachCommand& operator= (const CAttachCommand& aCAttachCommand);
public:

			//Operator== Equality Operator legal, but who cares.

  int operator== (const CAttachCommand& aCAttachCommand)
  { 
    return ((CTCLPackagedCommand::operator== (aCAttachCommand)) );
  }                             
  // Operations on the class:

public:
  virtual   int operator() (CTCLInterpreter& rInterp, 
			    CTCLResult& rResult, 
			    int nArgs, char* pArgs[])  ;
  int AttachFile (CTCLResult& rResult, int nArgs, char* pArgs[])  ;
  int AttachTape (CTCLResult& rResult, int nArgs, char* pArgs[])  ;
  int AttachPipe (CTCLResult& rResult, int nArgs, char* pArgs[])  ;

protected:
  static void     Usage(CTCLResult& rResult);
  static Switch_t ParseSwitch(char* pSwitch);

};

#endif
