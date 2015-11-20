// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#define private public
#include "CAnalysisPipeline.h"
#include "CSpecTclInterpreter.h"
#undef private
#include "CDecoder.h"
#include "CEventProcessor.h"
#include "CDataSource.h"
#include <tcl.h>

//stub

#include "CParameter.h"
void CParameter::invalidateAll() {}


class AnalysisPipelineTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(AnalysisPipelineTests);
  CPPUNIT_TEST(initiallyEmpty);
  CPPUNIT_TEST(add1);
  CPPUNIT_TEST(iterator);
  CPPUNIT_TEST(find);
  CPPUNIT_TEST(findnox);
  CPPUNIT_TEST(addbeforename);	
  CPPUNIT_TEST(addbeforenox);
  CPPUNIT_TEST(dispatch);
  CPPUNIT_TEST_SUITE_END();


private:
  CAnalysisPipeline* m_pPipe;
public:
  void setUp() {
    CSpecTclInterpreter::setInterp(Tcl_CreateInterp());
    m_pPipe = CAnalysisPipeline::instance();
    
  }
  void tearDown() {
    m_pPipe->clearPipeline();
    delete CAnalysisPipeline::m_pInstance;
    CAnalysisPipeline::m_pInstance = 0;
    delete CSpecTclInterpreter::m_pInstance;
    CSpecTclInterpreter::m_pInstance = 0;
    Tcl_DeleteInterp(CSpecTclInterpreter::m_pInterp);
    CSpecTclInterpreter::m_pInterp = 0;
  }
protected:
  void initiallyEmpty();
  void add1();
  void iterator();
  void find();
  void findnox();
  void addbeforename();
  void addbeforenox();
  void dispatch();
};

CPPUNIT_TEST_SUITE_REGISTRATION(AnalysisPipelineTests);

// Null event processor definition -- mostly what we'll stick in the pipeline.

class NullEventProcessor : public CEventProcessor
{
public:
  NullEventProcessor(const char* pName = 0) :
    CEventProcessor(pName) {}
  bool onEvent(void* pEvent, size_t nBytes, CAnalysisPipeline* pipeline) {
    return true;
  }
};


// Analysis pipeline is initially empty.
void AnalysisPipelineTests::initiallyEmpty() {
  EQ(static_cast<size_t>(0), m_pPipe->analysisSize());
}

// Add an element to the pipeline.



void AnalysisPipelineTests::add1()
{
  CEventProcessor* pEp = new NullEventProcessor("null");
  m_pPipe->addAnalysisElement(pEp);

  CAnalysisPipeline::PipelineIterator p = m_pPipe->m_analysis.begin();
  EQ(static_cast<size_t>(1), m_pPipe->analysisSize());
  EQ(pEp, *p);
  
  p++;
  ASSERT(p == m_pPipe->m_analysis.end());
}
/**
 * iterator
 *
 *  After adding a few elements in the pipeline ensure that
 *  analysisBegin and analysisEnd give sensible iterators.
 */
void
AnalysisPipelineTests::iterator()
{
  CEventProcessor* p1    = new NullEventProcessor("1");
  CEventProcessor* p2    = new NullEventProcessor("2");
  CEventProcessor* pLast = new NullEventProcessor("3");

  m_pPipe->addAnalysisElement(p1);  
  m_pPipe->addAnalysisElement(p2);  
  m_pPipe->addAnalysisElement(pLast);

  CAnalysisPipeline::PipelineIterator p = m_pPipe->analysisBegin();
  ASSERT(p != m_pPipe->analysisEnd());
  EQ(p1, *p);
  p++;
  EQ(p2, *p);
  p++;
  EQ(pLast, *p);

  p++;				// Should be at end now.

  ASSERT(p == m_pPipe->analysisEnd());

}

/**
 * find
 *   Once an item is put into the pipeline it should be findable
 */
void
AnalysisPipelineTests::find()
{
  CEventProcessor* p1    = new NullEventProcessor("1");
  CEventProcessor* p2    = new NullEventProcessor("2");
  CEventProcessor* pLast = new NullEventProcessor("3");

  m_pPipe->addAnalysisElement(p1);  
  m_pPipe->addAnalysisElement(p2);  
  m_pPipe->addAnalysisElement(pLast);

  CAnalysisPipeline::PipelineIterator p = m_pPipe->analysisFind("2");
  ASSERT(p != m_pPipe->analysisEnd()); // Findable
  EQ(p2, *p);			      // Found the right one.
}
/**
 * findnox 
 *
 *  Find for nonexistent element should give end:
 */
void
AnalysisPipelineTests::findnox()
{
  CEventProcessor* p1    = new NullEventProcessor("1");
  CEventProcessor* p2    = new NullEventProcessor("2");
  CEventProcessor* pLast = new NullEventProcessor("3");

  m_pPipe->addAnalysisElement(p1);  
  m_pPipe->addAnalysisElement(p2);  
  m_pPipe->addAnalysisElement(pLast);

  CAnalysisPipeline::PipelineIterator p  = m_pPipe->analysisFind("george");
  ASSERT( p == m_pPipe->analysisEnd());
}
/**
 * addbeforename
 *
 *  Add an element prior to a named element
 */
