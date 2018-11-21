// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#define private public
#include "CPipelineManager.h"
#undef private
#include "Asserts.h"
#include "EventProcessor.h"

#include <sstream>
#include <stdexcept>


class CAnalyzer;

class DummyProcessor : public CEventProcessor {
public:
  bool attached;
  DummyProcessor() : attached(0) {}
  Bool_t OnAttach(CAnalyzer& rA) {
    attached = true;
    return kfTRUE;
  }
  Bool_t OnDetach(CAnalyzer& rA) {
    attached = false;
    return kfFALSE;
  }
};

class PipeMgrTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(PipeMgrTests);
  CPPUNIT_TEST(initialName);
  CPPUNIT_TEST(registration_1);
  CPPUNIT_TEST(registration_2);
  CPPUNIT_TEST(registration_3);
  
  CPPUNIT_TEST(createpipe_1);
  CPPUNIT_TEST(createpipe_2);
  
  CPPUNIT_TEST(append_1);
  CPPUNIT_TEST(append_2);
  CPPUNIT_TEST(append_3);
  CPPUNIT_TEST(append_4);
  
  CPPUNIT_TEST(insert_1);
  CPPUNIT_TEST(insert_2);
  CPPUNIT_TEST(insert_3);
  CPPUNIT_TEST(insert_4);
  
  CPPUNIT_TEST(remove_1);
  CPPUNIT_TEST(remove_2);
  CPPUNIT_TEST(remove_3);   // 1-4 are name, name
  CPPUNIT_TEST(remove_4);
  CPPUNIT_TEST(remove_5);   // 5 on are remove by iterator.
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
    // Ensure the manager exists:
    
    CPipelineManager::getInstance();
  }
  void tearDown() {
    delete CPipelineManager::getInstance();
  }
protected:
  void initialName();
  void registration_1();
  void registration_2();
  void registration_3();
  
  void createpipe_1();
  void createpipe_2();
  
  void append_1();
  void append_2();
  void append_3();
  void append_4();
  
  void insert_1();
  void insert_2();
  void insert_3();
  void insert_4();
  
  void remove_1();
  void remove_2();
  void remove_3();
  void remove_4();
  void remove_5();
};

CPPUNIT_TEST_SUITE_REGISTRATION(PipeMgrTests);

void PipeMgrTests::initialName() {
  EQ(
     std::string("default"),
     CPipelineManager::getInstance()->getCurrentPipelineName()
  );
}

// Register 1.

void PipeMgrTests::registration_1()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  DummyProcessor* pDummy = new DummyProcessor;
  
  pMgr->registerEventProcessor("dummy", pDummy);
  std::vector<std::string> epNames = pMgr->getEventProcessorNames();
  
  EQ(size_t(1), epNames.size());
  EQ(std::string("dummy"), epNames[0]);
}

// Register a few of them:

void PipeMgrTests::registration_2()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  
  for (int i = 0; i < 10; i++) {
    std::stringstream name;
    name << "Dummy_" << i;
    pMgr->registerEventProcessor(name.str(), new DummyProcessor);
  }
  EQ(size_t(10), pMgr->eventProcessorCount());
  std::vector<std::string> names= pMgr->getEventProcessorNames();
  EQ(size_t(10), names.size());
}
/// Duplicate registration throws std::logic_error

void PipeMgrTests::registration_3()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  pMgr->registerEventProcessor("dummy", new DummyProcessor);
  CPPUNIT_ASSERT_THROW(
    pMgr->registerEventProcessor("dummy", new DummyProcessor),
    std::logic_error
  );
}
// Can create a new pipeline:

void PipeMgrTests::createpipe_1()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  pMgr->createPipeline("newPipe");
  
  // Should be two pipelines (don't forget default).
  // These tests rely on maps being sorted containers:
  
  EQ(size_t(2), pMgr->pipelineCount());
  std::vector<std::string> names = pMgr->getPipelineNames();
  EQ(size_t(2), names.size());
  EQ(std::string("newPipe"), names[1]);
  EQ(std::string("default"), names[0]);

}
// Duplicate pipe name is an error.

