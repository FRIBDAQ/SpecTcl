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
  
  CPPUNIT_TEST(setpipe_1);
  CPPUNIT_TEST(setpipe_2);
  
  CPPUNIT_TEST(clone_1);
  CPPUNIT_TEST(clone_2);
  CPPUNIT_TEST(clone_3);
  
  CPPUNIT_TEST(getprocessors_1);
  CPPUNIT_TEST(getprocessors_2);
  
  CPPUNIT_TEST(lookup_1);
  CPPUNIT_TEST(lookup_2);
  
  CPPUNIT_TEST(pliterate_1);
  CPPUNIT_TEST(pliterate_2);
  
  CPPUNIT_TEST(evpiterate_1);
  CPPUNIT_TEST(evpiterate_2);
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
  
  void setpipe_1();
  void setpipe_2();
  
  void clone_1();
  void clone_2();
  void clone_3();
  
  void getprocessors_1();
  void getprocessors_2();
  
  void lookup_1();
  void lookup_2();
  
  void pliterate_1();
  void pliterate_2();
  
  void evpiterate_1();
  void evpiterate_2();
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
/// Duplicate registration throwstd::logic_error

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
// Set the pipe success:

void PipeMgrTests::setpipe_1()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  
  pMgr->createPipeline("mypipe");
  pMgr->setCurrentPipeline("mypipe");
  
  EQ(std::string("mypipe"), pMgr->getCurrentPipelineName());
  CTclAnalyzer::EventProcessingPipeline* pipe =pMgr->m_pipelines["mypipe"];
  EQ(pipe, pMgr->getCurrentPipeline());
  
}
// Setting nonexistnent pipe is an error.

void PipeMgrTests::setpipe_2()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();

  
  CPPUNIT_ASSERT_THROW(
    pMgr->setCurrentPipeline("mypipe"),
    std::logic_error
  );
}
// Clone successful:

void PipeMgrTests::clone_1()
{
  DummyProcessor* d1;
  DummyProcessor* d2;
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  pMgr->registerEventProcessor("dummy1", d1 = new DummyProcessor);
  pMgr->registerEventProcessor("dummy2", d2 = new DummyProcessor);
  
  // add these processors to default:
  
  pMgr->appendEventProcessor("default", "dummy1");
  pMgr->appendEventProcessor("default", "dummy2");
  
  // Make a clone:
  
  pMgr->clonePipeline("default", "mypipe");
  
  CPPUNIT_ASSERT_NO_THROW(
    pMgr->setCurrentPipeline("mypipe")
  );
  
  CTclAnalyzer::EventProcessingPipeline* pPipe = pMgr->getCurrentPipeline();
  CTclAnalyzer::PipelineElement  e = pPipe->front();
  EQ(std::string("dummy1"), e.first);
  EQ((void*)d1, (void*)e.second);
  
  e = pPipe->back();             // Since it's a two element pipe.
  EQ(std::string("dummy2"), e.first);
  EQ((void*)d2, (void*)e.second);
}
// Nonexistent source pipe:

void PipeMgrTests::clone_2()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  CPPUNIT_ASSERT_THROW(
    pMgr->clonePipeline("nosuch", "a"),
    std::logic_error
  );
}
// Duplicate destination pipe:

void PipeMgrTests::clone_3()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  pMgr->createPipeline("new");
  CPPUNIT_ASSERT_THROW(
    pMgr->clonePipeline("default", "new"),
    std::logic_error
  );
}
// Initially there are no processors

void PipeMgrTests::getprocessors_1()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  std::vector<std::string> prcs = pMgr->getEventProcessorNames();
  EQ(size_t(0), prcs.size());
}
// If we register processors, then we can get their names.

void PipeMgrTests::getprocessors_2()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  pMgr->registerEventProcessor("prc1", new DummyProcessor);
  pMgr->registerEventProcessor("prc2", new DummyProcessor);
  
  std::vector<std::string> ps = pMgr->getEventProcessorNames();
  EQ(size_t(2), ps.size());
  EQ(std::string("prc1"), ps[0]);
  EQ(std::string("prc2"), ps[1]);
}
// Lookup found:

void PipeMgrTests::lookup_1()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  CEventProcessor* p1;
  CEventProcessor* p2;
  pMgr->registerEventProcessor("prc1", p1 = new DummyProcessor);
  pMgr->registerEventProcessor("prc2", p2 = new DummyProcessor);
  
  std::string name1 = pMgr->lookupEventProcessor(p1);
  std::string name2 = pMgr->lookupEventProcessor(p2);
  
  EQ(std::string("prc1"), name1);
  EQ(std::string("prc2"), name2);
}
// lookup failed:

void PipeMgrTests::lookup_2()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  CEventProcessor* p = new DummyProcessor;
  CPPUNIT_ASSERT_THROW(
    pMgr->lookupEventProcessor(p),
    std::logic_error
  );
}
// Initially pipline iteration can get you a single pipeline.

void PipeMgrTests::pliterate_1()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  auto p = pMgr->pipelineBegin();
  
  EQ(std::string("default"), p->first);
  
  p++;
  ASSERT(p == pMgr->pipelineEnd());
  
}

// Put a few other pipelines in the mix. Iteration is alpha order.

void PipeMgrTests::pliterate_2()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  pMgr->createPipeline("a");
  pMgr->createPipeline("b");
  
  // We have pipelines "a", "b", "default" in that order.
  
  auto p = pMgr->pipelineBegin();
  EQ(std::string("a"), p->first);
  p++;
  EQ(std::string("b"), p->first);
  p++;
  EQ(std::string("default"), p->first);
  p++;
  ASSERT(p == pMgr->pipelineEnd());
}
// Initially empty pipeline:

void PipeMgrTests::evpiterate_1()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  ASSERT(pMgr->processorsBegin() == pMgr->processorsEnd());
}
// Add a few event processors - they should iterate in alpha order.

void PipeMgrTests::evpiterate_2()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  
  pMgr->registerEventProcessor("aaa", new DummyProcessor); // first
  pMgr->registerEventProcessor("ddd", new DummyProcessor); // third
  pMgr->registerEventProcessor("b", new DummyProcessor);   // second.
  
  auto p = pMgr->processorsBegin();
  EQ(std::string("aaa"), p->first);
  p++;
  EQ(std::string("b"), p->first);
  p++;
  EQ(std::string("ddd"), p->first);
  p++;
  ASSERT(p == pMgr->processorsEnd());
}