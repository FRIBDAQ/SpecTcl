/*
  EventFilter.h
*/

// Preprocessor includes, definitions.
#ifndef __EVENTFILTER_H //Required for current class
#define __EVENTFILTER_H

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

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

#ifndef __OUTPUTEVENTSTREAM_H
#include <OutputEventStream.h>
#define __OUTPUTEVENTSTREAM_H
#endif

#ifndef __EVENT_H
#include <Event.h>
#define __EVENT_H
#endif

// Class.
class CEventFilter : public CEventSink {
  // Attributes.
  std::string m_Name;
  Bool_t m_fEnabled;
  COutputEventStream* m_pOutputEventStream;

 public:
  // Constructors.
  //CEventFilter();
  CEventFilter(COutputEventStream&);
  CEventFilter(const CEventFilter&);
  virtual ~CEventFilter();

  // Operators.
  virtual void operator()(CEventList&); // If flag is set, call event formatter.
  CEventFilter& operator=(const CEventFilter&);
  Bool_t operator==(const CEventFilter&);
  Bool_t operator!=(const CEventFilter&);

  // Additional functions.
  void Enable();
  void Disable();
  virtual void FormatOutputEvent(CEvent&) = 0;
  COutputEventStream& AttachOutputEventStream(COutputEventStream&);
  COutputEventStream& DetachOutputEventStream();
  //protected: // Commented out for now.
  virtual Bool_t CheckCondition(CEvent&) = 0;
}; // CEventFilter.

// Dictionary types:
typedef CDictionary<CEventFilter>		FilterDictionary;
typedef FilterDictionary::DictionaryIterator	FilterDictionaryIterator;

#endif
