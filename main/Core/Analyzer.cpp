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


static const char* Copyright = "(C) Copyright Michigan State University 2006, All rights reserved";
//  CAnalyzer.cpp
// Encapsulates an event analyzer.
// This sort of object takes as inputs
// a data source and produces as output
// EventList objects which are passed in to
// a histogrammer for analysis.
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////
//
// Header Files:
//

#include <config.h>
#include <iostream>
#include <stdint.h>
#include "Exception.h"
#include "EventFormatError.h"
#include "Globals.h"
#include "FilterBufferDecoder.h"
#include "FilterEventProcessor.h"

#include "Analyzer.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Static class member initializations:
UInt_t CAnalyzer::m_nDefaultEventThreshold = 128;
UInt_t CAnalyzer::m_nDefaultParameterCount = 512;

// Functions for class CAnalyzer
/////////////////////////////////////////////////////////////////////////
//
//  Function:
//    ~CAnalyzer()
//  Operation Type:
//     Destructor.
//
CAnalyzer::~CAnalyzer()
{
  // Destructor must make sure that all of the sub-objects are detached
  // as well as clearing the event list.
  m_EventList.clear();
  m_EventPool.clear();
  DetachAll();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void OnBuffer (UInt_t nBytes, Address_t pData)
//  Operation Type:
//     Hook
//
void CAnalyzer::OnBuffer(UInt_t nBytes, Address_t pData)
{
  // Called to process a buffer of data.  The default action is to 
  // initiate buffer decoding by the Decoder.  The decode is a collaborative
  // process between the analyzer, decoder and the event unpacker.
  if(m_pDecoder) {
    (*m_pDecoder)(nBytes, pData, *this);
  }
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void OnStateChange (UInt_t nType, CBufferDecoder& rDecoder)
//  Operation Type:
//     Hook
//
void CAnalyzer::OnStateChange(UInt_t nType, CBufferDecoder& rDecoder)
{
  //  Called as a result of a state change buffer.
  //  The default action is null.  
  //
  // Formal Parameters:
  //     UInt_t nType:
  //        Type of the buffer being passed in.
  //     CBufferDecoder& rDecoder:
  //         Reference to the buffer decoder which
  //         is being used to decode events.
  //
  // Exceptions:
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void OnPhysics (CBufferDecoder& rDecoder)
//  Operation Type:
//     Hook
//
void CAnalyzer::OnPhysics(CBufferDecoder& rDecoder) {
  // Processes a buffer of physics data.  The
  // default assumes an NSCL buffer format and,
  // for each event in the data buffer, calls the
  // EventUnPacker with a CEvent object for
  // it to fill.  
  //
  // Formal Parameters:
  //        CBufferDecoder& rDecoder:
  //            Refers to the buffer decoder responsible for 
  //            knowing about the global buffer structure.
  //  
  //   Action is null if:
  //     1. There is no event decoder.
  //     2. There is no buffer decoder.
  //
  //  If there is no event sink, the buffers are decoded just not
  //  sent to the sink.
  //    This may be useful to derived classes in other environments.
  //
  if(m_pDecoder) {
    UInt_t    nEvents     = m_pDecoder->getEntityCount();
    UInt_t    nBufferSize = m_pDecoder->getBodySize();
    UInt_t    nEventNo    = 0;
    UInt_t    nOffset     = 0;
    Address_t pData       = m_pDecoder->getBody();
    UInt_t    nEventSize;

    CEvent* pEvent = 0;
    try {
      while(nEvents) {
	m_fPartialEntity = false;

	if(nOffset >= nBufferSize) { // Someone lied about event sizes:
	  cerr << "-------------------------------------------------\n";
	  cerr << "Unpacker tried to run analyzer off end of the buffer\n";
	  cerr << "Current offset = " << nOffset;
	  cerr << " Buffer size = " << nBufferSize << endl;
	  cerr << "Analysis continue with the next buffer\n";
	  cerr << "--------------------------------------------------\n";
	  if(m_pSink) {
	    (*m_pSink)(m_EventList); // Dump partial event list.
	  }
	  ClearEventList();
	  return;
	}
	pEvent     = CreateEvent();
	try {
	  m_fAbort = kfFALSE;
	  nEventSize = OnEvent(pData, *pEvent);
	  if(!m_fAbort) {
	    m_EventList[nEventNo] = pEvent;
	    nEventNo++; 
	  }
	  else {                           // Must return the event (bug 144)
	  	ReturnEvent(pEvent);           // to the pool to prevent memory leak.
	  }
	  if ((nEventSize == 0) && !(m_fPartialEntity)) { // If we didn't throw now we'd hang here.
	    throw CEventFormatError((int)CEventFormatError::knSizeMismatch,
				    std::string("Packer returned event size = 0"),
				    (UInt_t*)pData, 16, 0,0);
	  }
	}
	catch (CEventFormatError& rError) { // See if we can continue...
	  if(!rError.EventSizeOk()) throw; // Nope. so rethrow.
	  delete pEvent;
	  cerr << "-------------------------------------------------------\n";
	  cerr << "Event format error detected while analyzing buffer\n";
	  cerr << rError.ReasonText() << endl;
	  cerr << "Attempting to continue with next event in buffer\n";
	  cerr << "-------------------------------------------------------\n";
	}
	if (!m_fPartialEntity) {
	  nEvents--;
	}
	pData     = (Address_t)((uint8_t*)pData + nEventSize);
	nOffset  += nEventSize;
	if(nEventNo >= m_nEventThreshold) {
	  if(m_pSink) {		// Flush the event list to the sink.
	    (*m_pSink)(m_EventList);
	  }
	  ClearEventList();
	  nEventNo = 0;
	}
      }
      // Dump any partial event list:
      if(nEventNo) {
	if(m_pSink) {
	  (*m_pSink)(m_EventList);
	}
	ClearEventList();
	nEventNo = 0;
      }

    }
    catch(CEventFormatError& rError) {
      delete pEvent;		// Kill hanging event.
      cerr << "---------------------------------------------------------\n";
      cerr << "Event format error detected while processing buffer" << endl;
      cerr << rError.ReasonText() << endl;
      cerr << "Event processing continues with next buffer" << endl;
      cerr << "---------------------------------------------------------\n";
      
    }
    catch(CException& rError) {
      delete pEvent;		// Kill hanging event.      
      cerr << "---------------------------------------------------------\n";
      cerr << "Unexpected exception caught while analyzing events.\n";
      cerr << rError.ReasonText() << endl;
      cerr << "Attempting to continue processing with next buffer\n";
      cerr << "---------------------------------------------------------\n";
    }
    // BUGBUGBUG - check that we are really at the end of the buffer
    //             otherwise either the entity count or the event size has
    //             lied to us.

  }
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void OnScaler(CBufferDecoder& rDecoder)
//  Operation Type:
//     Hook
//
void CAnalyzer::OnScaler(CBufferDecoder& rDecoder) {
  // Called when a scaler buffer is read from the
  //  event source.  The default action is null
  //  
  // Formal Parameters:
  //       CBufferDecoder&  rDecoder:
  //            Refers to the object which is responsible
  //            for decoding the global buffer structure.
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void OnOther(UInt_t nType, CBufferDecoder& rDecoder)
//  Operation Type:
//     Hook
//
void CAnalyzer::OnOther(UInt_t nType, CBufferDecoder& rDecoder) {
  // Called when some buffer type other than run
  // state change, physics or scaler has been read from
  // the data source.  Default action is null.
  // 
  // Formal parameters:
  //
  //    UInt_t nType:
  //          Buffer type.
  //
  //     CBufferDecoder& rDecoder:
  //           Refers to the decoder object which knows
  //           about the global buffer structure.
  // Exceptions:
  // Clean up.

  
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    void OnEndFile()
//  Operation Type:
//     Hook
//
void CAnalyzer::OnEndFile() {
  // Called when an end file was encountered on the
  // input file.  The default action is null.
  
  m_pDecoder->OnEndFile();
}
/**
 * OnInitialize
 *
 * This is called after all SpecTcl initialization is completed.
 * No_op for us.
 */
void
CAnalyzer::OnInitialize() {}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    CBufferDecoder* AttachDecoder(CBufferDecoder& rDecoder)
//  Operation Type:
//     mutator
//
CBufferDecoder* CAnalyzer::AttachDecoder(CBufferDecoder& rDecoder) {
  // Attaches a buffer decoder to the analyzer.
  // Buffer decoders are responsible for knowing the
  // global buffer structure.
  // They are called by OnBuffer() to determine what the
  // buffer type is and callback the appropriate Onxxx()
  // type dependent buffer processor.
  //
  // Formal Parameters:
  //
  //       CBufferDecoder& rDecoder:
  //             Refers to the buffer decoder.
  // Returns prior buffer decoder pointer.
  //
  CBufferDecoder* pDecoder = m_pDecoder;
  setDecoder(&rDecoder);

  return pDecoder;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    CBufferDecoder* DetachDecoder()
//  Operation Type:
//     mutator
//
CBufferDecoder* CAnalyzer::DetachDecoder() {
  // Detaches the buffer decoder from the analyzer. 
  // Current decoder is returned.
  CBufferDecoder* pDecoder = m_pDecoder;
  setDecoder((CBufferDecoder*)kpNULL);

  return pDecoder;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    CEventSink* AttachSink(CEventSink& rsink)
//  Operation Type:
//
//
CEventSink* CAnalyzer::AttachSink(CEventSink& rSink) {
  CEventSink* pSink = m_pSink;
  setEventSink(&rSink);

  return pSink;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    CEventSink* DetachSink()
//  Operation Type:
//
//
CEventSink* CAnalyzer::DetachSink() {
  CEventSink* pSink = m_pSink;
  setEventSink((CEventSink*)kpNULL);

  return pSink;
}
/*!
    This function provides support for super events.  A super event is an
    entity in a physics buffer that has several physics events packed inside
    of it.  The event processor needs to be called once for each event in the
    super-event without OnPhysics decrementing the entity count.
    The event processor would therefore invoke this function
    for all events in a super event except the last.  A call to this
    function will disable the decrement of the remaining entity count
    in OnPhysics. 
 
    Without this support, OnPhysics would not invoke the event processor a
    sufficient number of times.  It is up to the event processor to maitain
    sufficient state to know when it is in the middle of a super event and 
    when it is expecting the front of a super event.

*/
void
CAnalyzer::entityNotDone()
{
  m_fPartialEntity = true;
}
///////////////////////////////////////////////////////////////////////////
//
// Function:
//    DetachAll()
// Operation Type:
//    Utiltiy function
//
void CAnalyzer::DetachAll() {
  DetachDecoder();
  DetachSink();
}

///////////////////////////////////////////////////////////////////////////
//
// Function:
//   UInt_t OnEvent(Address_t pRawData, CEvent& anEvent)
//
// Operation Type:
//   Behavioral - overrridable.
//
UInt_t CAnalyzer::OnEvent(Address_t pRawData, CEvent& anEvent) {
  return 0;			// force an error.
}

//////////////////////////////////////////////////////////////////////
//
// Function
//   void CopyEventList(const CEventList& rhs)
// Operation type:
//   Utility.
//
void CAnalyzer::CopyEventList(const CEventList& rhs) {
  m_EventList.clear();
  m_EventList = rhs;
}

/////////////////////////////////////////////////////////////////////
//
// Function:
//   void CopyEventPool(const CEventList& rhs)
//
// Operation Type:
//   Utility.
//
void CAnalyzer::CopyEventPool(const CEventList& rhs) {
  m_EventPool.clear();
  m_EventPool = rhs;
}
/*!
   This function returns an event to the event pool. It can be called if,
   for any reason, CreateEvent results in an event that is not eventually
   submitted to the event list.  For example, OnPhysics will call this
   if the user's event processing pipeline rejects an event by returning
   kfFalse.  Thanks to Jon Elson of Wash-U for pointing out this
   stupidity of mine.
*/
void
CAnalyzer::ReturnEvent(CEvent* pEvent)
{
	CEventVector& rPool(m_EventPool.getVector());
	rPool.push_back(pEvent);
}
