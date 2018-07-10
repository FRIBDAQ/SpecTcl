// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include <Asserts.h>
#include <TCLInterpreter.h>
#define private public
#include "TCLAnalyzer.h"
#undef private
#include "CRingBufferDecoder.h"
#include "EventProcessor.h"
#include "Globals.h"

// Mostly we're worried about the changes -- the cases where
// we've removed event processors that reported failure.
//

// Here are some event processor test classes:

class FailAttach : public CEventProcessor
{
public:
    virtual Bool_t OnAttach(CAnalyzer& a) {
        return kfFALSE;
    }
};

class FailInit : public CEventProcessor
{
public:
    virtual Bool_t OnInitialize() { return kfFALSE; }
};

// The testsuite.

class FailEvpTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(FailEvpTests);
  CPPUNIT_TEST(attachfail);
  CPPUNIT_TEST(initfail);
  CPPUNIT_TEST(oninit1);
  CPPUNIT_TEST(oninit2);
  CPPUNIT_TEST(oninit3);
  CPPUNIT_TEST_SUITE_END();


private:
    CTCLInterpreter* m_pInterp;
    CBufferDecoder*  m_pDecoder;
    CTclAnalyzer*    m_pAnalyzer;
    
public:
  void setUp() {
    m_pInterp = new CTCLInterpreter;
    m_pDecoder = static_cast<CBufferDecoder*>(new CRingBufferDecoder);
    m_pAnalyzer = new CTclAnalyzer(*m_pInterp, 1, 1);
    gpAnalyzer = m_pAnalyzer;
    m_pAnalyzer->m_initialized = true;
  }
  void tearDown() {
    delete m_pAnalyzer;
    delete m_pDecoder;
    delete m_pInterp;
    gpAnalyzer = nullptr;
  }
protected:
  void attachfail();
  void initfail();
  void oninit1();
  void oninit2();
  void oninit3();
};

CPPUNIT_TEST_SUITE_REGISTRATION(FailEvpTests);


// Failing an attach means that the event processor won't be in the
// pipline.

void FailEvpTests::attachfail() {
    

    FailAttach ep;
    m_pAnalyzer->AddEventProcessor("Default", ep, "Failure");
    EQ(UInt_t(0), m_pAnalyzer->size("Default"));
}

// Failing init means adding the event processor won't add.

void FailEvpTests::initfail() {
    
    FailInit ep;
    m_pAnalyzer->AddEventProcessor("Default", ep, "Failure");
    
    EQ(UInt_t(0), m_pAnalyzer->size("Default"));
}
// OnInitialize needs a few tests because it needs to keep
// track of an remove only the failing event processors.
// we also need to register with m_initialized false.

// Adding a single element that fails OnInit after OnInitialize
// the element is gone.

void FailEvpTests::oninit1()
{
    
    FailInit ep;
    m_pAnalyzer->m_initialized = false;  // not initted yet.
    
    m_pAnalyzer->AddEventProcessor("Default", ep, "Failure");
    
    m_pAnalyzer->OnInitialize();
    EQ(UInt_t(0), m_pAnalyzer->size("Default"));
}
// Let's be sure that adding a good one 'sticks'.

void FailEvpTests::oninit2()
{
    
    CEventProcessor ep;
    m_pAnalyzer->m_initialized = false;  // not initted yet.
    
    m_pAnalyzer->AddEventProcessor("Default", ep, "Success");
    
    m_pAnalyzer->OnInitialize();
    EQ(UInt_t(1), m_pAnalyzer->size("Default"));
}

// If one fails but the other succeeds, the suceeding one
// should be there.
void FailEvpTests::oninit3()
{
    FailInit        ep1;
    CEventProcessor ep2;
    m_pAnalyzer->m_initialized = false;
    
    m_pAnalyzer->AddEventProcessor("Default", ep1, "Failed");
    m_pAnalyzer->AddEventProcessor("Default", ep2, "success");
    
    m_pAnalyzer->OnInitialize();
    
    EQ(UInt_t(1), m_pAnalyzer->size("Default"));
    
    // Should find the sucess one:
    
    CTclAnalyzer::EventProcessorIterator p =
        m_pAnalyzer->FindEventProcessor("Default", "success");
    ASSERT(p != m_pAnalyzer->end("Default"));
    
}