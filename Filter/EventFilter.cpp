/*
  EventFilter.cpp
*/

// Copyright Notice.
static const char* Copyright = 
  "EventFilter.cpp: Copyright 2002 NSCL, All rights reserved\n";

// Header Files.
#include "EventFilter.h"
#include "OutputEventStream.h"
#include <vector>
#include "Histogrammer.h"
#include "Parameter.h"
#include "Event.h"
#include "EventList.h"

// Constructors.
//CEventFilter::CEventFilter(){
//}

CEventFilter::CEventFilter(COutputEventStream& rOutputEventStream) {
}

CEventFilter::CEventFilter(const CEventFilter& rRhs) {
}

CEventFilter::~CEventFilter() {
  //delete rOutputEventStream; // Is this needed?
}

// Operators.
void CEventFilter::operator()(CEventList& rEvents) {
  for(int i=0; i<rEvents.size(); i++) {
    if(CheckCondition(*rEvents[i])) { // CAREFUL!!! ***************************
      FormatOutputEvent(*rEvents[(UInt_t)i]);
    }
  }
}

CEventFilter& CEventFilter::operator=(const CEventFilter& rRhs) {
  return *this;
}

Bool_t CEventFilter::operator==(const CEventFilter& rRhs) {
  return (m_Name == rRhs.m_Name &&
	  m_fEnabled == rRhs.m_fEnabled);
}

Bool_t CEventFilter::operator!=(const CEventFilter& aCEventFilter) {
  return !(operator==(aCEventFilter));
}

// Additional functions.
void CEventFilter::Enable() {
  m_fEnabled = kfTRUE;
}

void CEventFilter::Disable() {
  m_fEnabled = kfFALSE;
}

Bool_t CEventFilter::CheckCondition(CEvent& rEvent) {
  // Overridden by sub-class.
  return kfTRUE; // Umm! *******************************************
}

void CEventFilter::FormatOutputEvent(CEvent& rEvent) {
  (*m_pOutputEventStream).ReceiveEvent(rEvent); // rOutputEventStream undeclared. Causing problems. *Go over class structure carefully* ************
}

COutputEventStream& CEventFilter::AttachOutputEventStream(COutputEventStream& rOutputEventStream) {
  m_pOutputEventStream = &rOutputEventStream; // m_pOutputEventStream undeclared. **********************
  return rOutputEventStream; // Returns the original, which should be the same.
}

COutputEventStream& CEventFilter::DetachOutputEventStream() {
  m_pOutputEventStream = (COutputEventStream*)kpNULL;
  return *m_pOutputEventStream; // Returns the original setting.
}
