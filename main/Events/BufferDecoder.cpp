/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

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
//  CBufferDecoder.cpp
//     Abstract base class which provides decode capability for buffers
//     The idea is to break apart the buffer cracking into software which is
//     aware of buffer bodies and buffer header structures.  This is useful
//     because typically buffer headers don't depend on the experiment, but
//     rather the daq system, while buffer bodies depend on both experiment
//     and DAQ.  CBufferDecoders provide a single repository for buffer header
//     structure knowledge.
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

// Header Files:
#include <config.h>
#include "BufferDecoder.h"
#include "EventFormatError.h"
#include "Analyzer.h"
#include <BufferTranslator.h>
#include <TranslatorPointer.h>
#include <buftypes.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif
// Functions for class CBufferDecoder

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void operator()(UInt_t nBytes, Address_t pBuffer, CAnalyzer& rAnalyzer)
//  Operation Type:
//     Function operator.
//
void CBufferDecoder::operator()(UInt_t nBytes, Address_t pBuffer, CAnalyzer& rAnalyzer) {
  // Called to initiate the decoding of an event buffer.
  // Decoding is a collaborative effort between three
  // objects, a CAnalayzer, a CEventUnpacker and
  // an object derived from us, a CBufferDecoder
  //   The analyzer knows what functions each buffer
  // type should supply.  The EventUnpacker knows the experiment
  // specific format of event buffer bodies, and the BufferDecoder
  //  knows how to decode the header of the buffer.
  //
  // Formal Parameters:
  //         UInt_t      nBytes:
  //           Number of bytes in the buffer.
  //        Address_t   pBuffer:
  //           Points to a buffer which will be decoded.
  //       CAnalyzer& rAnalyzer:
  //           Refers to an analyzer object.  An appropriate
  //           Onxxxx member is called depending on the
  //           type of the bufer.
  if(m_pTranslator)
    delete m_pTranslator;       // delete the old BufferTranslator
  m_pBuffer = pBuffer;		// First set the buffer context...
  m_nSize   = nBytes;

  Short_t Signature16;
  Int_t Signature32;
  getByteOrder(Signature16, Signature32);
  m_pTranslator = BufferFactory::CreateBuffer(pBuffer, Signature32);

  UInt_t nType = getBufferType(); // We'll dispatch to the analyzer depending
  //  on the type of buffer.

  // Callbacks to us are only legal for the body of this switch since after
  // that, the buffer pointer is set back to zero.
  switch(nType) {
  case BEGRUNBF:
  case ENDRUNBF:		// These are all run state change buffer
  case PAUSEBF:			// types.
  case RESUMEBF:
    rAnalyzer.OnStateChange(nType, *this);
    break;
  case SCALERBF:		// Types of scaler buffer.s
  case SNAPSCBF:
    rAnalyzer.OnScaler(*this);
    break;
  case DATABF:
    rAnalyzer.OnPhysics(*this);
    break;
  default:
    rAnalyzer.OnOther(nType, *this);
  }
  m_pBuffer = 0;		// Finally declare the buffer once more invalid
}


void
CBufferDecoder::OnAttach(CAnalyzer& junk)
{}
void
CBufferDecoder::OnDetach(CAnalyzer& junk)
{}

bool
CBufferDecoder::blockMode()
{
    return true;
}


/**
 * OnSourceAttach
 *   Called just after a new event source was attached.
 */
void
CBufferDecoder::OnSourceAttach()
{}
/**
 * OnSourceDetach
 *     Called just before an event source is detached.
 */
void
CBufferDecoder::OnSourceDetach()
{}

/**
 * OnEndFile
 *    Called just after a source sensed an end file.
 */
void
CBufferDecoder::OnEndFile()
{}

////////////////////////////////////////////////////////////////////////////
//
// Function:
//   void ThrowIfNoBuffer(const char* pszWhatImDoing)
// Operation type:
//   Utility function.
//
void CBufferDecoder::ThrowIfNoBuffer(const char* pszWhatImDoing) {
  //  Throws an exception if there is no current buffer.
  if(!m_pBuffer) {
    CEventFormatError e((int)CEventFormatError::knNoCurrentBuffer,
			pszWhatImDoing,
			(UInt_t*)kpNULL, 0, 0);
    throw e;
  }
} 
