/*
  GatedEventFilter.h
*/

// Preprocessor includes, definitions.
#ifndef __GATEDEVENTFILTER_H //Required for current class
#define __GATEDEVENTFILTER_H

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#define __HISTOTYPES_H
#endif

#ifndef __DICTIONARY_H
#include "Dictionary.h"
#define __DICTIONARY_H
#endif

#ifndef __PARAMETER_H
#include "Parameter.h"
#define __PARAMETER_H
#endif

#ifndef __EVENT_H
#include <Event.h>
#define __EVENT_H
#endif

#ifndef __EVENTSINK_H
#include "EventSink.h"
#define __EVENTSINK_H
#endif

#ifndef __GATECONTAINER_H
#include <GateContainer.h>
#define __GATECONTAINTER_H
#endif

#ifndef __EVENTFILTER_H
#include <EventFilter.h>
#define __EVENTFILTER_H
#endif

// Class.
class CGatedEventFilter : public CEventFilter {
  // Attributes.
  CGateContainer* m_pGateContainer;
  vector<UInt_t> m_vParameterIds;

 public:
  // Constructors.
  CGatedEventFilter();
  CGatedEventFilter(COutputEventStream&);
  virtual ~CGatedEventFilter();

  // Operators.
  void operator()(CEventList&); // If flag is set, call event formatter.
  void operator()(CEvent&); // If flag is set, call event formatter.
  //CGatedEventFilter& operator=(const CGatedEventFilter&);
  Bool_t operator==(const CGatedEventFilter&);
  Bool_t operator!=(const CGatedEventFilter&);

  // Additional functions.
  void setGateContainer(CGateContainer&);
  void setParameterIds(const vector<UInt_t>&);
  std::string getGateName();
  UInt_t getGateID();
 protected:
  Bool_t CheckCondition(CEvent&); //, const vector<UInt_t>&);
 public:
  void FormatOutputEvent(CEvent&); //, const vector<UInt_t>&);
}; // CGatedEventFilter.

#endif
