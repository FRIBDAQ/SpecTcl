//  CTapeException.h:
//
//    This file defines the CTapeException class.
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

#ifndef __CTAPEEXCEPTION_H  //Required for current class
#define __CTAPEEXCEPTION_H
                               //Required for base classes
#ifndef __EXCEPTION_H
#include "Exception.h"
#endif                
#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif               
                               
class CTapeException  : public CException        
{
  UInt_t m_nTapeError;	    // Snapshot of the mtaccess error code at the time
			    // the exception was instantiated.
  
public:
			//Default constructor

  CTapeException (  UInt_t nTapeError, const char* pDoing ) :
    CException(pDoing),
    m_nTapeError(nTapeError) 
  {}
  CTapeException(UInt_t nTapeError, const string& rDoing) :
    CException(rDoing),
    m_nTapeError(nTapeError)
  {}
  virtual ~CTapeException ( ) { }       //Destructor
  
	
			//Copy constructor

  CTapeException (const CTapeException& aCTapeException )   : 
    CException (aCTapeException) 
  {   
    m_nTapeError = aCTapeException.m_nTapeError;            
  }                                     

			//Operator= Assignment Operator

  CTapeException operator= (const CTapeException& aCTapeException)
  { 
    if (this == &aCTapeException) return *this;          
    CException::operator= (aCTapeException);
    m_nTapeError = aCTapeException.m_nTapeError;  
 
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CTapeException& aCTapeException)
  { 
    return (
	    (CException::operator== (aCTapeException)) &&
	    (m_nTapeError == aCTapeException.m_nTapeError) 
	    );
  }       
  // Selectors:
  //   NOTE:  
  //      These are class specific and should not be used unless you know
  //      what you're doing
  //
public:
                       //Get accessor function for attribute
  UInt_t getTapeError() const
  {
    return m_nTapeError;
  }
  //  Mutator:  Available only for derived classes:
                   
protected:
  void setTapeError (UInt_t am_nTapeError)
  { 
    m_nTapeError = am_nTapeError;
  }
  //
  // Operations in the CException Interface:
  //
public:             
  virtual   const char* ReasonText () const  ;
  virtual   Int_t ReasonCode () const  ;
 
};

#endif
