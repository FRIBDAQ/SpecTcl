/*
  OutputEventStream.cpp
*/

// Copyright Notice.
static const char* Copyright =
  "OutputEventStream.cpp: Copyright 2003 NSCL, All rights reserved\n";

// Header Files.
//#include <stdlib.h>
#include <vector>
#include <fstream>

#include "Event.h"
#include "EventList.h"

#include "OutputEventStream.h"

// Constructors.
COutputEventStream::COutputEventStream() : // Should no longer be called due to the implementation and use of EventFilter::ParseFileName which sets the default file name.
  m_fActive(false),
  m_sFileName(""),
  m_nMAXBUFFERSIZE(16) // Arbitrarily chosen.
{
  char* pHomeDir = getenv("HOME");
  if(pHomeDir) {
    m_sFileName = string(pHomeDir);
  }
  m_sFileName += "/filteroutput.txt";

  Buffer.clear(); // I just hope to * that this is not a necessary action in C++.
  Open();
}

COutputEventStream::COutputEventStream(string& rFileName) :
  m_fActive(false),
  m_sFileName(rFileName),
  m_nMAXBUFFERSIZE(16) // Arbitrarily chosen.
{
  Buffer.clear(); // I just hope to * that this is not a necessary action in C++.
  Open();
}

COutputEventStream::~COutputEventStream() {
  Close();
}

// Operators.
Bool_t COutputEventStream::operator()(const string& rFileName) {
  m_sFileName = rFileName;
  return kfTRUE;
};

Bool_t COutputEventStream::operator()(const CEvent& rEvent) {
  return ReceiveEvent(rEvent);
};

COutputEventStream& COutputEventStream::operator=(const COutputEventStream& rRhs) {
  return *this;
};

// Additional functions.
string COutputEventStream::getFileName() {
  return m_sFileName;
}

Bool_t COutputEventStream::Open() { // Always means that the file has been opened and the stream is active.
  Close(); // Clear everything.

  if(!m_fActive) {
    m_ofs.open(m_sFileName.c_str(), ofstream:: out | ofstream::app); // open accepts only a char*.
    m_fActive = m_ofs.is_open(); //m_ofs.good();
  }
  return m_fActive;
}

Bool_t COutputEventStream::Close() {
  if(m_ofs.is_open()) {
    SendBuffer();
    m_ofs.close();
  }
  m_fActive = false;
  return !m_fActive; // Always true which always means success with me, but this is arbitrary.
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
    cerr << "Error: Trying to output to inactive stream.\n";
    return false;
  }
}

Bool_t COutputEventStream::SendBuffer() {
  if(m_ofs.is_open() && m_ofs.good() && Buffer.size()>0) {
    // Send buffer.
    //for(int i=0; i<Buffer.size(); i++) { // For each and every CEvent in Buffer,
    for(vector<CEvent>::iterator i=Buffer.begin(); i!=Buffer.end(); i++) { // Just trying.
      //for(int j=0; j<((CEvent)Buffer[i]).size(); j++) { // For each and every Parameter in CEvent,
      for(CEventIterator j=i->begin(); j!=i->end(); j++) { // Just tryin'
	// Output the parameter in the event in the buffer.
	//m_ofs << ((CEvent)(Buffer[i]))[j] << " "; // Done with parameter.
	//m_ofs << (*j) << " "; // Done with parameter.
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
