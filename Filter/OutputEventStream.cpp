/*
  OutputEventStream.cpp
*/

// Copyright Notice.
static const char* Copyright = 
  "OutputEventStream.cpp: Copyright 2003 NSCL, All rights reserved\n";

// Header Files.
#include "OutputEventStream.h"
#include "EventFilter.h"
#include <vector>
#include "Histogrammer.h"
#include "Parameter.h"
#include "Event.h"
#include "EventList.h"
#include <fstream.h>

// Constructors.
COutputEventStream::COutputEventStream() {
  m_nMAXBUFFERSIZE = 16; // Arbitrarily chosen.
  Open();
}

COutputEventStream::COutputEventStream(const CEvent& rEvent) {
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
  m_nFd.open("/tmp/coutputeventstream.txt"); // CHECK FOR EXCEPTIONS ************
  return (m_nFd.good());
}

Bool_t COutputEventStream::Close() {
  SendBuffer(); // Send whatever is left.
  m_nFd.close(); // CHECK FOR EXCEPTIONS *************************
  return kfTRUE;
}

Bool_t COutputEventStream::ReceiveEvent(const CEvent& rEvent) {
  if(Buffer.size() >= m_nMAXBUFFERSIZE) {
    SendBuffer();
  }
  Buffer.push_back(rEvent);
}

Bool_t COutputEventStream::SendBuffer() {
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
}
