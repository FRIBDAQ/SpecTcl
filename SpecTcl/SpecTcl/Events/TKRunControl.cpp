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

#include "TKRunControl.h"                               
#include <TCLInterpreter.h>
#include <tcl.h>
#include <tk.h>
#include <iostream.h>

static const char* Copyright = 
"CTKRunControl.cpp: Copyright 1999 NSCL, All rights reserved\n";

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
  m_pInterp->Eval(m_sEndScript);
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
  m_FileHandler.Set();		// Re-enable file handler.
  CRunControl::OnBuffer(nBytes); // Note on end of file this will clear().

}