void
AnalysisPipelineTests::addbeforename()
{
  CEventProcessor* p1    = new NullEventProcessor("1");
  CEventProcessor* p2    = new NullEventProcessor("2");
  CEventProcessor* pLast = new NullEventProcessor("3");

  m_pPipe->addAnalysisElement(p1);  
  m_pPipe->addAnalysisElement(p2);  
  m_pPipe->addAnalysisElement(pLast, "2"); // Well not last anymore.

  // The order should be 1,3,2

  CAnalysisPipeline::PipelineIterator p = m_pPipe->analysisBegin();
  EQ(p1, *p);
  p++;
  EQ(pLast, *p);
  p++;
  EQ(p2, *p);
  p++;
  ASSERT(p == m_pPipe->analysisEnd());
}
/**
 * addbeforenox 
 *    Check for the right exception when adding before a nonexistent
 *    element.
 */
void
AnalysisPipelineTests::addbeforenox()
{
  CEventProcessor* p1    = new NullEventProcessor("1");
  CEventProcessor* p2    = new NullEventProcessor("2");
  CEventProcessor* pLast = new NullEventProcessor("3");

  m_pPipe->addAnalysisElement(p1);  
  m_pPipe->addAnalysisElement(p2);  

  bool threw = false;
  bool threwRight = false;
  std::string msg;

  try {
    m_pPipe->addAnalysisElement(pLast, "next-to-last");
  }
  catch(analysis_pipeline_exception& e) {
    threw = true;
    threwRight =- true;
    msg = e.what();
  }
  catch (...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(threwRight);
  EQ(std::string("analysis pipeline has no element named : 'next-to-last'"), msg);
}
/**
 * dispatch
 *   Ensure analysis pipeline elements also get dispatched data.
 */
class RecordingEventProcessor : public CEventProcessor
{
public:
  static std::vector<std::string> m_NameOrder;

  RecordingEventProcessor(const char* pName) : CEventProcessor(pName) 
  {
    m_NameOrder.clear();	// They get created in clumps so this is ok.
  }
  virtual bool onEvent(void* pEvent, size_t n, CAnalysisPipeline* pipe) {
    m_NameOrder.push_back(getName());
    return true;
  }
};
std::vector<std::string> RecordingEventProcessor::m_NameOrder;
class StatsDecoder : public CDecoder
{
public:
  int m_nEvents;
  int m_onData;
  int m_getItemSize;
  int m_getItemPointer;
  int m_next;

public:
  StatsDecoder(int nEvents = 1) :
    m_nEvents(nEvents),
    m_onData(0), m_getItemSize(0), m_getItemPointer(0), m_next(0) {}

  virtual void   onData(CDataSource* pSource) {m_onData++;}
  virtual size_t getItemSize()               {
    m_getItemSize++; 
    return sizeof(*this);
  }
  virtual void*  getItemPointer()            {
    m_getItemPointer++; 
    if (m_nEvents == 0) return 0; // EOF.
    m_nEvents--;
    return this;

  }
  virtual bool   next()                      {
    m_next++; 
    return m_nEvents != 0;
  }
};
class CNullDataSource : public CDataSource
{
public:
  virtual ~CNullDataSource() {}

  // Interface definition.

  virtual void    onAttach(CTCLInterpreter& interp, const char* pSource) {}
  virtual void    createEvent(CTCLInterpreter& interp, ChannelHandler* pHandler, void* pClientData) {}
  virtual size_t  read(void* pBuffer, size_t nBytes) { return 0;}
  virtual void    close() {}
  virtual bool    isEof() {return true;}
};

void
AnalysisPipelineTests::dispatch()
{  
  RecordingEventProcessor* p1 = new RecordingEventProcessor("first");
  RecordingEventProcessor* p2 = new RecordingEventProcessor("second");
  RecordingEventProcessor* p3 = new RecordingEventProcessor("last");

  m_pPipe->addAnalysisElement(p1);
  m_pPipe->addAnalysisElement(p2);
  m_pPipe->addAnalysisElement(p3);

  StatsDecoder decoder;
  m_pPipe->setDecoder(&decoder);
  CNullDataSource source;
  m_pPipe->setDataSource(&source);

  // Get an event processed:

  m_pPipe->sourceReadable(&source, m_pPipe);

  // All of the elements should have been processed in order 
  // recorded taht in m_NameOrder:

  EQ(static_cast<size_t>(3), RecordingEventProcessor::m_NameOrder.size());
  EQ(std::string("first"), RecordingEventProcessor::m_NameOrder[0]);  
  EQ(std::string("second"), RecordingEventProcessor::m_NameOrder[1]);  
  EQ(std::string("last"), RecordingEventProcessor::m_NameOrder[2]);


}
