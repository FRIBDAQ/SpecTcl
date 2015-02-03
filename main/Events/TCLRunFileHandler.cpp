/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

static const char* Copyright = "(C) Copyright Michigan State University 2006, All rights reserved";
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

#include <config.h>
#include "TCLRunFileHandler.h"                               
#include "TCLInterpreter.h"
#include "TKRunControl.h"
#include "TCLVariable.h"
#include <tcl.h>
#include <tk.h>
#include <stdio.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif



const UInt_t knWaitTime = 500; // default Ms to wait for file to be readable.
ULong_t        nWaitTime(knWaitTime);

// Functions for class CTCLRunFileHandler

/*!
  Construct the run file handler.  We init our data elements, and
  bind knWaitTime to the the TCL Variable SpecTclIODwellMax.
*/
CTCLRunFileHandler::CTCLRunFileHandler(CTCLInterpreter* pInterp, 
				       CRunControl& rRun) :
    CTCLIdleProcess(pInterp),
    m_pRun(&rRun),
    m_nBufferSize(0),
    m_pInterp(pInterp)
{
  CTCLVariable Dwell(pInterp, "SpecTclIODwellMax", false);

  UInt_t    nCurrentDwell(nWaitTime);
  Dwell.Link(&nWaitTime, TCL_LINK_INT);

  char sDwell[100];
  snprintf(sDwell, sizeof(sDwell), "%d", nCurrentDwell);
  Dwell.Set(sDwell);
}        


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

  // Drain the Tcl events:

  Tcl_Interp* pInterp = m_pInterp->getInterpreter();
  while(Tcl_DoOneEvent(TCL_DONT_WAIT))
    ;

  // Process data from the file.

  const CFile* pSource = m_pRun->getEventSource();
  if(pSource->IsReadable(nWaitTime)) {
    m_pRun->OnBuffer(m_nBufferSize); // If a buffer is available process else.
  }
  else {
    Set();			// Repropagate.
  }

}
