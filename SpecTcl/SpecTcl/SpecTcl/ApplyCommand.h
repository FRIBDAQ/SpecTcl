// Class: CApplyCommand                     //ANSI C++
//
// Implements the apply command.
// apply applies a single gate to a set of spectra.
// The syntax of this command is:
//    apply gatename spectrum1 [spectrum2 ...]
//        to apply a gate to several spectra or:
//    apply -list  spectrum [spectrum2...]
//        to list the gate applied on each of the selected
//       spectra.
//  Note that spectra can only have a single gate applied.
//  Applying a gate to a spectrum which is gated, replaces that
//  spectrum's gate... Since gates can be arbitrarily complex entities,
//  in practice, this is not a problem.
//
     
//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved ApplyCommand.h
//

#ifndef __APPLYCOMMAND_H  //Required for current class
#define __APPLYCOMMAND_H

                               //Required for base classes
#ifndef __TCLPACKAGEDCOMMAND_H
#include "TCLPackagedCommand.h"
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif
   
class CGatePackage;		// Forward class type definition.
class CTCLInterpreter;		// Forward class type definition
           
class CApplyCommand  : public CTCLPackagedCommand        
{                       


public:

   // Constructor:  

  CApplyCommand (CTCLInterpreter* pInterp, CTCLCommandPackage& rPack) :
    CTCLPackagedCommand("apply", pInterp, rPack)
  { 
  } 
  ~CApplyCommand ( ) { }  //Destructor 
  
   //Copy constructor: Illegal

private:
  CApplyCommand (const CApplyCommand& aCApplyCommand );
public:
   // Operator= Assignment Operator: Illegal
private:
  CApplyCommand& operator= (const CApplyCommand& aCApplyCommand);
public:

   //Operator== Equality Operator - Makes no sense,but supplied anyway.

  int operator== (const CApplyCommand& aCApplyCommand) {
    return CTCLPackagedCommand::operator==(aCApplyCommand);
  }
public:

  virtual   int operator() (CTCLInterpreter& rInterp, 
			    CTCLResult& rResult, int nParams, 
			    char* pParams[])    ;

  Int_t ApplyGate (CTCLInterpreter& rInterp, CTCLResult& rResult, 
		   UInt_t nArgs, char*  pArgs[])    ;
  
  Int_t ListApplications (CTCLInterpreter& rInterp, CTCLResult& rResult,
			  UInt_t nArgs, char* pArgs[])    ;

  // Utility functions.
 
protected:
  static string Usage();	// Produces usage information.
};

#endif
