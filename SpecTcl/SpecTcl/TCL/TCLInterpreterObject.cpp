//  CTCLInterpreterObject.cpp
//  Base class of objects which cannot exist without
// a TCL Intpreter to provide them with services.
// The usual function of the TCL interpreter object is
// to provide Tcl interpreter pointers which can then
// be passed in to Tcl_* functions.

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


#include "TCLInterpreterObject.h"                               


static const char* Copyright = 
"CTCLInterpreterObject.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CTCLInterpreterObject

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CTCLInterpreter* Bind ( CTCLInterpreter* pBinding )
//  Operation Type:
//     Connection
//
CTCLInterpreter* 
CTCLInterpreterObject::Bind(CTCLInterpreter* pBinding) 
{
// Binds the object to a TCL interpreter object.
// The prior object is returned, or NULL if there was
// no prior binding.
// 
// Formal Parameters:
//    CTCLInterpreter* pBinding:
//       New binding to apply to the interpreter.
// Exceptions:  

  CTCLInterpreter*  pPrior = m_pInterpreter;
  m_pInterpreter =  pBinding;
  return            pPrior;
}
