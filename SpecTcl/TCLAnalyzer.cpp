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
static char* pCopyright = 
"TCLAnalyzer.cpp - (c) Copyright 2000 NSCL, All rights reserved";
#include "TCLAnalyzer.h"
#include "EventProcessor.h"
#include <buftypes.h>


// CounterInfo describes the characteristics of a counter.

struct CounterInfo {
  Counter eCounterType;
  char*   pSubscript;		// Subscript of Statistics array.
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
  m_pLastSequence(0),
  m_pRunNumber(0),
  m_pRunTitle(0),
  m_pRunState(0)
{
  m_pBuffersAnalyzed = new CTCLVariable(&rInterp, 
					string("BuffersAnalyzed"),
					kfFALSE);
  ClearVariable(*m_pBuffersAnalyzed);

  m_pLastSequence    = new CTCLVariable(&rInterp,
					string("LastSequence"),
					kfFALSE);
  ClearVariable(*m_pLastSequence);

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
  delete m_pBuffersAnalyzed;
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


void
CTclAnalyzer::OnStateChange(UInt_t nType, CBufferDecoder& rDecoder)
{
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

void
CTclAnalyzer::IncrementVariable(CTCLVariable& rVar) 
{
  string Script("incr ");
  Script += rVar.getVariableName();
  rVar.getInterpreter()->GlobalEval(Script);
}
void
CTclAnalyzer::SetVariable(CTCLVariable& rVar, int newval)
{
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
UInt_t
CTclAnalyzer::OnEvent(Address_t pRawData, CEvent& anEvent)
{
  IncrementCounter(EventsAnalyzed);
  IncrementCounter(EventsAnalyzedThisRun);
  EventProcessingPipeline::iterator p = m_lAnalysisPipeline.begin();
  const CBufferDecoder* cpDecoder(getDecoder());
  CBufferDecoder* pDecoder((CBufferDecoder*)cpDecoder);
  while(p != m_lAnalysisPipeline.end()) {
    CEventProcessor* pProcessor(*p);
    if(!pProcessor->operator()(pRawData, anEvent, *this, *pDecoder)) {
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
void
CTclAnalyzer::OnBegin(CBufferDecoder* pDecoder)
{
  ClearVariable(*m_pBuffersAnalyzed);
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
  EventProcessingPipeline::iterator p = m_lAnalysisPipeline.begin();
  while(p != m_lAnalysisPipeline.end()) {
    CEventProcessor *pProcessor(*p);
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
void
CTclAnalyzer::OnEnd(CBufferDecoder*   rDecoder)
{
  m_pRunState->Set("Halted");

  // Iterate through the pipeline's OnEnd() members.
  // The loop is broken on the first false return from a processor.
  //
  EventProcessingPipeline::iterator p = m_lAnalysisPipeline.begin();
  while(p != m_lAnalysisPipeline.end()) {
    CEventProcessor *pProcessor(*p);
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
void
CTclAnalyzer::OnPause(CBufferDecoder* rDecoder)
{
  m_pRunState->Set("Paused");

  // Iterate through the pipeline's OnPause() members.
  // The loop is broken on the first false return from a processor.
  //
  EventProcessingPipeline::iterator p = m_lAnalysisPipeline.begin();
  while(p != m_lAnalysisPipeline.end()) {
    CEventProcessor *pProcessor(*p);
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
void
CTclAnalyzer::OnResume(CBufferDecoder* rDecoder)
{
  m_pRunState->Set("Active");
  // Iterate through the pipeline's OnResume() members.
  // The loop is broken on the first false return from a processor.
  //
  EventProcessingPipeline::iterator p = m_lAnalysisPipeline.begin();
  while(p != m_lAnalysisPipeline.end()) {
    CEventProcessor *pProcessor(*p);
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
void
CTclAnalyzer::OnPhysics(CBufferDecoder& rDecoder)
{
  CAnalyzer::OnPhysics(rDecoder); // Analyze the buffer.
  IncrementVariable(*m_pBuffersAnalyzed);
  SetVariable(*m_pLastSequence,rDecoder.getSequenceNo());
}
/////////////////////////////////////////////////////////////////////
//
// Function:
//   void AddEventProcessor(CEventProcessor& rProcessor)
// 
// Operation Type:
//   Mutator.
/*
   Adds an event processor to the end of the processing pipeline.
*/
void
CTclAnalyzer::AddEventProcessor(CEventProcessor& rProcessor)
{
  m_lAnalysisPipeline.push_back(&rProcessor);
  rProcessor.OnAttach(*this);
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
void
CTclAnalyzer::IncrementCounter(Counter eSelect, UInt_t incr = 1)
{
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
void
CTclAnalyzer::ClearCounter(Counter eSelect)
{
  *(m_vStatisticsInts[(Int_t)eSelect]) = 0;
}



