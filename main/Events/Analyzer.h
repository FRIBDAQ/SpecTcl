/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

//  CAnalyzer.h:
//
//    This file defines the CAnalyzer class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
// Major Modification history:
//
// May 2001, Ron Fox
//  Cache event creation so that we don't have to create again and again.
//  Events are re-used from physics buffer to buffer.
//
/////////////////////////////////////////////////////////////

#ifndef __ANALYZER_H  //Required for current class
#define __ANALYZER_H

#ifndef __EVENTLIST_H
#include "EventList.h"
#define __EVENTLIST_H
#endif

#ifndef __BUFFERDECODER_H
#include "BufferDecoder.h"
#define __BUFFERDECODER_H
#endif

#ifndef __EVENTSINK_H
#include "EventSink.h"
#define __EVENTSINK_H
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#define __HISTOTYPES_H
#endif

#ifndef __FILTEREVENTPROCESSOR_H
#include <FilterEventProcessor.h>
#define __FILTEREVENTPROCESSOR_H
#endif

// Forward class declaration:
class CFilterEventProcessor;

class CAnalyzer {
  // Attributes:
  UInt_t          m_nEventThreshold; // Events processed before histogramming.
  UInt_t          m_nParametersInEvent;	// Parameters in an event.
  CEventList      m_EventList;
  CEventList      m_EventPool;
  CBufferDecoder* m_pDecoder;
  CEventSink*     m_pSink;
  Bool_t          m_fAbort;
  Bool_t          m_fPartialEntity;

  CFilterEventProcessor* m_pFilterEventProcessor; // For event filtering.

 public:
  static UInt_t   m_nDefaultEventThreshold;
  static UInt_t   m_nDefaultParameterCount;

  // constructors destructors and other canonical member funtions.
 public:
  // Default constructor
  CAnalyzer() :
  m_nEventThreshold(CAnalyzer::m_nDefaultEventThreshold),  
    m_nParametersInEvent(CAnalyzer::m_nDefaultParameterCount),
    m_EventList(CAnalyzer::m_nDefaultEventThreshold),
    m_EventPool(),
    m_pDecoder(0),
    m_pSink(0),
    m_pFilterEventProcessor((CFilterEventProcessor*)kpNULL) // For event filtering.
    {};

  virtual ~CAnalyzer();       //Destructor

  // Constructor with arguments
  CAnalyzer(UInt_t am_nParametersInEvent,
	    UInt_t nThreshold = CAnalyzer::m_nDefaultEventThreshold) :
  m_nEventThreshold(nThreshold),
    m_nParametersInEvent(am_nParametersInEvent),
    m_EventList(nThreshold),
    m_pDecoder(0),
    m_pSink(0),
    m_pFilterEventProcessor((CFilterEventProcessor*)kpNULL) // For event filtering.
    {};

  // Copy constructor
  CAnalyzer(const CAnalyzer& aCAnalyzer) {
    m_nEventThreshold    = aCAnalyzer.m_nEventThreshold;
    m_nParametersInEvent = aCAnalyzer.m_nParametersInEvent;
    CopyEventList(aCAnalyzer.m_EventList);
    CopyEventPool(aCAnalyzer.m_EventPool);
    m_pDecoder           = aCAnalyzer.m_pDecoder;
    if(m_pDecoder)
      m_pDecoder->OnAttach(*this);
    m_pSink              = aCAnalyzer.m_pSink;
    if(m_pSink)
      m_pSink->OnAttach(*this);
  };

  // Operator= Assignment Operator
  CAnalyzer& operator=(const CAnalyzer& aCAnalyzer) {
    if (this == &aCAnalyzer) return *this;

    m_nEventThreshold    = aCAnalyzer.m_nEventThreshold;
    m_nParametersInEvent = aCAnalyzer.m_nParametersInEvent;
    CopyEventList(aCAnalyzer.m_EventList);
    CopyEventPool(aCAnalyzer.m_EventPool);

    // The various attached objects must be detached before assigned:
    DetachAll();
    m_pDecoder           = aCAnalyzer.m_pDecoder;
    if(m_pDecoder)
      m_pDecoder->OnAttach(*this);
    m_pSink              = aCAnalyzer.m_pSink;
    if(m_pSink)
      m_pSink->OnAttach(*this);

    return *this;
  };

