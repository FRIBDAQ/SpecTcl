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

#ifndef CTAPEEXCEPTION_H  //Required for current class
#define CTAPEEXCEPTION_H
                               //Required for base classes
#include "Exception.h"
#include <string>
#include <histotypes.h>


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
  CTapeException(UInt_t nTapeError, const std::string& rDoing) :
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
  virtual   const  char* ReasonText () const  ;
  virtual   Int_t ReasonCode () const  ;
 
};

#endif
