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
/*
CGatedEventFilter::CGatedEventFilter(const CGatedEventFilter& rRhs) {
  m_pGateContainer = rRhs.m_pGateContainer;
  m_vParameterIds = rRhs.m_vParameterIds;
  m_pOutputEventStream = rRhs.m_pOutputEventStream;
}
*/

CGatedEventFilter::~CGatedEventFilter() {
}

// Operators.
CGatedEventFilter& CGatedEventFilter::operator=(const CGatedEventFilter& rRhs) {
  m_pGateContainer = rRhs.m_pGateContainer;
  m_vParameterIds = rRhs.m_vParameterIds;
  m_pOutputEventStream = rRhs.m_pOutputEventStream;
}

// Additional functions.
void CGatedEventFilter::setGateContainer(CGateContainer& rCGateContainer) {
  m_pGateContainer = &rCGateContainer; // WATCH OUT!!!
}

Bool_t CGatedEventFilter::CheckCondition(const CEvent& rEvent) { // Formerly returned UInt_t.
  /*
    Note: CGateContainer always points to a gate, even if it is invalid. Also, we do not need to use getGate(). We simply just pass the event to the function call operator of the gate container. Furthermore, CGateContainer returns whether the event passed the gate so we can simply return this output without having to check the condition using inGate, CheckGate, or whatever else. Simple, complete, concise.
  */
  return (Bool_t)m_pGateContainer; // Check this!
  // Write to COutputEventStream, or toss.
}

void CGatedEventFilter::FormatOutputEvent(const CEvent& rEvent) {
}
