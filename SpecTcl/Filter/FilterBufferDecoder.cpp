/*
  FilterBufferDecoder.cpp
*/

// Copyright Notice:
static const char* Copyright =
  "OutputEventStream.cpp: Copyright 2003 NSCL, All rights reserved\n";

// Header files:
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <rpc/xdr.h> // rpc.h includes xdr.h
#include <assert.h>
#include <string.h>

#include "histotypes.h"
#include "buffer.h"
#include "buftypes.h"

#include "Globals.h"
#include "Histogrammer.h"
#include "Event.h"
#include "EventList.h"

#include "Analyzer.h"
#include "EventFormatError.h"
#include "BufferTranslator.h"
#include "TranslatorPointer.h"

#include "FilterBufferDecoder.h"

// Constructors:
CFilterBufferDecoder::CFilterBufferDecoder() :
  m_fActive(kfFALSE),
  m_fXDRError(kfFALSE),
  m_sTag(""),
  m_nValidParameters(0),
  m_pBitMask((char*)kpNULL),
  m_nEvents(0),
  m_nOffset(0),
  m_nOutputBufferOffset(0),
  m_nBUFFERSIZE(8192), // 8KB. Default.
  m_pOutputBuffer((char*)kpNULL)
{
  m_pOutputBuffer = (char*) new char[m_nBUFFERSIZE];
};

CFilterBufferDecoder::~CFilterBufferDecoder() {
  if(m_pBitMask != (char*)kpNULL) {
    delete[] m_pBitMask;
    m_pBitMask = (char*)kpNULL;
  }

  if(m_pOutputBuffer != (char*)kpNULL) {
    delete[] m_pOutputBuffer;
    m_pOutputBuffer = (char*)kpNULL;
  }
};

// Operators:
void CFilterBufferDecoder::operator()(UInt_t nBytes,
				      Address_t pBuffer,
				      CAnalyzer& rAnalyzer)
{
  cerr << endl; // REMOVE.**************************************
  m_fXDRError = kfFALSE;
  m_sTag = "";
  m_nValidParameters = 0;
  m_nEvents = 0;
  m_nOffset = 0;
  m_nOutputBufferOffset = 0;

  if(m_pBitMask != (char*)kpNULL) {
    delete[] m_pBitMask;
    m_pBitMask = (char*)kpNULL;
  }
  // We do not know the number of parameters, so we don't make the bit-mask array yet.

  if(m_pOutputBuffer != (char*)kpNULL) {
    delete[] m_pOutputBuffer;
    m_pOutputBuffer = (char*)kpNULL;
  }
  m_pOutputBuffer = (char*) new char[m_nBUFFERSIZE];

  // Local variables:
  UInt_t nParIndex = 0;
  string sParName = "";
  UInt_t nParId = 0;
  Float_t nFloat = 0;
  string sBitMask = "";

  xdrmem_create(&m_xdrs, (char*)pBuffer, m_nBUFFERSIZE, XDR_DECODE); // READ from memory buffer.
  // Neither headers nor events should span buffer boundaries, so the very first thing we read should be a string specifying the format of data immediately following.
  while(!m_fXDRError) {
    if(XDRstring(m_sTag)) { // Read the tag.
      if(m_sTag == "header") {
	cerr << "Header beginning..." << endl;
	// Start processing header data.
	// NOTE: Block size MUST be sufficiently large to read entire header on first pass.
	//       Also, arrays sizes will always be equal to the number of parameters defined in the header/documentation buffer.
	XDRuint(m_nValidParameters);
	m_pBitMask = (char*) new char[m_nValidParameters + 1]; // Add 1 for the null terminator.
	m_pBitMask[m_nValidParameters] = '\n'; // Set null terminator right now.
	cerr << " Number of valid parameters: " << m_nValidParameters << endl;

	for(UInt_t i = 0; i < m_nValidParameters; i++) {
	  cerr << " Parameter:" << endl;
	  XDRuint(nParIndex); // Simply an index (starting from 0) for the parameter.
	  cerr << "  Index: " << nParIndex << endl;
	  // Some simple error checking:
	  if(i != nParIndex) {
	    m_fXDRError = kfTRUE;
	    cerr << "Error: Inconsistent parameter indices.\n";
	  }
	  XDRstring(sParName);
	  cerr << "  Name: " << sParName << endl;
	  XDRuint(nParId);
	  cerr << "  Id: " << nParId << endl;
	}
	cerr << "... End of header." << endl << endl;
	continue;
      } else if(m_sTag == "event") {
	cerr << "Event beginning..." << endl;
	// Start processing event data.
	if(XDRstring(sBitMask)) {
	  cerr << " Bit Mask: ";
	  for(UInt_t i = 0; i < m_nValidParameters; i++) {
	    cerr << sBitMask[i] << ", ";
	  }
	  cerr << endl;

	  for(UInt_t i = 0; i < m_nValidParameters; i++) {
	    // Not really necessary, but...
	    m_pBitMask[i] = sBitMask[i];

	    if(sBitMask[i] == '1') {
	      XDRfloat(nFloat);
	      cerr << " ParData (" << i << "): " << nFloat << endl;
	    } else if(sBitMask[i] == '0') {
	      // Ignore.
	    } else {
	      cerr << "Error: Corrupt array element (" << sBitMask[i] << ")." << endl;
	    }
	  }
	} else {
	  break;
	}
	cerr << "... End of event." << endl << endl; //**************************
	continue;
      } else if(m_sTag == "endofrecord") {
	cerr << "End of record received." << endl;
	m_fXDRError = kfFALSE; // Reset error flag, but still ...
	break; // break out of the loop.
      } else {
	m_fXDRError = kfTRUE;
	cerr << "Error: Invalid tag type.\n";
	break;
      }
    } else {
      m_fXDRError = kfTRUE;
      cerr << "Error: Invalid data format. Tag type must first be specified.\n";
    }
    cerr << "*** Done with while loop." << endl; //***************************
  }

  cerr << "*** Leaving CFilterBufferDecoder::operator()." << endl; // ********************************
  return; // Exit.
}