  // Operator== Equality Operator
  int operator==(const CAnalyzer& aCAnalyzer) {
    return (
	    (m_nEventThreshold    == aCAnalyzer.m_nEventThreshold)    &&
	    (m_nParametersInEvent == aCAnalyzer.m_nParametersInEvent) &&
	    (m_EventList          == aCAnalyzer.m_EventList)          &&
	    (m_pDecoder           == aCAnalyzer.m_pDecoder)           &&
	    (m_pSink              == aCAnalyzer.m_pSink)
	    );
  };

  // Selectors:
 public:
  UInt_t getEventThreshold() const {
    return m_nEventThreshold;
  }

  UInt_t getParametersInEvent() const {
    return m_nParametersInEvent;
  }

  CEventList& getEventList()  {
    return m_EventList;
  }

 CBufferDecoder* getDecoder() {
    return m_pDecoder;
  }

  CEventSink* getSink() {
    return m_pSink;
  }

  // Mutators -- available only to derived objects:
 protected:
  void setEventThreshold(UInt_t am_nEventThreshold) {
    m_nEventThreshold = am_nEventThreshold;
  }

  void setParametersInEvent(UInt_t am_nParametersInEvent) { 
    m_nParametersInEvent = am_nParametersInEvent;
  }

  void setEventList(CEventList* am_EventList) {
    CopyEventList(*am_EventList);
  }

  void setDecoder(CBufferDecoder* am_pDecoder) {
    if(m_pDecoder)
      m_pDecoder->OnDetach(*this);
    m_pDecoder = am_pDecoder;
    if(m_pDecoder)
      m_pDecoder->OnAttach(*this);
  }

  void setEventSink(CEventSink* aSink) {
    if(m_pSink)
      m_pSink->OnDetach(*this);
    m_pSink = aSink;
    if(m_pSink)
      m_pSink->OnAttach(*this);
  }

  //  Public interface:
 public:
  virtual void OnBuffer(UInt_t nBytes, Address_t pData);
  virtual void OnStateChange(UInt_t nType, CBufferDecoder& rDecoder);
  virtual void OnPhysics(CBufferDecoder& rDecoder);
  virtual void OnScaler(CBufferDecoder& rDecoder);
  virtual void OnOther(UInt_t nType, CBufferDecoder& rDecoder);
  virtual void OnEndFile();
  virtual void OnInitialize();
  
  CBufferDecoder* AttachDecoder(CBufferDecoder& rDecoder);
  CBufferDecoder* DetachDecoder();
  CEventSink*     AttachSink(CEventSink& rSink);
  CEventSink*     DetachSink();
  void            entityNotDone();

  // Utility functions.
 protected:
  virtual UInt_t OnEvent(Address_t pRawData, CEvent& anEvent);

  void           AbortEvent() {
    m_fAbort = kfTRUE;
  }

  void CopyEventList(const CEventList& rhs);
  void CopyEventPool(const CEventList& rhs);
  void DetachAll();

  CEvent* CreateEvent() {
    CEventVector& rVec(m_EventPool.getVector());
    CEvent* pEvent;
    if(rVec.empty()) {
      pEvent =  new CEvent(m_nParametersInEvent);
    } else {
      pEvent = rVec.back();
      rVec.pop_back();
    }
    if(pEvent) pEvent->clear(); // BUG prevention.
    return pEvent;
  }
  void ReturnEvent(CEvent* pEvent);

  void ClearEventList() {
    CEventVector& evlist(m_EventList.getVector());
    CEventVector& evpool(m_EventPool.getVector());
    size_t s = evlist.size();
    // CEventListIterator p = evlist.begin();
    // for(; p != evlist.end(); p++) {
    for (int i = 0; i < s; i++) {
      CEvent* pEvent = evlist[i];
      if(pEvent) {
	evpool.push_back(pEvent);
      	evlist[i] = (CEvent*)kpNULL;
      }
    }
  }

};

#endif
