//  CRangeError.h:
//
//    This file defines the CRangeError class.
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

#ifndef __CRANGEERROR_H  //Required for current class
#define __CRANGEERROR_H
                               //Required for base classes
#ifndef __CEXCEPTION_H
#include "Exception.h"
#endif                             
#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif  
                               
class CRangeError  : public CException        
{
  Int_t m_nLow;			// Lowest allowed value for range (inclusive).
  Int_t m_nHigh;		// Highest allowed value for range.
  Int_t m_nRequested;		// Actual requested value which is outside
				// of the range.
  string m_ReasonText;            // Reason text will be built up  here.
public:
  //   The type below is intended to allow the client to categorize the
  //   exception:

  enum {
    knTooLow,			// CRangeError::knTooLow  - below m_nLow
    knTooHigh			// CRangeError::knTooHigh - above m_nHigh
  };
			//Constructors with arguments

  CRangeError (  Int_t nLow,  Int_t nHigh,  Int_t nRequested,
		 const char* pDoing) :       
    CException(pDoing),
    m_nLow (nLow),  
    m_nHigh (nHigh),  
    m_nRequested (nRequested)
  { UpdateReason(); }
  CRangeError(Int_t nLow, Int_t nHigh, Int_t nRequested,
	      const string& rDoing) :
    CException(rDoing),
    m_nLow(nLow),
    m_nHigh(nHigh),
    m_nRequested(nRequested)
  { UpdateReason(); }
  virtual ~ CRangeError ( ) { }       //Destructor

			//Copy constructor

  CRangeError (const CRangeError& aCRangeError )   : 
    CException (aCRangeError) 
  {
    m_nLow = aCRangeError.m_nLow;
    m_nHigh = aCRangeError.m_nHigh;
    m_nRequested = aCRangeError.m_nRequested;
    UpdateReason();
  }                                     

			//Operator= Assignment Operator

  CRangeError operator= (const CRangeError& aCRangeError)
  { 
    if (this != &aCRangeError) {
      CException::operator= (aCRangeError);
      m_nLow = aCRangeError.m_nLow;
      m_nHigh = aCRangeError.m_nHigh;
      m_nRequested = aCRangeError.m_nRequested;
      UpdateReason();
    }

    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CRangeError& aCRangeError)
  { 
    return (
	    (CException::operator== (aCRangeError)) &&
	    (m_nLow == aCRangeError.m_nLow) &&
	    (m_nHigh == aCRangeError.m_nHigh) &&
	    (m_nRequested == aCRangeError.m_nRequested) 
	    );
  }
  // Selectors - Don't use these unless you're a derived class
  //             or you need some special exception type specific
  //             data.  Generic handling should be based on the interface
  //             for CException.
public:                             

  Int_t getLow() const
  {
    return m_nLow;
  }
  Int_t getHigh() const
  {
    return m_nHigh;
  }
  Int_t getRequested() const
  {
    return m_nRequested;
  }
  // Mutators - These can only be used by derived classes:

protected:
  void setLow (Int_t am_nLow)
  { 
    m_nLow = am_nLow;
    UpdateReason();
  }
  void setHigh (Int_t am_nHigh)
  { 
    m_nHigh = am_nHigh;
    UpdateReason();
  }
  void setRequested (Int_t am_nRequested)
  { 
    m_nRequested = am_nRequested;
    UpdateReason();
  }
  //
  //  Interfaces implemented from the CException class.
  //
public:                    
  virtual   const char* ReasonText () const  ;
  virtual   Int_t ReasonCode () const  ;
 
  // Protected utilities:
  //
protected:
  void UpdateReason();
};

#endif