void PipeMgrTests::createpipe_2()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  pMgr->createPipeline("newPipe");
  CPPUNIT_ASSERT_THROW(
    pMgr->createPipeline("newPipe"),
    std::logic_error
  );
}
// Append one to non default pipeline - does not get attached.

void PipeMgrTests::append_1()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  pMgr->createPipeline("newPipe");
  
  DummyProcessor* pd;
  pMgr->registerEventProcessor("dummy", pd = new DummyProcessor);
  pMgr->appendEventProcessor("newPipe", "dummy");
  ASSERT(!pd->attached);
  
  std::vector<std::string> names = pMgr->getEventProcessorsInPipeline("newPipe");
  EQ(size_t(1), names.size());
  EQ(std::string("dummy"), names[0]);
  
  EQ(size_t(0), pMgr->getCurrentPipeline()->size());
}
// Append to current ("default") attaches:

void PipeMgrTests::append_2()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  DummyProcessor* pd;
  pMgr->registerEventProcessor("dummy", pd = new DummyProcessor);
  pMgr->appendEventProcessor("default", "dummy");
  ASSERT(pd->attached);
    
}
// Append to no such pipeline is a failure

void
PipeMgrTests::append_3()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  DummyProcessor* pd;
  pMgr->registerEventProcessor("dummy", pd = new DummyProcessor);
  CPPUNIT_ASSERT_THROW(
    pMgr->appendEventProcessor("newPipe", "dummy"),
    std::logic_error
  );
}
// Append nonexistent processor is a failure:

void
PipeMgrTests::append_4()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  CPPUNIT_ASSERT_THROW(
    pMgr->appendEventProcessor("default", "dummy"),
    std::logic_error
  );
}
// insert event processor at the front of a pipeline:

void
PipeMgrTests::insert_1()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  DummyProcessor* d1;
  DummyProcessor* d2;
  
  pMgr->registerEventProcessor("dummy1", d1 = new DummyProcessor);
  pMgr->registerEventProcessor("dummy2", d2 = new DummyProcessor);
  
  pMgr->appendEventProcessor("default", "dummy1");
  
  CTclAnalyzer::EventProcessorIterator b = pMgr->getCurrentPipeline()->begin();
  pMgr->insertEventProcessor("default", "dummy2", b);
  
  // Order should be dummy1, dummy2 both should be attached.
  
  std::vector<std::string> names = pMgr->getEventProcessorsInPipeline("default");
  EQ(size_t(2), names.size());
  EQ(std::string("dummy2"), names[0]);
  EQ(std::string("dummy1"), names[1]);
  ASSERT(d1->attached);
  ASSERT(d2->attached);
}
// Can use insert as an append:

void PipeMgrTests::insert_2()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  DummyProcessor* d1;
  DummyProcessor* d2;
  
  pMgr->registerEventProcessor("dummy1", d1 = new DummyProcessor);
  pMgr->registerEventProcessor("dummy2", d2 = new DummyProcessor);
  
  pMgr->appendEventProcessor("default", "dummy1");
  
  CTclAnalyzer::EventProcessorIterator b = pMgr->getCurrentPipeline()->end();
  pMgr->insertEventProcessor("default", "dummy2", b);

  std::vector<std::string> names = pMgr->getEventProcessorsInPipeline("default");
  EQ(size_t(2), names.size());
  EQ(std::string("dummy2"), names[1]);
  EQ(std::string("dummy1"), names[0]);
  ASSERT(d1->attached);
  ASSERT(d2->attached);

}
// insert fails on bad event processor name.

void PipeMgrTests::insert_3()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  DummyProcessor* d1;
  
  pMgr->registerEventProcessor("dummy1", d1 = new DummyProcessor);
  pMgr->appendEventProcessor("default", "dummy1");
  
  CTclAnalyzer::EventProcessorIterator b = pMgr->getCurrentPipeline()->end();
  
  CPPUNIT_ASSERT_THROW(
    pMgr->insertEventProcessor("default", "dummy2", b),
    std::logic_error
  );
  
}
// Insert with bad pipename errors:

