//  CTCLResult.cpp
// Encapsulates the result protocol
// as an object bound to a specific TCL interpreter.
// Note that while there can be several result objects
// bound to the same interpreter, the nature of Tcl implies
// that they will all be manipulating the same result.
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


#include "TCLResult.h"                               
#include "TCLString.h"
#include "TCLInterpreter.h"
#include <tcl.h>
#include <histotypes.h>
#include <assert.h>

static const char* Copyright = 
"CTCLResult.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CTCLResult

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CTCLResult& operator= ( const char* pString )
//  Operation Type:
//     Assignment operator
//
CTCLResult& 
CTCLResult::operator=(const char* pString) 
{
// Assigns the value of a string to the result.
// 
// Formal Parameters:
//      const char*         pString,
//      const std::string& rString
//          Ways to specify the new result string.
// NOTE:
//     Tcl_SetResult is called with a deletproc of TCL_VOLATILE
//     to force a copy of the string to be made.
//
// Exceptions:  

  CTCLInterpreter* pInterp = AssertIfNotBound();

  Tcl_SetResult(pInterp->getInterpreter(), (char*)pString, TCL_VOLATILE);

  return *this;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CTCLResult& operator+= ( const char* pString )
//  Operation Type:
//     Append operator
//
CTCLResult& 
CTCLResult::operator+=(const char* pString) 
{
// Appends a string to the result string.
// 
// Formal Parameters:
//     const char*         pString,
//     const std::string& rString:
//        Different ways to pass the string.
// Returns 
//    Reference to self to support chaining.
//

  CTCLInterpreter* pInterp = AssertIfNotBound();

  Tcl_AppendResult(pInterp->getInterpreter(), pString, (char*)kpNULL);

  return *this;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Clear (  )
//  Operation Type:
//     Reinitializer
//
void 
CTCLResult::Clear() 
{
// Clears the current contents of the result string.

  CTCLInterpreter* pInterp = AssertIfNotBound();

  Tcl_ResetResult(pInterp->getInterpreter());

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void AppendElement ( const char* pString )
//  Operation Type:
//     Appender.
//
void 
CTCLResult::AppendElement(const char* pString) 
{
// Appends the input string to the result as a list element.
//
// Formal Parameters:
//      const char*         pString,
//      const std::string& rString
//         Different ways to specify the input string.
// 

  CTCLInterpreter* pInterp = AssertIfNotBound();

  Tcl_AppendElement(pInterp->getInterpreter(),  (char*)pString);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//     operator char* (  )
//  Operation Type:
//     Type conversion
//
 
CTCLResult::operator const char*() 
{
  CTCLInterpreter* pInterp = AssertIfNotBound();
  Tcl_Interp* pI = pInterp->getInterpreter();

  return pI->result;
  

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//      operator std::string (  )
//  Operation Type:
//     Type conversion
//
  
CTCLResult::operator std::string() 
{
// Returns an STL string containing a copy
// of the result string.

  return std::string(operator const char*());

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//      operator CTCLString (  )
//  Operation Type:
//     Type conversion
//
  
CTCLResult::operator CTCLString() 
{
// Return the result as a CTCLString class
//

  return CTCLString(operator const char*());

}
