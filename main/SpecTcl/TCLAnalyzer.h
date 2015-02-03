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

//
// TCLAnalyzer.h:
//   This file contains a definition of the CTclAnalyzer class.
//   this class is a derivation of the SpecTcl CAnalyzer class for
//   TCL based analyzers.  It supports maintenance of variables from
//   which analysis efficiency can be computed (e.g. when SpecTcl is
//   running in an online environment.
//
//   The variables maintained are:
//      BuffersAnalyzed - Number of buffers analyzed for this run.
//      LastSequence    - Sequence number of the last buffer analyzed.
//
// Author:
//    Ron FOx
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
//    

#ifndef __TCLANALYZER_H
#define __TCLANALYZER_H

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#define __HISTOTYPES_H
#endif

#ifndef __ANALYZER_H
#include <Analyzer.h>
#define __ANALYZER_H
#endif

#ifndef __TCLINTERPRETER_H
#include <TCLInterpreter.h>
#define __TCLINTERPRETER_H
#endif

#ifndef __TCLVARIABLE_H
#include <TCLVariable.h>
#define __TCLVARIABLE_H
#endif

#ifndef __STL_LIST
#include <list>
#define __STL_LIST
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif


#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

// Names of the variables maintained:
static const char* BufferCountVar    = "BuffersAnalyzed";
static const char* LastSequenceVar   = "LastSeqeunce";

class CEventProcessor;
// Counter is an enumerated type which describes the sorts of
// statistics counters maintained.
enum Counter {
  RunsAnalyzed          = 0,
  EventsAnalyzed        = 1,
  EventsAnalyzedThisRun = 2,
  EventsAccepted        = 3,
  EventsAcceptedThisRun = 4,
  EventsRejected        = 5,
  EventsRejectedThisRun = 6
};

/*!
  This is the analyzer that holds the event processing pipeline.
  @author Ron Fox
  @version 1.0
  @created 08-Mar-2005 09:57:35 AM
*/
class CTclAnalyzer : public CAnalyzer {
  // public data types:
public:
  typedef STD(pair)<STD(string), CEventProcessor*> PipelineElement;
  typedef STD(list)<PipelineElement> EventProcessingPipeline;
  typedef EventProcessingPipeline::iterator EventProcessorIterator;
  typedef STD(vector)<CTCLVariable*>  VariableArray;
  
  // member data:

private:

  CTCLInterpreter& m_rInterpreter;
  CTCLVariable* m_pBuffersAnalyzed; // # buffers analyzed.
  CTCLVariable* m_pLastSequence; // Last sequence number analyzed.
  CTCLVariable* m_pRunNumber;
  CTCLVariable* m_pRunTitle;
  CTCLVariable* m_pRunState;
  UInt_t        m_nEventSize;

  EventProcessingPipeline m_lAnalysisPipeline;
  VariableArray           m_vStatistics;
  STD(vector)<Int_t*>     m_vStatisticsInts;

  UInt_t        m_nSequence;	//!< Anonymous naming sequence #.
  bool          m_initialized;

public:
  CTclAnalyzer(CTCLInterpreter& rInterp, UInt_t nP, UInt_t nBunch);
  ~CTclAnalyzer();
private:
  CTclAnalyzer(const CTclAnalyzer& rSrc); // Copy constructor > not allowed <
  CTclAnalyzer& operator=(const CTclAnalyzer& rhs);
  
  // Selectors:
  
public:
  CTCLInterpreter* getInterpreter() {
    return &m_rInterpreter;
  }
  
public:
  // The following override base class members:

  virtual void OnStateChange(UInt_t nType, CBufferDecoder& rDecoder);
  virtual void OnPhysics(CBufferDecoder& rDecoder);
  virtual void OnOther(UInt_t nType, CBufferDecoder& rDecoder);
  virtual void OnScaler(CBufferDecoder& rDecoder);
  virtual void OnEndFile();

  // Extensions to base class functionality:
  virtual void OnBegin(CBufferDecoder* rDecoder);
  virtual void OnEnd(CBufferDecoder*   rDecoder);
  virtual void OnPause(CBufferDecoder* rDecoder);
  virtual void OnResume(CBufferDecoder* rDecoder);
  virtual void OnInitialize();



  // Manipulating and inquiring the event processing pipeline.

  void AddEventProcessor(CEventProcessor& rProcessor,
			 const char* pName = 0); // Append to pipe.
  EventProcessorIterator FindEventProcessor(STD(string) name);
  EventProcessorIterator FindEventProcessor(CEventProcessor& processor);
  void InsertEventProcessor(CEventProcessor& processor, 
			    EventProcessorIterator here, 
			    const char* name = 0);
  CEventProcessor* RemoveEventProcessor(STD(string) name);
  CEventProcessor* RemoveEventProcessor(EventProcessorIterator here);
  UInt_t size();
  EventProcessorIterator begin();
  EventProcessorIterator end();


  // Maintaining counters.

  void IncrementCounter(Counter eSelect, UInt_t incr = 1);
  void ClearCounter(Counter eSelect);

  // Handling object tuning parameters:

  void SetEventSize(UInt_t nSize) {m_nEventSize = nSize; }
  void IncrementEventSize(UInt_t nIncr=2) {m_nEventSize += nIncr;}
  UInt_t GetEventSize() {
    return m_nEventSize;
  }
  
protected:
  // Utilities:
  virtual UInt_t OnEvent(Address_t pRawData, CEvent& anEvent);
  static void IncrementVariable(CTCLVariable& rVar);
  static void SetVariable(CTCLVariable& rVar, int newval);
  static void ClearVariable(CTCLVariable& rVar) {
    SetVariable(rVar, 0);
  }

  STD(string) AssignName();

  // Nested utility classes these are predicates that are
  // used for stl algorithm calls.

private:
  //! Match pipeline element by name.
  class MatchName 
  {
  private:
    STD(string)       m_sName;
    CEventProcessor*  m_pLastMatch;
  public:
    MatchName(STD(string) name);
    bool operator()(PipelineElement& element);
    CEventProcessor*  getLastMatch() const;
  };
  //! Match pipeline element by pointer.
  class MatchAddress 
  {
  private:
    CEventProcessor* m_pProcessor;
    CEventProcessor* m_pLastMatch;
  public:
    MatchAddress(CEventProcessor& processor);
    bool operator()(PipelineElement& element);
    CEventProcessor* getLastMatch() const;
  };

};

#endif
