/*
  EventSinkPipeline.cpp
*/

// Copyright Notice.
static const char* Copyright =
  "OutputEventStream.cpp: Copyright 2003 NSCL, All rights reserved\n";

// Header Files.
#include <config.h>
#include "Event.h"
#include "EventList.h"

#include "EventSinkPipeline.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


// Constructors.
CEventSinkPipeline::CEventSinkPipeline() {}

CEventSinkPipeline::~CEventSinkPipeline() {}

// Operators.
void CEventSinkPipeline::operator()(CEventList& rEventList) {
  for(EventSinkIterator i = CEventSinkPipeline::begin(); i != CEventSinkPipeline::end(); i++) {
    // Clear the gates.

    (**i)(rEventList); // run the sink.
  }
};




// Additional functions.
void CEventSinkPipeline::AddEventSink(CEventSink& rEventSink) {
  m_lSinks.push_back(&rEventSink);
}

void CEventSinkPipeline::RemoveEventSink(CEventSink& rEventSink) {//(EventSinkIterator) {
  m_lSinks.remove(&rEventSink);
}

EventSinkIterator CEventSinkPipeline::begin() {
  return m_lSinks.begin();
}

EventSinkIterator CEventSinkPipeline::end() {
  return m_lSinks.end();
}

UInt_t CEventSinkPipeline::size() {
  return m_lSinks.size();
}
