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

//  CTCLInterpreter.h:
//
//    This file defines the CTCLInterpreter class.
//    Most of the manipulator functions follow quite closely the calling
//    sequences of the TCL functions as described in 
//     "TCL and the TK Toolkit" by Ousterhout... .with the interpreter 
//    parameter taken from the object's attribute.  Refer to that, or another
//    TCL book for documentation on the purpose and parameters of each function
//
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



#ifndef __TCLInterpreter_H  //Reuired for current class
#define __TCLInterpreter_H

#ifndef __TCL_H
#include <tcl.h>
#define __TCL_H
#endif          




#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

#ifndef __TCLSTRING_H
#include "TCLString.h"
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

class CTCLObject;

                     
class CTCLInterpreter      
{
  Tcl_Interp* m_pInterpreter;  // // Pointer to the interpreter.
  
public:
			//Default constructor - Create new interpreter.

  CTCLInterpreter () :  
    m_pInterpreter(Tcl_CreateInterp())   { } 
  virtual  ~CTCLInterpreter ( ) { // Destructor.
    Tcl_DeleteInterp(m_pInterpreter);
  }

			//Constructor with arguments - Bind to existing interp.
  CTCLInterpreter (  Tcl_Interp* am_pInterpreter  )  :
    m_pInterpreter (am_pInterpreter)  { }        
	
			//Copy constructor -- not allowed.
private:
  CTCLInterpreter (const CTCLInterpreter& aCTCLInterpreter ) ;
public:
			//Operator= Assignment Operator -- not allowed
private:
  CTCLInterpreter operator= (const CTCLInterpreter& aCTCLInterpreter);
public:

			//Operator== Allowed but not so useful.
			//Update to access 1:M associated class attributes      
  int operator== (const CTCLInterpreter& aCTCLInterpreter)
  { 
    return (
	    (m_pInterpreter == aCTCLInterpreter.m_pInterpreter) 
	    );
  }                             
  // Selectors -- note that to allow these objects to interact with other
  //              objects and C code, the read selector for m_pInterpreter
  //              doesn't return a const pointer.
public:
  Tcl_Interp* getInterpreter()
  {
    return m_pInterpreter;
  }
  // Mutator - Allowed only be derived classes:
protected:
  void setInterpreter (Tcl_Interp* am_pInterpreter)
  { 
    m_pInterpreter = am_pInterpreter;
  }
  //  The remainder of the operations are actual TCL interfaces through the
  //  interpreter encpsulated by this class.

public:                       
  // Evaluate script from a string:

  STD(string) Eval (const char* pScript) ;
  STD(string) Eval(const CTCLString& rScript)  {
    return Eval((const char*)rScript);
  }
  STD(string) Eval(const STD(string)& rScript)  {
    return Eval(rScript.c_str()); 
  }
  //
  // Evaluate a scripts stored in a file.
  //
  STD(string) EvalFile (const char* pFilename)   ;
  STD(string) EvalFile(const CTCLString& rFilename)  {
    return EvalFile((const char*)(rFilename));
  }
  STD(string) EvalFile(const STD(string)& rFilename)  {
    return EvalFile(rFilename.c_str());
  }
  //  Evaluate a script at the global level (stack empty context).
  //
  STD(string) GlobalEval (const char* pScript)   ;
  STD(string) GlobalEval (const CTCLString& rScript)  {
    return GlobalEval((const char*)rScript);
  }
  STD(string) GlobalEval(const STD(string)& rScript)  {
    return GlobalEval(rScript.c_str());
  }
  // Evaluate a script and store it it's commands in the 
  // history list for later retrieval
  //
  STD(string) RecordAndEval (const char* pScript, Bool_t fEval=kfFALSE);
  STD(string) RecordAndEval(const CTCLString& rScript, 
			    Bool_t fEval=kfFALSE)  {
    return RecordAndEval((const char*)(rScript), fEval);
  }
  STD(string) RecordAndEval(const STD(string)& rScript,
			    Bool_t fEval = kfFALSE)  {
    return RecordAndEval(rScript.c_str(), fEval);
  }
  //
  // Evaluate and expression with a string result:
  //
  STD(string) ExprString (const char* pExpression)   ;
  STD(string) ExprString(const CTCLString& rExpr)  {
    return ExprString((const char*)(rExpr));
  }
  STD(string) ExprString(const STD(string)& rExpr)  {
    return ExprString(rExpr.c_str());
  }
  //  Evaluate string 
  //
  Long_t ExprLong (const char* pExpression)   ;
  Long_t ExprLong (STD(string)& rExpression) {
    return ExprLong(rExpression.c_str());
  }
  Long_t ExprLong (const CTCLString& rExpr)  {
    return ExprLong((const char*)(rExpr));
  }
  // Evaluate a double:
  //
  DFloat_t ExprDouble (const char* pExpression)   ;
  DFloat_t ExprDouble (const CTCLString& rExpression)  {
    return ExprDouble ((const char*)(rExpression));
  }
  DFloat_t ExprDouble(const STD(string)& rExpression)  {
    return ExprDouble(rExpression.c_str());
  }
  // Evaluate a boolean:
  //
  Bool_t ExprBoolean (const char*  pExpression)   ;
  Bool_t ExprBoolean (const CTCLString& rExpression)  {
    return ExprBoolean((const char*)(rExpression));
  }
  Bool_t ExprBoolean(const STD(string)& rExpression)  {
    return ExprBoolean(rExpression.c_str());
  }
  // Substitute for tilde in some filename.
  // 
  STD(string) TildeSubst (const char* pFilename) const  ;
  STD(string) TildeSubst (const CTCLString& rName) const {
    return TildeSubst((const char*)(rName));
  }
  STD(string) TildeSubst (const STD(string)& rName) const {
    return TildeSubst(rName.c_str());
  }
  
