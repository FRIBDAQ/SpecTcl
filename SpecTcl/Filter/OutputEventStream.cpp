/*
  OutputEventStream.cpp
*/

// Copyright Notice.
static const char* Copyright = 
  "OutputEventStream.cpp: Copyright 2003 NSCL, All rights reserved\n";

// Header Files.
#include <vector>
#include <fstream.h>

#include "OutputEventStream.h"
//#include "Parameter.h"
#include "Event.h"
#include "EventList.h"

// Constructors.
COutputEventStream::COutputEventStream() :
  m_fActive(false)
{
  m_nMAXBUFFERSIZE = 16; // Arbitrarily chosen.
  Open();
}

COutputEventStream::COutputEventStream(const CEvent& rEvent) :
  m_fActive(false)
{
  m_nMAXBUFFERSIZE = 16; // Arbitrarily chosen.
  Open();
}

COutputEventStream::~COutputEventStream() {
  Close();
}

// Operators.
Bool_t COutputEventStream::operator()() {
  return kfTRUE;
};

COutputEventStream& COutputEventStream::operator=(const COutputEventStream& rRhs) {
  return *this;
};

// Additional functions.
Bool_t COutputEventStream::Open() { // Open for APPEND *************************
  if(!m_fActive) {
    if(!m_nFd.is_open()) {
      m_nFd.open("/tmp/coutputeventstream.txt");
    }
    if(m_nFd.good()) {
      m_fActive = true;
      return true;
    } else {
      return false;
    }
  }
}

Bool_t COutputEventStream::Close() {
  if(m_fActive) { // Attempt to close only if not already so.
    if(m_nFd.is_open()) {
      SendBuffer(); // Send whatever is left.
      m_nFd.close(); // No return value. No exceptions likely.
    }
    m_fActive = false;
  }
  return !m_fActive; // true always means success with me, but this is arbitrary.
}

Bool_t COutputEventStream::ReceiveEvent(const CEvent& rEvent) {
  if(m_fActive) {
    if(Buffer.size() >= m_nMAXBUFFERSIZE) {
      SendBuffer();
    }
    Buffer.push_back(rEvent);
  }
}

Bool_t COutputEventStream::SendBuffer() {
  if(m_nFd.is_open() && m_nFd.good() && Buffer.size()>0) {
    // Send buffer.
    for(int i=0; i<Buffer.size(); i++) { // For each and every CEvent in Buffer,
      for(int j=0; j<((CEvent)(Buffer[i])).size(); j++) { // For each and every Parameter in CEvent,
	// Output the parameter in the event in the buffer.
	m_nFd << ((CEvent)(Buffer[i]))[j] << " "; // Done with parameter.
      }
      // Done with event.
      m_nFd << endl;
    }
    // Done with buffer.
    m_nFd << endl;
    m_nFd << flush;
    Buffer.clear();
    return kfTRUE;
  } else {
    return kfFALSE;
  }
}
