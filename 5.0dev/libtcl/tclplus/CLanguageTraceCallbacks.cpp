/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/
#include <config.h>
#include "CLanguageTraceCallbacks.h"
#include "TCLInterpreter.h"

using namespace std;

/*!
   Constrution just means save the parameters in the member data:
*/
CLanguageTraceCallback::CLanguageTraceCallback(VariableTraceCallback* pCallback,
					       void*                  pClientData) :
  m_pCallback(pCallback),
  m_pClientData(pClientData)
{
}
/*!
   Destructor is noop.
*/
CLanguageTraceCallback::~CLanguageTraceCallback() {}
/*!
   Function call operator... called for a trace.  We delegate to the
   callback.
   \param pInterp (CTCLInterpreter*)
       Pointer to the interpreter object that is calling us.
   \param pVariable (char*):
       Pointer to the name of the variable or basename of the array that
       fired the trace.
   \param pElement (char*)
       If the trace was fired on an array element this is the index, otherwise this
       will be null.
   \param flags (int):
       A bitmask describing why the traace fired.

     \note If the user tried to trick us by providing a null function pointer,
     we raise that as a TCL error.
*/
char*
CLanguageTraceCallback::operator()(CTCLInterpreter* pInterp,
				   char*            pVariable, 
				   char*            pElement, 
				   int              Flags)
{
  if(m_pCallback) {
    return  (*m_pCallback)(pInterp->getInterpreter(), 
			   pVariable, pElement, 
			   Flags, m_pClientData);
  } 
  else {
    return const_cast<char*>("CLanguageTraceCallback::operator() invoked with null function ptr");
  }
}
