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


static const char* Copyright = "(C) Copyright Michigan State University 2008, All rights reserved";
//
//  TCLAnalyzer.cpp:
//    Implementation file for the CTclAnalyzer class.  See
//    TCLAnalyzer.h for information.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
//        
#include <config.h>
#include "TCLAnalyzer.h"
#include "EventProcessor.h"
#include "SpecTcl.h"
#include <buftypes.h>
#include <CTreeParameter.h>
#include <iostream>

#include <algorithm>
#include <stdio.h>
#include "CPipelineManager.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif



// CounterInfo describes the characteristics of a counter.
struct CounterInfo {
  Counter eCounterType;
  const char*   pSubscript;		// Subscript of Statistics array.
  Bool_t  fZeroOnBegin;		// kfTRUE if stat is zeroed at eor.
};

static CounterInfo CounterTable[] = {
  {RunsAnalyzed,          "RunsAnalyzed",          kfFALSE},
  {EventsAnalyzed,        "EventsAnalyzed",        kfFALSE},
  {EventsAnalyzedThisRun, "EventsAnalyzedThisRun", kfTRUE},
  {EventsAccepted,        "EventsAccepted",        kfFALSE},
  {EventsAcceptedThisRun, "EventsAcceptedThisRun", kfTRUE},
  {EventsRejected,        "EventsRejected",        kfFALSE},
  {EventsRejectedThisRun, "EventsRejectedThisRun", kfTRUE}
};
static const int nCounterTableSize = sizeof(CounterTable)/sizeof(CounterInfo);

CTclAnalyzer::CTclAnalyzer(CTCLInterpreter& rInterp, UInt_t nP,
			   UInt_t nBunch) :
  CAnalyzer(nP, nBunch),
  m_rInterpreter(rInterp),
  m_pBuffersAnalyzed(0),
  m_nBuffersAnalyzed(0),
  m_pLastSequence(0),
  m_nLastSequence(0),
  m_pRunNumber(0),
  m_pRunTitle(0),
  m_pRunState(0),
  m_nSequence(0),
  m_initialized(false)
{
  m_pBuffersAnalyzed = new CTCLVariable(&rInterp, 
					string("BuffersAnalyzed"),
					kfFALSE);
  m_pBuffersAnalyzed->Link(&m_nBuffersAnalyzed, TCL_LINK_INT);
  m_nBuffersAnalyzed = 1;

  m_pLastSequence    = new CTCLVariable(&rInterp,
					string("LastSequence"),
					kfFALSE);
  m_pLastSequence->Link(&m_nLastSequence, TCL_LINK_INT);
  m_nLastSequence = 0;

  m_pRunNumber = new CTCLVariable(&rInterp, string("RunNumber"), kfFALSE);
  ClearVariable(*m_pRunNumber);

  m_pRunTitle = new CTCLVariable(&rInterp, string("RunTitle"), kfFALSE);
  m_pRunTitle->Set(">>> Unknown <<<");

  m_pRunState = new CTCLVariable(&rInterp, string("OnlineState"), kfFALSE);
  m_pRunState->Set(">>> Unknown <<<");

  // Statistics is actually an array.
  // A variable is made for each element and stored in m_vStatistics
  // an int is made and linked to that element.
  
  for(int i=0; i < nCounterTableSize; i++) {
    string ElementName("Statistics(");
    ElementName += CounterTable[i].pSubscript;
    ElementName += ")";
    CTCLVariable* pElement = new CTCLVariable(&rInterp, ElementName, kfFALSE);
    Int_t*       pnElement = new Int_t(0);
    pElement->Link(pnElement, TCL_LINK_INT | TCL_LINK_READ_ONLY);
    m_vStatistics.push_back(pElement);
    m_vStatisticsInts.push_back(pnElement);
  }
}

CTclAnalyzer::~CTclAnalyzer() {
  m_pBuffersAnalyzed->Unlink();
  delete m_pBuffersAnalyzed;
  m_pLastSequence->Unlink();
  delete m_pLastSequence;
  delete m_pRunNumber;
  delete m_pRunTitle;
  delete m_pRunState;
  for(VariableArray::iterator p = m_vStatistics.begin(); 
      p != m_vStatistics.end(); p++) {
    (*p)->Unlink();
    delete *p;
  }
  for(vector<Int_t*>::iterator p = m_vStatisticsInts.begin(); 
      p != m_vStatisticsInts.end(); p++) {
    delete *p;
  }
}

