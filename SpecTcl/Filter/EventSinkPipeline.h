/*
  EventSinkPipeline.h
*/

// Preprocessor includes, definitions.
#ifndef __EVENTSINKPIPELINE_H // Required for current class
#define __EVENTSINKPIPELINE_H


#ifndef __STL_LIST
#include <list>
#ifndef __STL_LIST
#define __STL_LIST
#endif
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

typedef STD(list)<CEventSink*>::iterator EventSinkIterator;

// Class.
class CEventSinkPipeline : public CEventSink {
  STD(list)<CEventSink*> m_lSinks;
  //typedef STD(list)<CEventSink*>::iterator EventSinkIterator;

 public:
  // Constructors.
  CEventSinkPipeline();
  virtual ~CEventSinkPipeline();

  // Operators.
  virtual void operator()(CEventList&);

  // Additional functions.
  void AddEventSink(CEventSink&);
  void RemoveEventSink(CEventSink&); //RemoveEventSink(EventSinkIterator)
  EventSinkIterator begin();
  EventSinkIterator end();
  UInt_t size();
}; // CEventSinkPipeline.

#endif
