/*
  FilterEventProcessor.cpp
*/

// Copyright Notice:
static const char* Copyright =
  "FilterEventProcessor.cpp: Copyright 2003 NSCL, All rights reserved\n";

// Header files:
#include <string.h>

#include <Globals.h>
#include <Parameter.h>
#include <EventProcessor.h>
#include <EventList.h>
#include <Histogrammer.h>

#include "FilterEventProcessor.h"

// Constructors:
CFilterEventProcessor::CFilterEventProcessor() :
  m_fReadError(kfFALSE),
  m_fHeaderReceived(kfFALSE),
  m_nBUFFERSIZE(8192), // 8K. Default.
  m_pBuffer((char*)kpNULL),
  m_nOffset(0),
  m_sTag(""),
  m_nValidParameters(0),
  m_pBitMask((char*)kpNULL)
{
  m_vParInfo.clear();
  m_vEvents.clear();
};

CFilterEventProcessor::~CFilterEventProcessor() {
  m_sTag = "";
};

// Operators:
#if 0 // Commented-out.
Bool_t CFilterEventProcessor::operator()(const Address_t pEvent,
					 CEvent& rEvent,
					 CAnalyzer& rAnalyzer,
					 CBufferDecoder& rDecoder);
#endif
Bool_t CFilterEventProcessor::operator()(const Address_t pData,
					 CAnalyzer& rAnalyzer,
					 CBufferDecoder& rDecoder)
{
  m_fReadError = kfFALSE;
  m_pBuffer = (char*)kpNULL;
  m_nOffset = 0;
  m_sTag = "";
  //m_nValidParameters = 0; // NO! The buffer need NOT have header information at the beginning so we will use what is already specified.
  m_pBitMask = (char*)kpNULL;

  Float_t nParameter = 0;
  string sBitMask = "";
  UInt_t nParIndex = 0;
  string sParName = "";
  UInt_t nParId = 0;

  m_pBuffer = (char*)pData;

  while(!m_fReadError) {
    m_sTag = "";
    nParIndex = 0;
    sParName = "";
    nParId = 0;

    read_string(m_sTag);
    if(m_sTag == "header") {
      m_nValidParameters = 0;
      m_vParInfo.clear(); // Clear out old parameter data.

      // Start reading header data.
      read_uint(m_nValidParameters);
      CParameter* pParameter = (CParameter*)kpNULL;
      for(UInt_t i = 0; i < m_nValidParameters; i++) {
	CParInfo* pParInfo = new CParInfo;
	// Index:
	read_uint(nParIndex); //Not needed, but has to be done in order to move the offset.
	//pParInfo->setParIndex(nParIndex); // Not done. The index is... the index! (DUH!)
	// Name:
	read_string(sParName);
	pParInfo->setParName(sParName);
	// ID:
	read_uint(nParId);
	pParInfo->setParId(nParId);
	// Check validity. Set (in)active.
	pParameter = ((CHistogrammer*)gpEventSink)->FindParameter(sParName);
	if(pParameter != (CParameter*)kpNULL) { // If parameter is present in dictionary...
	  pParInfo->setActive(kfTRUE);
	} else {
	  pParInfo->setActive(kfFALSE);
	  cerr << "Error: Invalid parameter. (" << sParName << ")" << endl; // Should (more ideally) throw an exception.
	}
	m_vParInfo.push_back(pParInfo);
      }
      continue; // Go to next loop iteration.
    } else if(m_sTag == "event") {
      //read_array();
      read_string(sBitMask);
      m_pBitMask = (char*)(sBitMask.c_str());
      CEvent* pEvent = new CEvent;
      for(UInt_t i = 0; i < m_nValidParameters; i++) {
	if((m_vParInfo[i])->isActive()) {
	  if(m_pBitMask[i] == '1') {
	    read_float(nParameter);
	    (*pEvent)[i] = nParameter;
	  }
	}
      }
      m_vEvents.push_back(pEvent);
      continue;
    } else if(m_sTag == "endofrecord") {
      m_sTag = "";
      break; // Break out of loop.
    } else {
      cerr << "Error: Invalid tag type. (" << m_sTag << ")" << endl;
      m_sTag = "";
      m_fReadError = kfTRUE;
      break;
    }
  }
  m_sTag = "";
  return kfTRUE;
};

