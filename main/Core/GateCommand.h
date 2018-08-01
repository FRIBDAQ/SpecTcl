/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

// Class: CGateCommand                     //ANSI C++
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

#ifndef GATECOMMAND_H  //Required for current class
#define GATECOMMAND_H

                               //Required for base classes
#include "TCLPackagedCommand.h"
#include <histotypes.h>
#include "GateFactory.h"
#include <string>

class CGatePackage;		// Forward class definition.
class CTCLInterpreter;		// Forward class definition
class CTCLObject;
class CGateObserver;

/*!
  \para Functionality:
 Implements the gate command.
 This command creates gates and enters them
 in the gate dictionary.

 \para Syntax:
    gate name type contents<BR>
             type is one of {cut, band, contour, not, and, or, true, false}
             contents is a list whose structure depends on the gate type:
	     - cut   { parameter { xlow xhigh}}
	     - band  See contour below.
	     - contour { {xpar ypar}  { {x1 y1} {x2 y2} {x3 y3}...}
	     - not     gatename
	     - and,
	     - or       {gate1 gate2 ...}
  <BR>
  gate -list [-byid] <BR>
       Lists gates in dictinonary
  <BR>
  gate -delete [-id] gatelist<BR>
       Replaces gate with deleted gate.
\verbatim
   gate -trace add    ?script?
   gate -trace delete ?script?
   gate -trace change ?script?

\endverbatim
  Where ?script? is a script to run whenthe associated action happens
to the gate dictionary.  The script will have the name of the affected gate
appended to its invocation (yes it should be a proc typically).  If ?script?
is not supplied, the current script is displayed as the command result. 
If ?script? is empty, no  script is associated with the action.

*/
class CGateCommand  : public CTCLPackagedCommand        
{
private:
  // Script handlers.

  CTCLObject*     m_pAddScript;
  CTCLObject*     m_pDeleteScript;
  CTCLObject*     m_pChangeScript;
  CGateObserver*  m_pObserver;

public:				// Data types:
  enum Switches {		//!< Set of command line switches.
    newgate,
    deletegate,
    listgates,
    id,
    byid,
    trace,
    notswitch
  };
  struct GateFactoryTable {	//!< Drives the decoding of gate definition strings.
    const char*                  pGateType; //!< Textual gate type.
    CGateFactory::GateType eGateType; //!< Gate factory gate type enum.
    Bool_t                 fGateList; //!< kfTRUE if list is a list of gates.
    UInt_t                 nParameters; //!< Number of parameters involved.
    Bool_t                 fNoParams;  //!< kfTRUE if gate description has no params
  };
  
public:

   // Constructor 

  CGateCommand (CTCLInterpreter* pInterp, CTCLCommandPackage& rPack);
  ~CGateCommand ( );	// Destructor.

  //Copy constructor alternative to compiler provided default copy constructor
  // Copy construction is illegal:

private:
  CGateCommand (const CGateCommand& aCGateCommand );
public:

   // Operator= Assignment Operator alternative to compiler provided 
   // default operator= (Illegal)

private:
  CGateCommand& operator= (const CGateCommand& aCGateCommand);
  int operator==(const CGateCommand& rhs) const;
  int operator!=(const CGateCommand& rhs) const;
public:

	
  // Class operations:
public:

 virtual   int operator() (CTCLInterpreter& rInterp, CTCLResult& rResult, 
			   int nArgs, char* pArgs[])    ;
  void   invokeAddScript(std::string name);
  void   invokeDeleteScript(std::string name);
  void   invokeChangedScript(std::string name);


protected:

    Int_t NewGate (CTCLInterpreter& rInterp, CTCLResult& rResult, 
		   UInt_t nArgs, char* pArgs[])   ;

  virtual   Int_t ListGates (CTCLInterpreter& rInterp, CTCLResult& rResult, 
			     UInt_t  nArgs, char* pArgs[])   ;

    Int_t DeleteGates (CTCLInterpreter& rInterp, CTCLResult& rRestul, 
		       UInt_t nArgs, char* pArgs[])   ;


protected:
  Int_t  traceGates(CTCLInterpreter& rInterp,
		    CTCLResult&      rResult,
		    UInt_t           nArgs,
		    char*            pArgs[]);
  void   invokeAScript(CTCLObject* pScript,
		       std::string parameter);

  static Switches MatchSwitches(char* pKey);
  static std::string   Usage();
  static GateFactoryTable* MatchGateType(const char* pGateType);

};
#endif
