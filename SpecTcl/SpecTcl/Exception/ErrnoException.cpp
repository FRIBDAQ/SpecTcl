//  CErrnoException.cpp
//    Encapsulates exceptions which are thrown due to bad
//    errno values.
//
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file///////////////////////////////////////////

//
// Header Files:
//

#include "ErrnoException.h"                               
#include <errno.h>

static const char* Copyright = 
"CErrnoException.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CErrnoException

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    const char*  ReasonText (  )
//  Operation Type:
//     Selector.
//
const char*
CErrnoException::ReasonText() const 
{
  return strerror(m_nErrno);

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t ReasonCode (  )
//  Operation Type:
//     Selector.
//
Int_t 
CErrnoException::ReasonCode() const 
{
  return m_nErrno;
}
