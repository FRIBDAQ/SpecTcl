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


//
// Header Files:
//
#include <config.h>
#include "TCLVersionHacks.h"
#include "TCLInterpreter.h"                               
#include "TCLException.h"
#include "TCLList.h"
#include "TCLObject.h"


using namespace std;
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
  return std::string(Tcl_GetStringResult(m_pInterpreter));
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
  return std::string(Tcl_GetStringResult(m_pInterpreter));
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
  return std::string(Tcl_GetStringResult(m_pInterpreter));

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

  int Status = Tcl_RecordAndEval(m_pInterpreter, (char*)pScript, 
				 fEvaluate ? TCL_EVAL_GLOBAL : 
				             TCL_NO_EVAL);

  if(Status != TCL_OK) {
    CTCLException except(*this,
			Status,
			"CTCLInterpreter::RecordAndEval - Evaluating script");
    throw except;
  }

  return std::string(Tcl_GetStringResult(m_pInterpreter));

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

  Tcl_Obj* expressionObject = Tcl_NewStringObj(pExpression, -1);
  Tcl_IncrRefCount(expressionObject); // So I can explicitly kill it.
  Tcl_Obj* resultObject;

  int Status = Tcl_ExprObj(m_pInterpreter, expressionObject, &resultObject);
  Tcl_DecrRefCount(expressionObject); // Destroy expression.

  // Throw an exception on error (e.g. invalid exception).

  if(Status != TCL_OK) {
    string msg = "CTCLInterpreter::ExprString - Evaluating string expression : ";
    msg += pExpression;
    CTCLException e(*this,
		    Status,
		    msg);
	  
    throw e;  
  }

  // Get the string form of the object, kill the reference count,
  // and return the string:

  string result(Tcl_GetString(resultObject));
  Tcl_DecrRefCount(resultObject);

  return result;


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

  Tcl_Obj* expressionObject = Tcl_NewStringObj(pExpression, -1);
  Tcl_IncrRefCount(expressionObject); // So I can explicitly kill it.
  Long_t result;

  int Status = Tcl_ExprLongObj(m_pInterpreter, expressionObject, &result);
  Tcl_DecrRefCount(expressionObject); // Destroy expression.

  // Throw an exception on error (e.g. invalid exception).

  if(Status != TCL_OK) {
    string msg = "CTCLInterpreter::ExprString - Evaluating string expression : ";
    msg += pExpression;
    CTCLException e(*this,
		    Status,
		    msg);
	  
    throw e;  
  }
  return result;


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

  Tcl_Obj* expressionObject = Tcl_NewStringObj(pExpression, -1);
  Tcl_IncrRefCount(expressionObject); // So I can explicitly kill it.
  DFloat_t result;

  int Status = Tcl_ExprDoubleObj(m_pInterpreter, expressionObject, &result);
  Tcl_DecrRefCount(expressionObject); // Destroy expression.

  // Throw an exception on error (e.g. invalid exception).

  if(Status != TCL_OK) {
    string msg = "CTCLInterpreter::ExprString - Evaluating string expression : ";
    msg += pExpression;
    CTCLException e(*this,
		    Status,
		    msg);
	  
    throw e;  
  }
  return result;


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
  Tcl_Obj* expressionObject = Tcl_NewStringObj(pExpression, -1);
  Tcl_IncrRefCount(expressionObject); // So I can explicitly kill it.
  int result;

  int Status = Tcl_ExprBooleanObj(m_pInterpreter, expressionObject, &result);
  Tcl_DecrRefCount(expressionObject); // Destroy expression.

  // Throw an exception on error (e.g. invalid exception).

  if(Status != TCL_OK) {
    string msg = "CTCLInterpreter::ExprString - Evaluating string expression : ";
    msg += pExpression;
    CTCLException e(*this,
		    Status,
		    msg);
	  
    throw e;  
  }


  return (result ? kfTRUE : kfFALSE);
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

  return std::string(Tcl_GetStringResult(m_pInterpreter));

}

////////////////////////////////////////////////////////////////////////
//  The functions below handle inquiry about Tcl Channels.
//  Tcl Channels are abtractions of the system's I/O subsystem
//  that are extensible and operating system independent.
//

/*!
    GetChannel - Given the name of a Tcl Channel registered with 
    this interpreter, returns a Tcl channel handle.  This is a pointer
    to an opaque structure. It can be used to construct a CTCLChannel
    object.

    \param rName   (const string&)
        The name of the channel to lookup.
    \param pMode   (Int_t* [out] == kpNULL
        If non null, the integer pointed to by this will be filed in with
	a mask defining the access mode the channel supports.  This will 
	be the logical or of:
	- TCL_READABLE  you may read the channel.
	- TCL_WRITABLE  you may write the channel.
    \return Tcl_Channel
    \retval NULL   - There is no channel by this name.
    \retval nonnull- Opaque pointer to the channel.
*/
Tcl_Channel
CTCLInterpreter::GetChannel(const string& rName, Int_t* pMode)
{
  return Tcl_GetChannel(m_pInterpreter, (tclConstCharPtr)rName.c_str(), pMode);
}
/*!
   GetChannelNames Given a pattern returns all the names
   of TCL channels that match the pattern.  Note that "*" will
   return the names of all channels. 
  
   \param rPattern (const string&):
      The glob pattern to match.

   \return vector<string>
   \retval Empty vector: no matches.
   \retval Each element of the vector is a channel name.

*/
vector<string>
CTCLInterpreter::GetChannelNames(const string& rPattern)
{
  vector<string> result;

  int status = Tcl_GetChannelNamesEx(m_pInterpreter, (tclConstCharPtr)rPattern.c_str());
  
  // The result contains a TCL list if everything was ok.
  // otherwise it's an error which for now just results in an
  // emtpy channel name list:
  //
  if(status == TCL_OK) {
    Tcl_Obj* pResult = Tcl_GetObjResult(m_pInterpreter);
    if(pResult) {
      char* pList = Tcl_GetStringFromObj(pResult, (Int_t*)kpNULL);
      if(pList) {
	CTCLList ResultList(this, pList);
	ResultList.Split(result);
      }
    }
    
  }

  return result;
}
/*!
    Set the interpreter result from a character string
    \param resultString : const char*
        Pointer to a null terminated string that will be set as the result.
*/
void
CTCLInterpreter::setResult(const char* resultString)
{
  Tcl_Obj* pObject = Tcl_NewStringObj(resultString, -1);
  setResult(pObject);
}
/*!
 Set the result from an std::string:
 \param resultString : std::string
    String to set the result to.
*/
void
CTCLInterpreter::setResult(string resultString)
{
  setResult(resultString.c_str());
}
/*!
   SEt the interpreter result from a Tcl_Obj*
   \param resultObj : Tcl_obj*
      The object to set the result to.
*/
void
CTCLInterpreter::setResult(Tcl_Obj* resultObj)
{

  Tcl_SetObjResult(m_pInterpreter, resultObj);
}
/*!
   SEt the interpreter result from a result object.
   \param resultObj : CTCLObject& 
      Reference to the object containing the result string.
*/
void
CTCLInterpreter::setResult(CTCLObject& resultObj)
{
  setResult(resultObj.getObject());
}

    
/**
 ** Set the interpreter result from an integer.
 ** @param value - integer result to set.
 */
void
CTCLInterpreter::setResult(int value)
{
  CTCLObject result;
  result.Bind(*this);
  result = value;
  setResult(result);
}
