/*
  EventSinkPipeline.h
*/

// Preprocessor includes, definitions.
#ifndef __EVENTSINKPIPELINE_H // Required for current class
#define __EVENTSINKPIPELINE_H

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

typedef CEventSink::iterator EventSinkIterator;

// Class.
class CEventSinkPipeline : public CEventSink {
  list<CEventSink> m_lSinks;

 public:
  // Constructor.
  CEventSinkPipeline();

  // Destructor.
  virtual ~CEventSinkPipeline();

  EventSinkIterator AddEventSink(CEventSink &rEventSink);
  void RemoveEventSink(EventSinkIterator); // CAREFUL HERE! ***************
  EventSinkIterator begin();
  EventSinkIterator end();
  UInt_t size();
  void operator() (CEventList& rEventList); // WHAT DOES THIS TAKE??? **************

}; // CEventSinkPipeline.

#endif
