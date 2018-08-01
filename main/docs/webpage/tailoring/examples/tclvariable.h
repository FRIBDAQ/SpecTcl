//  CTCLVariable.h:
//
//    This file defines the CTCLVariable class.
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

#ifndef TCLVARIABLE_H  //Required for current class
#define TCLVARIABLE_H
                               //Required for base classes
#include "TCLInterpreterObject.h"
#include <histotypes.h>
#include <string>
                               
class CTCLVariable  : public CTCLInterpreterObject        
{
  std::string m_sVariable;	// Name of the variable represented.
  Bool_t m_fTracing;		// kfTRUE if tracing is enabled.
  Int_t  m_nTraceFlags;		// Set of trace flags for variable.
  std::string m_sTraceIndex;
public:

			//Constructor with arguments

  CTCLVariable (std::string am_sVariable,  Bool_t am_fTracing  )  :   
    CTCLInterpreterObject(),
    m_sVariable (am_sVariable),  
    m_fTracing (am_fTracing)  
  { }         
  CTCLVariable (CTCLInterpreter* pInterp, 
		std::string am_sVariable,  Bool_t am_fTracing  )  :   
    CTCLInterpreterObject(pInterp),
    m_sVariable (am_sVariable),  
    m_fTracing (am_fTracing)  
  { }        
  ~CTCLVariable();
			//Copy constructor
 
  CTCLVariable (const CTCLVariable& aCTCLVariable )   : 
    CTCLInterpreterObject (aCTCLVariable),
    m_sVariable(" "),
    m_fTracing(kfFALSE)
  {   
    DoAssign(aCTCLVariable);
  }                                     

			//Operator= Assignment Operator

  CTCLVariable& operator= (const CTCLVariable& aCTCLVariable)
  { 
    if (this == &aCTCLVariable) return *this;          
    CTCLInterpreterObject::operator=(aCTCLVariable);
    DoAssign(aCTCLVariable);
  
    return *this;                                                                                                 
  }                                     

			//Operator== Equality Operator

  int operator== (const CTCLVariable& aCTCLVariable)
  { 
    return (
	    (CTCLInterpreterObject::operator== (aCTCLVariable)) &&
	    (m_sVariable == aCTCLVariable.m_sVariable) &&
	    (m_fTracing == aCTCLVariable.m_fTracing) 
	    );
  }                             
  // Selectors

public:

  std::string getVariableName() const
  {
    return m_sVariable;
  }
  Bool_t IsTracing() const
  {
    return m_fTracing;
  }
  // Mutators:

public:

  void setVariableName (std::string am_sVariable)
  { 
    if(IsTracing()) UnTrace();
    m_sVariable = am_sVariable;
  }
protected:
  void setTracing (Bool_t am_fTracing)
  { 
    m_fTracing = am_fTracing;
  }
  //  Additional operations:
  //
public:                   
  virtual   char*  operator() (char* pName, 
			       char* pSubscript, 
			       int Flags)  ;

   static  char* TraceRelay (ClientData pObject, Tcl_Interp* pInterpreter, 
			     char*      pName,   char*       pIndex, 
			     int flags)  ;
  const char* Set (char* pValue, int flags=TCL_LEAVE_ERR_MSG)  ;
  const char* Set (char* pSubscript, char* pValue, 
		   int flags=TCL_LEAVE_ERR_MSG)  ;
  const char* Get (int flags=TCL_LEAVE_ERR_MSG, char* pIndex=0)  ;
  int Link (void* pVariable, int Type)  ;
  void Unlink ()  ;
  int Trace (int flags=TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS,
	     char* pIndex = (char*)kpNULL)  ;

  void UnTrace ()  ;
 


  // Protecterd utility functions:
  //
protected:
  void DoAssign(const CTCLVariable& rRhs);
};

#endif
