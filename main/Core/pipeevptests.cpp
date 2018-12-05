// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "CPipelineEventProcessor.h"
#include "Analyzer.h"
#include "TCLAnalyzer.h"
#include "NSCLBufferDecoder.h"
#include "EventProcessor.h"
#include "Event.h"

#include <Asserts.h>

// These are two event processor classes that can be put into a
// pipeline.  The first one succeeds and notes that it was called.
// the second one fails but notes it was called:

class SucceedingEventProcessor : public CEventProcessor
{
public:
  bool   called;
public:
  SucceedingEventProcessor() : called(false) {}
  virtual Bool_t operator()(const Address_t pEvent,
                            CEvent& rEvent,
                            CAnalyzer& rAnalyzer,
                            CBufferDecoder& rDecoder) {
    called = true;
    return kfTRUE;
  }
  Bool_t OnAttach(CAnalyzer& rAnalyzer) {
    called = true;
    return kfTRUE;
  }
  Bool_t OnDetach(CAnalyzer& rAnalyzer) {
    called = true;
    return kfTRUE;
  }
   Bool_t OnBegin(CAnalyzer& rAnalyzer, CBufferDecoder& dec) {
    called = true;
    return kfTRUE;
  }
  Bool_t OnEnd(CAnalyzer& rAnalyzer, CBufferDecoder& dec) {
    called = true;
    return kfTRUE;
  }
  Bool_t OnPause(CAnalyzer& rAnalyzer, CBufferDecoder& dec) {
    called = true;
    return kfTRUE;
  }
  Bool_t OnResume(CAnalyzer& rAnalyzer, CBufferDecoder& dec) {
    called = true;
    return kfTRUE;
  }
  Bool_t OnOther(UInt_t t, CAnalyzer& rAnalyzer, CBufferDecoder& dec) {
    called = true;
    return kfTRUE;
  }
  Bool_t OnEventSourceOpen(std::string name) {
    called = true;
    return kfTRUE;
  }
  Bool_t OnEventSourceEOF()  {
    called = true;
    return kfTRUE;
  }
  Bool_t OnInitialize()  {
    called = true;
    return kfTRUE;
  }
};

class FailingEventProcessor : public CEventProcessor
{
public:
  bool called;
public:
  FailingEventProcessor() : called(false) {}
  virtual Bool_t operator()(const Address_t pEvent,
                            CEvent& rEvent,
                            CAnalyzer& rAnalyzer,
                            CBufferDecoder& rDecoder) {
    called = true;
    return kfFALSE;
  }
  Bool_t OnAttach(CAnalyzer& rAnalyzer) {
    called = true;
    return kfFALSE;
  }
  Bool_t OnDetach(CAnalyzer& rAnalyzer) {
    called = true;
    return kfFALSE;
  }
  Bool_t OnBegin(CAnalyzer& rAnalyzer, CBufferDecoder& dec) {
    called = true;
    return kfFALSE;
  }
  Bool_t OnEnd(CAnalyzer& rAnalyzer, CBufferDecoder& dec) {
    called = true;
    return kfFALSE;
  }
  Bool_t OnPause(CAnalyzer& rAnalyzer, CBufferDecoder& dec) {
    called = true;
    return kfFALSE;
  }
  Bool_t OnResume(CAnalyzer& rAnalyzer, CBufferDecoder& dec) {
    called = true;
    return kfFALSE;
  }
  Bool_t OnOther(UInt_t t, CAnalyzer& rAnalyzer, CBufferDecoder& dec) {
    called = true;
    return kfFALSE;
  }
  Bool_t OnEventSourceOpen(std::string name) {
    called = true;
    return kfFALSE;
  }
  Bool_t OnEventSourceEOF() {
    called = true;
    return kfFALSE;
  }
  Bool_t OnInitialize() {
    called = true;
    return kfFALSE;
  }
};

class pipeEvpTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(pipeEvpTests);
  CPPUNIT_TEST(event_1);
  CPPUNIT_TEST(event_2);
  
  CPPUNIT_TEST(attach_1);
  CPPUNIT_TEST(attach_2);
  
  CPPUNIT_TEST(detach_1);
  CPPUNIT_TEST(detach_2);
  
  CPPUNIT_TEST(begin_1);
  CPPUNIT_TEST(begin_2);
  
  CPPUNIT_TEST(end_1);
  CPPUNIT_TEST(end_2);
  
  CPPUNIT_TEST(pause_1);
  CPPUNIT_TEST(pause_2);
  
  CPPUNIT_TEST(resume_1);
  CPPUNIT_TEST(resume_2);
  
  CPPUNIT_TEST(other_1);
  CPPUNIT_TEST(other_2);
  
  CPPUNIT_TEST(open_1);
  CPPUNIT_TEST(open_2);
  
  CPPUNIT_TEST(eof_1);
  CPPUNIT_TEST(eof_2);
  
  CPPUNIT_TEST(init_1);
  CPPUNIT_TEST(init_2);
  CPPUNIT_TEST_SUITE_END();


