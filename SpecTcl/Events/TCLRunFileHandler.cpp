//  CTCLRunFileHandler.cpp
// File handler for use with a TKRunControl object.
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


#include "TCLRunFileHandler.h"                               
#include "TKRunControl.h"
#include <tcl.h>
#include <tk.h>

static const char* Copyright = 
"CTCLRunFileHandler.cpp: Copyright 1999 NSCL, All rights reserved\n";

static const UInt_t knWaitTime = 500; // Ms to wait for file to be readable.

// Functions for class CTCLRunFileHandler

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void operator() ( )
//  Operation Type:
//     interface.
//
void 
CTCLRunFileHandler::operator()() 
{
// This function is called whent one or more of the allowed events is
// possible on the file descriptor associated with this handler.  This
// function is called from CallbackRelay which establishes object
// context from the Tcl/TK C API.
//
// Formal Parameters:
//     int mask:
//       Inditates the type of event which occured.  Can be a  logical or of:
//       TK_READABLE    - fd is readable.
//       TK_WRITABLE    - fd is writeable.
//       TK_EXCEPTION  - Some exceptional condition occured on the fd.
//

  const CFile* pSource = m_pRun->getEventSource();
  if(pSource->IsReadable(knWaitTime)) {
    m_pRun->OnBuffer(m_nBufferSize); // If a buffer is available process else.
  }
  else {
    Set();			// Repropagate.
  }

}
