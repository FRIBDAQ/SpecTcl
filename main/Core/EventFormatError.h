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


//  CEventFormatError.h:
//
//    This file defines the CEventFormatError class.
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

#ifndef EVENTFORMATERROR_H  //Required for current class
#define EVENTFORMATERROR_H
                               //Required for base classes
#include "Exception.h"
#include <vector>
#include <string>
#include <histotypes.h>

class CEventFormatError  : public CException        
{
private:
  Int_t m_nReason;		// Reason for the exception
  std::vector<UShort_t> m_vEventData; // Vector containing data from bad event.
  UInt_t m_nDetectedOffset;	// Offset into m_vEventData where error
				// was detected.
  UInt_t m_nEventSize;		// If able to determine, actual event size.
  Bool_t m_fSizeAccurate;	// kfTRUE if m_nEventSize is accurate.
  std::string m_sReason;		// Current reason text.
public:
  enum {			// Defined error codes.
    knNoCurrentBuffer,		// Current Buffer not defined.
    knSizeMismatch,		// Size should have been different.
    knPacketIdMismatch,
    knInvalidPacketID,
    knBadPacketContents,	// For future expansion...
    knEmptyParameterMap,
    knUnused5,
    knUnused6,
    knUnused7,
    knUnused8,
    knUnUsed9,
    knUnused10,
    knFirstUserReason		// From here on in are for user codes.
  };
  const int knUnused1, knUnused2;
public:
			//Constructors with arguments

  CEventFormatError(Int_t nReason, const char* pWasDoing) :
    CException(pWasDoing),
    m_nReason(nReason),
    m_nDetectedOffset(0),
    m_nEventSize(0),
    m_fSizeAccurate(kfFALSE),
    knUnused1((int)knPacketIdMismatch),
    knUnused2((int)knInvalidPacketID)
  {
    UpdateReasonText();
  }
  CEventFormatError(Int_t nReason, const char* pWasDoing,
		    UInt_t* pFirstWord, UInt_t nWords,
		    UInt_t  nOffset,
		    UInt_t  nEventSize = 0) :
    CException(pWasDoing),
    m_nReason(nReason),
    m_vEventData(pFirstWord, pFirstWord + (nWords -1 )),
    m_nDetectedOffset(nOffset),
    m_nEventSize(nEventSize),
    m_fSizeAccurate(nEventSize ? kfTRUE : kfFALSE),
    knUnused1((int)knPacketIdMismatch),
    knUnused2((int)knInvalidPacketID)
  {
    UpdateReasonText();
  }
  CEventFormatError(Int_t nReason, const std::string& rWasDoing,
		    UInt_t* pFirstWord, UInt_t nWords,
		    UInt_t  nOffset,
		    UInt_t  nEventSize = 0) :
    CException(rWasDoing),
    m_nReason(nReason),
    m_vEventData(pFirstWord, pFirstWord + (nWords -1 )),
    m_nDetectedOffset(nOffset),
    m_nEventSize(nEventSize),
    m_fSizeAccurate(nEventSize ? kfTRUE : kfFALSE),
    knUnused1((int)knPacketIdMismatch),
    knUnused2((int)knInvalidPacketID)
  {
    UpdateReasonText();
  }


  virtual  ~CEventFormatError ( ) { }       //Destructor

	
			//Copy constructor

  CEventFormatError (const CEventFormatError& aCEventFormatError )   : 
    CException (aCEventFormatError) ,
    knUnused1((int)knPacketIdMismatch),
    knUnused2((int)knInvalidPacketID)
  {
    m_nReason = aCEventFormatError.m_nReason;
    m_vEventData = aCEventFormatError.m_vEventData;
    m_nDetectedOffset = aCEventFormatError.m_nDetectedOffset;
    m_nEventSize = aCEventFormatError.m_nEventSize;
    m_fSizeAccurate = aCEventFormatError.m_fSizeAccurate;
    UpdateReasonText();
  }                                     

			//Operator= Assignment Operator

  CEventFormatError operator= (const CEventFormatError& aCEventFormatError)
  {
    if (this != &aCEventFormatError) {
      CException::operator= (aCEventFormatError);
      m_nReason = aCEventFormatError.m_nReason;
      m_vEventData = aCEventFormatError.m_vEventData;
      m_nDetectedOffset = aCEventFormatError.m_nDetectedOffset;
      m_nEventSize = aCEventFormatError.m_nEventSize;
      m_fSizeAccurate = aCEventFormatError.m_fSizeAccurate;
      UpdateReasonText();
    }

    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CEventFormatError& aCEventFormatError)
  { return (
	    (CException::operator== (aCEventFormatError)) &&
	    (m_nReason == aCEventFormatError.m_nReason) &&
	    (m_vEventData == aCEventFormatError.m_vEventData) &&
	    (m_nDetectedOffset == aCEventFormatError.m_nDetectedOffset) &&
	    (m_nEventSize == aCEventFormatError.m_nEventSize) &&
	    (m_fSizeAccurate == aCEventFormatError.m_fSizeAccurate) 
	    );
  }           
  // Selectors (readonly)

public:                  
                       //Get accessor function for attribute
  Int_t getReason() const
  {
    return m_nReason;
  }
  std::vector<UShort_t> getEventData() const
  {
    return m_vEventData;
  }
  UInt_t getDetectedOffset() const
  {
    return m_nDetectedOffset;
  }
  UInt_t getEventSize() const
  {
    return m_nEventSize;
  }
  Bool_t getSizeAccurate() const
  {
    return m_fSizeAccurate;
  }
  // Selectors (mutators)
  //
protected:

  void setReason (Int_t am_nReason)
  { 
    m_nReason = am_nReason;    
    UpdateReasonText();
  }
  void setEventData (std::vector<UShort_t> am_vEventData)
  { 
    m_vEventData = am_vEventData;
    UpdateReasonText();
  }
  void setEventData(UShort_t pStart, UInt_t nWords)
  {
    std::vector<UShort_t> v(pStart, pStart + (nWords -1));
    setEventData(v);
  }
  void setDetectedOffset (UInt_t am_nDetectedOffset)
  { 
    m_nDetectedOffset = am_nDetectedOffset;
    UpdateReasonText();
  }
  void setEventSize (UInt_t am_nEventSize)
  { 
    m_nEventSize = am_nEventSize;
    m_fSizeAccurate = kfTRUE;	// Wouldn't set if not true.
    UpdateReasonText();
  }
  void setSizeAccurate (Bool_t am_fSizeAccurate = kfFALSE) // Usually false.
  { 
    m_fSizeAccurate = am_fSizeAccurate;
    UpdateReasonText();
  }
  // Implementations of the standard CException interface:
  //
public:
  virtual   const char*  ReasonText () const  ;
  virtual   Int_t ReasonCode ()  const;
  //
  // Functions specific to the subclass of Exception:
  //
public:
  std::vector<UShort_t>& EventData ()  ;
  UInt_t Where ()  ;
  UInt_t EventSize ()  ;
  Bool_t EventSizeOk ()  ;
  
  //  Protected utilities:

protected:
  void UpdateReasonText();
};

#endif








