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


// Class: CPseudoCommand                     //ANSI C++
//
// Processes the pseudo parameter manipulation command.
//  This command has the following syntaxes:
//
//  pseudo name { dependent_params } { body }
//  pseudo -list [names... ]
//  pseudo -delete [names...]
//
//   The defining command's three parameters
// are the name of a previously declared but unfilled
// parameter, a list of dependent parameters and a
// procedure body to evaulate the parameter.
// In fact, SpecTcl will create a procedure named
//  name_Procedure  where name is the pseudo's
// name.
//   Example:
//
//      parameter p1  0 9
//      parameter p2  1 9
//      parameter sum 100 10
//      pseudo sum { p1 p2 } { return [expr $p1 + $p2 ] }
//
//    This will create a procedure:
//      proc sum_Procedure { p1 p2 } {
//                           return [expr $p1 + $p2]}
//   for each event, the values of p1 and p2 will be passed
//   in to this procedure and the return value will
//   be placed in the parameter bound to Sum at the
//   time the pseudo was defined.  Note that p1 and p2
//   can be pseudos as well as long as their evaluation
//   scripts are defined prior to Sum's.
//
//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved PseudoCommand.h
//

#ifndef PSEUDOCOMMAND_H  //Required for current class
#define PSEUDOCOMMAND_H

                               //Required for base classes
#include "TCLPackagedObjectProcessor.h"
class CTCLInterpreter;
class CTCLObject;
                               
class CPseudoCommand  : public CTCLPackagedObjectProcessor        
{                       
			
protected:

public:
  // The following table of switches is thinking ahead to future enhancements:

  enum SwitchValue_t {
    SwNotSwitch,
    SwNew,
    SwList,
    SwDelete,
    SwById,
    SwByName			// Add additional switches here.
  };
  
  // Constructors and other cannonical operations:
  
  CPseudoCommand (CTCLInterpreter* pInterp) :
    CTCLPackagedObjectProcessor(*pInterp, "::spectcl::serial::pseudo", true )
  { 
  } 
  virtual ~ CPseudoCommand ( )  // Destructor 
  { }  
  
  //Copy constructor 
private:
  CPseudoCommand (const CPseudoCommand& aCPseudoCommand );
  
  // Operator= Assignment Operator 
  
  CPseudoCommand& operator= (const CPseudoCommand& aCPseudoCommand);
  int operator==(const CPseudoCommand& rhs) const;
  int operator!=(const CPseudoCommand& rhs) const;
public:

  
  // Class operations.  
  
public:
  
  virtual   int operator() (CTCLInterpreter& rInterp, std::vector<CTCLObject>& objv);

  TCLPLUS::Int_t Create (CTCLInterpreter& rInterp, TCLPLUS::UInt_t nArgs, const char** pArgs);

  TCLPLUS::UInt_t List (CTCLInterpreter& rInterp, TCLPLUS::UInt_t nArgs, const char** pArgs);

  TCLPLUS::UInt_t Delete (CTCLInterpreter& rInterp,  TCLPLUS::UInt_t nArgs, const char** pArgs);
  
protected:
  void Usage (CTCLInterpreter& rInterp)   ;
  SwitchValue_t ParseSwitch (const char* pSwitch)   ;
  
private:
  
};

#endif
