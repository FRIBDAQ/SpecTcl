/*
  EventFilter.cpp
*/

// Copyright Notice.
static const char* Copyright = 
  "EventFilter.cpp: Copyright 2002 NSCL, All rights reserved\n";

// Header Files.
#include <vector>
#include <string>

#include "Histogrammer.h"
#include "Parameter.h"
#include "Event.h"
#include "EventList.h"

#include "EventFilter.h"
#include "OutputEventStream.h"

// Constructors.
CEventFilter::CEventFilter() :
  m_fEnabled(false),
  //m_sFileName(ParseFileName("")), // Initialized to default.
  m_pOutputEventStream((COutputEventStream*)kpNULL)
{
  string sEmpty = "";
  m_sFileName = ParseFileName(sEmpty); // Just to obtain the default file name.
}

CEventFilter::CEventFilter(string& rFileName) :
  m_fEnabled(false),
  m_sFileName(ParseFileName(rFileName)),
  m_pOutputEventStream((COutputEventStream*)kpNULL)
{}

/*
CEventFilter::CEventFilter(COutputEventStream& rOutputEventStream) :
  m_fEnabled(false),
  m_sFileName("~/filteroutput.txt"), // Just a default. Will be reset.
  m_pOutputEventStream(rOutputEventStream)
{
  // Allows the reuse of a previously defined output event stream.
  // Set the file name in the event filter to that already used in the output event stream.
  string sFileName = m_pOutputEventStream->getFileName();
  setFileName(sFileName);
}
*/

CEventFilter::~CEventFilter() {}

// Operators.
CEventFilter& CEventFilter::operator=(const CEventFilter& rRhs) {
  return *this;
}

// Additional functions.
void CEventFilter::Enable() {
  // Actually does a re-enable to reset the configuration with any new parameters and be on the safe side.
  Disable(); // Clear everything. May be redundant, especially when the file name is reset, but prevents a potential problem.

  m_pOutputEventStream = new COutputEventStream(m_sFileName);
  m_fEnabled = m_pOutputEventStream->Open();
}

void CEventFilter::Disable() {
  if(m_pOutputEventStream != (COutputEventStream*)kpNULL) {
    /*
    if(m_pOutputEventStream->Close()) { // Successfully closed. This should always be the case (as presently implemented).
      delete m_pOutputEventStream; // Prevent memory leaks!
      m_pOutputEventStream = (COutputEventStream*)kpNULL; // OTHERWISE, DELETE WOULD CAUSE A SEGMENTATION FAULT.
    }
    */
    m_pOutputEventStream->Close();
    delete m_pOutputEventStream;
    m_pOutputEventStream = (COutputEventStream*)kpNULL; // OTHERWISE, DELETE WOULD CAUSE A SEGMENTATION FAULT.
  }
  m_fEnabled = false;
}

string CEventFilter::ParseFileName(string& rFileName) {
  /*
    Not currently implemented in OutputEventStream due to potential conflict with respect to setting the default file name when none is specified.
    This should not pose any problems as the user only sets file names with respect to the filter (which then calls the output event stream), and this is taken care of right here.
  */
  char* pHomeDir = getenv("HOME");
  string sHomeDir = "", sFileName = "";

  if(pHomeDir) {
    sHomeDir = string(pHomeDir);
  }

  if(rFileName == "") { // If empty,
    sFileName = sHomeDir + "/filteroutput.txt"; // use default.
  } else if(rFileName[0] == '~') { // Else if it begins with a ~,
    sFileName = sHomeDir + rFileName.substr(1, (rFileName.length()-1));
  } else {
    sFileName = rFileName;
  }

  return sFileName;
}

void CEventFilter::setFileName(string& rFileName) {
  // Whenever this is called, a call to Enable() must be made for the filter to be used.
  Disable(); // Reset output event stream using new file name. (This Disable() call is unnecessary, but I'll leave it just to be explicit.)
  m_sFileName = ParseFileName(rFileName);
}

std::string CEventFilter::getFileName() {
  return m_sFileName;
}

Bool_t CEventFilter::CheckEnabled() {
  return m_fEnabled;
}

Bool_t CEventFilter::CheckCondition(CEvent& rEvent) {
  // OVERRIDDEN BY SUB-CLASS.
  return kfTRUE;
}

void CEventFilter::FormatOutputEvent(CEvent& rEvent) {
  // OVERRIDDEN BY SUB-CLASS.
  //(*m_pOutputEventStream).ReceiveEvent(rEvent);
}

COutputEventStream& CEventFilter::AttachOutputEventStream(COutputEventStream& rOutputEventStream) {
  m_pOutputEventStream = &rOutputEventStream;
  return rOutputEventStream; // Returns the original, which should be the same.
}

COutputEventStream& CEventFilter::DetachOutputEventStream() {
  m_pOutputEventStream = (COutputEventStream*)kpNULL;
  return *m_pOutputEventStream; // Returns the original setting.
}