// Additional functions.
// The following functions are required as they are inherited from CBufferDecoder as pure virtual functions.
const Address_t CFilterBufferDecoder::getBody() {
  CBufferDecoder::ThrowIfNoBuffer
    ("CFilterBufferDecoder::getBody - Getting buffer.");
  UInt_t address = (UInt_t)(getBuffer());
  address += m_nOffset; // HOW IS THIS DONE?  *********************************************************
  return ((Address_t)address);
}

UInt_t CFilterBufferDecoder::getBodySize() {
  return (m_nBUFFERSIZE - m_nOffset);
}

UInt_t CFilterBufferDecoder::getRun() { // Just a stub.
  return 0;
}

UInt_t CFilterBufferDecoder::getEntityCount() { // Just a stub.
  return 0;
}

UInt_t CFilterBufferDecoder::getSequenceNo() { // Just a stub.
  return 0;
}

UInt_t CFilterBufferDecoder::getLamCount() { // Just a stub.
  return 0;
}

UInt_t CFilterBufferDecoder::getPatternCount() { // Just a stub.
  return 0;
}

UInt_t CFilterBufferDecoder::getBufferType() { // Just a stub.
  return 0;
}

void CFilterBufferDecoder::getByteOrder(Short_t& Signature16, Int_t& Signature32) { // Just a stub.
  return;
}

string CFilterBufferDecoder::getTitle() { // Just a stub.
  return string("");
}

Bool_t CFilterBufferDecoder::isActive() {
  return m_fActive;
}

// The following XDR functions are intended to eliminate the need to repeat semantics.
// With slight modification, they can be used for either output (XDR_ENCODE) and input (XDR_DECODE) depending on the specified create operation.
Bool_t CFilterBufferDecoder::XDRstring(string& rString) {
  char* pString = (char*)kpNULL;
  if(xdr_string(&m_xdrs, &pString, 100)) {
    rString = string(pString);
    memcpy((m_pOutputBuffer + m_nOutputBufferOffset), &pString, strlen(pString));
    incr_offset(strlen(pString));
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: CFilterBufferDecoder::XDRstring unsuccessful.\n";
    return kfFALSE;
  }
}

