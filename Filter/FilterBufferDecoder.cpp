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

  m_nParameters(0),
  m_fEventData(kfFALSE),
  m_nEvents(0),
  m_nBodyOffset(0),
  m_nOffset(0),
  m_nBufferSize(8192) // 8KB.
{
  m_vParInfo.clear();
  m_vBuffer.clear();
};

CFilterBufferDecoder::~CFilterBufferDecoder() {};

// Operators:
void CFilterBufferDecoder::operator()(UInt_t nBytes,
				      Address_t pBuffer,
				      CAnalyzer& rAnalyzer)
{
  if(!m_fEventData) {
    // Beginning. Start reading documentation data.
    // NOTE: Block size MUST be sufficiently large to read entire header on first pass.
    xdrmem_create(&m_xdrs, (char*)pBuffer, m_nSize, XDR_DECODE); // Read from memory buffer.

    if(XDRstring(m_sSection)) {
      if(m_sSection == "header") {
	// Start reading header data.
	XDRuint(m_nParameters);
	string sParName = "";
	UInt_t nParId = 0;
	CParInfo* pParInfo = (CParInfo*)kpNULL;
	CParameter* pParameter = (CParameter*)kpNULL;
	for(UInt_t i = 0; i < m_nParameters; i++) {
	  XDRstring(sParName);
	  XDRuint(nParId);
	  pParInfo = new CParInfo;
	  pParInfo->setParName(sParName);
	  pParInfo->setParId(nParId);
	  // CHECK VALIDITY. SET (IN)ACTIVE. *************************
	  pParameter = ((CHistogrammer*)gpEventSink)->FindParameter(sParName);
	  if(pParameter != (CParameter*)kpNULL) { // If parameter is present in dictionary...
	    pParInfo->setActive(kfTRUE);
	  } else {
	    cerr << "Error: Invalid parameter (" + sParName + ").\n"; // Should (more ideally) throw an exception.
	    pParInfo->setActive(kfFALSE);
	  }
	  m_vParInfo.push_back(pParInfo);
	}

	m_fEventData = kfTRUE; // Now ready for event data.
	return;
      } else {
	cerr << "Error: No header. Invalid file format.\n";
	m_fXDRError = kfTRUE;
	return;
      }
    } // End XDRstring. Errors already checked.
  }

  // Now start reading event data.
  UInt_t True(1), False(0);
  Float_t nParameter = 0;
  while(1) { // Do forever.
    XDRarray(); // Read the bit-mask array into member array.
    // It may be advisable to check here that the number of elements in m_vParInfo matches that of the bit-mask array.********************
    for(UInt_t i = 0; i < m_vParInfo.size(); i++) {
      if((m_vParInfo[i])->isActive()) {
	if(*(ValidParameterArray[i]) == True) {
	  XDRfloat(nParameter);
	}
      }
    }
  }
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
  return (m_nBufferSize - m_nBodyOffset);
}

UInt_t CFilterBufferDecoder::getRun() { // Just a stub.
  return 0;
}

UInt_t CFilterBufferDecoder::getEntityCount() {
  // *******************************************************************
  if(m_fEventData) {

  }
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
// They can be used for either output (XDR_ENCODE) and input (XDR_DECODE) depending on the specified create operation.
Bool_t CFilterBufferDecoder::XDRstring(string& rString) {
  char* pString = (char*)(rString.c_str());
  if(xdr_string(&m_xdrs, &pString, (sizeof(pString)))) {
    memcpy((m_pOutputBuffer + m_nOffset), &rString, sizeof(rString));
    m_nOffset += sizeof(rString);
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: xdr_string unsuccessful.\n";
    return kfFALSE;
  }
}

Bool_t CFilterBufferDecoder::XDRuint(UInt_t& rUInt) {
  if(xdr_u_int(&m_xdrs, &rUInt)) {
    memcpy((m_pOutputBuffer + m_nOffset), &rUInt, sizeof(rUInt));
    m_nOffset += sizeof(rUInt);
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: xdr_u_int unsuccessfull.\n";
    return kfFALSE;
  }
}

Bool_t CFilterBufferDecoder::XDRarray() {
  if(xdr_array(&m_xdrs, (char**)(&ValidParameterArray), &m_nValidParameters, 100, (UInt_t)sizeof(UInt_t), (xdrproc_t)xdr_u_int)) {
    m_nEvents++;
    memcpy((m_pOutputBuffer + m_nOffset), &ValidParameterArray, sizeof(ValidParameterArray));
    m_nOffset += sizeof(ValidParameterArray);
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: xdr_array unsuccessful.\n";
    return kfFALSE;
  }
}

Bool_t CFilterBufferDecoder::XDRfloat(Float_t& rFloat) {
  if(xdr_float(&m_xdrs, &rFloat)) {
    memcpy((m_pOutputBuffer + m_nOffset), &rFloat, sizeof(rFloat));
    m_nOffset += sizeof(rFloat);
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
    memcpy((m_pOutputBuffer + m_nOffset), &String, sizeof(String));
    m_nOffset = 0;
    return kfTRUE;
  } else {
    m_fXDRError = kfTRUE;
    cerr << "Error: xdr_string unsuccessful.\n";
    return kfFALSE;
  }

}
