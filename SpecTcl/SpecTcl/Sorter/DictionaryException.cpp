//  CDictionaryException.cpp
// Encapsulates an exception thrown by 
// a dictionary class.
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


#include "DictionaryException.h"                               
#include <stdio.h>
#include <assert.h>
#include <string.h>

static const char* Copyright = 
"CDictionaryException.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CDictionaryException

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    const char* ReasonText (  ) const
//  Operation Type:
//     Selector
//
const char* 
CDictionaryException::ReasonText() const
{
// Returns string describing the reason for the exception.

  return m_sReasonText.c_str();

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t ReasonCode (  )
//  Operation Type:
//     Selector
//
Int_t 
CDictionaryException::ReasonCode() const 
{
// Returns m_nReason
//   At initial design time this can be any of:
//     CDictionaryException::knDuplicateKey  - attempt to insert
//                                             duplicate keyword.
  //   CDictionaryException::knDuplicateId   - Duplicate id insertion attempted
//     CDictionaryException::knNoSuchId      - Failed search on ID.
//     CDictinoaryException::knNoSuchKey     - Failed search on Key.
//

  return m_nReason;

}
////////////////////////////////////////////////////////////////////////
//
// Function:
//   void UpdateReasonText() 
// Operation Type:
//   Protected Utility.
//
void
CDictionaryException::UpdateReasonText()
{
  // The user must have given us a valid reason.
  //
  assert( (m_nReason == knDuplicateKey) ||
	  (m_nReason == knNoSuchId)     ||
	  (m_nReason == knDuplicateId)  ||
	  (m_nReason == knNoSuchKey)    ||
	  (m_nReason == knWrongGateType)
	  );


  switch(m_nReason) {
  case knDuplicateKey:
    m_sReasonText = "Duplicate Key during Dictionary insertion\n";
    break;
  case knDuplicateId:
    m_sReasonText = "Duplicate Id during Dictionary insertion\n";
    break;
  case knNoSuchId:
    m_sReasonText = "Failed search of dictionary by Id number\n";
    break;
  case knNoSuchKey:
    m_sReasonText = "Failed search of dictionary by Key string\n";
    break;
  case knWrongGateType:
    m_sReasonText = "Gate cannot be applied to this spectrum type\n";
    break;
  default:
    assert( 0 != 0);		// In view of the opening assert, this would
				// be one hell of an error.
  }

  m_sReasonText += "Key was:  ";
  m_sReasonText += m_fNameValid ? m_sName : 
                                  "-undefined- ";
  m_sReasonText += " Id was: ";
  char szBuffer[20];
  if(m_fIdValid) {
    sprintf(szBuffer, "%d\n", m_nId);
  }
  else {
    strcpy(szBuffer, "-undefined-\n");
  }
  m_sReasonText += szBuffer;
  
}
