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
//  CNSCLBufferDecoder.cpp
//     Understands the header of NSCL buffer.
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
#include "NSCLJumboBufferDecoder.h"                               
#include "Analyzer.h"
#include "EventFormatError.h"
#include <BufferTranslator.h>
#include <TranslatorPointer.h>
#include <histotypes.h>
#include "buffer.h"
#include <buftypes.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <Iostream.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


// Functions for class CNSCLJumboBufferDecoder

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    Address_t getBody()
//  Operation Type:
//     Selector
//
const Address_t CNSCLJumboBufferDecoder::getBody() {
  // Returns a pointer to the body of the current buffer
  // A CEventFormatError is thrown if called when not processing
  // a buffer (in the dynamic call chain from operator().
  CBufferDecoder::ThrowIfNoBuffer
    ("CNSCLJumboBufferDecoder::getBody - getting buffer");
  BHEADER* pHeader = (BHEADER*)getBuffer(); // This is a header style ptr.

  assert(pHeader != kpNULL);	            // Should have thrown if so.

  return (const Address_t)(&(pHeader[1]));  // Points past header.
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    Uint_t getBodySize()
//  Operation Type:
//     Selector
//
UInt_t CNSCLJumboBufferDecoder::getBodySize() {
  // Returns the number of bytes which are in the
  // body of the current buffer (note that the header
  // is completely hidden from the user).  If
  // there isn't a current buffer (the caller isn't in
  // the direct dynamic call stack below operator()), then
  // CEventFormatError is thrown.
  CBufferDecoder::ThrowIfNoBuffer
    ("CNSCLJumboBufferDecoder::getBodySize - Getting buffer header");

  BHEADER* pHeader = (BHEADER*)getBuffer();
  assert(pHeader != (BHEADER*)kpNULL);

  TranslatorPointer<UINT16> TP(*(getBufferTranslator()), &(pHeader->nwds));
  TranslatorPointer<UINT16> VER(*(getBufferTranslator()), &(pHeader->buffmt));

  UInt_t nSize = (*TP);
  if (*VER >= 6 ) {
    TranslatorPointer<UINT16> HI(*(getBufferTranslator()), &(pHeader->nwdsHigh));
    UInt_t nHigh = (*HI);
    nSize |= (nHigh << 16);
  }

  nSize      *=  sizeof(UINT16);          //  Total buffer size in bytes
  nSize      -= sizeof(BHEADER);               //  less header size is...
  assert(nSize >= 0);		                // positive and ... 
  return nSize;		                // Buffer size.
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    UInt_t getRun()
//  Operation Type:
//     Selector
//
UInt_t CNSCLJumboBufferDecoder::getRun() {
  // Returns the run number for the run which produced 
  // the current buffer. If no buffer is being processed, the
  // a CEventFormatError is thrown.  Note, for systems which
  // don't record the run number in all buffers it is acceptable
  // to:
  //   a. cache the run number when it is seen.
  //   b. return the cached run number if there is no
  //      valid buffer instead of throwing CEventFormatError.
  CBufferDecoder::ThrowIfNoBuffer
    ("CNSCLJumboBufferDecoder::getRun - Getting header pointer");

  BHEADER* pHeader = (BHEADER*)getBuffer();
  assert(pHeader != kpNULL);

  TranslatorPointer<INT16> TP(*(getBufferTranslator()), &(pHeader->run));

  return (UInt_t)(*TP);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    UInt_t getEntityCount()
//  Operation Type:
//     Selector
//
UInt_t CNSCLJumboBufferDecoder::getEntityCount() {
  // Where applicable returns the number of entities in the buffer
  // This is currently only clearly defined for the following buffer types:
  //  a. Physics buffers -- The number of events.
  //  b. Scaler buffers - The number of scalers.
  //
  //  If called when no buffer is current, throws a CEventFormatError
  // exception.
  CBufferDecoder::ThrowIfNoBuffer
    ("CNSCLJumboBufferDecoder::getEntityCount - Getting header pointer");
  BHEADER *pHeader = (BHEADER*)getBuffer();
  assert(pHeader != kpNULL);

  TranslatorPointer<INT16> TP(*(getBufferTranslator()), &(pHeader->nevt));

  return (UInt_t)(*TP);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t getSequenceNo (  )
//  Operation Type:
//     Selector
//
UInt_t CNSCLJumboBufferDecoder::getSequenceNo() {
  // Returns the sequence number associated with the
  // buffer.  The object is free to return whatever it feels
  // like returning if there are no sequence numbers.
  //
  CBufferDecoder::ThrowIfNoBuffer
    ("CNSCLJumboBufferDecoder::getSequenceNo - Getting header pointer");
  BHEADER* pHeader = (BHEADER*)getBuffer();
  assert(pHeader != kpNULL);

  TranslatorPointer<INT32> TP(*(getBufferTranslator()), 
			      (Address_t)&(pHeader->seq));

  return (UInt_t)(*TP);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    UInt_t getLamCount()
//  Operation Type:
//     selector
//
UInt_t CNSCLJumboBufferDecoder::getLamCount() {
  // Returns the number of lam masks which
  // are read out in the event.  This may be zero.
  //
  // Throws CEventFormatError if there is no current
  // buffer.
  CBufferDecoder::ThrowIfNoBuffer
    ("CNSCLJumboBufferDecoder::getLamCount - Getting header pointer");
  BHEADER* pHeader = (BHEADER*)getBuffer();
  assert(pHeader != kpNULL);

  TranslatorPointer<INT16> TP(*(getBufferTranslator()), &(pHeader->nlam));

  return (UInt_t)(*TP);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    UInt_t getPatternCount()
//  Operation Type:
//     Selector
//
UInt_t CNSCLJumboBufferDecoder::getPatternCount() {
  // Returns the number of pattern registers associated with
  // events in the buffer.  If there is no current buffer, throws
  // CEventFormatError
  CBufferDecoder::ThrowIfNoBuffer
    ("CNSCLJumboBufferDecoder::getPatternCount - Getting header pointer");
  BHEADER* pHeader = (BHEADER*)getBuffer();
  assert(pHeader != kpNULL);

  TranslatorPointer<INT16> TP(*(getBufferTranslator()), &(pHeader->nbit));

  return (UInt_t)(*TP);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    UInt_t getBufferType()
//  Operation Type:
//     Selector
//
UInt_t CNSCLJumboBufferDecoder::getBufferType() {
  // Returns the type of the buffer (allows operator()
  // to be coded regardless of buffer format).
  //
  // If there is  no current buffer, throws EventFormatError
  //
  // Exceptions:  
  CBufferDecoder::ThrowIfNoBuffer
    ("CNSCLJumboBufferDecoder::getBufferType - Getting data buffer");
  BHEADER* pHeader = (BHEADER*)getBuffer();
  assert(pHeader != kpNULL);

  TranslatorPointer<INT16> TP(*(getBufferTranslator()), &(pHeader->type));

  UInt_t nType = (abs(*TP));
  return nType;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void getByteOrder (Short_t& Signature16, Int_t& Signature32)
//  Operation Type:
//     Selector
//
void CNSCLJumboBufferDecoder::getByteOrder(Short_t& Signature16,
				      Int_t& Signature32)
{
  // Returns byte order information about the
  // originating system.  For NSCL buffers this is
  // in the buffer, for other daq systems, implementers
  // may need to hard-code this.
  //
  // Throws CEventFormatError if there is no current
  //  buffer.
  //
  // Formal Parameters:
  //      Short_t&     Signature16:
  //              16 bit byte ordering signature.
  //      Int_t&          Signature32:
  //                32 bit byte ordering signature.

  CBufferDecoder::ThrowIfNoBuffer
    ("CNSCLJumboBufferDecoder::getByteOrder() - getting buffer type");
  BHEADER* pBuffer = (BHEADER*)getBuffer();
  assert(pBuffer != kpNULL);
  assert(sizeof(Short_t) >= 2);
  assert(sizeof(Int_t)   >= 4);

  Signature16 = pBuffer->ssignature;
  Signature32 = pBuffer->lsignature;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//     virtual   void operator()(UInt_t nBytes, Address_t pBuffer, 
//			     CAnalyzer& rAnalyzer) 
// Operation Type:
//     Behavioral override.
//
/*
  This function gains control when a buffer is recieved.
  We're going to >extend< the default behavior to cache the title in the
  event this is a control buffer.
*/
void CNSCLJumboBufferDecoder::operator()(UInt_t nBytes,
				    Address_t pBuffer,
				    CAnalyzer& rAnalyzer)
{
  // Figure out the buffer type... note can't use getBufferType() since
  // the buffer pointer has not been squirreled away yet.
  setBuffer(pBuffer);		// Now we can use getBufferType().
  BHEADER* pHeader((BHEADER*)pBuffer);
  UShort_t type = pHeader->type;

  if(type == BEGRUNBF) {
    ctlbody* pBody=(ctlbody*)(&pHeader[1]); // Point to the body.
    char czTitle[sizeof(pBody->title) + 1]; // Title string in buffer may
    memset(czTitle,0, sizeof(pBody->title)+1); // not be null terminated..
    strncpy(czTitle, pBody->title, sizeof(pBody->title)); // copy to cz string
    m_sTitle = czTitle; // and cache in the Title member data.
  }

  // Finally perform the default behavior.
  CBufferDecoder::operator()(nBytes, pBuffer, rAnalyzer);
}

/////////////////////////////////////////////////////////////////
//
// Function:
//     virtual   string getTitle();
// Operation type:
//     Selector:
string CNSCLJumboBufferDecoder::getTitle() {
  return m_sTitle;
}


/*!
    Return true if the sizes in the current buffer are 32 bits
*/
bool
CNSCLJumboBufferDecoder::size32()
{
  BHEADER* pHeader = static_cast<BHEADER*>(getBuffer());

  TranslatorPointer<UShort_t> p(*(getBufferTranslator()), pHeader->buffmt);
  UShort_t  version  = *p;

  return (version >= 6);
  
}
