//  CRunControl.cpp
// Interfaces event sources (files) with
// analyzers.  This allows analyzers to be
//  used in many environmental settings as well
//  as supporting a supplier of buffers which
//  is not a traditional file.
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

#include "RunControl.h"                               
#include <assert.h>

static const char* Copyright = 
"CRunControl.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CRunControl


//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void OnBuffer ( UInt_t nBytes, Address_t pData )
//  Operation Type:
//     Callback
//
void 
CRunControl::OnBuffer(UInt_t nBytes) 
{
// Called when a buffer of data is available.
// This function interfaces the event source  with the analyzer.
//
//  Formal Parameters:
//
//     UInt_t nBytes:
//       Number of bytes to read from file.

  assert(m_pAnalyzer != (CAnalyzer*)kpNULL);
  assert(m_pEventSource != (CFile*)kpNULL);

  Char_t* pBuffer = new Char_t[nBytes];	// Create a buffer...

  Int_t nRead = m_pEventSource->Read(pBuffer, nBytes);
  if(nRead > 0) {
    m_pAnalyzer->OnBuffer(nRead, pBuffer);
  }
  else {
    OnEnd();		// Do Our end file handling.
    m_pAnalyzer->OnEndFile();
  }
  delete []pBuffer;
}
///////////////////////////////////////////////////////////////////////
//
//  Function:
//      CFile*    Attach(CFile* pNewfile)
//  Operation Type:
//      Connection.
//
CFile*
CRunControl::Attach(CFile* pNewFile)
{
  // Attaches a different file to the run control
  // object.
  //
  // Formal Parameters:
  //     CFile* pNewFile:
  //             Pointer to the new event source file.
  // Returns:
  //    CFile* Pointer to the previous event source file.
  //
  CFile* pPrior = m_pEventSource;
  m_pEventSource= pNewFile;
  return pPrior;
}
////////////////////////////////////////////////////////////////
//
// Function :
//    void OnEnd()
// Operation Type:
//    Hook.
//
void
CRunControl::OnEnd()
{
}
