/*
  GatedEventFilter.h
*/

// Preprocessor includes, definitions.
#ifndef __GATEDEVENTFILTER_H //Required for current class
#define __GATEDEVENTFILTER_H

#ifndef __DICTIONARY_H
#include "Dictionary.h"
#endif

#ifndef __PARAMETER_H
#include "Parameter.h"
#endif

#ifndef __EVENTSINK_H
#include "EventSink.h"
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __GATECONTAINER_H
#include <GateContainer.h>
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
  //CGatedEventFilter(const CGatedEventFilter&);
  virtual ~CGatedEventFilter();

  // Operators.
  virtual Bool_t operator()();
  CGatedEventFilter& operator=(const CGatedEventFilter&);
  Bool_t operator==(const CGatedEventFilter&);
  Bool_t operator!=(const CGatedEventFilter&);

  // Additional functions.
  void setGateContainer(CGateContainer&);
 protected:
  Bool_t CheckCondition(const CEvent& rEvent);
 public:
  virtual void FormatOutputEvent(const CEvent& rEvent);
}; // CGatedEventFilter.

#endif
