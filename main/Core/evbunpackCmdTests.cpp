// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#define private public                // Not sure I really need this.
#include "CUnpackEvbCommand.h"
#include "CPipelineManager.h"
#include "CEventBuilderEventProcessor.h"  // Not sure I need this either.
#undef private
#include "EventProcessor.h"


#include <Asserts.h>
#include <TCLInterpreter.h>
#include <TCLException.h>
#include <Histogrammer.h>
#include <Globals.h>


class evbunpackCmdTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(evbunpackCmdTests);
  CPPUNIT_TEST(create_1);              // Processor properly created.
  CPPUNIT_TEST(create_2);              // Existing pipe name
  CPPUNIT_TEST(create_3);              // Too few parameters.
  CPPUNIT_TEST(create_4);              // Too many parameters.
  
  CPPUNIT_TEST(add_1);                 // Add pipeline to evp.
  CPPUNIT_TEST(add_2);              // Add to nox evp.
  CPPUNIT_TEST(add_3);              // add of nox pipe.
  CPPUNIT_TEST(add_4);              // Add when sid already has a pipe.
  CPPUNIT_TEST(add_5);              // Too few parameters.
  CPPUNIT_TEST(add_6);              // Too many parameters.
  CPPUNIT_TEST(add_7);              // non integer source id.
  
  CPPUNIT_TEST(list_0);             // List when nothing made.
  CPPUNIT_TEST(list_1);             // List when we've made some.
  CPPUNIT_TEST(list_2);             // List with filter pattern.
  CPPUNIT_TEST(list_3);             // List with too many parameters.
  CPPUNIT_TEST_SUITE_END();


private:
  CPipelineManager*  m_pMgr;
  CTCLInterpreter*   m_pInterp;
  CUnpackEvbCommand* m_pCmd;
  CHistogrammer*     m_pHistogrammer;
public:
  void setUp() {
    m_pMgr = CPipelineManager::getInstance();
    m_pInterp = new CTCLInterpreter;
    m_pCmd    = new CUnpackEvbCommand(*m_pInterp);
    m_pHistogrammer = new CHistogrammer;
    gpEventSink     = m_pHistogrammer;
  }
  void tearDown() {

    delete CPipelineManager::m_pInstance;
    CPipelineManager::m_pInstance == 0;
    
    delete m_pCmd;
    delete m_pHistogrammer;
    gpEventSink = nullptr;
    delete m_pInterp;
    
  }
protected:
  void create_1();
  void create_2();
  void create_3();
  void create_4();
  
  void add_1();
  void add_2();
  void add_3();
  void add_4();
  void add_5();
  void add_6();
  void add_7();
  
  void list_0();
  void list_1();
  void list_2();
  void list_3();
private:
  void makeSomePipes();
  std::string lindex(CTCLObject& objv, int index);
  void lindex(CTCLObject& result, CTCLObject& objv, int index);
  std::vector<std::string> list2vec(CTCLObject& objv);

};


void evbunpackCmdTests::makeSomePipes()
{
  m_pMgr->createPipeline("pipe1");
  m_pMgr->createPipeline("pipe2");
}

std::string
evbunpackCmdTests::lindex(CTCLObject& obj, int index)
{
  // It's assumed obj is bound.

  CTCLObject objResult; objResult.Bind(*m_pInterp);
  objResult = obj.lindex(index);
  return std::string(objResult);
}

void
evbunpackCmdTests::lindex(CTCLObject& result, CTCLObject& obj, int index)
{
  result.Bind(*m_pInterp);
  result = obj.lindex(index);
}

std::vector<std::string>
evbunpackCmdTests::list2vec(CTCLObject& objv)
{
  int size = objv.llength();
  std::vector<std::string> result;
  for (int i =0; i < size; i++) {
    result.push_back(lindex(objv, i));
  }

  return result;
}



CPPUNIT_TEST_SUITE_REGISTRATION(evbunpackCmdTests);

// If I create an event processor it should wind up in the registry.

void evbunpackCmdTests::create_1() {
  CPPUNIT_ASSERT_NO_THROW(
    m_pInterp->GlobalEval("evbunpack create test 10.0 testing")
  );
  
  std::vector<std::string> evpnames = m_pMgr->getEventProcessorNames();
  EQ(size_t(1), evpnames.size());
  EQ(std::string("test"), evpnames[0]);
  
  // Now check this is known to the command:
  
  EQ(size_t(1), m_pCmd->m_Processors.count("test"));
  CEventBuilderEventProcessor* p = m_pCmd->m_Processors["test"];
  
  EQ(double(10.0), p->m_ClockMHz);
  EQ(std::string("testing"), p->m_baseName);
  
  
}

