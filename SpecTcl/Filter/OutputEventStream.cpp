/*
  OutputEventStream.cpp
*/

// Copyright Notice.
static const char* Copyright =
  "OutputEventStream.cpp: Copyright 2003 NSCL, All rights reserved\n";

// Header Files.
#include <vector>
#include <fstream.h>

#include "Event.h"
#include "EventList.h"

#include "OutputEventStream.h"

// Constructors.
COutputEventStream::COutputEventStream() :
  m_fActive(false),
  m_nMAXBUFFERSIZE(16) // Arbitrarily chosen.
{
  Open();
}

COutputEventStream::COutputEventStream(const CEvent& rEvent) :
  m_fActive(false),
  m_nMAXBUFFERSIZE(16) // Arbitrarily chosen.
{
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
Bool_t COutputEventStream::Open() { // Always means that the file has been opened and the stream is active.
  if(!m_fActive) {
    if(!m_ofs.is_open()) {
      m_ofs.open("/tmp/coutputeventstream.txt"); //, fstream::app);
    }
    if(m_ofs.good()) {
      m_fActive = true;
      return true;
    } else {
      return false;
    }
  }
}

Bool_t COutputEventStream::Close() {
  if(m_fActive) { // Attempt to close only if not already so.
    if(m_ofs.is_open()) {
      SendBuffer(); // Send whatever is left.
      m_ofs.close(); // No return value. No exceptions likely.
    }
    m_fActive = false;
  }
  return !m_fActive; // true always means success with me, but this is arbitrary.
}

Bool_t COutputEventStream::ReceiveEvent(const CEvent& rEvent) {
  if(m_fActive) {
    if(Buffer.size() >= m_nMAXBUFFERSIZE) {
      if(!SendBuffer()) { // Send buffer. If code is an error, return false.
	cerr << "Error sending buffer.\n";
	return false; // Failure sending buffer.
      }
    }
    Buffer.push_back(rEvent);
    return true;
  } else {
    // ERROR! Trying to output an event to an inactive stream.
    cerr << "Error! Trying to output to inactive stream.\n";
    return false;
  }
}

Bool_t COutputEventStream::SendBuffer() {
  if(m_ofs.is_open() && m_ofs.good() && Buffer.size()>0) {
    // Send buffer.
    //for(int i=0; i<Buffer.size(); i++) { // For each and every CEvent in Buffer,
    for(vector<CEvent>::iterator i=Buffer.begin(); i!=Buffer.end(); i++) { // Just trying.
      //for(int j=0; j<((CEvent)(Buffer[i])).size(); j++) { // For each and every Parameter in CEvent,
      for(CEventIterator j=(*i).begin(); j!=(*i).end(); j++) { // Just tryin'
	// Output the parameter in the event in the buffer.
	//m_ofs << ((CEvent)(Buffer[i]))[j] << " "; // Done with parameter.
	m_ofs << (*j) << " "; // Done with parameter.
      }
      // Done with event.
      m_ofs << "EOE" << endl;
    }
    // Done with buffer.
    m_ofs << "EOB" << endl;
    m_ofs << flush;
    cout << flush;
    Buffer.clear();
    return kfTRUE;
  } else {
    return kfFALSE;
  }
}
