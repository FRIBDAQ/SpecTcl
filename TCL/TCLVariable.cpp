//  CTCLVariable.cpp
// Encapsulates a TCL Variable.  TCL Variables are string valued.
// Their contents can be gotten or set.  in addition, by deriving
// from this class, overriding operator(), and invoking Trace()
// Action can be taken each time a variable is to be set.
// Finally, the value of a variable can be bound to other
// storage
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

#include "TCLVariable.h"                               
#include "TCLInterpreter.h"
#include <tcl.h>
#include <histotypes.h>
#include <assert.h>

static const char* Copyright = 
"CTCLVariable.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CTCLVariable

CTCLVariable::~CTCLVariable ( ) {
  m_sVariable = "";
  if(m_fTracing) UnTrace();
}	


//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    char*  operator() ( char* pName, char* pSubscript, int Flags )
//  Operation Type:
//     Trace handler.
//
char*  
CTCLVariable::operator()(char* pName, char* pSubscript, int Flags) 
{
// Called when trace is enabled on a variable.
// This function is called either just before returning
//  the variable's value on read
//  or just after setting the value on write.
//  
// Formal Parameters:
//    char* pName:
//        Name of the variable being modified.
//   char* pSubscript:
//       Subscript of the variable, if the var is an array, or NULL
//       if not.
//   Int_t Flags:
//      One or more of the following bits:
//         TCL_TRACE_READS      - If Read invoked the trace.
//         TCL_TRACE_WRITES     - If Write invoked the trace.
//         TCL_TRACE_UNSET      - If an unset invoked the trace.
//         TCL_GLOBAL_ONLY      - If the variable is global and not
//                                reachable from the current script
//                                execution context.
//         TCL_TRACE_DESTROYED  - variable is being UNSET only.
//         TCL_INTERP_DESTROYED - Interpreter which owns the variable
//                                is being destroyed don't call it.
// NOTE:
//   Default implementation of this function is a no-op.
//
// Returns:
//    NULL If all is ok else a pointer to a static character string describing
//   an error.
//

  return "Unimplemented variable trace.";

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    char* TraceRelay ( ClientData pObject, Tcl_Interp* pInterpreter, 
//                       char* pName, char* pIndex, int flags )
//  Operation Type:
//     contextualizer
//
char* 
CTCLVariable::TraceRelay(ClientData pObject, Tcl_Interp* pInterpreter, 
			 char* pName, 
			 char* pIndex, 
			 int flags) 
{
// This is the actual trace function set by the Trace() method.
//  We use the ClientData to establish object context and
//  invoke the operator() virtual member.  
// 
// Formal Parameters:
//      ClientData   pObject:
//          Actually a pointer to the object whose operator() is
//          invoked.
//      Tcl_Interp* pInterp:
//          Interpreter in which context the variable is being affected.
//          note that this function will. It is an assertion failuerable
//          offense for this interpreter not to match the interpreter
//          the variable is bound to.
//
//      char* pName:
//           Name of the variable being traced.
//      char* pSubscript:
//           If the variable is an array, the index being traced.
//       int flags:
//           Tcl flags, see operator() for values.
// 

  CTCLVariable*     pVariable = (CTCLVariable*)pObject;
  CTCLInterpreter*  pInterp   = pVariable->AssertIfNotBound();

  assert(pInterp->getInterpreter() == pInterpreter);

  return pVariable->operator()(pName, pIndex, flags);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    char* Set ( char* pValue, int flags=TCL_LEAVE_ERR_MSG )
//  Operation Type:
//     modifier
//
const char* 
CTCLVariable::Set(const char* pValue, int flags) 
{
// Sets the variable with a new value.
// 
// Formal Parameters:
//       char* pNewValue:
//           New value for the variable.
//       int flags = TCL_LEAVE_ERR_MSG:
//           set flags which can be any of:
//              TCL_GLOBAL_ONLY      - Reference global regardless of context.
//              TCL_LEAVE_ERR_MSG - leave error message in result if fails.
//              TCL_APPEND_VALUE    - Append value to existing string.
//              TCL_LIST_ELEMENT      - Treat value as list element. 
//  Returns:
//      NULL - success
//      pointer to static error message otherwise.

  CTCLInterpreter* pInterp = AssertIfNotBound();

  return Tcl_SetVar(pInterp->getInterpreter(), 
		    (char*)(m_sVariable.c_str()), (char*)pValue, flags);


}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    char* Set ( char* pSubscript, char* pValue, int flags=TCL_LEAVE_ERR_MSG )
//  Operation Type:
//     Modifier
//
const char* 
CTCLVariable::Set(const char* pSubscript, char* pValue, int flags) 
{
// See Set(char* pName, int flags) for most of the details, this function
// accepts an additional string parameter (first one) which is either the index
// to the variable as an array or a NULL if the variable is to be treated as 
//  a scalar.

  CTCLInterpreter* pInterp = AssertIfNotBound();
  
  return Tcl_SetVar2(pInterp->getInterpreter(),
		     (char*)(m_sVariable.c_str()), (char*)pSubscript, 
		     (char*)pValue, flags);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    char* Get ( int flags=TCL_LEAVE_ERR_MSG, char* pIndex=0 )
//  Operation Type:
//     Interogatory
//
const char* 
CTCLVariable::Get(int flags, char* pIndex) 
{
// Returns the value of a variable.   NULL is returned on
// error.
//
// Formal Parameters:
//     int flags:
//        flag values:
//        See Set().
//     char* pIndex=0
//          If variable is an array, this specifies the index.
//
  CTCLInterpreter* pInterp = AssertIfNotBound();

  const char* p = Tcl_GetVar2(pInterp->getInterpreter(),
			      (char*)(m_sVariable.c_str()), pIndex, flags);
  return p;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Link ( void* pVariable, int Type )
//  Operation Type:
//     connector
//
int
CTCLVariable::Link(void* pVariable, int Type) 
{
// Connects a program variable to a 
// TCL variable.  Note that this variable can be
// a member of an object as long as it will be unlinked
// prior to destruction.    
// 
// Formal Parameters:
//     char* pName:
//       Name of the variable
//     void* pAddress:
//        Pointer to the linked variable.
//     int type:
//        any of:
//      TCL_LINK_INT         - pAddress -> int
//      TCL_LINK_DOUBLE- pAddress -> double
//      TCL_LINK_STRING - pAddress -> char*
//          possibly or'd with:
//      TCL_LINK_READ_ONLY - Variable should be treated as TCL readonly.
//
// Returns:
//     TCL_OK - if link performed (?)

  CTCLInterpreter* pInterp = AssertIfNotBound();

  return Tcl_LinkVar(pInterp->getInterpreter(), (char*)(m_sVariable.c_str()),
		     (char*)pVariable, Type);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Unlink (  )
//  Operation Type:
//     connector
//
void 
CTCLVariable::Unlink() 
{
// Removes a link between a variable and a TCL Variable.

  CTCLInterpreter* pInterp = AssertIfNotBound();

  Tcl_UnlinkVar(pInterp->getInterpreter(), (char*)(m_sVariable.c_str()));

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int Trace (  )
//  Operation Type:
//     connector
//
int 
CTCLVariable::Trace(int flags, char* pIndex) 
{
// Sets a trace on the variable.  When the variable is
// accessed,  TraceRelay  is called which sets up a call
// to operator().
// NOTE:
//     Typically this should only be called on an object of a class
//     derived from CTCLVariable since that's the only way to 
//     get an operator() which does anything.
  //
  // Formal Parameters:
  //     int flags:
  //        Bitwise or of any of the following:
  //        TCL_TRACE_READS    - Reads of the variable are traced.
  //        TCL_TRACE_WRITES   - Writes of the variable are traced.
  //        TCL_TRACE_UNSETS   - Unsets of the variable are traced.
  //        Default is to set all of these flags.
  //     char* pIndex:
  //        If the variable is an array, specifies which element should
  //        be traced.  Note that for scalars, this should be  NULL.
  //        Defaults to kpNULL
  //
  CTCLInterpreter* pInterp = AssertIfNotBound();

  if(m_fTracing)UnTrace();	// Remove any trace already set.
  int status = Tcl_TraceVar2(pInterp->getInterpreter(), 
			     (char*)(m_sVariable.c_str()), pIndex,
			     flags, TraceRelay, (ClientData)this);
  if(status == TCL_OK) {
    m_fTracing = kfTRUE;
    m_nTraceFlags = flags;
    m_sTraceIndex = pIndex ? pIndex : "";
  }
  return status;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void UnTrace ()
//  Operation Type:
//     
//
void 
CTCLVariable::UnTrace() 
{
// Remove trace on variable.
//  Formal Parameters:
//    Index of the array element from which to remove trace.

 
  if(m_fTracing) {
    CTCLInterpreter *pInterp = AssertIfNotBound();

    Tcl_UntraceVar2(pInterp->getInterpreter(),
		    (char*)(m_sVariable.c_str()), 
		    (char*)(m_sTraceIndex.size() ? m_sTraceIndex.c_str() : 
                                           (char*)kpNULL),
		    m_nTraceFlags, TraceRelay, (ClientData)this);
  }

}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//    void DoAssign(const CTCLVariable& rRhs)
// Operation Type:
//    Protected utility.
//
void
CTCLVariable::DoAssign(const CTCLVariable& rRhs)
{
  // Does the grunt work of the various assignment like

  if(m_fTracing) UnTrace();

  m_sVariable    = rRhs.m_sVariable;
  m_fTracing     = rRhs.m_fTracing;
  m_nTraceFlags  = rRhs.m_nTraceFlags;
  m_sTraceIndex  = rRhs.m_sTraceIndex;

  if(m_fTracing) {
    Trace(m_nTraceFlags, (char*)(m_sTraceIndex.size() ? 
				      m_sTraceIndex.c_str() : 
				      (const char*)kpNULL));
  }

}