// Can't duplicate an existing event processor.

void evbunpackCmdTests::create_2()
{
  m_pMgr->registerEventProcessor("test", new CEventProcessor);
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("evbunpack create test 10.0 testing"),
    CTCLException
  );
}
// Missing a parameter

void evbunpackCmdTests::create_3()
{

  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("evbunpack create test 10.0"),
    CTCLException
  );  
}
// too many parameters:

void evbunpackCmdTests::create_4()
{

  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("evbunpack create test 10.0 testing junk"),
    CTCLException
  );  
}
// Successful add of a pipeline to one of our event processors.

void evbunpackCmdTests::add_1()
{
  makeSomePipes();
  m_pInterp->GlobalEval("evbunpack create test 10.0 test");
  CPPUNIT_ASSERT_NO_THROW(
    m_pInterp->GlobalEval("evbunpack addprocessor test 1 pipe1")
  );
  
  CEventBuilderEventProcessor* pProc = m_pCmd->m_Processors["test"];
  
  ASSERT(pProc->haveEventProcessor(1));
}
// Add pipe to no such event processor is an error:

void evbunpackCmdTests::add_2()
{
  makeSomePipes();
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("evbunpack addprocessor test 1 pipe1"),
    CTCLException
  );
}
// Add nonexistent pipe to existing event prosessor.

void evbunpackCmdTests::add_3()
{
  m_pInterp->GlobalEval("evbunpack create test 10.0 test");
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("evbunpack addprocessor test 1 pipe1"),
    CTCLException
  );  
}
// Add when the sid already has a processor is an error:

void evbunpackCmdTests::add_4()
{
  makeSomePipes();
  m_pInterp->GlobalEval("evbunpack create test 10.0 test");
  CPPUNIT_ASSERT_NO_THROW(
    m_pInterp->GlobalEval("evbunpack addprocessor test 1 pipe1")
  );
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("evbunpack addprocessor test 1 pipe2"),
    CTCLException
  );
}
// Too few command parameters:

void evbunpackCmdTests::add_5()
{
  m_pInterp->GlobalEval("evbunpack create test 10.0 test");
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("evbunpack addprocessor test 1"),
    CTCLException
  );
}
// Too many command parameters

void evbunpackCmdTests::add_6()
{
  makeSomePipes();
  m_pInterp->GlobalEval("evbunpack create test 10.0 test");
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("evbunpack addprocessor test 1 pipe1 junk"),
    CTCLException
  );
}
// Invalid source id.

void evbunpackCmdTests::add_7()
{
  makeSomePipes();
  m_pInterp->GlobalEval("evbunpack create test 10.0 test");
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("evbunpack addprocessor test 3.1416 pipe1"),
    CTCLException
  );
}
// Initial list produces an empty list

void evbunpackCmdTests::list_0()
{
  CTCLObject result;
  result.Bind(*m_pInterp);
  
  result = m_pInterp->GlobalEval("evbunpack list");
  EQ(0, result.llength());
}

// list when we've made some event processors:

void evbunpackCmdTests::list_1()
{
  m_pInterp->GlobalEval("evbunpack create test 10.0 test");
  m_pInterp->GlobalEval("evbunpack create btest 10.0 btest");
  m_pInterp->GlobalEval("evbunpack create atest 10.0 atest");
  
  CTCLObject result;
  result.Bind(*m_pInterp);
  
  result = m_pInterp->GlobalEval("evbunpack list");
  EQ(3, result.llength());
  std::vector<std::string> names = list2vec(result);
  std::vector<std::string> sb = {"atest", "btest", "test"};
  for (int i = 0; i < sb.size(); i++) {
    EQ(sb[i], names[i]);
  }
}
// List but using a filter pattern to restrict the result:

void evbunpackCmdTests::list_2()
{
  m_pInterp->GlobalEval("evbunpack create test 10.0 test");
  m_pInterp->GlobalEval("evbunpack create btest 10.0 btest");
  m_pInterp->GlobalEval("evbunpack create atest 10.0 atest");
  
  CTCLObject result;
  result.Bind(*m_pInterp);
  
  result = m_pInterp->GlobalEval("evbunpack list a*");
  EQ(1, result.llength());
  EQ(std::string("atest"), lindex(result, 0));
}

// Too many parameters means an error.

void evbunpackCmdTests::list_3()
{
  m_pInterp->GlobalEval("evbunpack create test 10.0 test");
  m_pInterp->GlobalEval("evbunpack create btest 10.0 btest");
  m_pInterp->GlobalEval("evbunpack create atest 10.0 atest");
  
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("evbunpack list a* junk"),
    CTCLException
  );
}