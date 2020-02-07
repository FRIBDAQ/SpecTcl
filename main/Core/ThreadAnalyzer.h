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

//
// ThreadAnalyzer.h:
//   This file contains a definition of the CThreadAnalyzer class.
//   this class is a derivation of the SpecTcl CAnalyzer class for
//   multi-thread based analyzers. 
//
// Author:
//    Giordano Cerizza
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
//    

#ifndef THREADANALYZER_H
#define THREADANALYZER_H

#include <histotypes.h>
#include <Analyzer.h>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <ParameterMapper.h>

class CEventProcessor;
class CParameterMapper;

class CThreadAnalyzer : public CAnalyzer {

 public:
  
  typedef std::pair<std::string, CEventProcessor*> PipelineElement;
  typedef std::list<PipelineElement> EventProcessingPipeline;
  typedef std::map <std::string, EventProcessingPipeline > MapEventProcessingPipeline;
  typedef EventProcessingPipeline::iterator EventProcessorIterator;

 private:
  
  UInt_t    m_nEventSize;
  UInt_t    nEventNo;

  CEventList eventPool;
  CEventList eventList;  
  
 public:
  CThreadAnalyzer();
  ~CThreadAnalyzer();

  // private:
  CThreadAnalyzer(const CThreadAnalyzer& rhs);
  CThreadAnalyzer& operator=(const CThreadAnalyzer& rhs);
  
public:

  // The following override base class members:
  virtual void OnStateChange(UInt_t nType, EventProcessingPipeline& source, CBufferDecoder& rDecoder);
  virtual void OnPhysics(long thread, CBufferDecoder& rDecoder, UInt_t nBufferSize, Address_t pData, EventProcessingPipeline& pipeline, BufferTranslator& trans, CEventList& lst, DAQ::DDAS::CParameterMapper& map);
  virtual void OnOther(UInt_t nType, EventProcessingPipeline& source, CBufferDecoder& rDecoder);
  virtual void OnScaler(EventProcessingPipeline& source, CBufferDecoder& rDecoder);
  virtual void OnEndFile();

  // Extensions to base class functionality:
  virtual void OnBegin(EventProcessingPipeline& source, CBufferDecoder& rDecoder);
  virtual void OnEnd(EventProcessingPipeline& source, CBufferDecoder& rDecoder);
  virtual void OnPause(EventProcessingPipeline& source, CBufferDecoder& rDecoder);
  virtual void OnResume(EventProcessingPipeline& source, CBufferDecoder& rDecoder);
  virtual void OnInitialize();

  // Handling object tuning parameters:
  void SetEventSize(UInt_t nSize) {m_nEventSize = nSize; }
  void IncrementEventSize(UInt_t nIncr=2) {m_nEventSize += nIncr;}
  UInt_t GetEventSize() {
    return m_nEventSize;
  }
  
 protected:
  
  virtual UInt_t OnEvent(Address_t pRawData, CEvent& anEvent, CBufferDecoder& rDecoder, EventProcessingPipeline& pipecopy, BufferTranslator& trans, long thread, DAQ::DDAS::CParameterMapper& map);

 private:
  CEvent* CreateEvent() {
    CEventVector& rVec(eventPool.getVector());
    CEvent* pEvent;
    if(rVec.empty()) {
      pEvent =  new CEvent(m_nParametersInEvent);
    } else {
      pEvent = rVec.back();
      rVec.pop_back();
    }
    if(pEvent) pEvent->clear(); // BUG prevention.
    else pEvent= new CEvent(m_nParametersInEvent);
    return pEvent;
  }

  void ReturnEvent(CEvent* pEvent);

  void ClearEventList() {
    CEventVector& evlist(eventList.getVector());
    CEventVector& evpool(eventPool.getVector());
    CEventListIterator p = evlist.begin();
    for(; p != evlist.end(); p++) {
      if(*p) {
	CEvent* pEvent = *p;
	evpool.push_back(pEvent);
	*p = (CEvent*)kpNULL;
      }
    }
  }
  
};

#endif
