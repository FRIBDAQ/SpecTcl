/*
  EventFilter.h
*/

// Preprocessor includes, definitions.
#ifndef __EVENTFILTER_H //Required for current class
#define __EVENTFILTER_H

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

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
  string m_sFileName;
  vector<string> m_vParameterNames;
  COutputEventStream* m_pOutputEventStream;

 public:
  // Constructors.
  CEventFilter();
  CEventFilter(string& rFileName);
  //CEventFilter(COutputEventStream&); // I doubt that this will ever be used, even if it at all could be.
  virtual ~CEventFilter();

  // Operators.
  virtual void operator()(CEventList&) = 0; // If flag is set, call event formatter.
  virtual void operator()(CEvent&) = 0; // If flag is set, call event formatter.
  CEventFilter& operator=(const CEventFilter&);

  // Additional functions.
  void Enable();
  void Disable();
  string ParseFileName(string&);
  void setFileName(string&);
  string getFileName();
  COutputEventStream& AttachOutputEventStream(COutputEventStream&);
  COutputEventStream& DetachOutputEventStream();
  virtual void FormatOutputEvent(CEvent&);
  //protected:
  Bool_t CheckEnabled();
  virtual Bool_t CheckCondition(CEvent&);
}; // CEventFilter.

#endif
