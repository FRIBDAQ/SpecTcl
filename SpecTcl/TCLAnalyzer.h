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
#endif

#ifndef __ANALYZER_H
#include <Analyzer.h>
#endif

#ifndef __TCLINTERPRETER_H
#include <TCLInterpreter.h>
#endif

#ifndef __TCLVARIABLE_H
#include <TCLVariable.h>
#endif

#ifndef __STL_LIST
#include <list>
#define __STL_LIST
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif

// Names of the variables maintained:

static const char* BufferCountVar    = "BuffersAnalyzed";
static const char* LastSequenceVar   = "LastSeqeunce";

class CEventProcessor;
typedef list<CEventProcessor*> EventProcessingPipeline;
typedef vector<CTCLVariable*>  VariableArray;
//  Counter is an enumerated type which describes the sorts of 
//  statistics counters maintained.

enum Counter {
  RunsAnalyzed          = 0,
  EventsAnalyzed        = 1,
  EventsAnalyzedThisRun = 2,
  EventsAccepted        = 3,
  EventsAcceptedThisRun = 4,
  EventsRejected        = 5,
  EventsRejectedThisRun = 6
};

class CTclAnalyzer : public CAnalyzer
{
  CTCLInterpreter& m_rInterpreter;
  CTCLVariable* m_pBuffersAnalyzed; // # buffers analyzed.
  CTCLVariable* m_pLastSequence; // Last sequence number analyzed.
  CTCLVariable* m_pRunNumber;
  CTCLVariable* m_pRunTitle;
  CTCLVariable* m_pRunState;
  UInt_t        m_nEventSize;

  EventProcessingPipeline m_lAnalysisPipeline;
  VariableArray           m_vStatistics;
  vector<Int_t*>           m_vStatisticsInts;

public:
  CTclAnalyzer(CTCLInterpreter& rInterp, UInt_t nP, UInt_t nBunch);
  ~CTclAnalyzer();
private:
  CTclAnalyzer(const CTclAnalyzer& rSrc); // Copy constructor > not allowed <
  CTclAnalyzer& operator=(const CTclAnalyzer& rhs);
public:
  // The following override base class members:
  
  virtual void OnStateChange(UInt_t nType, CBufferDecoder& rDecoder); //
  virtual void OnPhysics(CBufferDecoder& rDecoder);


  // Extensions to base class functionality:

  virtual void OnBegin(CBufferDecoder* rDecoder); // 
  virtual void OnEnd(CBufferDecoder*   rDecoder); // 
  virtual void OnPause(CBufferDecoder* rDecoder); //
  virtual void OnResume(CBufferDecoder* rDecoder);//
  void AddEventProcessor(CEventProcessor& rProcessor);     // Append to pipe.
  void IncrementCounter(Counter eSelect, UInt_t incr = 1); //
  void ClearCounter(Counter eSelect);                      // 

  void SetEventSize(UInt_t nSize) {m_nEventSize = nSize; }
  void IncrementEventSize(UInt_t nIncr=2) {m_nEventSize += nIncr;}
  UInt_t GetEventSize() { return m_nEventSize; }
  
protected:
  // Utilities:
  virtual UInt_t OnEvent(Address_t pRawData, CEvent& anEvent);
  static void IncrementVariable(CTCLVariable& rVar);           //
  static void SetVariable(CTCLVariable& rVar, int newval);     //
  static void ClearVariable(CTCLVariable& rVar) {    
    SetVariable(rVar, 0);
  }



};

#endif
