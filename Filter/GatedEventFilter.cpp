/*
  GatedEventFilter.cpp
*/

// Copyright Notice.
static const char* Copyright =
  "GatedEventFilter.cpp: Copyright 2002 NSCL, All rights reserved\n";

// Header Files.
#include <vector>

#include "Histogrammer.h"
#include "Parameter.h"
#include "Event.h"
#include "EventList.h"

#include "GatedEventFilter.h"

// Functions.
// Constructors.
CGatedEventFilter::CGatedEventFilter() {}

CGatedEventFilter::CGatedEventFilter(string& rFileName) {}

//CGatedEventFilter::CGatedEventFilter(COutputEventStream& rOutputEventStream) {}

/*
CGatedEventFilter::CGatedEventFilter(const CGatedEventFilter& rRhs) {
  m_pGateContainer = rRhs.m_pGateContainer;
  m_vParameterIds = rRhs.m_vParameterIds;
  m_pOutputEventStream = rRhs.m_pOutputEventStream;
}
*/

CGatedEventFilter::~CGatedEventFilter() {}

// Operators.
void CGatedEventFilter::operator()(CEventList& rEventList) {
  // NOTE: There is a !BUG! in CEventList::end(). There may be null events before the end.
  for(CEventListIterator i = rEventList.begin(); i != rEventList.end(); i++) {
    if((*i) != (CEvent*)kpNULL) { // Check to ensure that the event is valid.
      operator()(**i);
    } else { // Invalid event received.
      break; // Break out immediately - The rest are null too.
    }
  }
}

void CGatedEventFilter::operator()(CEvent& rEvent) {
  if(CheckCondition(rEvent)) {
    FormatOutputEvent(rEvent);
  }
}

/*
CGatedEventFilter& CGatedEventFilter::operator=(const CGatedEventFilter& rRhs) {
  m_pGateContainer = rRhs.m_pGateContainer;
  m_vParameterIds = rRhs.m_vParameterIds;
  m_pOutputEventStream = rRhs.m_pOutputEventStream;
}
*/

Bool_t CGatedEventFilter::operator==(const CGatedEventFilter& rhs) {
  return ((m_fEnabled == rhs.m_fEnabled) &&
	  (m_pOutputEventStream == rhs.m_pOutputEventStream) &&
	  (m_pGateContainer == rhs.m_pGateContainer) &&
	  (m_vParameterIds == rhs.m_vParameterIds));
};

Bool_t CGatedEventFilter::operator!=(const CGatedEventFilter& rhs) {
  return !(operator==(rhs));
};

// Additional functions.
void CGatedEventFilter::setGateContainer(CGateContainer& rCGateContainer) {
  m_pGateContainer = &rCGateContainer; // WATCH OUT!!!
}

void CGatedEventFilter::setParameterNames(const vector<string>& rvParameterNames) {
  m_vParameterNames = rvParameterNames;
}

void CGatedEventFilter::setParameterIds(const vector<UInt_t>& rvParameterIds) {
  m_vParameterIds = rvParameterIds;
}

std::string CGatedEventFilter::getGateName() {
  return m_pGateContainer->getName();
}

UInt_t CGatedEventFilter::getGateID() {
  return m_pGateContainer->getNumber();
}

Bool_t CGatedEventFilter::CheckCondition(CEvent& rEvent) { //, const vector<UInt_t>& vParameterIds) {
  /*
    Note: CGateContainer always points to a gate, even if it is invalid. Also, we do not need to use getGate(). We simply just pass the event to the function call operator of the gate container. Furthermore, CGateContainer returns whether the event passed the gate so we can simply return this output without having to check the condition using inGate, CheckGate, or whatever else. Simple, complete, concise.
  */
  return (m_pGateContainer->getGate())->inGate(rEvent, m_vParameterIds);
}

void CGatedEventFilter::FormatOutputEvent(CEvent& rEvent) { //, const vector<UInt_t>& vParameterIds) {
  // Check to see if event passes all parameters upon which it is being filtered.
  if((m_pOutputEventStream!=(COutputEventStream*)kpNULL) && CheckEnabled() && CheckCondition(rEvent)) { //, m_vParameterIds)) { // If all set up and the event passes the filter ...
    // Write to COutputEventStream,
    m_pOutputEventStream->ReceiveEvent(rEvent);
  } else {
    // or toss.
  }
}