void CTclAnalyzer::OnStateChange(UInt_t nType, CBufferDecoder& rDecoder) {
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

void CTclAnalyzer::IncrementVariable(CTCLVariable& rVar) {
  //string Script("incr ");
  //Script += rVar.getVariableName();
  //rVar.getInterpreter()->GlobalEval(Script);
}

void CTclAnalyzer::SetVariable(CTCLVariable& rVar, int newval) {
  string Script("set ");
  char sval[100];
  sprintf(sval, "%d", newval);
  Script += rVar.getVariableName();
  Script += " ";
  Script += sval;
  rVar.getInterpreter()->GlobalEval(Script);
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
UInt_t CTclAnalyzer::OnEvent(Address_t pRawData, CEvent& anEvent) {
  IncrementCounter(EventsAnalyzed);
  IncrementCounter(EventsAnalyzedThisRun);
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  
  EventProcessorIterator p = pMgr->getCurrentPipeline()->begin();
  const CBufferDecoder* cpDecoder(getDecoder());
  CBufferDecoder* pDecoder((CBufferDecoder*)cpDecoder);

  // Set up tree parameter processing:

  CTreeParameter::setEvent(anEvent);

  while(p != pMgr->getCurrentPipeline()->end()) {
    CEventProcessor* pProcessor(p->second);
    Bool_t success;
    try {
      success = pProcessor->operator()(pRawData, anEvent, *this, *pDecoder);
    } 
    catch (string msg) {
      cerr << "Event processor " << p->first << "threw: '" << msg << "'" << endl;
      success = kfFALSE;
    }
    catch (CException& r) {
      IncrementCounter(EventsRejected);
      IncrementCounter(EventsRejectedThisRun);
      throw;
    }
    catch (...) {
      cerr << "Event processor" << p->first << " threw an unanticipated exception " << endl;
      success = kfFALSE;
    }
    if(!success) {
      IncrementCounter(EventsRejected);
      IncrementCounter(EventsRejectedThisRun);
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
void CTclAnalyzer::OnBegin(CBufferDecoder* pDecoder) {
  SetVariable(*m_pBuffersAnalyzed, -1);
  ClearVariable(*m_pLastSequence);
  m_pRunState->Set("Active");
  SetVariable(*m_pRunNumber, pDecoder->getRun());
  m_pRunTitle->Set(pDecoder->getTitle().c_str());
  IncrementCounter(RunsAnalyzed);
  for(UInt_t i = 0; i < nCounterTableSize; i++) {
    if(CounterTable[i].fZeroOnBegin) {
      ClearCounter(CounterTable[i].eCounterType);
    }
  }
  // Iterate through the pipeline's OnBegin() members.
  // The loop is broken on the first false return from a processor.
  //
  
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  EventProcessorIterator p = pMgr->getCurrentPipeline()->begin();
  while(p != pMgr->getCurrentPipeline()->end()) {
    CEventProcessor *pProcessor(p->second);
    if(!(pProcessor->OnBegin(*this, *pDecoder))) break;
    p++;
  }
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
void CTclAnalyzer::OnEnd(CBufferDecoder* rDecoder) {
  m_pRunState->Set("Halted");

  // Iterate through the pipeline's OnEnd() members.
  // The loop is broken on the first false return from a processor.
  //
  CPipelineManger* pMgr = CPipelineManager::getInstance();
  
  EventProcessorIterator p = pMgr->getCurrentPipeline()->begin();
  while(p != pMgr->getCurrentPipeline()->end()) {
    CEventProcessor *pProcessor(p->second);
    if(!(pProcessor->OnEnd(*this, *rDecoder))) break;
    p++;
  }
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
void CTclAnalyzer::OnPause(CBufferDecoder* rDecoder) {
  m_pRunState->Set("Paused");

  // Iterate through the pipeline's OnPause() members.
  // The loop is broken on the first false return from a processor.
  //
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  
  EventProcessorIterator p = pMgr->getCurrentPipeline()->begin();
  while(p != pMgr->getCurrentPipeline()->end()) {
    CEventProcessor *pProcessor(p->second);
    if(!(pProcessor->OnPause(*this, *rDecoder))) break;
    p++;
  }
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
void CTclAnalyzer::OnResume(CBufferDecoder* rDecoder) {
  m_pRunState->Set("Active");
  // Iterate through the pipeline's OnResume() members.
  // The loop is broken on the first false return from a processor.
  //
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  EventProcessorIterator p = pMgr->getCurrentPipeline()->begin();
  while(p != pMgr->getCurrentPipeline->end()) {
    CEventProcessor *pProcessor(p->second);
    if(!(pProcessor->OnResume(*this, *rDecoder))) break;
    p++;
  }
}

/////////////////////////////////////////////////////////////////////
//
// Function:
//   virtual void OnPhysics(CBufferDecoder& rDecoder)
// Operation Type:
//    override for default behavior (extension).
//
void CTclAnalyzer::OnPhysics(CBufferDecoder& rDecoder) {
  CAnalyzer::OnPhysics(rDecoder); // Analyze the buffer.
  // IncrementVariable(*m_pBuffersAnalyzed);
  m_nBuffersAnalyzed++;
  // SetVariable(*m_pLastSequence,rDecoder.getSequenceNo());
  m_nLastSequence = rDecoder.getSequenceNo();
}

//////////////////////////////////////////////////////////////
//
// Function:
//    void IncrementCounter(Counter eSelect, UInt_t incr = 1)
// Operation Type:
//    Utility.
/*
  Increments a counter by the specified amount.
  The counter is specified by the eSelect, an enumerated constant.
  This enumerated value corresponds to an array index as well.
*/
void CTclAnalyzer::IncrementCounter(Counter eSelect, UInt_t incr) {
  *(m_vStatisticsInts[(Int_t)eSelect]) += incr;
}

////////////////////////////////////////////////////////////
// 
// Function:
//   void ClearCounter(Counter eSelect)
// Operation Type:
//   Utility
//
/*
  Clear a counter given it's index in enum form.
*/
void CTclAnalyzer::ClearCounter(Counter eSelect) {
  *(m_vStatisticsInts[(Int_t)eSelect]) = 0;
}

/*!
   Called when an unrecognized buffer is received.
   Iterate through the event pipeline calling event processor
   OnOther member functions.
   \param nType (UInt_t [in]):
     A type identifier for the buffer type.
   \param rDecoder (CBufferDecoder& [in]):
      Decoder that called us.

*/
void CTclAnalyzer::OnOther(UInt_t nType, CBufferDecoder& rDecoder) {

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
  
}
/*!
  Called for scaler buffers, we delegate to onOther..
*/
void
CTclAnalyzer::OnScaler(CBufferDecoder& rDecoder)
{
  OnOther(rDecoder.getBufferType(), rDecoder);
}
/**
 * OnInitialize
 *
 * Called after all SpecTcl initialization is done.
 * Iterates through the event processors invoking OnInitialize in each.
 */
void
CTclAnalyzer::OnInitialize()
{
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
    
    for (int i=0; i < processorsToRemove.size(); i++) {
        RemoveEventProcessor(GetCurrentPipeline(), processorsToRemove[i]);
    }
}

/*!
   Called when the event source hit an end file.
    call the OnEventSourceEOF member for each event processing pipeline
    element.
*/

void
CTclAnalyzer::OnEndFile()
{
  SpecTcl* api = SpecTcl::getInstance();
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  CTclAnalyzer::EventProcessorIterator p = pMgr->getCurrentPipeline()->begin()

  while (p != pMgr->getCurrentPipeline()->end()) {
    CEventProcessor *pProcessor = p->second;
    if (!pProcessor->OnEventSourceEOF()) break;
    p++;
  }
  // Set the Run State to Halted:

  m_pRunState->Set("Halted");    // <-- Added this line
}


/*!
  Assigns a unique name of the form Anonymous::n where n is an integer.  This is
  used to assign unique names to EventProcessors the user has not named.
*/
string 
CTclAnalyzer::AssignName()
{
  string name;
  while (1) {
    char buffer[100];

    name = "Anonymous::";
    snprintf(buffer, sizeof(buffer), "%d", m_nSequence);
    m_nSequence++;
    name += buffer;

    if(FindEventProcessor(current, buffer) == end(current)) {
      break;
    }
  }
  return name;
}

