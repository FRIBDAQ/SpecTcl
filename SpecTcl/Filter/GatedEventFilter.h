/*
  GatedEventFilter.h
*/

// Preprocessor includes, definitions.
#ifndef __GATEDEVENTFILTER_H //Required for current class
#define __GATEDEVENTFILTER_H

#ifndef __DICTIONARY_H
#include "Dictionary.h"
#define __DICTIONARY_H
#endif

#ifndef __PARAMETER_H
#include "Parameter.h"
#define __PARAMETER_H
#endif

#ifndef __EVENTSINK_H
#include "EventSink.h"
#define __EVENTSINK_H
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#define __HISTOTYPES_H
#endif

#ifndef __GATECONTAINER_H
#include <GateContainer.h>
#define __GATECONTAINTER_H
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif

#ifndef __EVENTFILTER_H
#include <EventFilter.h>
#define __EVENTFILTER_H
#endif

#ifndef __EVENT_H
#include <Event.h>
#define __EVENT_H
#endif

// Class.
class CGatedEventFilter : public CEventFilter {
  // Attributes.
  CGateContainer* m_pGateContainer;
  vector<Int_t> m_vParameterIds;
  COutputEventStream* m_pOutputEventStream;

 public:
  // Constructors.
  CGatedEventFilter();
  CGatedEventFilter(COutputEventStream&);
  CGatedEventFilter(const CGatedEventFilter&);
  ~CGatedEventFilter(); // Virtual.

  // Operators.
  Bool_t operator()(); // Virtual.
  CGatedEventFilter& operator=(const CGatedEventFilter&);
  Bool_t operator==(const CGatedEventFilter&);
  Bool_t operator!=(const CGatedEventFilter&);

  // Additional functions.
  void setGateContainer(CGateContainer&);
 protected:
  Bool_t CheckCondition(const CEvent& rEvent);
 public:
  void FormatOutputEvent(const CEvent& rEvent); // Virtual.
}; // CGatedEventFilter.

#endif
