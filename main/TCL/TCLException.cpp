/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

static const char* Copyright = "(C) Copyright Michigan State University 2015, All rights reserved";
//  CTCLException.cpp
// Encapsulates an exception which can be 
// thrown by the TCL interpreter classes.
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

#include <config.h>
#include "TCLException.h"                               
#include "TCLInterpreter.h"
#include <tcl.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Functions for class CTCLException

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void AddErrorInfo ( const char* pMessage )
//  Operation Type:
//     Mutator.
//
void 
CTCLException::AddErrorInfo(const char* pMessage) 
{
// Adds a string to the stack trace being formed
// in the interpreter's errorInfo variable.
//
// Formal Parameters:
//     const char*              pMessage
//     const CTCLString&   rMessage
//     const std::string&      rMessage:
//          The message to add to the string.

  Tcl_Interp* pInterp = getInterpreter()->getInterpreter();
  Tcl_AddErrorInfo(pInterp, (char*)pMessage);

}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void SetErrorCode ( const char* pMessage, const char* pMnemonic="???",
//                        const char* pFacility="TCL", 
//                        const char* pSeverity="FATAL" )
//  Operation Type:
//     Mutator.
//
void 
CTCLException::SetErrorCode(const char* pMessage, const char* pMnemonic, 
			    const char* pFacility, const char* pSeverity) 
{
// Sets the TCL Error code.  In this implementation,
//  the error code is modelled in the following structure:
//         Facility Mnemonic Severity Message
//   These fields are all textual strings.  If any
//    single string is omitted, it is replaced by a
//    suitable default
//     All strings can be supplied in all of the 'normal'
//    types, however all strings must have the same type
//
//       Formal Parameters:
//             Facility = "TCL"
//                 Facility throwing the error.
//             Mnemonic = "???"
//                  Mnemonic for the error message
//             Severity = "FATAL"
//                  Severity of the error.
//              Message
//                   Error message string.

  Tcl_Interp* pInterp = getInterpreter()->getInterpreter();
  Tcl_SetErrorCode(pInterp,
		   pFacility, pSeverity, pMnemonic, pMessage, NULL);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    const char* ReasonText (  )
//  Operation Type:
//     Selector.
//
const  char* 
CTCLException::ReasonText() const
{
// Returns  a pointer to the reason for the
// failure.  This is currently the Result string.
// Traceback etc depend on not doing any other Tcl script eval..

  static   std::string result =  m_ResultText.c_str();
  result += " ";
  result += CException::WasDoing();

  // Get the stack trace.

  Tcl_Interp* pInterp = getInterpreter()->getInterpreter();

  Tcl_Obj* options = Tcl_GetReturnOptions(pInterp, TCL_ERROR);
  Tcl_Obj* key     = Tcl_NewStringObj("-errorinfo", -1);
  Tcl_Obj* stackTrace;
  Tcl_IncrRefCount(key);
  Tcl_DictObjGet(NULL, options, key, &stackTrace);
  
  result += " ";
  result += Tcl_GetStringFromObj(stackTrace, NULL);
  Tcl_DecrRefCount(key);


  return result.c_str();

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t ReasonCode (  )
//  Operation Type:
//     Selector
//
Int_t 
CTCLException::ReasonCode() const 
{
// Returns the reason for the exception
// at present, this is almost always TCL_ERROR
//
// Exceptions:  

  return getReason();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CTCLResult GetResult (  )
//  Operation Type:
//     Selector.
//
CTCLResult 
CTCLException::GetResult()
{
// Returns the result string object.
// associated with this interpreter.
// allows that string to be manipulated
// or examined or both by the client.

  
  CTCLResult result(getInterpreter());
  return result;

}






















