/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321
*/
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

#ifndef XAMINEGATEEXCEPTION_H  //Required for current class
#define XAMINEGATEEXCEPTION_H
                               //Required for base classes
#include "Exception.h"
#include "XamineGate.h"
#include <Xamine.h>   //Required for include files, eg <CList.h>
#include <histotypes.h>

#include <string>
#include <sstream>

class CXamineGateException  : public CException        
{
  Int_t        m_nError;	// Xamine gate manipulation error code
  CXamineGate m_Gate;		// Gate which caused the problem.
  mutable std::stringstream    m_ReasonStream;	// Reason for error.
  mutable Bool_t       m_fReasonProduced; // kfTRUE if m_ReasonStream accurate.
  
public:

  CXamineGateException (Int_t am_nError,  
			const CXamineGate& am_Gate,
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
  const CXamineGate& getGate() const
  {
    return m_Gate;
  }
  // Mutators:

protected:                   

  void setError (Int_t am_nError)
  { 
    m_nError = am_nError;
  }
  void setGate (CXamineGate am_Gate)
  { 
    m_Gate = am_Gate;
  }
  // Operations:

public:
                   
  virtual   const char* ReasonText ()  const;

  virtual   Int_t ReasonCode ()  const;
 

};

#endif
