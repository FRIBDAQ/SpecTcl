/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2019.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Giordano Cerizza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


static const char* Copyright = "(C) Copyright Michigan State University 2019, All rights reserved";
//
//  ThreadAnalyzer.cpp:
//    Implementation file for the CThreadAnalyzer class.  See
//    ThreadAnalyzer.h for information.
//
// Author:
//    Giordano Cerizza
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
//        
#include <config.h>
#include <iostream>
#include <stdint.h>
#include <buftypes.h>
#include <CTreeParameter.h>
#include <algorithm>
#include <stdio.h>
#include <set>
#include "Exception.h"
#include "EventFormatError.h"
#include "Globals.h"
#include "ThreadAnalyzer.h"
#include "ZMQRDPatternClass.h"
#include "EventProcessor.h"
#include "SpecTcl.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

CThreadAnalyzer::CThreadAnalyzer()
{
}

CThreadAnalyzer::~CThreadAnalyzer()
{
}

void CThreadAnalyzer::OnStateChange(UInt_t nType, EventProcessingPipeline& source, CBufferDecoder& rDecoder) {
  switch(nType) {
  case BEGRUNBF:
    OnBegin(source, rDecoder);
    break;
  case ENDRUNBF:
    OnEnd(source, rDecoder);
    break;
  case PAUSEBF:
    OnPause(source, rDecoder);
    break;
  case RESUMEBF:
    OnResume(source, rDecoder);
    break;
  default:
    break;
  }
}

////////////////////////////////////////////////////////////////////////
//
//  Function:
//     UInt_t OnEvent(Address_t pRawData, CEvent& anEvent)
// Operation type:
//    override.
/*
  Iterates through the processor pipeline
  calling each processor and giving it a chance to process the event.
  Returns the value of m_nEventSize.  Aborts the event if necessary.
*/
UInt_t CThreadAnalyzer::OnEvent(Address_t pRawData, CEvent& anEvent, CBufferDecoder& rDecoder, EventProcessingPipeline& pipecopy, BufferTranslator& trans, long thread, DAQ::DDAS::CParameterMapper& map) {

  EventProcessorIterator p = pipecopy.begin();

  // pass the decoder from argument
  const CBufferDecoder* cpDecoder(&rDecoder);
  CBufferDecoder* pDecoder((CBufferDecoder*)cpDecoder);

  // Set up tree parameter processing 
  long dummy;
  CTreeParameter::setEvent(anEvent, dummy);
  
  while(p != pipecopy.end()) {
    CEventProcessor* pProcessor(p->second);
    Bool_t success;
    try {
      success = pProcessor->operator()(pRawData, anEvent, *this, *pDecoder, trans, thread);
    } 
    catch (string msg) {
      cerr << "Event processor " << p->first << "threw: '" << msg << "'" << endl;
      success = kfFALSE;
    }
    catch (CException& r) {
      throw;
    }
    catch (...) {
      cerr << "Event processor" << p->first << " threw an unanticipated exception " << endl;
      success = kfFALSE;
    }
    if(!success) {
      AbortEvent();
      return m_nEventSize;
    }
    
    p++;
  }
  return m_nEventSize;
}

////////////////////////////////////////////////////////////////////////
//
// Function:
//   virtual void OnBegin(EventProcessingPipeline& source) 
void CThreadAnalyzer::OnBegin(EventProcessingPipeline& source, CBufferDecoder& rDecoder) {
  EventProcessorIterator p;
  for (p = source.begin(); p != source.end(); p++) {
    CEventProcessor *pProcessor(p->second);
    pProcessor->OnBegin(*this, rDecoder);
  }
}

////////////////////////////////////////////////////////////////
//
// Function:
//   virtual void OnEnd(EventProcessingPipeline& source) 
void CThreadAnalyzer::OnEnd(EventProcessingPipeline& source, CBufferDecoder& rDecoder) {
  EventProcessorIterator p;
  for (p = source.begin(); p != source.end(); p++) {
    CEventProcessor *pProcessor(p->second);
    pProcessor->OnEventSourceEOF();
  }
}

