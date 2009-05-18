/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/

#ifndef __CTCLTRACEDVARIABLE_H
#define __CTCLTRACEDVARIABLE_H

#ifndef __CTCLVARIABLE_H
#include <TCLVariable.h>
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

// Forward definitions:

class CTCLInterpreter;
class CVariableTraceCallback;

/*! 
    This class is an extension of the CTCLVariable class designed to make
dealing with tracing somewhat simpler.  The class will contain a 'trace callback'
object which is dispatched to by the trace handler.  Thus, in theory, in stead of
having a ton of derived CTCLVariables, you'll hopefully only have a few 
CVariableTraceCallback objects.  The characteristics of the trace are set up
at construction time and remain fixed for the lifetime of an object.
*/
class CTCLTracedVariable : public CTCLVariable
{
private:
  CVariableTraceCallback*    m_pCallback; //!< the callback object.
public:
  CTCLTracedVariable(CTCLInterpreter*           pInterp,
		  STD(string)                Name,
		  CVariableTraceCallback&    Trace,
		  int                        flags = (TCL_TRACE_READS  |
						      TCL_TRACE_WRITES | 
						      TCL_TRACE_UNSETS));
  virtual ~CTCLTracedVariable();
 
  // Supporting copy like stuff would require that I assume the callback objects
  // support copy construction, and they are beyond my control, so play it safe:
private:
  CTCLTracedVariable(const CTCLTracedVariable& rhs);
  CTCLTracedVariable& operator=(const CTCLTracedVariable& rhs);
  int operator==(const CTCLTracedVariable& rhs) const;
  int operator!=(const CTCLTracedVariable& rhs) const;
public:

  // Selector(s):

  CVariableTraceCallback& getCallback() 
  {
    return *m_pCallback;
  }
  // class functions:

public:
  virtual const char* operator()(char* pName, char* pElement, int flags);

};

#endif
