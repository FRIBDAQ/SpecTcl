/*
  EventFilter.cpp
*/

// Copyright Notice.
static const char* Copyright = 
  "EventFilter.cpp: Copyright 2002 NSCL, All rights reserved\n";

// Header Files.
#include <vector>

#include "Histogrammer.h"
#include "Parameter.h"
#include "Event.h"
#include "EventList.h"

#include "EventFilter.h"
#include "OutputEventStream.h"

// Constructors.
CEventFilter::CEventFilter() :
  m_fEnabled(false),
  m_pOutputEventStream((COutputEventStream*)kpNULL)
{}

CEventFilter::CEventFilter(COutputEventStream& rOutputEventStream) :
  m_fEnabled(false),
  m_pOutputEventStream(&rOutputEventStream)
{}

CEventFilter::~CEventFilter() {}

// Operators.
/*
void CEventFilter::operator()(CEventList& rEvents) {
  for(CEventListIterator i = rEvents.begin(); i != rEvents.end(); i++) {
    CEventFilter::operator()(**i);
  }
}

void CEventFilter::operator()(CEvent& rEvent) {
  if(CheckCondition(rEvent)) { // CAREFUL!!! ***************************
    FormatOutputEvent(rEvent);
  }
}
*/

CEventFilter& CEventFilter::operator=(const CEventFilter& rRhs) {
  return *this;
}

// Additional functions.
void CEventFilter::Enable() {
  if(!m_fEnabled) {
    if(m_pOutputEventStream == (COutputEventStream*)kpNULL) {
      m_pOutputEventStream = new COutputEventStream;
    }
    m_fEnabled = m_pOutputEventStream->Open();
  }
}

void CEventFilter::Disable() {
  if(m_fEnabled) {
    if(m_pOutputEventStream != (COutputEventStream*)kpNULL) {
      if(m_pOutputEventStream->Close()) { // Successfully closed.
	delete m_pOutputEventStream;
	m_pOutputEventStream = (COutputEventStream*)kpNULL; // OTHERWISE, DELETE WOULD CAUSE A SEGMENTATION FAULT.
	m_fEnabled = false;
      }
    } else { // No outputeventstream.
      m_fEnabled = false;
    }
  }
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
