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

void CThreadAnalyzer::OnStateChange(UInt_t nType, CBufferDecoder& rDecoder) {
  CAnalyzer::OnStateChange(nType, rDecoder);
  switch(nType) {
  case BEGRUNBF:
    OnBegin(&rDecoder);
    break;
  case ENDRUNBF:
    OnEnd(&rDecoder);
    break;
  case PAUSEBF:
    OnPause(&rDecoder);
    break;
  case RESUMEBF:
    OnResume(&rDecoder);
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
UInt_t CThreadAnalyzer::OnEvent(Address_t pRawData, CEvent& anEvent, CBufferDecoder& rDecoder, EventProcessingPipeline& pipecopy, BufferTranslator& trans) {

  EventProcessorIterator p = pipecopy.begin();

  // pass the decoder from argument
  const CBufferDecoder* cpDecoder(&rDecoder);
  CBufferDecoder* pDecoder((CBufferDecoder*)cpDecoder);

  // Set up tree parameter processing:
  CTreeParameter::setEvent(anEvent);

  while(p != pipecopy.end()) {
    CEventProcessor* pProcessor(p->second);
    Bool_t success;
    try {
      success = pProcessor->operator()(pRawData, anEvent, *this, *pDecoder, trans);
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
//   virtual void OnBegin(CBufferDecoder* rDecoder)
// Operation Type:
//   Behavioral default.
//
/*
  Gains control when SpecTcl recieves a begin run buffer.
  The action is to:
  1. Zero m_pBuffers Analyzed.
  2. Zero Last Sequence.
  3. Set the run number and
  4. Set run title variables.
  5. Set the runstate to active.
  6. Increment Statistics(RunsAnalyzed).
  7. Clear the appropriate set of counters.
*/
void CThreadAnalyzer::OnBegin(CBufferDecoder* pDecoder) {

  // Iterate through the pipeline's OnBegin() members.
  // The loop is broken on the first false return from a processor.
  //
  /*  
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  EventProcessorIterator p = pMgr->getCurrentPipeline()->begin();
  while(p != pMgr->getCurrentPipeline()->end()) {
    CEventProcessor *pProcessor(p->second);
    if(!(pProcessor->OnBegin(*this, *pDecoder))) break;
    p++;
  }
  */
}

////////////////////////////////////////////////////////////////
//
// Function:
//   virtual void OnEnd(CBufferDecoder*   rDecoder)
// Operation type:
//   Default behavior.
//
/*
  Gets control when an end run buffer is received action is to:
  1. Set the run state to Halted.
  
*/
void CThreadAnalyzer::OnEnd(CBufferDecoder* rDecoder) {

  //  m_pRunState->Set("Halted");

  // Iterate through the pipeline's OnEnd() members.
  // The loop is broken on the first false return from a processor.
  //
  /*
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  
  EventProcessorIterator p = pMgr->getCurrentPipeline()->begin();
  while(p != pMgr->getCurrentPipeline()->end()) {
    CEventProcessor *pProcessor(p->second);
    if(!(pProcessor->OnEnd(*this, *rDecoder))) break;
    p++;
  }
  */
}

/////////////////////////////////////////////////////////////////
//
// Function:
//   virtual void OnPause(CBufferDecoder*   rDecoder)
// Operation Type:
//   Default behavior.
/*
  This function gains control when a pause run buffer is recieved.
  Action is to set the run state to paused.
*/
void CThreadAnalyzer::OnPause(CBufferDecoder* rDecoder) {

  //  m_pRunState->Set("Paused");

  // Iterate through the pipeline's OnPause() members.
  // The loop is broken on the first false return from a processor.
  //
  /*
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  
  EventProcessorIterator p = pMgr->getCurrentPipeline()->begin();
  while(p != pMgr->getCurrentPipeline()->end()) {
    CEventProcessor *pProcessor(p->second);
    if(!(pProcessor->OnPause(*this, *rDecoder))) break;
    p++;
  }
  */
}

///////////////////////////////////////////////////////////////////
//
// Function:
//   virtual void OnResume(CBufferDecoder* rDecoder)
// Operation Type:
//   Default behavior
//
/*
  Gains control when the run is resumed.
  Action is to set the run state to Active.
*/
void CThreadAnalyzer::OnResume(CBufferDecoder* rDecoder) {

  //  m_pRunState->Set("Active");

  // Iterate through the pipeline's OnResume() members.
  // The loop is broken on the first false return from a processor.
  //
  /*
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  EventProcessorIterator p = pMgr->getCurrentPipeline()->begin();
  while(p != pMgr->getCurrentPipeline()->end()) {
    CEventProcessor *pProcessor(p->second);
    if(!(pProcessor->OnResume(*this, *rDecoder))) break;
    p++;
  }
  */
}

/////////////////////////////////////////////////////////////////////
//
// Function:
//   virtual void OnPhysics(CBufferDecoder& rDecoder)
// Operation Type:
//    override for default behavior (extension).
//
void CThreadAnalyzer::OnPhysics(long thread, CBufferDecoder& rDecoder, UInt_t nBufferSize, Address_t pData, EventProcessingPipeline& pipeline, BufferTranslator& trans, CEventList& lst) {
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
  //     2. There is no buffer decoder.  //
  //  If there is no event sink, the buffers are decoded just not
  //  sent to the sink.
  //    This may be useful to derived classes in other environments.
  //
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
	nEventSize = OnEvent(pData, *pEvent, rDecoder, pipeline, trans);
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

void CThreadAnalyzer::OnOther(UInt_t nType, CBufferDecoder& rDecoder) {

  /*
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  EventProcessorIterator p = pMgr->getCurrentPipeline()->begin();

  //int i = 0;
  while(p != pMgr->getCurrentPipeline()->end()) {
    CEventProcessor* pProcessor(p->second);
    if(!pProcessor->OnOther(nType, *this, rDecoder)) {
      break ;
    }
    p++;
    //i++;
  }
  */
  
}

/*!
  Called for scaler buffers, we delegate to onOther..
*/
void
CThreadAnalyzer::OnScaler(CBufferDecoder& rDecoder)
{
  /*
  OnOther(rDecoder.getBufferType(), rDecoder);
  */
}

/**
 * OnInitialize
 *
 * Called after all SpecTcl initialization is done.
 * Iterates through the event processors invoking OnInitialize in each.
 */
void
CThreadAnalyzer::OnInitialize()
{
  /*
    SpecTcl* api = SpecTcl::getInstance();
    
    CPipelineManager* mgr = CPipelineManager::getInstance();
    CTclAnalyzer::EventProcessorIterator p = mgr->getCurrentPipeline()->begin();
    m_initialized = true;
    
    std::vector<std::string> processorsToRemove;
    
    while (p != mgr->getCurrentPipeline()->end()) {
        CEventProcessor* pProcessor = p->second;
        if (!pProcessor->OnInitialize()) {
            // An event processor failed  OnInitialze - complain and remove.
            
            std::cerr << "****ERROR*****\nEvent processor " << p->first << " failed OnInitialize.";
            std::cerr << " (returned kfFalse) will be removed\n";
            processorsToRemove.push_back(p->first);
        }
        p++;
    }
    // Now remove any failed even processors:
    
    std::string pipeName  = api->GetCurrentPipeline();
    for (int i=0; i < processorsToRemove.size(); i++) {
      
        api->RemoveEventProcessor(pipeName.c_str(), processorsToRemove[i].c_str());
    }
  */
}

/*!
   Called when the event source hit an end file.
    call the OnEventSourceEOF member for each event processing pipeline
    element.
*/

void
CThreadAnalyzer::OnEndFile()
{
  /*
  SpecTcl* api = SpecTcl::getInstance();
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  CTclAnalyzer::EventProcessorIterator p = pMgr->getCurrentPipeline()->begin();

  while (p != pMgr->getCurrentPipeline()->end()) {
    CEventProcessor *pProcessor = p->second;
    if (!pProcessor->OnEventSourceEOF()) break;
    p++;
  }
  // Set the Run State to Halted:

  m_pRunState->Set("Halted");    // <-- Added this line
  */
}

void
CThreadAnalyzer::ReturnEvent(CEvent* pEvent)
{
  CEventVector& rPool(eventPool.getVector());
  rPool.push_back(pEvent);
}
