#ifndef __OUTPUTEVENTSTREAM_H //Required for current class
#define __OUTPUTEVENTSTREAM_H

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif

#ifndef __STL_FSTREAM
#include <fstream>
#define __STL_FSTREAM
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#define __HISTOTYPES_H
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

class COutputEventStream {
  Bool_t m_fActive;
  UInt_t m_nMAXBUFFERSIZE;
  vector<CEvent> Buffer;
  ofstream m_ofs;

 public:
  // Constructors.
  COutputEventStream();
  COutputEventStream(const CEvent&);
  virtual ~COutputEventStream();

  // Operators.
  Bool_t operator()();
  COutputEventStream& operator=(const COutputEventStream&);

  // Additional functions.
  Bool_t COutputEventStream::Open();
  Bool_t COutputEventStream::Close();
  Bool_t COutputEventStream::ReceiveEvent(const CEvent&);
  Bool_t COutputEventStream::SendBuffer();
}; // COutputEventStream.

#endif
