/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/
#include <config.h>
#include "TCLTracedVariable.h"
#include "VariableTraceCallback.h"
#include "TCLInterpreter.h"

using namespace std;

/*!
  Construct a traced variable object.  This involves:
  - Constructing the base class.
  - Initializing the callback data element.
  - Setting up the trace characteristics requested by the user.

  \param pInterp (CTCLInterpreter*)
      The TCL interpreter on which this variable was created.
  \param Name (string):
      The name of the variable.  If the variable is an array element, use
      TCL dereference format (e.g. A(BCDE)).
  \param traceback (CVariableTraceCallback&)
      Reference to the variable trace callback function that will be called
      when the trace fires.
  \param flags (int):
      A bitwise or of bits indicating what should fire a trace:
      - TCL_TRACE_READS  - Read accesses fire the trace.
      - TCL_TRACE_WRITES - Write accesses fire the trace.
      - TCL_TRACE_UNSETS - unset var  fires the trace.
*/
CTCLTracedVariable::CTCLTracedVariable(CTCLInterpreter*        pInterp,
				 string                  Name,
				 CVariableTraceCallback& traceback,
				 int                     flags) :
  CTCLVariable(pInterp, Name, kfFALSE),
  m_pCallback(&traceback)
{
  Trace(flags, NULL);
}
/*!
  Destructor... Since we are going out of scope we need to first
  untrace.
*/
CTCLTracedVariable::~CTCLTracedVariable()
{
  UnTrace();
}
/*!
  Function call operator.. This is called in response to the 
  trace fire.  
  \param pName  (char*)
     Pointer to the name of the variable that traced. This can differ
     from the name of the variable because if the variable was an array element,
     this will be the array's base name.
   \param pElement (char*)
     Pointer to the name of the array element or NULL If not an array element.
   \param flags (int):
     The flag bit indicating which trace fired.
   \return char*
   \retval  NULL - no errors.
   \retval  not null - Pointer to an error message string, the access will fail
            with an error, and this string will represent the error

*/
char*
CTCLTracedVariable::operator()(char* pName, char* pElement, int flags)
{
  return (*m_pCallback)(getInterpreter(), pName, pElement, flags);
}
