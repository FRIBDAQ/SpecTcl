//  CXamineGateException.cpp
// Encapsulates exceptions which can be thrown
// by gate manipulation member functions of  the CXamine
// class.  The exception contains an error code as well
// as a prototype gate for the gate which caused the
// problem.
//
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


#include "XamineGateException.h"                               
#include <assert.h>

static const char* Copyright = 
"CXamineGateException.cpp: Copyright 1999 NSCL, All rights reserved\n";


// Static constants - these are indexed by negative of status code:
//

const static char* pMessages[] = {
  "Successful completion -- code should not have thrown this",
  "NoSuchSpectrum        -- Requested Spectrum does not exist",
  "InappropriateGate     -- Gate type was inappropriate for the spectrum",
  "ObjectTableFull       -- Xamine's object table was already full",
  "DuplicateId           -- An object with this Id exists on this spectrum",
  "NoSuchObject          -- Search failed to find a matching object.",
  "CheckErrno            -- This exception should be an CErronException",
  "BadCoordinates        -- Bad Button box coordinates (inapplicable)",
  "BadType               -- Invalid button type (inapplicable)",
  "BadPrompter           -- Invalid prompter type (inapplicable)",
  "NoSuch Message        -- Invalid error code at exception instantiation"
};
const static UInt_t nMessages = sizeof(pMessages)/sizeof(char*);
const static char* pGateType[] = {
  "One-D Cut",
  "Two-D Contour",
  "Two-D Band"
};


// Functions for class CXamineGateException

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    const char* ReasonText (  )
//  Operation Type:
//     Selector
//
const char* 
CXamineGateException::ReasonText() 
{
// Returns a pointer to a text string which contains
// an explanation of the error.
// The text string is a pointer to an undocumented
//  member variable.
//
  assert(m_nError <= 0);
  UInt_t nError = -m_nError;
  if(nError >= nMessages) 
    nError = nMessages - 1;

  if(!m_fReasonProduced) {
    m_ReasonStream << " Exception thrown in Xamine Gate maninpulation: " <<
      endl;
    m_ReasonStream << pMessages[nError] << endl;
    m_ReasonStream << " Gate information: " << endl;
    m_ReasonStream << "    Spectrum   : " << m_Gate.getSpectrum() << endl;
    m_ReasonStream << "    Gate id    : " << m_Gate.getId()       << endl;
    m_ReasonStream << "    Gate Name  : " << m_Gate.getName().c_str() << endl;
    m_ReasonStream << "    Gate Type  : " << pGateType[m_Gate.getGateType()]
		                          << endl;
    
    m_fReasonProduced = kfTRUE;
  }
  return m_ReasonStream.str();

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t ReasonCode (  )
//  Operation Type:
//     Selector
//
Int_t 
CXamineGateException::ReasonCode() 
{
// Returns the reason for the error
// Exceptions:  

  return m_nError;
}
