/*
  FilterEventProcessor.cpp
*/

// Copyright Notice:
static const char* Copyright =
  "OutputEventStream.cpp: Copyright 2003 NSCL, All rights reserved\n";

// Header files:
#include <Globals.h>
#include <EventProcessor.h>

#include "FilterEventProcessor.h"

// Constructors:
CFilterEventProcessor::CFilterEventProcessor() {};
CFilterEventProcessor::~CFilterEventProcessor() {};

// Operators:
Bool_t CFilterEventProcessor::operator()(const Address_t pEvent,
					 CEvent& rEvent,
					 CAnalyzer& rAnalyzer,
					 CBufferDecoder& rDecoder)
{
  // COMPLETE THIS! **********************************************************************
  // Process the next event:
  // Read the bit-mask array.
  // For the length of the bit-mask array, if the index is valid, then read the floating point parameter.
  return kfTRUE;
};

// Additional functions:
Bool_t CFilterEventProcessor::OnOther() {
  // COMPLETE THIS! **********************************************************************
  gpBufferDecoder->getBody();
  return kfTRUE;
};
