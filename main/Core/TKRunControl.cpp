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
//  CTKRunControl.cpp
// Provides a specialized run control for the TCL/TK
// operating environment with file based event sources.
// A CTCLFileHandler is used to indicate when data is ready.
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
#include "TKRunControl.h"                               
#include <TCLInterpreter.h>
#include <TCLException.h>
#include <tcl.h>
#include <tk.h>
#include <iostream>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

//  Static member initializations:

UInt_t CTKRunControl::m_nDefaultBufferSize=8*kn1K; // Default buffer size.

// Functions for class CTKRunControl

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Start (  )
//  Operation Type:
//     State transition
//
void 
CTKRunControl::Start() 
{
// Starts analyzing data.
// Exceptions:  


  m_FileHandler.Set();
  CRunControl::Start();		// Update the internal state variables.
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Stop (  )
//  Operation Type:
//     state transition
//
void 
CTKRunControl::Stop() 
{
// Stops analysis of data.
// Exceptions:  

  m_FileHandler.Clear();
  CRunControl::Stop();		// Update internal state variables.
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//    void OnEnd()
// Operation Type:
//    Hook
//
void
CTKRunControl::OnEnd()
{
  // Called when the data source gives an end file condition.
  // in our case, we execute the script stored in m_sEndScript

  CRunControl::OnEnd();

  cerr << "End file encountered on Data Source\n";
  try {				// May need this later when user scripts allowed
    m_pInterp->Eval(m_sEndScript);
  }
  catch(CTCLException& except) {
  }
}
///////////////////////////////////////////////////////////////////////////
//
// Function:
//    void OnBuffer(Uint_t nBytes)
// Operation Type:
//    Hook
//
void
CTKRunControl::OnBuffer(UInt_t nBytes)
{
  if(getRunning())m_FileHandler.Set();		// Re-enable file handler.
  CRunControl::OnBuffer(nBytes); // Note on end of file this will clear().

}