Bool_t CFilterBufferDecoder::XDRuint(UInt_t& rUInt) {
  if(xdr_u_int(&m_xdrs, &rUInt)) {
    memcpy((m_pOutputBuffer + m_nOutputBufferOffset), &rUInt, sizeof(rUInt)); // Size should be 4.
    incr_offset(sizeof(rUInt));
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: CFilterBufferDecoder::XDRuint unsuccessfull.\n";
    return kfFALSE;
  }
}

#if 0 // Commented out.
Bool_t CFilterBufferDecoder::XDRarray() {
  if(xdr_array(&m_xdrs, (char**)(*m_pValidParameterArray), &m_nValidParameters, 100, (UInt_t)sizeof(UInt_t), (xdrproc_t)xdr_u_int)) {
    cerr << " Array received: ";
    for(int i = 0; i < m_nValidParameters; i++) {
      cerr << (UInt_t)((**m_pValidParameterArray)[i]) << ",";
    }
    cerr << " (Length = " << m_nValidParameters << ")." << endl;
    m_nEvents++;
    //memcpy((m_pOutputBuffer + m_nOutputBufferOffset), &ValidParameterArray, sizeof(ValidParameterArray));
    // Send the array size first, then send the elements.
    memcpy((m_pOutputBuffer + m_nOutputBufferOffset), &m_nValidParameters, sizeof(m_nValidParameters));
    incr_offset(sizeof(m_nValidParameters));
    UInt_t nElement = 0;
    for(UInt_t i = 0; i < m_nValidParameters; i++) {
      memcpy((m_pOutputBuffer + m_nOutputBufferOffset), &nElement, sizeof(nElement));
      incr_offset(sizeof(nElement));
    }
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: CFilterBufferDecoder::XDRarray unsuccessful.\n";
    return kfFALSE;
  }
}
#endif

Bool_t CFilterBufferDecoder::XDRfloat(Float_t& rFloat) {
  if(xdr_float(&m_xdrs, &rFloat)) {
    memcpy((m_pOutputBuffer + m_nOutputBufferOffset), &rFloat, sizeof(rFloat)); // Size should be 4.
    incr_offset(sizeof(rFloat));
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: CFilterBufferDecoder::XDRfloat unsuccessful.\n";
    return kfFALSE;
  }
}

#if 0 // Commented out.
Bool_t CFilterBufferDecoder::XDRfill(UInt_t nBytes) { // Pad the rest of the buffer with nBytes zeroes.
  // This function should only be used for WRITING out enough characters to pad the buffer.

  // First send an end-of-record tag.
  m_sTag = "endofrecord";
  XDRstring(m_sTag);
  char* pTag = m_sTag.c_str();
  nBytes -= (strlen(pTag) + 1); // Add 1 to include null terminator, and decrement nBytes by that whole thing.

  nBytes--; // nBytes-1 in order to reserve space for the final null terminator.
  string String = "";
  for(UInt_t i = 0; i < nBytes; i++) {
    String += "0"; // Append nBytes zeroes.
  }
  char* pString;
  pString = (char*)(String.c_str());

  if(xdr_string(&m_xdrs, &pString, (strlen(pString)))) {
    memcpy((m_pOutputBuffer + m_nOutputBufferOffset), &pString, strlen(pString)); // Size should be nBytes. Beware: sizeof(char*) = 4 ALWAYS. Use strlen.
    m_nOutputBufferOffset = 0;
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: CFilterBufferDecoder::XDRfill XDRstring unsuccessful.\n";
    return kfFALSE;
  }

}
#endif

void CFilterBufferDecoder::incr_offset(UInt_t nIncrement) {
  // Increment the offset, checking that it is within bounds.
  m_nOutputBufferOffset += nIncrement;
  if(m_nOutputBufferOffset >= m_nBUFFERSIZE) {
    cerr << "Error: CFilterBufferDecoder::incr_offset given illegal offset value.\n";
    m_nOutputBufferOffset = 0;
  }
}
