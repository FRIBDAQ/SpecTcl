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

//
// Header Files:
//

#include "BufferDecoder.h"                               
#include "EventFormatError.h"
#include "Analyzer.h"
#include <BufferTranslator.h>
#include <TranslatorPointer.h>
#include <buftypes.h>


static const char* Copyright = 
"BufferDecoder.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CBufferDecoder

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void operator() (UInt_t nBytes, Address_t pBuffer, CAnalyzer& rAnalyzer )
//  Operation Type:
//     Function operator.
//
void 
CBufferDecoder::operator()(UInt_t nBytes, Address_t pBuffer, CAnalyzer& rAnalyzer) 
{
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
//
// 

  m_pBuffer = pBuffer;		// First set the buffer context...
  m_nSize   = nBytes;

  Short_t Signature16;
  Int_t Signature32;
  getByteOrder( Signature16, Signature32 );
  m_pTranslator = BufferFactory::CreateBuffer( pBuffer, Signature32 );

  UInt_t nType = getBufferType(); // We'll dispatch to the analyzer depending
				//  on the type of buffer.

  // Callbacks to us are only legal for the body of this switch since after
  // that, the buffer pointer is set back to zero.
  // 
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
////////////////////////////////////////////////////////////////////////////
//
// Function:
//   void ThrowIfNoBuffer(const char* pszWhatImDoing)
// Operation type:
//   Utility function.
//
void
CBufferDecoder::ThrowIfNoBuffer(const char* pszWhatImDoing)
{
  //
  //  Throws an exception if there is no current buffer.
  //
  if(!m_pBuffer) {
    CEventFormatError e((int)CEventFormatError::knNoCurrentBuffer,
			pszWhatImDoing,
			(UInt_t*)kpNULL, 0, 0);
    throw e;
  }
} 