private:
  CAnalyzer*      m_pAnalyzer;
  CBufferDecoder* m_pDecoder;
  CEvent*         m_pEvent;
public:
  void setUp() {
    m_pAnalyzer = new CAnalyzer;
    m_pDecoder  = new CNSCLBufferDecoder;
    m_pEvent    = new CEvent;
  }
  void tearDown() {
    delete m_pEvent;
    delete m_pDecoder;
    delete m_pAnalyzer;
  }
private:
  void makeGoodPipe(CTclAnalyzer::EventProcessingPipeline& pipe);
  void makeBadPipe(CTclAnalyzer::EventProcessingPipeline& pipe);
protected:
  void event_1();
  void event_2();
  
  void attach_1();
  void attach_2();
  
  void detach_1();
  void detach_2();
  
  void begin_1();
  void begin_2();
    
  void end_1();
  void end_2();
    
  void pause_1();
  void pause_2();
    
  void resume_1();
  void resume_2();
  
  void other_1();
  void other_2();
  
  void open_1();
  void open_2();
  
  void eof_1();
  void eof_2();
  
  void init_1();
  void init_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(pipeEvpTests);


static  SucceedingEventProcessor evp1;
static  SucceedingEventProcessor evp2;
static  SucceedingEventProcessor evp3;
static  SucceedingEventProcessor evp4;
static  FailingEventProcessor    evp5;

void
pipeEvpTests::makeGoodPipe(CTclAnalyzer::EventProcessingPipeline& pipe)
{
  pipe.clear();
  evp1.called = false;
  evp2.called = false;
  evp3.called = false;
  evp4.called = false;

  pipe.push_back({"evp1", &evp1});
  pipe.push_back({"evp2", &evp2});
  pipe.push_back({"evp3", &evp3});
  pipe.push_back({"evp4", &evp4});

}

void
pipeEvpTests::makeBadPipe(CTclAnalyzer::EventProcessingPipeline& pipe)
{
  pipe.clear();
  evp1.called = false;
  evp5.called = false;
  evp3.called = false;
  evp4.called = false;
  
  pipe.push_back({"evp1", &evp1});
  pipe.push_back({"evp5", &evp5});
  pipe.push_back({"evp3", &evp3});
  pipe.push_back({"evp4", &evp4});
}

// Successful execution of event processing.

void pipeEvpTests::event_1() {
  // Build the pipeline:
  
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeGoodPipe(pipe);  
  
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(evp(nullptr, *m_pEvent, *m_pAnalyzer, *m_pDecoder));
  ASSERT(evp1.called);
  ASSERT(evp2.called);
  ASSERT(evp3.called);
  ASSERT(evp4.called);
}

// Failing execution of event processing:

void pipeEvpTests::event_2()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeBadPipe(pipe);
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(!evp(nullptr, *m_pEvent, *m_pAnalyzer, *m_pDecoder));
  ASSERT(evp1.called);
  ASSERT(evp5.called);
  ASSERT(!evp3.called);
  ASSERT(!evp4.called);
  
}

// successful execution of on attache:

void pipeEvpTests::attach_1()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeGoodPipe(pipe);  
  
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(evp.OnAttach(*m_pAnalyzer));
  ASSERT(evp1.called);
  ASSERT(evp2.called);
  ASSERT(evp3.called);
  ASSERT(evp4.called);
}

// Failed attach.

void pipeEvpTests::attach_2()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeBadPipe(pipe);
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(!evp.OnAttach(*m_pAnalyzer));
  ASSERT(evp1.called);
  ASSERT(evp5.called);
  ASSERT(!evp3.called);
  ASSERT(!evp4.called);
}

// Succesful detach:
void pipeEvpTests::detach_1()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeGoodPipe(pipe);  
  
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(evp.OnDetach(*m_pAnalyzer));
  ASSERT(evp1.called);
  ASSERT(evp2.called);
  ASSERT(evp3.called);
  ASSERT(evp4.called);
}



// failed detach

void pipeEvpTests::detach_2()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeBadPipe(pipe);
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(!evp.OnDetach(*m_pAnalyzer));
  ASSERT(evp1.called);
  ASSERT(evp5.called);
  ASSERT(!evp3.called);
  ASSERT(!evp4.called);
}


// Succesful begin:

void pipeEvpTests::begin_1()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeGoodPipe(pipe);  
  
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(evp.OnBegin(*m_pAnalyzer, *m_pDecoder));
  ASSERT(evp1.called);
  ASSERT(evp2.called);
  ASSERT(evp3.called);
  ASSERT(evp4.called);  
}

