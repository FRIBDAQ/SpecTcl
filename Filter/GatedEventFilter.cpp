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

/*!
   Construct a gated event filter with the default filter 
   filename.   The gate container can either be supplied now
   or later.
   \param pGate (CGateContainer* in default = NULL):
      Pointer to the gate container that will be used to
      select events for inclusion in the output stream.
      If this is NULL (defaulted e.g.), a gate must be
      added to the filter later on by calling
      setGateContainer 
      
   \note If the filter is enabled with no
         gate container, no events will be passed to the output
         stream.
         
*/
CGatedEventFilter::CGatedEventFilter(CGateContainer* pGate) :
   CEventFilter(),
   m_pGateContainer(pGate)
{}
/*!
   Construct a gated event filter with a given filename.
   \param rFilename (string& in):
       Name of the file to which filtered data will be written.
   \param pGate (CGateContainer* in):
       Pointer to the gate container that will determine which
      events are passed to the output event stream (see previous
      function).
*/
CGatedEventFilter::CGatedEventFilter(string& rFileName,
                                     CGateContainer* pGate) :
   CEventFilter(rFileName),
   m_pGateContainer(pGate)
{
}


/*!
   Destroy the gated event filter.  The client of this class
   is responsible for maintaining the gate container, that makes
   this function a no-op.
*/
CGatedEventFilter::~CGatedEventFilter() 
{
}




// Additional functions.
/*!
   Place a new gate container into the filter.  The gate container
   is a pointer like object to a gate.  The gate determines which
   events will get passed to the output event stream.
   \param rGateContainer (CGateContainer& [in]):
     The gate container to replace the existing one.
*/
void 
CGatedEventFilter::setGateContainer(CGateContainer& rCGateContainer) 
{
  m_pGateContainer = &rCGateContainer; 
}


/*!
  Return the name of the gate that has been associated with the
  filter.  If the filter has not gate associated with it, it will
   return the string: "-not yet defined-"
*/
std::string 
CGatedEventFilter::getGateName() {
   if(m_pGateContainer) {
     return m_pGateContainer->getName();
   } else {
      return string("-not yet defined-");
   }
}

/*!
  Return the gate id associated with a filter.  If the gate is not
  yet defined, return -1.
*/
Int_t CGatedEventFilter::getGateID() {

   if(m_pGateContainer) {
      return m_pGateContainer->getNumber();
   } 
   else {
      return -1;
   }
}


/*!
   Virtual function override that's called to check the condition
   that passes events to the output event stream.
   This function:
   -  recursively clears the cache associated with the gate.
   - Returns the value of the gate checked on the event.
   \param rEvent (CEvent& in):
     The event to check against the conditionl
   \return bool 
    - True if the gate was made and the filter should pass the
      event to the output stream.
    - False if the gate was not made and the filter should
      drop the event.
   \note
     If the gate is not defined (m_pGateContainer == kpNULL),
     all events are dropped.
*/
Bool_t 
CGatedEventFilter::CheckCondition(CEvent& rEvent) { //, const vector<UInt_t>& vParameterIds) {
   if(m_pGateContainer) {
      (*m_pGateContainer)->RecursiveReset();
      return (*m_pGateContainer)(rEvent);
   }
   else {
      return false;
   }
}