/////////////////////////////////////////////////////////////////
//
// Function:
//   virtual void OnPause(EventProcessingPipeline& source) 
void CThreadAnalyzer::OnPause(EventProcessingPipeline& source, CBufferDecoder& rDecoder) {
  EventProcessorIterator p;
  for (p = source.begin(); p != source.end(); p++) {
    CEventProcessor *pProcessor(p->second);
    pProcessor->OnPause(*this, rDecoder);
  }
}

///////////////////////////////////////////////////////////////////
//
// Function:
//   virtual void OnResume(EventProcessingPipeline& source) 
void CThreadAnalyzer::OnResume(EventProcessingPipeline& source, CBufferDecoder& rDecoder) {
  EventProcessorIterator p;
  for (p = source.begin(); p != source.end(); p++) {
    CEventProcessor *pProcessor(p->second);
    pProcessor->OnResume(*this, rDecoder);
  }
}

///////////////////////////////////////////////////////////////////
//
// Function:
//   virtual void OnOther(UInt_t nType, EventProcessingPipeline& source)
void CThreadAnalyzer::OnOther(UInt_t nType, EventProcessingPipeline& source, CBufferDecoder& rDecoder) {
  EventProcessorIterator p;
  for (p = source.begin(); p != source.end(); p++) {
    CEventProcessor *pProcessor(p->second);
    pProcessor->OnOther(nType, *this, rDecoder);
  }
}

///////////////////////////////////////////////////////////////////
//
//  Called for scaler buffers, we delegate to onOther..
//
void
CThreadAnalyzer::OnScaler(EventProcessingPipeline& source, CBufferDecoder& rDecoder) {
}

///////////////////////////////////////////////////////////////////
//
// Function:
//   virtual void OnInitialize()
void
CThreadAnalyzer::OnInitialize() {
}

///////////////////////////////////////////////////////////////////
//
// Function:
//   virtual void OnEndFile()
void
CThreadAnalyzer::OnEndFile() {
}

/////////////////////////////////////////////////////////////////////
//
// Function:
//   virtual void OnPhysics(...)
// Operation Type:
//    override for default behavior (extension).
//
void CThreadAnalyzer::OnPhysics(long thread, CBufferDecoder& rDecoder, UInt_t nBufferSize, Address_t pData, EventProcessingPipeline& pipeline, BufferTranslator& trans, CEventList& lst, DAQ::DDAS::CParameterMapper& map) {
  UInt_t    nEvents     = 1;
  UInt_t    nEventNo    = 0;
  UInt_t    nOffset     = 0;
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
	ClearEventList();
	return;
      }
      pEvent     = CreateEvent();
      try {
	
	m_fAbort = kfFALSE;
	nEventSize = OnEvent(pData, *pEvent, rDecoder, pipeline, trans, thread, map);
	if(!m_fAbort) {
	  eventList[nEventNo] = pEvent;
	  lst[nEventNo] = pEvent;
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
	ClearEventList();
	nEventNo = 0;
      }
    }
    // Dump any partial event list:
    if(nEventNo) {
      ClearEventList();
      nEventNo = 0;
    }
  }
  catch(CEventFormatError& rError) {
    delete pEvent;            // Kill hanging event.
    cerr << "---------------------------------------------------------\n";
    cerr << "Event format error detected while processing buffer" << endl;
    cerr << rError.ReasonText() << endl;
    cerr << "Event processing continues with next buffer" << endl;
    cerr << "---------------------------------------------------------\n";
    
  }
  catch(CException& rError) {
    delete pEvent;            // Kill hanging event.
    cerr << "---------------------------------------------------------\n";
    cerr << "Unexpected exception caught while analyzing events.\n";
    cerr << rError.ReasonText() << endl;
    cerr << "Attempting to continue processing with next buffer\n";
    cerr << "---------------------------------------------------------\n";
  }
}

void
CThreadAnalyzer::ReturnEvent(CEvent* pEvent)
{
  CEventVector& rPool(eventPool.getVector());
  rPool.push_back(pEvent);
}