// Failed begin:

void pipeEvpTests::begin_2()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeBadPipe(pipe);
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(!evp.OnBegin(*m_pAnalyzer, *m_pDecoder));
  ASSERT(evp1.called);
  ASSERT(evp5.called);
  ASSERT(!evp3.called);
  ASSERT(!evp4.called); 
}


// Succesful end

void pipeEvpTests::end_1()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeGoodPipe(pipe);  
  
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(evp.OnEnd(*m_pAnalyzer, *m_pDecoder));
  ASSERT(evp1.called);
  ASSERT(evp2.called);
  ASSERT(evp3.called);
  ASSERT(evp4.called);  
}

// Failed end:

void pipeEvpTests::end_2()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeBadPipe(pipe);
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(!evp.OnEnd(*m_pAnalyzer, *m_pDecoder));
  ASSERT(evp1.called);
  ASSERT(evp5.called);
  ASSERT(!evp3.called);
  ASSERT(!evp4.called); 
}


// Succesful pause

void pipeEvpTests::pause_1()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeGoodPipe(pipe);  
  
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(evp.OnPause(*m_pAnalyzer, *m_pDecoder));
  ASSERT(evp1.called);
  ASSERT(evp2.called);
  ASSERT(evp3.called);
  ASSERT(evp4.called);  
}

// Failed pause:

void pipeEvpTests::pause_2()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeBadPipe(pipe);
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(!evp.OnPause(*m_pAnalyzer, *m_pDecoder));
  ASSERT(evp1.called);
  ASSERT(evp5.called);
  ASSERT(!evp3.called);
  ASSERT(!evp4.called); 
}


// Succesful resume:

void pipeEvpTests::resume_1()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeGoodPipe(pipe);  
  
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(evp.OnResume(*m_pAnalyzer, *m_pDecoder));
  ASSERT(evp1.called);
  ASSERT(evp2.called);
  ASSERT(evp3.called);
  ASSERT(evp4.called);  
}

// Failed resume:

void pipeEvpTests::resume_2()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeBadPipe(pipe);
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(!evp.OnResume(*m_pAnalyzer, *m_pDecoder));
  ASSERT(evp1.called);
  ASSERT(evp5.called);
  ASSERT(!evp3.called);
  ASSERT(!evp4.called); 
}

// Succesful onother

void pipeEvpTests::other_1()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeGoodPipe(pipe);  
  
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(evp.OnOther(1, *m_pAnalyzer, *m_pDecoder));
  ASSERT(evp1.called);
  ASSERT(evp2.called);
  ASSERT(evp3.called);
  ASSERT(evp4.called);  
}

// Failed onother

void pipeEvpTests::other_2()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeBadPipe(pipe);
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(!evp.OnOther(1, *m_pAnalyzer, *m_pDecoder));
  ASSERT(evp1.called);
  ASSERT(evp5.called);
  ASSERT(!evp3.called);
  ASSERT(!evp4.called); 
}
// Succesful open

void pipeEvpTests::open_1()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeGoodPipe(pipe);  
  
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(evp.OnEventSourceOpen("testing"));
  ASSERT(evp1.called);
  ASSERT(evp2.called);
  ASSERT(evp3.called);
  ASSERT(evp4.called);  
}

// Failed open

void pipeEvpTests::open_2()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeBadPipe(pipe);
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(!evp.OnEventSourceOpen("testing"));
  ASSERT(evp1.called);
  ASSERT(evp5.called);
  ASSERT(!evp3.called);
  ASSERT(!evp4.called); 
}

// Succesful EOF

void pipeEvpTests::eof_1()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeGoodPipe(pipe);  
  
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(evp.OnEventSourceEOF());
  ASSERT(evp1.called);
  ASSERT(evp2.called);
  ASSERT(evp3.called);
  ASSERT(evp4.called);  
}

// Failed eof

void pipeEvpTests::eof_2()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeBadPipe(pipe);
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(!evp.OnEventSourceEOF());
  ASSERT(evp1.called);
  ASSERT(evp5.called);
  ASSERT(!evp3.called);
  ASSERT(!evp4.called); 
}


// Succesful init

void pipeEvpTests::init_1()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeGoodPipe(pipe);  
  
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(evp.OnInitialize());
  ASSERT(evp1.called);
  ASSERT(evp2.called);
  ASSERT(evp3.called);
  ASSERT(evp4.called);  
}

// Failed init

void pipeEvpTests::init_2()
{
  CTclAnalyzer::EventProcessingPipeline pipe;
  makeBadPipe(pipe);
  CPipelineEventProcessor evp(&pipe);
  
  ASSERT(!evp.OnInitialize());
  ASSERT(evp1.called);
  ASSERT(evp5.called);
  ASSERT(!evp3.called);
  ASSERT(!evp4.called); 
}