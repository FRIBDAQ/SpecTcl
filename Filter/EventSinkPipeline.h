/*
  EventSinkPipeline.h
*/

// Preprocessor includes, definitions.
#ifndef __EVENTSINKPIPELINE_H // Required for current class
#define __EVENTSINKPIPELINE_H


#ifndef __STL_LIST
#include <list>
#define __STL_LIST
#endif

#ifndef __EVENT_H
#include "Event.h"
#define __EVENT_H
#endif

#ifndef __EVENTSINK_H
#include "EventSink.h"
#define __EVENTSINK_H
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#define __HISTOTYPES_H
#endif

typedef list<CEventSink*>::iterator EventSinkIterator;

// Class.
class CEventSinkPipeline : public CEventSink {
  list<CEventSink*> m_lSinks;
  //typedef list<CEventSink*>::iterator EventSinkIterator;

 public:
  // Constructors.
  CEventSinkPipeline();
  virtual ~CEventSinkPipeline();

  // Operators.
  void operator()(CEventList&);
  void operator()(CEvent&);

  // Additional functions.
  void AddEventSink(CEventSink&);
  void RemoveEventSink(CEventSink&); //RemoveEventSink(EventSinkIterator)
  EventSinkIterator begin();
  EventSinkIterator end();
  UInt_t size();
}; // CEventSinkPipeline.

#endif
