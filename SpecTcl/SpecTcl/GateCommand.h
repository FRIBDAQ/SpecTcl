// Class: CGateCommand                     //ANSI C++
//
// Implements the gate command.
// This command creates gates and enters them
// in the gate dictionary.
//    gate name type contents
//             type is one of {cut, band, contour, not, and, or, true, false}
//             contents is a list whose structure depends on the gate type:
//                  cut - { parameter { xlow xhigh}}
//                  band
//                  contour { {xpar ypar}  { {x1 y1} {x2 y2} {x3 y3}...}
//                  not     gatename
//                  and,
//                  or       {gate1 gate2 ...}
//  gate -list [-byid]
//       Lists gates in dictinoary
//  gate -delete [-id] gatelist
//       Replaces gate with deleted gate.
//
//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved GateCommand.h
//

#ifndef __GATECOMMAND_H  //Required for current class
#define __GATECOMMAND_H

                               //Required for base classes
#ifndef __TCLPACKAGEDCOMMAND_H
#include "TCLPackagedCommand.h"
#endif
  
#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __GATEFACTORY_H
#include "GateFactory.h"
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

class CGatePackage;		// Forward class definition.
class CTCLInterpreter;		// Forward class definition

class CGateCommand  : public CTCLPackagedCommand        
{                       
public:			// Data types:
  enum Switches {		// Set of command line switches.
    newgate,
    deletegate,
    listgates,
    id,
    byid,
    notswitch
  };
  struct GateFactoryTable {	// Drives the decoding of gate definition strings.
    char*                  pGateType; // Textual gate type.
    CGateFactory::GateType eGateType; // Gate factory gate type enum.
    Bool_t                 fGateList; // kfTRUE if list is a list of gates.
    UInt_t                 nParameters; // Number of parameters involved.
    Bool_t                 fNoParams;  // kfTRUE if gate description has no params
  };
  
public:

   // Constructor 

  CGateCommand (CTCLInterpreter* pInterp, CTCLCommandPackage& rPack):
    CTCLPackagedCommand("gate", pInterp, rPack)
  {  } 

   ~CGateCommand ( ) { }	// Destructor.

  //Copy constructor alternative to compiler provided default copy constructor
  // Copy construction is illegal:

private:
  CGateCommand (const CGateCommand& aCGateCommand );
public:

   // Operator= Assignment Operator alternative to compiler provided 
   // default operator= (Illegal)

private:
  CGateCommand& operator= (const CGateCommand& aCGateCommand);
public:

   //Operator== Equality Operator - Doesn't make sense, but allowed.

  int operator== (const CGateCommand& aCGateCommand) const {
    return CTCLPackagedCommand::operator==(aCGateCommand);
  }
	
  // Class operations:
public:

 virtual   int operator() (CTCLInterpreter& rInterp, CTCLResult& rResult, 
			   int nArgs, char* pArgs[])    ;
 
protected:

    Int_t NewGate (CTCLInterpreter& rInterp, CTCLResult& rResult, 
		   UInt_t nArgs, char* pArgs[])   ;

  virtual   Int_t ListGates (CTCLInterpreter& rInterp, CTCLResult& rResult, 
			     UInt_t  nArgs, char* pArgs[])   ;

    Int_t DeleteGates (CTCLInterpreter& rInterp, CTCLResult& rRestul, 
		       UInt_t nArgs, char* pArgs[])   ;

protected:
  static Switches MatchSwitches(char* pKey);
  static string   Usage();
  static GateFactoryTable* MatchGateType(const char* pGateType);
};
#endif
