/*
  EventFilter.h
*/

// Preprocessor includes, definitions.
#ifndef __EVENTFILTER_H //Required for current class
#define __EVENTFILTER_H

#ifndef __EVENT_H
#include <Event.h>
#define __EVENT_H
#endif

#ifndef __EVENTLIST_H
#include <EventList.h>
#define __EVENTLIST_H
#endif

#ifndef __EVENTSINK_H
#include <EventSink.h>
#define __EVENTSINK_H
#endif

#ifndef __OUTPUTEVENTSTREAM_H
#include <OutputEventStream.h>
#define __OUTPUTEVENTSTREAM_H
#endif

// Class.
class CEventFilter : public CEventSink {
  // Attributes.
 protected:
  Bool_t m_fEnabled;
  COutputEventStream* m_pOutputEventStream;

 public:
  // Constructors.
  CEventFilter();
  CEventFilter(COutputEventStream&);
  virtual ~CEventFilter();

  // Operators.
  virtual void operator()(CEventList&) = 0; // If flag is set, call event formatter.
  virtual void operator()(CEvent&) = 0; // If flag is set, call event formatter.
  CEventFilter& operator=(const CEventFilter&);

  // Additional functions.
  void Enable();
  void Disable();
  virtual void FormatOutputEvent(CEvent&);
  COutputEventStream& AttachOutputEventStream(COutputEventStream&);
  COutputEventStream& DetachOutputEventStream();
  //protected: // Commented out for now.
  Bool_t CheckEnabled();
  virtual Bool_t CheckCondition(CEvent&);
}; // CEventFilter.

#endif
