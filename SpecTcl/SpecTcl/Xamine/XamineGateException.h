//  CXamineGateException.h:
//
//    This file defines the CXamineGateException class.
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

#ifndef __XAMINEGATEEXCEPTION_H  //Required for current class
#define __XAMINEGATEEXCEPTION_H
                               //Required for base classes
#ifndef __EXCEPTION_H
#include "Exception.h"
#endif

#ifndef __DISPLAYGATE_H
#include "DisplayGate.h"
#endif
            
#ifndef __XAMINE_XAMINE_H                   
#include <Xamine.h>   //Required for include files, eg <CList.h>
#define __XAMINE_XAMINE_H
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __CXX_STSSTREAM_H
#include <strstream.h>
#define __CXX_STRSTREAM_H
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

class CXamineGateException  : public CException        
{
  Int_t        m_nError;	// Xamine gate manipulation error code
  CDisplayGate m_Gate;		// Gate which caused the problem.
  strstream    m_ReasonStream;	// Reason for error.
  Bool_t       m_fReasonProduced; // kfTRUE if m_ReasonStream accurate.
  
public:

  CXamineGateException (Int_t am_nError,  
			const CDisplayGate& am_Gate,
			const std::string& rDoing)  :
    CException(rDoing),
    m_nError(am_nError),
    m_Gate(am_Gate),
    m_fReasonProduced(kfFALSE)
  { }
   ~ CXamineGateException ( ) { }       //Destructor	
			//Copy constructor
			//Update to access 1:M part class attributes
			//Update to access 1:1 associated class attributes
			//Update to access 1:M associated class attributes      
  CXamineGateException (const CXamineGateException& aCXamineGateException )  : 
    CException (aCXamineGateException),
    m_nError(aCXamineGateException.m_nError),
    m_Gate(aCXamineGateException.m_Gate)
  {   

                
  }                                     

			//Operator= Assignment Operator

  CXamineGateException& operator= 
                       (const CXamineGateException& aCXamineGateException)
  { 
    if (this == &aCXamineGateException) return *this;          
    CException::operator= (aCXamineGateException);
    m_nError = aCXamineGateException.m_nError;
    m_Gate = aCXamineGateException.m_Gate;
    
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CXamineGateException& aCXamineGateException)
  { 
    return (
	    (CException::operator== (aCXamineGateException)) &&
	    (m_nError == aCXamineGateException.m_nError) &&
	    (m_Gate == aCXamineGateException.m_Gate) 
	    );
  }                             
  // Selectors:

public:
  Int_t getError() const
  {
    return m_nError;
  }
  const CDisplayGate& getGate() const
  {
    return m_Gate;
  }
  // Mutators:

protected:                   

  void setError (Int_t am_nError)
  { 
    m_nError = am_nError;
  }
  void setGate (CDisplayGate am_Gate)
  { 
    m_Gate = am_Gate;
  }
  // Operations:

public:
                   
  virtual   const char* ReasonText ()  ;

  virtual   Int_t ReasonCode ()  ;
 

};

#endif