// Additional functions:
Bool_t CFilterEventProcessor::OnOther() {
  // COMPLETE THIS!!! **************************************************************************
  m_pBuffer = (char*)(gpBufferDecoder->getBody());
  return kfTRUE;
};

Bool_t CFilterEventProcessor::read_string(string& rString) {
  // String must be null-terminated!
  char* pString = (char*)kpNULL;
  rString = "";

  pString = (char*)(m_pBuffer + m_nOffset);
  rString = string(pString);

  incr_offset(strlen(pString) + 1); // Add 1 for the null terminator.
  return kfTRUE;
};

Bool_t CFilterEventProcessor::read_uint(UInt_t& rUInt) {
  // Assuming 32-bit. Bit/byte ordering/order are dependent on the "endianness" of the architecture. BEWARE!
  rUInt = 0;
  char byte0, byte1, byte2, byte3;
  byte0 = m_pBuffer[m_nOffset];
  byte1 = m_pBuffer[m_nOffset + 1];
  byte2 = m_pBuffer[m_nOffset + 2];
  byte3 = m_pBuffer[m_nOffset + 3];

#if 0 // Commented-out. Incorrect, probably due to machine "endianness".
  rUInt =
    (((UInt_t)byte0 & 0xFF) << 24) |
    (((UInt_t)byte1 & 0xFF) << 16) |
    (((UInt_t)byte2 & 0xFF) << 8) |
    ((UInt_t)byte3 & 0xFF);
#endif
  rUInt =
    (((UInt_t)byte3 & 0xFF) << 24) |
    (((UInt_t)byte2 & 0xFF) << 16) |
    (((UInt_t)byte1 & 0xFF) << 8) |
    ((UInt_t)byte0 & 0xFF);

  incr_offset(4);
  return kfTRUE;
};

#if 0 // Commented-out. No longer necessary.
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
  return kfTRUE;
};
#endif

Bool_t CFilterEventProcessor::read_float(Float_t& rFloat) {
  // Assuming 32-bit.
  rFloat = 0;
#if 0 // Commented-out. (Does not work.)
  char byte0, byte1, byte2, byte3;
  byte0 = m_pBuffer[m_nOffset];
  byte1 = m_pBuffer[m_nOffset + 1];
  byte2 = m_pBuffer[m_nOffset + 2];
  byte3 = m_pBuffer[m_nOffset + 3];

  rFloat = (Float_t)(
		     (((UInt_t)byte3 & 0xFF) << 24) |
		     (((UInt_t)byte2 & 0xFF) << 16) |
		     (((UInt_t)byte1 & 0xFF) << 8)  |
		     (((UInt_t)byte0 & 0xFF) << 0)
		     );
#endif
  memcpy((char*)(&rFloat), (char*)(m_pBuffer + m_nOffset), sizeof(Float_t));
  cerr << "Parameter data (float): " << rFloat << endl;

  incr_offset(sizeof(Float_t));
  return kfTRUE;
};

Bool_t CFilterEventProcessor::read_char(char& rChar) {
  rChar = m_pBuffer[m_nOffset];
  return kfTRUE;
};

void CFilterEventProcessor::incr_offset(UInt_t nIncrement) {
  // Increment the offset, checking that it is within bounds.
  m_nOffset += nIncrement;
  if(m_nOffset >= m_nBUFFERSIZE) {
    cerr << "Error: CFilterEventProcessor::incr_offset given illegal offset value.\n";
    m_nOffset = 0;
  }
};

vector<CEvent*>* CFilterEventProcessor::getEvents() {
  return &m_vEvents;
};
