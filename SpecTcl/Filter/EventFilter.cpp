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
/*
  Problem with file name parsing:
  There is already a ParseFileName function in OutputEventStream.
  Don't want to put another in here as that would give two essentially identical functions that have to be maintained to give identical results (especially in the case where a default file is necessary.
  At the same time, I cannot call COutputEventStream::ParseFileName until the filter is enabled and the output event stream object is created.
  Hence, I hold onto the input file name until the output event stream is created and I am able to use its parser to set the file name.
*/
CEventFilter::CEventFilter() :
  m_fEnabled(kfFALSE),
  m_sFileName(""), // Initialized to default.
  m_pOutputEventStream((COutputEventStream*)kpNULL)
{
  m_vParameterNames.clear();
}

CEventFilter::CEventFilter(string& rFileName) :
  m_fEnabled(kfFALSE),
  m_sFileName(rFileName),
  m_pOutputEventStream((COutputEventStream*)kpNULL)
{
  m_vParameterNames.clear();
}

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

CEventFilter::~CEventFilter() {
  Disable();
}

// Operators.
CEventFilter& CEventFilter::operator=(const CEventFilter& rRhs) {
  return *this;
}

// Additional functions.
void CEventFilter::Enable() {
  // Actually does a re-enable to reset the configuration with any new parameters and be on the safe side.
  Disable(); // Clear everything. May be redundant, especially when the file name is reset, but prevents a potential problem.

  m_pOutputEventStream = new COutputEventStream(m_sFileName, m_vParameterNames); // Constructor will call COutputEventStream::Open().
  m_sFileName = m_pOutputEventStream->getFileName(); // Read back the parsed file name.
  //m_fEnabled = m_pOutputEventStream->Open(); // Can also set with m_pOutputEventStream->isActive(). // Open() will be done on creation.
  m_fEnabled = kfTRUE;
}

void CEventFilter::Disable() { // Must be consecutively callable.
  if(m_pOutputEventStream != (COutputEventStream*)kpNULL) {
    //m_pOutputEventStream->Close(); // Close() will be done on deletion.
    delete m_pOutputEventStream; // Deconstructor will call COutputEventStream::Close().
  }
  m_pOutputEventStream = (COutputEventStream*)kpNULL; // Otherwise, delete would cause a SegFault later.
  m_fEnabled = kfFALSE;
}

void CEventFilter::setFileName(string& rFileName) {
  // Whenever this is called, a call to Enable() must be made for the filter to be used.
  Disable(); // Reset output event stream using new file name. (This Disable() call is unnecessary, but I'll leave it just to be explicit.)
  m_sFileName = rFileName;
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
