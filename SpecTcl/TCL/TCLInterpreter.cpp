//  CTCLInterpreter.cpp
// Encapsulates a TCL Command interpreter.
// in an object oriented way.  Note that many
// of the TCL functions have been split off
// into the TCLInterpreterObjects.  These are
// classes which implement functionally distinct
// segments of TCL functionality.
//
//  This implementation is based on the documentation
// in parts III and IV of _Tcl_and_the_Tk_Toolkit_
//  by J. K. Ousterhout, Addison Wesley 1994 
// ISBN 0-201-63337-x
//
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//

#include "TCLInterpreter.h"                               
#include "TCLException.h"

static const char* Copyright = 
"CTCLInterpreter.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CTCLInterpreter

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string Eval ( const char* pScript )
//  Operation Type:
//     Script evaluation.
//
std::string 
CTCLInterpreter::Eval(const char* pScript) 
{
// Evaluates a TCL script contained in any of several types
// of strings. If the evaluation was successful, the result
// string is returned.  If not, then a CTCLException is
// thrown.
//
// Formal Parameters:
//      The following represent a set of
//      alternative representations of the
//      script variable type:
//          const char*            pScript
//          const CTCLString& rScript
//          const std::string&    rScript
//               String containing the script.
// Returns:
//    Result of the evaluation.

  int Status = Tcl_Eval(m_pInterpreter, (char*)pScript);
  
  if(Status != TCL_OK) {
    throw CTCLException(*this,
			Status,
			"CTCLInterpreter::Eval - Evaluating expression");
  }
  return std::string(m_pInterpreter->result);
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string EvalFile ( const char* pFilename )
//  Operation Type:
//     Script Evaluation
//
std::string 
CTCLInterpreter::EvalFile(const char* pFilename) 
{
// Evaluates the script contained in the file.
//  If successful, returns the string result of
//  the evaluation. If failed, throws a CTCLException
// 
// Formal Parameters:
//      Below are alternative representations of
//      the filename:
//
//       const char*            pFilename
//       const CTCLString& rFilename
//       const std::string&    rFilename:
//            Name of the file contaning
//            a script to evaluate.
//  Returns:
//     Result string of the script..
//     the result string has the error on failure.

  int Status = Tcl_EvalFile(m_pInterpreter, (char*)pFilename);
  if(Status != TCL_OK) {
    throw CTCLException(*this,
			Status,
			"CTCLInterpreter::EvalFile - Evaluating file script");;
  }
  return std::string(m_pInterpreter->result);
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string GlobalEval ( const char* pScript )
//  Operation Type:
//     script evaluator
//
std::string 
CTCLInterpreter::GlobalEval(const char* pScript) 
{
// Evaluates a script at the global level.
// See Eval for parameters etc.

  int Status = Tcl_GlobalEval(m_pInterpreter, (char*)pScript);
  
  if(Status != TCL_OK) {
    CTCLException except(*this,
			Status,
			"CTCLInterpreter::Eval - Evaluating expression");
    throw except;
  }
  return std::string(m_pInterpreter->result);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string RecordAndEval ( const char* pScript )
//  Operation Type:
//     script evaluation
//
std::string 
CTCLInterpreter::RecordAndEval(const char* pScript, Bool_t fEvaluate) 
{
// Records and optionally evaluates
//  the script parameter in the TCL History list.
//  
// Formal Parameters:
//        <The usual alternatives for strings which
//         contain the script>
//
//     Bool_t fEvaluate:
//           kfTRUE to evaluate the script prior to insertion.
//  Returns:
//      The results of the script on success or
//       throws an exception on error.

  int Status = Tcl_RecordAndEval(m_pInterpreter, (char*)pScript, fEvaluate);
  if(Status != TCL_OK) {
    CTCLException except(*this,
			Status,
			"CTCLInterpreter::RecordAndEval - Evaluating script");
    throw except;
  }

  return std::string(m_pInterpreter->result);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string ExprString ( const char* pExpression )
//  Operation Type:
//     Expression evaluation
//
std::string 
CTCLInterpreter::ExprString(const char* pExpression) 
{
// Evaluates an expression as a string.  The
// result of the evaluation is stored in the
// interpreter result string.
//
// Formal Parameters:
//     <The usual choices for strings>
// Returns:
//    The string.  On failure to evaluate,  a
//    CTCLException is thrown.

  int Status = Tcl_ExprString(m_pInterpreter, (char*)pExpression);
  if(Status != TCL_OK) {
    CTCLException e(*this,
		    Status,
	  "CTCLInterpreter::ExprString - Evaluating string expression");
    throw e;
  }
  return std::string(m_pInterpreter->result);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Long_t ExprLong ( const char* pExpression )
//  Operation Type:
//     Expression Evaluation
//
Long_t 
CTCLInterpreter::ExprLong(const char* pExpression) 
{
// Evaluates the specified string as an expression
// returning a longword value. If unable to evaulate
// to a long, then a CTCLException is thrown indicating
// why the failure occured, and the result string
// is set to an error message.
//
// Formal Parameters:
//    A string containing the expression
//   represented in any of the usual ways.
//
// Returns:
//   The longword value evaluated.
  Long_t l;
  int    Status =  Tcl_ExprLong(m_pInterpreter, 
				(char*)pExpression,
				&l);
  if(Status != TCL_OK) {
    CTCLException e(*this,
		    Status,
		    "CTCLInterpreter::ExprLong - Evaluating long expression");
    throw e;
  }
  return l;


}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    DFloat_t ExprDouble ( const char* pExpression )
//  Operation Type:
//     Expression evaluator
//
DFloat_t 
CTCLInterpreter::ExprDouble(const char* pExpression)
{
// Evaluates a string as an expression
// returning a floating point value.
// If the evaluation fails, then an exception
// is returned.   If the evaulation succeeds,
// the value is returned.
//
// Formal Parameters:
//    The usual assortment of string types
//     representing the expression.
//
// Returns:
//   The floating point value of the expression
//
  DFloat_t f;
  Int_t    Status = Tcl_ExprDouble(m_pInterpreter, 
				   (char*)pExpression,
				   &f);
  if(Status != TCL_OK) {
    CTCLException e(*this,
		    Status,
		    "CTCLInterpreter::ExprDouble - Evaluating dbl expression");
    throw e;
  }
  return f;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t ExprBoolean ( const char*  pExpression )
//  Operation Type:
//     Expression Evaluation
//
Bool_t 
CTCLInterpreter::ExprBoolean(const char*  pExpression) 
{
// Evaluates a character string as a
// boolean value.  The value of the
// bool is returned.  If the evaluation
// fails, a CTCLException is thrown.
//
// Formal Parameters:
//     The expression string expressed
//      in any of the assorted available string
//      types.
// Returns:
//    The value of the expression.

  Int_t flag;
  int   Status = Tcl_ExprBoolean(m_pInterpreter, (char*)pExpression, &flag);
  if(Status != TCL_OK) {
    CTCLException e(*this,
		    Status,
		    "CTCLInterpreter::ExprBoolean - Evaluating boolean expr.");
    throw e;
  }
  return (flag ? kfTRUE : kfFALSE);
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std:string TildeSubst ( const char* pFilename )
//  Operation Type:
//     parse
//
std::string 
CTCLInterpreter::TildeSubst(const char* pFilename) const 
{
// Provides support for filenames which begin with ~ 
// ~user/ by substituting this with the appropriate
//  path prefix.  If an error occurs during the subsititution
//  (e.g. user doesn't exist?) a CTCLException is thrown
//
// Formal Parameters:
//    The filename in any of the three acceptable
//     string types.
// Returns:
//     std::string - filename with the name appropriately
//     substituted.

  Tcl_DString result;
  char*       pResult = Tcl_TildeSubst(m_pInterpreter,
				       (char*)pFilename,
				       &result);
  std::string stdResult(pResult);
  Tcl_DStringFree(&result);
  return stdResult;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string PosixError (  )
//  Operation Type:
//     POSIX interface
//
std::string 
CTCLInterpreter::PosixError() const 
{
// Sets the interpreter error code with a string 
// that makes sense given the current value of
// errno, and returns a string identifying the
// error (strerror()??).

  return std::string(Tcl_PosixError(m_pInterpreter));

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string ErrnoId (  )
//  Operation Type:
//     POSIX interface
//
std::string 
CTCLInterpreter::ErrnoId()  
{
// Returns a symbolic string identifying the
// error currently stored in errno e.g. EBADF

  return std::string(Tcl_ErrnoId());

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string SignalId ( UInt_t nSignal )
//  Operation Type:
//     POSIX interface
//
std::string 
CTCLInterpreter::SignalId(UInt_t nSignal) 
{
// Returns a string identifying a signal
// e.g. SIGINT
//
// Formal parameters:
//   UInt_t nSignal:
//      The signal to lookup.

  return std::string(Tcl_SignalId((Int_t)nSignal));

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string SignalMsg ( UInt_t nSignal )
//  Operation Type:
//     POSIX interface.
//
std::string 
CTCLInterpreter::SignalMsg(UInt_t nSignal) 
{
// Returns a human readable message which
// identifies the signal
//
// Formal Parameters:
//    UInt_t nSignal:
//      Signal to identify

  return std::string(Tcl_SignalMsg(nSignal));

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void DetachProcess ( const std::vector>UInt_t>& rPids )
//  Operation Type:
//     POSIX interface
//
void 
CTCLInterpreter::DetachProcess(const std::vector<UInt_t>& rPids) const 
{
// Passes responsibility for a set of process ids to
// TCL's zombie process reaper functions  On error
// will throw the appropriate CTCLException..
//
// Formal Parameters:
//        one of the following:
//          const std::vector<UInt_t>& rPids:
//          const UInt_t nPids, const UInt_t* pPids:
//              where:
//                      nPids is the number of Process id's
//                      pointed to by pPids.
//                      pPids points to an array of process ids,
//                      and rPids refers to a vector of process ids.
//   

  UInt_t nPids = rPids.size();
  Int_t  *pPids= new Int_t[nPids];
  for(int i = 0; i < nPids; i++) {
    pPids[i] = rPids[i];
  }
  Tcl_DetachPids(nPids, (Tcl_Pid*)pPids);
  delete []pPids;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void AddCommand ( const char* pCommandName, 
//                      Tcl_CmdProc* pCommandProcessor, ClientData pData, 
//                      Tcl_CmdDeleteProc* pDeleteProcessor=kpNULL )
//  Operation Type:
//     Legacy bridge
//
void 
CTCLInterpreter::AddCommand(const char* pCommandName, 
			    Tcl_CmdProc* pCommandProcessor, 
			    ClientData pData, 
			    Tcl_CmdDeleteProc* pDeleteProcessor) const 
{
// Adds a non-object oriented command
// to the interpreter.  Object oriented commands
// are represented by CTCLProcessor objects which
// contain members which can register them on one or
// more interpreters.   This function is intended to
// support legacy C language command interpreters
// which might be used in the TCL++ environment.
//
// Formal Paramters:
//     const char*            pCommandName
//     const CTCLString& rCommandName
//     const std::string&    rCommandName:
//            Name of the command to register.
//     Tcl_CmdProc* pCmdProcessor:
//            Pointer to command processing function
//     ClientData       pClientData:
//            Data passed without interpretation to the cmd processor.
//      Tcl_CmdDeleteProcI pDeleteProcessor = kpNULL:
//            Pointer to function called if command is deleted from an interpreter.
//
//             
// Exceptions:  

  Tcl_CreateCommand(m_pInterpreter,
		    (char*)pCommandName,
		     pCommandProcessor,
		    pData,
		    pDeleteProcessor);
  
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void UnregisterCommand (  )
//  Operation Type:
//     Legacy bridge
//
void 
CTCLInterpreter::UnregisterCommand(const char* pCommand) const 
{
// Intended to support the deletion of
// commands processed by C language functions
// This member will remove a command if it exists or
// throw an exception if it does not.
//
// Formal Parameters:
//       const char*             pCommand
//       const CTCLString&  rCommand
//       const std::string&     rCommand:
//             The command to delete.
// NOTE:
//    The command's deletion processor will
//  be called by TCL in the event one was
//  established.

  Tcl_DeleteCommand(m_pInterpreter, (char*)pCommand);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string GetResultString (  )
//  Operation Type:
//     Selector
//
std::string 
CTCLInterpreter::GetResultString() const 
{
// Returns a string containing the TCL result string
// if no string has been set, an empty string is returned.

  return std::string(m_pInterpreter->result);

}

