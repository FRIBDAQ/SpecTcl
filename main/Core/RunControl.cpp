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

// Header Files:

#include <config.h>
#include "RunControl.h"
#include "Analyzer.h"
#include "BufferDecoder.h"
#include <SpecTcl.h>
#include <assert.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


// Functions for class CRunControl

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void OnBuffer ( UInt_t nBytes, Address_t pData )
//  Operation Type:
//     Callback
//
void CRunControl::OnBuffer(UInt_t nBytes) {
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
CFile* CRunControl::Attach(CFile* pNewFile) {
  // Attaches a different file to the run control
  // object.
  //
  // Formal Parameters:
  //     CFile* pNewFile:
  //             Pointer to the new event source file.
  // Returns:
  //    CFile* Pointer to the previous event source file.
  //
  
  /*
   * We need to let the current buffer decoder know about this.
   * OnSourceDetach happens prior disconnecting and OnSourceAttach
   * after connection is complete:
   */
  
  CBufferDecoder* pDecoder = SpecTcl::getInstance()->GetAnalyzer()->getDecoder();
  if (pDecoder) pDecoder->OnSourceDetach();
  
  CFile* pPrior = m_pEventSource;
  m_pEventSource= pNewFile;
  
  if (pDecoder) pDecoder->OnSourceAttach();
  return pPrior;
}

////////////////////////////////////////////////////////////////
//
// Function :
//    void OnEnd()
// Operation Type:
//    Hook.
//
void CRunControl::OnEnd() {}
