/*
  FilterEventProcessor.cpp
*/

#include <Histogrammer.h>

// Copyright Notice:
static const char* Copyright =
  "OutputEventStream.cpp: Copyright 2003 NSCL, All rights reserved\n";

// Header files:
#include <Globals.h>
#include <Parameter.h>
#include <EventProcessor.h>

#include "FilterEventProcessor.h"

// Constructors:
CFilterEventProcessor::CFilterEventProcessor() :
  m_fReadError(kfFALSE),
  m_fHeaderReceived(kfFALSE),
  m_nBUFFERSIZE(8192), // 8K. Default.
  m_nOffset(0),
  m_sTag(""),
  m_nValidParameters(0)
{
  m_pBuffer = new char[m_nBUFFERSIZE];
  m_vParInfo.clear();
  m_vBuffer.clear();

  if(m_pValidParameterArray) {
    delete m_pValidParameterArray; // Just in case.
  }
  m_pValidParameterArray = (UInt_t**)kpNULL;
};

CFilterEventProcessor::~CFilterEventProcessor() {
  delete m_pBuffer;

  if(m_pValidParameterArray) {
    delete m_pValidParameterArray; // Just in case.
  }
};

// Operators:
Bool_t CFilterEventProcessor::operator()(const Address_t pEvent,
					 CEvent& rEvent,
					 CAnalyzer& rAnalyzer,
					 CBufferDecoder& rDecoder)
{
  UInt_t True(1), False(0);
  Float_t nParameter = 0;

  while(!m_fReadError) {
    read_string(m_sTag);
    if(m_sTag == "header") {
      m_vParInfo.clear(); // Clear out old parameter data.
      // Start reading header data.
      read_uint(m_nValidParameters);
      string sParName = "";
      UInt_t nParId = 0;
      CParInfo* pParInfo = (CParInfo*)kpNULL;
      CParameter* pParameter = (CParameter*)kpNULL;
      for(UInt_t i = 0; i < m_nValidParameters; i++) {
	read_string(sParName);
	read_uint(nParId);
	pParInfo = new CParInfo;
	pParInfo->setParName(sParName);
	pParInfo->setParId(nParId);
	// Check validity. Set (in)active. **************************************************************************
	pParameter = ((CHistogrammer*)gpEventSink)->FindParameter(sParName);
	if(pParameter != (CParameter*)kpNULL) { // If parameter is present in dictionary...
	  pParInfo->setActive(kfTRUE);
	} else {
	  pParInfo->setActive(kfFALSE);
	  cerr << "Error: Invalid parameter (" + sParName + ").\n"; // Should (more ideally) throw an exception.
	}
	m_vParInfo.push_back(pParInfo);
      }
      continue; // Go to next loop iteration.
    } else if(m_sTag == "event") {
      read_array();
      for(UInt_t i = 0; i < m_nValidParameters; i++) {
	if((m_vParInfo[i])->isActive()) {
	  if((*m_pValidParameterArray)[i] == True) {
	    read_float(nParameter);
	  }
	}
      }
      continue;
    } else if(m_sTag == "endofrecord") {
      break; // Break out of loop.
    } else {
      cerr << "Error: Invalid tag type.\n";
      m_fReadError = kfTRUE;
      break;
    }
  }
  return kfTRUE;
};

// Additional functions:
Bool_t CFilterEventProcessor::OnOther() {
  // COMPLETE THIS! **********************************************************************
  gpBufferDecoder->getBody();
  return kfTRUE;
};

Bool_t CFilterEventProcessor::read_string(string& rString) {
  // String must be null-terminated!
  char* pString = (char*)(m_pBuffer[m_nOffset]);
  rString = string(pString);

  //incr_offset(rString.length() + 1); // The 1 being for the \0 null terminator.
  incr_offset(sizeof(pString)); // No need to add 1 for the null terminator. It is already counted by sizeof.
}

Bool_t CFilterEventProcessor::read_uint(UInt_t& rUint) {
  // Assuming 32-bit.
  char byte0, byte1, byte2, byte3;
  byte0 = m_pBuffer[m_nOffset];
  byte1 = m_pBuffer[m_nOffset + 1];
  byte2 = m_pBuffer[m_nOffset + 2];
  byte3 = m_pBuffer[m_nOffset + 3];

  rUint =
    (((Int_t)byte0 & 0xFF) << 24) |
    (((Int_t)byte1 & 0xFF) << 16) |
    (((Int_t)byte2 & 0xFF) << 8) |
    ((Int_t)byte3 & 0xFF);

  incr_offset(4);
}

Bool_t CFilterEventProcessor::read_array() {
  // I would prefer to use a vector which would allow simple appends without having to know the length for creation, but...
  if(m_pValidParameterArray) {
    delete m_pValidParameterArray;
  }
  //m_pValidParameterArray = ((Float_t[])*)kpNULL;
  UInt_t UIntArray[m_nValidParameters];
  m_pValidParameterArray = (UInt_t**)(&UIntArray);
  UInt_t nUInt = 0;
  for(UInt_t i = 0; i < m_nValidParameters; i++) {
    read_uint(nUInt);
    ((UInt_t[])(*m_pValidParameterArray))[i] = nUInt;
  }
}

Bool_t CFilterEventProcessor::read_float(Float_t& rFloat) {
  // Assuming 32-bit.
  char byte0, byte1, byte2, byte3;
  byte0 = m_pBuffer[m_nOffset];
  byte1 = m_pBuffer[m_nOffset + 1];
  byte2 = m_pBuffer[m_nOffset + 2];
  byte3 = m_pBuffer[m_nOffset + 3];

  rFloat =
    (((Int_t)byte0 & 0xFF) << 24) |
    (((Int_t)byte1 & 0xFF) << 16) |
    (((Int_t)byte2 & 0xFF) << 8) |
    ((Int_t)byte3 & 0xFF);

  incr_offset(4);
}

Bool_t CFilterEventProcessor::read_char(char& rChar) { // ************************************************************
  return true;
}

void CFilterEventProcessor::incr_offset(UInt_t nIncrement) {
  // Increment the offset, checking that it is within bounds.
  m_nOffset += nIncrement;
  if(m_nOffset >= m_nBUFFERSIZE) {
    cerr << "Error: CFilterEventProcessor::incr_offset given illegal offset value.\n";
    m_nOffset = 0;
  }
}
