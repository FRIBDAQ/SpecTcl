//  CTapeException.cpp
// Encapsulates error exceptions from the
// mtaccess library and tape handling objects.

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


#include "TapeException.h"  

extern "C" {                      
#include <mtaccess.h>
}

static const char* Copyright = 
"CTapeException.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CTapeException

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    const char* ReasonText (  )
//  Operation Type:
//     selector.
//
const char* 
CTapeException::ReasonText() const 
{
// Returns a pointer to the text string
// associated with the reason code.
//
// Exceptions:  

  return ::mtgetmsg(m_nTapeError);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t ReasonCode (  )
//  Operation Type:
//     Selector.
//
Int_t 
CTapeException::ReasonCode() const 
{
// Returns the current value of m_nTapeError.
// Exceptions:  

  return getTapeError();	// Allows for tape error reporting to change.
}