void PipeMgrTests::insert_4()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  DummyProcessor* d1;
  
  pMgr->registerEventProcessor("dummy1", d1 = new DummyProcessor);
  pMgr->appendEventProcessor("default", "dummy1");
  
  CTclAnalyzer::EventProcessorIterator b = pMgr->getCurrentPipeline()->end();
  
  CPPUNIT_ASSERT_THROW(
    pMgr->insertEventProcessor("newPipe", "dummy1", b),
    std::logic_error
  );
}
// Remove -- note this detaches too.

void PipeMgrTests::remove_1()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  DummyProcessor* d1;
  
  pMgr->registerEventProcessor("dummy1", d1 = new DummyProcessor);
  pMgr->appendEventProcessor("default", "dummy1");
  pMgr->removeEventProcessor("default", "dummy1");
  
  ASSERT(!d1->attached);
  std::vector<std::string> names = pMgr->getEventProcessorsInPipeline("default");
  EQ(size_t(0), names.size());
  
  // Should still be registered though:
  
  EQ(size_t(1), pMgr->eventProcessorCount());
}
// Remove removes the right one.

void PipeMgrTests::remove_2()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  DummyProcessor* d1;
  DummyProcessor* d2;
  
  pMgr->registerEventProcessor("dummy1", d1 = new DummyProcessor);
  pMgr->registerEventProcessor("dummy2", d2 = new DummyProcessor);
  
  pMgr->appendEventProcessor("default", "dummy1");
  pMgr->appendEventProcessor("default", "dummy2");
  
  pMgr->removeEventProcessor("default", "dummy1"); // dummy 2 is left.
  std::vector<std::string> names = pMgr->getEventProcessorsInPipeline("default");
  
  
  ASSERT(!d1->attached);
  ASSERT(d2->attached);
  EQ(size_t(1), names.size());
  EQ(std::string("dummy2"), names[0]);
}
// Bad pipename for remove is an error.

void PipeMgrTests::remove_3()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  DummyProcessor* d1;
  DummyProcessor* d2;
  
  pMgr->registerEventProcessor("dummy1", d1 = new DummyProcessor);
  pMgr->registerEventProcessor("dummy2", d2 = new DummyProcessor);
  
  pMgr->appendEventProcessor("default", "dummy1");
  pMgr->appendEventProcessor("default", "dummy2");
  
  CPPUNIT_ASSERT_THROW(
    pMgr->removeEventProcessor("junk", "dummy1"),
    std::logic_error
  );  
}
// Bad processor name for remove is an error.

void PipeMgrTests::remove_4()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  DummyProcessor* d1;
  DummyProcessor* d2;
  
  pMgr->registerEventProcessor("dummy1", d1 = new DummyProcessor);
  pMgr->registerEventProcessor("dummy2", d2 = new DummyProcessor);
  
  pMgr->appendEventProcessor("default", "dummy1");
  pMgr->appendEventProcessor("default", "dummy2");
  
  CPPUNIT_ASSERT_THROW(
    pMgr->removeEventProcessor("default", "dummy3"),
    std::logic_error
  );
}
// Remove using begin iterator as position to remove from.

void PipeMgrTests::remove_5()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  DummyProcessor* d1;
  DummyProcessor* d2;
  
  pMgr->registerEventProcessor("dummy1", d1 = new DummyProcessor);
  pMgr->registerEventProcessor("dummy2", d2 = new DummyProcessor);
  
  pMgr->appendEventProcessor("default", "dummy1");
  pMgr->appendEventProcessor("default", "dummy2");
  
  pMgr->removeEventProcessor("default", pMgr->getCurrentPipeline()->begin());
  
  std::vector<std::string> names = pMgr->getEventProcessorsInPipeline("default");
  EQ(size_t(1), names.size());
  EQ(std::string("dummy2"), names[0]);
  ASSERT(!d1->attached);
  ASSERT(d2->attached);
}