//  CParameterCommand.h:
//
//    This file defines the CParameterCommand class.
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

#ifndef __PARAMETERCOMMAND_H  //Required for current class
#define __PARAMETERCOMMAND_H
                               //Required for base classes
#ifndef __TCLPACKAGEDCOMMAND_H
#include "TCLPackagedCommand.h"
#endif  

#ifndef __TCLCOMMANDPACKAGE_H
#include "TCLCommandPackage.h"
#endif

// Forward class definitions.
//
class CTCLInterpreter;                             
class CTCLResult;
                               
class CParameterCommand  : public CTCLPackagedCommand        
{
  // Datatypes:
public:
  enum SwitchValue_t {
    NewSw, 
    ListSw,
    DeleteSw,
    IdSw,
    ByIdSw,
    NotSwitch			// Should be last, ideally
  };
public:

  CParameterCommand (CTCLInterpreter* pInterp, CTCLCommandPackage& rPackage ) :
     CTCLPackagedCommand("parameter", pInterp, rPackage)
  { }        
  virtual ~ CParameterCommand ( ) { }       //Destructor
	
			//Copy constructor [illegal]
private:
  CParameterCommand (const CParameterCommand& aCParameterCommand );
public:

			//Operator= Assignment Operator [illegal]
private:
  CParameterCommand& operator= (const CParameterCommand& aCParameterCommand);
public:
			//Operator== Equality Operator [wierd but..]

  int operator== (const CParameterCommand& aCParameterCommand)
  { 
    return (
	    (CTCLPackagedCommand::operator== (aCParameterCommand)) );
  }                             
  //
  // Operations on the Object:
  //
public:
  virtual   int operator() (CTCLInterpreter& rInterp, 
			    CTCLResult& rResult, 
			    int nArguments, char* pArguments[])  ;

  // Subcommands:
  //
  UInt_t Create (CTCLInterpreter& rInterp, CTCLResult& rResult, 
		UInt_t nArg, char*  pArg[])  ;

  UInt_t List (CTCLInterpreter& rInterp, CTCLResult& rResult, 
	       UInt_t nPars, char* pPars[])  ;

  UInt_t Delete (CTCLInterpreter& rInterp, CTCLResult& rResult, 
		 UInt_t nPars, Char_t* pPars[])  ;

  //  Utility members:

protected:
  //
  // General parsing utilities:
  //
  void Usage (CTCLInterpreter& rInterp, CTCLResult& rResult)  ;
  SwitchValue_t ParseSwitch (const char* pSwitch)  ;

  // Utilities for producing Tcl lists for parameter definitions.

  UInt_t ListParametersById(CTCLResult& rResult);
 
};

#endif