  STD(string) PosixError () const  ;
  static  STD(string) ErrnoId ()   ;
  static  STD(string) SignalId (UInt_t nSignal)   ;
  static  STD(string) SignalMsg (UInt_t nSignal)   ;
  void DetachProcess (const STD(vector)<UInt_t>& rPids) const  ;
  void DetachProcess(UInt_t nPids, Int_t* pPids) const {
    Tcl_DetachPids(nPids, (Tcl_Pid*)pPids);
  }
  void ReapDetachedProcesses () const {
    Tcl_ReapDetachedProcs();
  }
  void AddCommand (const char* pCommandName, 
		   Tcl_CmdProc* pCommandProcessor, 
		   ClientData pData, 
		   Tcl_CmdDeleteProc* pDeleteProcessor=
		                       (Tcl_CmdDeleteProc*)0) const  ;
  void AddCommand(const STD(string)& rCommandName,
		  Tcl_CmdProc* pCommandProcessor,
		  ClientData pData,
		  Tcl_CmdDeleteProc* pDeleteProcessor = 
		                    (Tcl_CmdDeleteProc*)0) const {
    AddCommand(rCommandName.c_str(), pCommandProcessor, pData, 
	       pDeleteProcessor);
  }
  void AddCommand(const CTCLString& rCommandName,
		  Tcl_CmdProc* pCommandProcessor,
		  ClientData pData,
		  Tcl_CmdDeleteProc* pDeleteProcessor = 
		                       (Tcl_CmdDeleteProc*)0) const {
    AddCommand((const char*)(rCommandName), pCommandProcessor,
	       pData, pDeleteProcessor);
  }
  void UnregisterCommand (const char* pCommandName) const  ;
  void UnregisterCommand (const STD(string)& rCommandName) const {
    UnregisterCommand(rCommandName.c_str());
  }
  void UnregisterCommand (const CTCLString& rCommandName) const {
    UnregisterCommand((const char*)(rCommandName));
  }
  STD(string) GetResultString () const  ;

  // Access to Tcl Channels inquiry:
  //
  Tcl_Channel GetChannel(const STD(string)& rName, Int_t* pMode=(Int_t*)kpNULL);
  STD(vector)<STD(string)> GetChannelNames(const STD(string)& rPattern);


  //
  // These two support access to the interpreter:
  //
  Tcl_Interp* operator-> ()	// Access interp->struct elements.
  { return m_pInterpreter; }
  //
  // Supports casts to Tcl_Interp*
  //
  operator Tcl_Interp* () { 
    return m_pInterpreter; 
  }
  // Modern support to set the interpreter result:

  void setResult(const char* resultString);
  void setResult(STD(string) resultString);
  void setResult(Tcl_Obj*    resultObj);
  void setResult(CTCLObject& resultObj);
};



#endif
