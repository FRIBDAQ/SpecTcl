//  CEventFormatError.cpp
// Used to describe errors detected in event format.
// The Analyzer catches these and determines how to
// recover, to continue the analysis.
//
// Notes about m_nReason:
//    This has some predefined values with particular
//     meanings and textual strings associated.
//    The client is also free to extend the error code
//    using codes which are larger than CEventFormatError::knFirstUserReason
//    The error reason returned for this will be "Unknown User Code".
//
//  >>>BUGBUG<<< - Could add static member functions to support 
//  registering code/string correspondences with an integer keyed map.
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


#include "EventFormatError.h"                               
#include <strstream.h>
#include <iomanip.h>

static const char* Copyright = 
"CEventFormatError.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CEventFormatError

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    const char*  ReasonText (  )
//  Operation Type:
//     selector.
//
const char*  
CEventFormatError::ReasonText() const 
{
// Returns reason text associated with the m_nReason value.
// Exceptions:  

  return m_sReason.c_str();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t& ReasonCode (  )
//  Operation Type:
//     Selector
//
Int_t 
CEventFormatError::ReasonCode()  const
{
// Returns the value of the Reason code.
// Note that while m_nReason is unsigned,
// call protocols allow and require this function
// to return an Int_t.
//
// Exceptions:  

  return m_nReason;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::vector<UShort_t>& EventData (  )
//  Operation Type:
//     selector
//
std::vector<UShort_t>& 
CEventFormatError::EventData() 
{
// Returns a reference to a vector which contains the 
// event data which was being analyzed at the time the
// format error was detected.  It is up to the thrower to 
// decide how much data to put in this vector.  Usually,
// the vector contains the first word of the event up to
// and a little past where the format error was detected.
//
//  See Where() as well.

  return m_vEventData;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t Where (  )
//  Operation Type:
//     selector.
//
UInt_t 
CEventFormatError::Where() 
{
// Returns the value of m_nDetectedOffset
//  m_vEventData[m_nDetectedOffset] contains
// the data around where the formatting error
// was detected.
// 
// 

  return m_nDetectedOffset;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t EventSize (  )
//  Operation Type:
//     selector
//
UInt_t 
CEventFormatError::EventSize() 
{
// If the unpacker which threw this
// was able to determine the exact size of
// the event, then this returns it.  Otherwise,
// this value is meaningless.
//
//  See also: EventSizeOk()

  return m_nEventSize;

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t EventSizeOk (  )
//  Operation Type:
//     selector
//
Bool_t 
CEventFormatError::EventSizeOk() 
{
// If EventSize() will return an accurate event size,
// this returns kfTRUE, if not, kfFALSE is returned.
// In fact, this returns the value stored in m_fSizeAccurate.
//

  return m_fSizeAccurate;

}

/////////////////////////////////////////////////////////////////////////
//
//  Function:
//     void UpdateReasonText()
//  Operation Type:
//     Protected utility.
//
void
CEventFormatError::UpdateReasonText()
{
  // The first part of the reason text is determined by the reason code.
  // We'll supply messages for the ones which are defined and supply
  // some default message for those which aren't.
  //
  switch(m_nReason) {
  case knNoCurrentBuffer:
    m_sReason = "Event Format Error - No Current Buffer Defined\n";
    break;
  case knSizeMismatch:
    m_sReason = "Event Format Error - Event size doesn't match decoding\n";
    break;
  case knPacketIdMismatch:
    m_sReason = "Unexpected event packet type encountered\n";
    break;
  case knInvalidPacketID:
    m_sReason = "Unknown or illegal packet id encountered.\n";
    break;
  case knBadPacketContents:
    m_sReason = "Event packet contents are invalid.\n";
    break;
  default:
    if(m_nReason < knFirstUserReason) {	// Reserved code complain, not loudly
      m_sReason = "Event Format Errror >>>Reserved Error Code<<<\n";
    }
    else {			// User code
      m_sReason = "Event Format Error >>User Error Code<<\n";
    }
    break;
  }
  // The next part of the message contains a dump of the event
  // with a nice little tag around the point where the error was located.
  // The dump is omitted if m_vEventData is empty.
  //
  if(!m_vEventData.empty()) {
    ostrstream msg;
    msg << "Event Dump (hex): "  << hex << setw(4) << setfill('0');
    for(UInt_t i = 0; i < m_vEventData.size(); i++) {
      if((i % 8) == 0) msg << endl;
      msg << (i == m_nDetectedOffset) ? "  >" : "  ";
      msg << m_vEventData[i];
      msg << (i == m_nDetectedOffset) ? "< " : "  ";
    }
    msg << endl;
    m_sReason += msg.str();
  }
  // The last part of the message tells the user whether or not this
  // error can be recovered without flushing the buffer:
  //

  if(m_fSizeAccurate) {
    ostrstream msg;
    msg << " The event size is accurate and is: " << m_nEventSize << endl;
    m_sReason += msg.str();
  }
  else {
    m_sReason += " The event size is not trustworthy \n";
  }
}
