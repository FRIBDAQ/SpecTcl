/*
  FilterEventUnpacker.cpp
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
  m_nParameters(0),
  m_fEventData(kfFALSE),
  m_nEvents(0),
  m_nOffset(0),
  m_nOutputBufferOffset(0),
  m_nBUFFERSIZE(8192) // 8KB. Default.
{
  if(m_pOutputBuffer) {
    delete m_pOutputBuffer;
  }
  char CharArray[m_nBUFFERSIZE];
  m_pOutputBuffer = (char**)(&CharArray);
};

CFilterBufferDecoder::~CFilterBufferDecoder() {
  if(m_pOutputBuffer) {
    delete m_pOutputBuffer;
  }
};

// Operators:
void CFilterBufferDecoder::operator()(UInt_t nBytes,
				      Address_t pBuffer,
				      CAnalyzer& rAnalyzer)
{
  m_sTag = "";
  m_fXDRError = kfFALSE;
  m_nParameters = 0;
  m_nEvents = 0;
  m_nOffset = 0;
  m_nOutputBufferOffset = 0;

  xdrmem_create(&m_xdrs, (char*)pBuffer, m_nSize, XDR_DECODE); // Read from memory buffer.
  // Neither headers nor events should span buffer boundaries, so the very first thing we read should be a string specifying the format of data immediately following.
  while(!m_fXDRError) {
    if(XDRstring(m_sTag)) { // Read the tag.
      if(m_sTag == "header") {
	// Start processing header data.
	// NOTE: Block size MUST be sufficiently large to read entire header on first pass.
	XDRuint(m_nParameters);

	UInt_t *(ValidParameterArray[m_nParameters]); // Expensive in a loop. Rewrite. (On second thought, header occurrence is very infrequent.)
	m_pValidParameterArray = (UInt_t***)(&ValidParameterArray);
	UInt_t nParIndex = 0;
	string sParName = "";
	UInt_t nParId = 0;
	for(UInt_t i = 0; i < m_nParameters; i++) {
	  XDRuint(nParIndex); // Simply an index (starting from 0) for the parameter.
	  // Error checking:
	  if(i != nParIndex) {
	    m_fXDRError = kfTRUE;
	    cerr << "Error: Inconsistent parameter indices.\n";
	  }
	  XDRstring(sParName);
	  XDRuint(nParId);
	}
      } else if(m_sTag == "event") {
	// Start processing event data. *******************************************************

      } else if(m_sTag == "endofrecord") {
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
  }

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
  char* pString = (char*)(rString.c_str());
  if(xdr_string(&m_xdrs, &pString, (sizeof(pString)))) {
    memcpy((m_pOutputBuffer + m_nOutputBufferOffset), &pString, sizeof(pString));
    incr_offset(sizeof(rString));
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: xdr_string unsuccessful.\n";
    return kfFALSE;
  }
}

Bool_t CFilterBufferDecoder::XDRuint(UInt_t& rUInt) {
  if(xdr_u_int(&m_xdrs, &rUInt)) {
    memcpy((m_pOutputBuffer + m_nOutputBufferOffset), &rUInt, sizeof(rUInt));
    incr_offset(sizeof(rUInt));
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: xdr_u_int unsuccessfull.\n";
    return kfFALSE;
  }
}

Bool_t CFilterBufferDecoder::XDRarray() {
  if(xdr_array(&m_xdrs, (char**)(*m_pValidParameterArray), &m_nValidParameters, 100, (UInt_t)sizeof(UInt_t), (xdrproc_t)xdr_u_int)) {
    m_nEvents++;
    //memcpy((m_pOutputBuffer + m_nOutputBufferOffset), &ValidParameterArray, sizeof(ValidParameterArray));
    // Send the array size first, then send the elements.
    memcpy((m_pOutputBuffer + m_nOutputBufferOffset), &m_nValidParameters, sizeof(m_nValidParameters));
    incr_offset(sizeof(m_nValidParameters));
    UInt_t nElement = 0;
    for(UInt_t i = 0; i < m_nParameters; i++) {
      memcpy((m_pOutputBuffer + m_nOutputBufferOffset), &nElement, sizeof(nElement));
      incr_offset(sizeof(nElement));
    }
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: xdr_array unsuccessful.\n";
    return kfFALSE;
  }
}

Bool_t CFilterBufferDecoder::XDRfloat(Float_t& rFloat) {
  if(xdr_float(&m_xdrs, &rFloat)) {
    memcpy((m_pOutputBuffer + m_nOutputBufferOffset), &rFloat, sizeof(rFloat));
    incr_offset(sizeof(rFloat));
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: xdr_float unsuccessful.\n";
    return kfFALSE;
  }
}

Bool_t CFilterBufferDecoder::XDRfill(UInt_t nBytes) { // Pad with nBytes zeroes.
  // This function should only be used for WRITING out enough characters to pad the buffer.
  string String = "";
  for(UInt_t i = 0; i < nBytes; i++) {
    String += "0"; // Append nBytes zeroes.
  }
  char* pString;
  pString = (char*)(String.c_str());

  if(xdr_string(&m_xdrs, &pString, (sizeof(pString)))) {
    memcpy((m_pOutputBuffer + m_nOutputBufferOffset), &pString, sizeof(pString));
    m_nOutputBufferOffset = 0;
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: xdr_string unsuccessful.\n";
    return kfFALSE;
  }

}

void CFilterBufferDecoder::incr_offset(UInt_t nIncrement) {
  // Increment the offset, checking that it is within bounds.
  m_nOutputBufferOffset += nIncrement;
  if(m_nOutputBufferOffset >= m_nBUFFERSIZE) {
    cerr << "Error: Illegal offset value.\n";
    m_nOffset = 0;
  }
}
