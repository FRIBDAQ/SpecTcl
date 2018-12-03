// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <EventProcessor.h>
#define private public
#include "CPipelineManager.h"
#undef private
#include "CPipelineCommand.h"

#include <TCLInterpreter.h>
#include <TCLException.h>
#include <TCLObject.h>
#include <string>




class pcmdTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(pcmdTests);
  CPPUNIT_TEST(mk_1);
  CPPUNIT_TEST(mk_2);
  CPPUNIT_TEST(mk_3);
  CPPUNIT_TEST(mk_4);
  
  CPPUNIT_TEST(ls_1);
  CPPUNIT_TEST(ls_2);
  CPPUNIT_TEST(ls_3);
  
  CPPUNIT_TEST(add_1);     // add one to a default.
  CPPUNIT_TEST(add_2);     // Add a few sprinked around.
  CPPUNIT_TEST(add_3);     // Too few parameters.
  CPPUNIT_TEST(add_4);     // Too many parameters.
  CPPUNIT_TEST(add_5);     // Bad pipeline name.
  CPPUNIT_TEST(add_6);     // bad event processor name.
  
  CPPUNIT_TEST(current_1);    // List empty pipeline.
  CPPUNIT_TEST(current_2);    // List pipeline with stuff.
  CPPUNIT_TEST(current_3);    // too many parameters.
  
  CPPUNIT_TEST(lall_1);      // list-all with no processors added.
  CPPUNIT_TEST(lall_2);      // list-all with evps added to default.
  CPPUNIT_TEST(lall_3);      // list-all with evps added to several pipes.
  CPPUNIT_TEST(lall_4);     // list-all with matching pattern.
  CPPUNIT_TEST(lall_5);     // Too many command parameters.
  
  CPPUNIT_TEST(use_1);      // Use a valid pipeline.
  CPPUNIT_TEST(use_2);      // Use with not enough parameters.
  CPPUNIT_TEST(use_3);      // Use undefined pipeline name.
  CPPUNIT_TEST(use_4);      // too many parameters.

  CPPUNIT_TEST(rm_1);       // Successful remove.
  CPPUNIT_TEST(rm_2);       // Too many parameters.
  CPPUNIT_TEST(rm_3);       // Too few parameters.
  CPPUNIT_TEST(rm_4);       // invalid pipe name.
  CPPUNIT_TEST(rm_5);       // EVP not in pipeline - covers nonex evp as well.
  
  CPPUNIT_TEST(clr_1);      // Clear full pipeline.
  CPPUNIT_TEST(clr_2);      // Clear empty pipeline.
  CPPUNIT_TEST(clr_3);      // clear no pipeline name - error.
  CPPUNIT_TEST(clr_4);      // Clear extra parameter error
  CPPUNIT_TEST(clr_5);      // Clear nox pipeline error.
  
  CPPUNIT_TEST(clone_1);    // Clone duplicates the event processors.
  CPPUNIT_TEST(clone_2);    // Clone of an empty pipeline is perfectly fine too.
  CPPUNIT_TEST(clone_3);    // CLone with too few parameters.
  CPPUNIT_TEST(clone_4);    // Clone with too many parameters.
  CPPUNIT_TEST(clone_5);    // Clone with no such source pipeline.
  CPPUNIT_TEST(clone_6);    // Clone onto existing pipeline.
  
  CPPUNIT_TEST(pman_1);     // no subcommand is an error.
  CPPUNIT_TEST(pman_2);     // invalid subcommand is an error.
  CPPUNIT_TEST_SUITE_END();


private:
  CTCLInterpreter*  m_pInterp;
  CPipelineCommand* m_pCommand;
public:
  void setUp() {
    m_pInterp  = new CTCLInterpreter;
    m_pCommand = new CPipelineCommand(*m_pInterp);
    CPipelineManager::getInstance();
  }
  void tearDown() {
    delete CPipelineManager::m_pInstance;
    delete m_pCommand;
    delete m_pInterp;
  }
private:
  void lsSetup();
  void registerProcessors();
  std::string lindex(CTCLObject& objv, int index);
  void lindex(CTCLObject& result, CTCLObject& objv, int index);
  std::vector<std::string> list2vec(CTCLObject& objv);
protected:
  void mk_1();
  void mk_2();
  void mk_3();
  void mk_4();
  
  void ls_1();
  void ls_2();
  void ls_3();
  
  void add_1();
  void add_2();
  void add_3();
  void add_4();
  void add_5();
  void add_6();
  
  void current_1();
  void current_2();
  void current_3();
  
  void lall_1();
  void lall_2();
  void lall_3();
  void lall_4();
  void lall_5();
  
  void use_1();
  void use_2();
  void use_3();
  void use_4();
  
  void rm_1();
  void rm_2();
  void rm_3();
  void rm_4();
  void rm_5();
  
  void clr_1();
  void clr_2();
  void clr_3();
  void clr_4();
  void clr_5();
  
  void clone_1();
  void clone_2();
  void clone_3();
  void clone_4();
  void clone_5();
  void clone_6();
  
  void pman_1();
  void pman_2();
};
CPPUNIT_TEST_SUITE_REGISTRATION(pcmdTests);


std::string
pcmdTests::lindex(CTCLObject& obj, int index)
{
  // It's assumed obj is bound.
  
  CTCLObject objResult; objResult.Bind(*m_pInterp);
  objResult = obj.lindex(index);
  return std::string(objResult);
}

void
pcmdTests::lindex(CTCLObject& result, CTCLObject& obj, int index)
{
  result.Bind(*m_pInterp);
  result = obj.lindex(index);
}

std::vector<std::string>
pcmdTests::list2vec(CTCLObject& objv)
{
  int size = objv.llength();
  std::vector<std::string> result;
  for (int i =0; i < size; i++) {
    result.push_back(lindex(objv, i));
  }
  
  return result;
}

// Make a pipe:

void pcmdTests::mk_1() {
  CPPUNIT_ASSERT_NO_THROW(
    m_pInterp->GlobalEval("pman mk newpipe")
  );
  
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  EQ(size_t(2), pMgr->pipelineCount());
  std::vector<std::string> pipes = pMgr->getPipelineNames();
  EQ(std::string("default"), pipes[0]);
  EQ(std::string("newpipe"), pipes[1]);
}
// Make without pipename is an error:

void pcmdTests::mk_2() {
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman mk"),
    CTCLException
  );
}
// Make with extra param is an error:

void pcmdTests::mk_3() {
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman mk newpipe junk"),
    CTCLException
  );
}
// Make with duplicate pipe is an error:

void pcmdTests::mk_4()
{
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman mk default"),
    CTCLException
  );
}


// Tests for pman ls:

// Setup a few pipelines:

void pcmdTests::lsSetup()
{
  m_pInterp->GlobalEval("pman mk apipe");
  m_pInterp->GlobalEval("pman mk anotherpipe");
}

// list with no pattern lists all in alpha order:
//  anotherpipe apipe default
//
void pcmdTests::ls_1()
{
  lsSetup();                 // Make the pipes.
  
  std::string result = m_pInterp->GlobalEval("pman ls");
  CTCLObject objResult;
  objResult.Bind(*m_pInterp);
  objResult = result;
  
  EQ(3, objResult.llength());

  EQ(std::string("anotherpipe"), lindex(objResult, 0));  
  EQ(std::string("apipe"), lindex(objResult,1));
  EQ(std::string("default"), lindex(objResult, 2));

}

// list with pattern:

void pcmdTests::ls_2()
{
  lsSetup();
  
  std::string result = m_pInterp->GlobalEval("pman ls d*");
  EQ(std::string("default"), result);
}
// too  many params is an error:

void pcmdTests::ls_3()
{
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman ls a b"),
    CTCLException
  );
}

// Add tests - need a way to register some event processors:

void pcmdTests::registerProcessors()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  pMgr->registerEventProcessor("evp1", new CEventProcessor);
  pMgr->registerEventProcessor("evp2", new CEventProcessor);
  pMgr->registerEventProcessor("evp3", new CEventProcessor);
  pMgr->registerEventProcessor("aProcessor", new CEventProcessor);}

// add_1 - add an event processor to default.

void pcmdTests::add_1()
{
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  registerProcessors();
  
  m_pInterp->GlobalEval("pman add default aProcessor");
  std::vector<std::string> evps = pMgr->getEventProcessorsInPipeline("default");
  
  EQ(size_t(1), evps.size());
  EQ(std::string("aProcessor"), evps[0]);
}
// add_2 add event processors to several pipelines

void pcmdTests::add_2()
{
  lsSetup();
  registerProcessors();
  
  CPipelineManager* pMgr = CPipelineManager::getInstance();
  
  CPPUNIT_ASSERT_NO_THROW(
    m_pInterp->GlobalEval("pman add apipe  evp1");
  );
  m_pInterp->GlobalEval("pman add anotherpipe evp2");
  m_pInterp->GlobalEval("pman add default evp3");
  m_pInterp->GlobalEval("pman add default aProcessor");
  
  std::vector<std::string> apipe = {"evp1"};
  std::vector<std::string> anotherpipe = {"evp2"};
  std::vector<std::string> deflt = {"evp3", "aProcessor"};
  std::vector<std::vector<std::string>> contents = {anotherpipe, apipe, deflt};
  std::vector<std::string> pipes = {"anotherpipe", "apipe", "default"};
  for (int pipe =0; pipe < contents.size(); pipe++) {
    std::vector<std::string> evpnames = pMgr->getEventProcessorsInPipeline(pipes[pipe]);
    EQ(contents[pipe].size(), evpnames.size());
    for (int i = 0; i < contents[i].size(); i++) {
      EQ(contents[pipe][i], evpnames[i]);
    }
  }
  
}
// Need a pipename and an evpname:

void pcmdTests::add_3()
{
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman add default"),
    CTCLException
  );
}
// Too many parameters is bad too:

void pcmdTests::add_4()
{
  registerProcessors();
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman add default evp1 junk"),
    CTCLException
  );
}

// Bad pipeline name:

void pcmdTests::add_5()
{
  registerProcessors();
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman add junk evp1"),
    CTCLException
  );
}
// Bad event processor name

void pcmdTests::add_6()
{
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman add default junk"),
    CTCLException
  );
}

// current with no event processors in the current pipeline.

void pcmdTests::current_1()
{
  std::string result = m_pInterp->GlobalEval("pman current");
  CTCLObject objResult; objResult.Bind(*m_pInterp);
  objResult = result;
  EQ(2, objResult.llength());
  
  
  // First element is the pipeline name:
  
  EQ(std::string("default"), lindex(objResult, 0));
  
  // Second element is an empty list:
  
  CTCLObject elements = objResult.lindex(1);
  elements.Bind(*m_pInterp);
  EQ(0, elements.llength());
}

// List nonempty pipeline:

void pcmdTests::current_2()
{
  registerProcessors();
  m_pInterp->GlobalEval("pman add default evp1");
  m_pInterp->GlobalEval("pman add default evp2");
  m_pInterp->GlobalEval("pman add default evp3");
  
  std::string result = m_pInterp->GlobalEval("pman current");
  CTCLObject objResult; objResult.Bind(*m_pInterp);
  objResult = result;
  
  CTCLObject processors = objResult.lindex(1);
  processors.Bind(*m_pInterp);
  
  EQ(3, processors.llength());
  EQ(std::string("evp1"), lindex(processors, 0));
  EQ(std::string("evp2"), lindex(processors, 1));
  EQ(std::string("evp3"), lindex(processors, 2));
  
  
}
// Too many parameters is an error:

void pcmdTests::current_3()
{
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman current junk"),
    CTCLException
  );
}

//  list-all with nothing added - get a one element list with a two element
//  list inside containing {default {}}.

void pcmdTests::lall_1()
{
  std::string result   = m_pInterp->GlobalEval("pman ls-all");
  CTCLObject objResult; objResult.Bind(*m_pInterp);
  objResult = result;
  
  EQ(1, objResult.llength());
  CTCLObject defpipe;
  lindex(defpipe, objResult, 0);    // Defpipe is bound though.
  
  EQ(2, defpipe.llength());
  EQ(std::string("default"), lindex(defpipe, 0));
  EQ(std::string(""), lindex(defpipe, 1));
}
// list-all with event processors added to the current/default pipe (only).

void pcmdTests::lall_2()
{
  registerProcessors();
  m_pInterp->GlobalEval("pman add default evp1");
  m_pInterp->GlobalEval("pman add default evp2");
  
  std::string result = m_pInterp->GlobalEval("pman ls-all");
  CTCLObject objResult; objResult.Bind(*m_pInterp); objResult = result;
  
  EQ(1, objResult.llength());
  CTCLObject defpipe;
  lindex(defpipe, objResult, 0);         // {default {evp1 evp2}}
  
  EQ(std::string("default"), lindex(defpipe, 0));
  
  CTCLObject procs; lindex(procs, defpipe, 1);
  std::vector<std::string> procnames =  list2vec(procs);
  std::vector<std::string> expected ={"evp1", "evp2"};
  EQ(expected.size(), procnames.size());
  for(int i = 0; i < expected.size(); i++) {
    EQ(expected[i], procnames[i]);
  }
}
// list-all with event processors added to several pipes.

void pcmdTests::lall_3()
{
  lsSetup();
  registerProcessors();
  
  m_pInterp->GlobalEval("pman add apipe evp1");
  m_pInterp->GlobalEval("pman add apipe evp2");
  m_pInterp->GlobalEval("pman add default evp3");
  m_pInterp->GlobalEval("pman add default aProcessor");
  
  // This is what we'd expect to get from list-all with no filter pattern:
  
  std::vector<std::string> pipes = {"anotherpipe", "apipe", "default"};
  std::vector<std::vector<std::string>> contents = {
    {},                                            // anotherpipeline is empty,
    {"evp1", "evp2"},                              // apipeline
    {"evp3", "aProcessor"}                         // default.
  };
  
  CTCLObject result;
  result.Bind(*m_pInterp);
  result = m_pInterp->GlobalEval("pman ls-all");
  
  EQ(int(pipes.size()), result.llength());
  
  for (int i = 0; i < pipes.size(); i++) {
    CTCLObject pipeInfo;
    lindex(pipeInfo, result, i);
    EQ(pipes[i], lindex(pipeInfo, 0));
    CTCLObject pcontents;
    lindex(pcontents, pipeInfo, 1);
    
    std::vector<std::string> pnames = list2vec(pcontents);
    
    EQ(contents[i].size(), pnames.size());
    for (int p =0; p < contents[i].size(); p++) {
      EQ(contents[i][p], pnames[p]);
    }
  }
}
// list-all with a filter pattern

void pcmdTests::lall_4()
{
  lsSetup();
  registerProcessors();
  
  m_pInterp->GlobalEval("pman add apipe evp1");
  m_pInterp->GlobalEval("pman add apipe evp2");
  m_pInterp->GlobalEval("pman add default evp3");
  m_pInterp->GlobalEval("pman add default aProcessor");
  
  CTCLObject objResult; objResult.Bind(*m_pInterp);
  objResult = m_pInterp->GlobalEval("pman ls-all d*");  // only default.
  EQ(1, objResult.llength());
  
  CTCLObject dflt;
  lindex(dflt, objResult, 0);
  EQ(std::string("default"), lindex(dflt, 0));
  CTCLObject pnObj;
  lindex(pnObj, dflt, 1);
  std::vector<std::string> pnames = list2vec(pnObj);
  std::vector<std::string> expected = {"evp3", "aProcessor"};
  
  EQ(expected.size(), pnames.size());
  for (int i =0; i < expected.size(); i++) {
    EQ(expected[i], pnames[i]);
  }
}
// Too many command parameters is an error:
void pcmdTests::lall_5()
{
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman list-all d* junk"),
    CTCLException
  );
}

// Use a valid pipeline - it becomes current.

void pcmdTests::use_1()
{
  lsSetup();
  CPPUNIT_ASSERT_NO_THROW(
    m_pInterp->GlobalEval("pman use apipe")
  );
  
  CTCLObject result; result.Bind(*m_pInterp);
  result = m_pInterp->GlobalEval("pman current");
  EQ(std::string("apipe"), lindex(result, 0));
  
}
// Not enough parameters is an error:

void pcmdTests::use_2()
{
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman use"),
    CTCLException
  );
}
// Nonexistent pipeline is an error:

void pcmdTests::use_3()
{
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman use apipe"),
    CTCLException
  );
}
// Too many parameters is also an error:

void pcmdTests::use_4()
{
  lsSetup();
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman use apipe junk"),
    CTCLException
  );
}
// remove an event processor from a pipeline:

void pcmdTests::rm_1()
{
  registerProcessors();
  m_pInterp->GlobalEval("pman add default evp1");
  m_pInterp->GlobalEval("pman add default evp2");
  m_pInterp->GlobalEval("pman add default evp3");
  
  CPPUNIT_ASSERT_NO_THROW(
    m_pInterp->GlobalEval("pman rm default evp2")
  );                           // Take out the middle one.
  
  CTCLObject result; result.Bind(*m_pInterp);
  result = m_pInterp->GlobalEval("pman current");
  
  CTCLObject proclist;
  lindex(proclist, result, 1);
  std::vector<std::string> processors = list2vec(proclist);
  std::vector<std::string> expected = {"evp1", "evp3"};
  
  EQ(expected.size(), processors.size());
  for (int i = 0; i < expected.size(); i++) {
    EQ(expected[i], processors[i]);
  }
}
// Too many parameters on the rm is an error:

void pcmdTests::rm_2()
{
  registerProcessors();
  m_pInterp->GlobalEval("pman add default evp1");
  m_pInterp->GlobalEval("pman add default evp2");
  m_pInterp->GlobalEval("pman add default evp3");
  
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman rm default evp2 junk"),
    CTCLException
  );
}
// Too few parameters in rm:

void pcmdTests::rm_3()
{
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman rm default"),
    CTCLException
  );
}

// Invalid pipename is an error for rm:

void pcmdTests::rm_4()
{
  registerProcessors();
  m_pInterp->GlobalEval("pman add default evp1");
  m_pInterp->GlobalEval("pman add default evp2");
  m_pInterp->GlobalEval("pman add default evp3");
  
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman rm junk evp2"),
    CTCLException
  );
}
// EVP not in pipeline is an error:

void pcmdTests::rm_5()
{
  registerProcessors();
  m_pInterp->GlobalEval("pman add default evp1");
  m_pInterp->GlobalEval("pman add default evp2");
  m_pInterp->GlobalEval("pman add default evp3");
  
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman rm default aProcessor"),
    CTCLException
  );
}
// Clear pipeline with stuff in it:

void pcmdTests::clr_1()
{
  registerProcessors();
  m_pInterp->GlobalEval("pman add default evp1");
  m_pInterp->GlobalEval("pman add default evp2");
  m_pInterp->GlobalEval("pman add default evp3");
  
  CPPUNIT_ASSERT_NO_THROW(
    m_pInterp->GlobalEval("pman clear default")
  );
  
  CTCLObject result; result.Bind(*m_pInterp);
  result = m_pInterp->GlobalEval("pman current");
  
  CTCLObject evps;
  lindex(evps, result, 1);
  EQ(0, evps.llength());              // NO event processors left after clear.
  
}
// Clear a cleared pipeline is ok.

void pcmdTests::clr_2()
{
  CPPUNIT_ASSERT_NO_THROW(
    m_pInterp->GlobalEval("pman clear default")
  );
  
  CTCLObject result; result.Bind(*m_pInterp);
  result = m_pInterp->GlobalEval("pman current");
  
  CTCLObject evps;
  lindex(evps, result, 1);
  EQ(0, evps.llength()); 
}

// Clear with no pipeline name is an error:

void pcmdTests::clr_3()
{
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman clear"),
    CTCLException
  );
}
// Clear with extra parameter is an error too:

void pcmdTests::clr_4()
{
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman clear default junk"),
    CTCLException
  );
}
// Clear with non existing pipeline name is an error:

void pcmdTests::clr_5()
{
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman clear junk"),
    CTCLException
  );
}
// Clone of a pipeline duplicates the list of event processors:

void pcmdTests::clone_1()
{
  registerProcessors();
  m_pInterp->GlobalEval("pman add default evp1");
  m_pInterp->GlobalEval("pman add default evp2");
  m_pInterp->GlobalEval("pman add default evp3");
  
  CPPUNIT_ASSERT_NO_THROW(
    m_pInterp->GlobalEval("pman clone default newpipe")
  );
    
  CTCLObject result; result.Bind(*m_pInterp);
  result = m_pInterp->GlobalEval("pman ls-all newpipe");
  
  // Should have made a new pipe:
  
  EQ(1, result.llength());
  CTCLObject newpipe;
  lindex(newpipe, result, 0);            // "newpipe {evp1 evp2 evp3}"
  EQ(std::string("newpipe"), lindex(newpipe, 0));
  CTCLObject prcList; lindex(prcList, newpipe, 1);   // processor list.
  
  std::vector<std::string> processors = list2vec(prcList);
  std::vector<std::string> expected = {"evp1", "evp2", "evp3"};
  
  EQ(expected.size(), processors.size());
  for (int i = 0; i < expected.size(); i++) {
    EQ(expected[i], processors[i]);
  }
}
// Clone of an empty pipeline results in an empty pipeline.

void pcmdTests::clone_2()
{
    CPPUNIT_ASSERT_NO_THROW(
      m_pInterp->GlobalEval("pman clone default newpipe")
    );
    
    CTCLObject result; result.Bind(*m_pInterp);
  result = m_pInterp->GlobalEval("pman ls-all newpipe");
  
  // Should have made a new pipe:
  
  EQ(1, result.llength());
  CTCLObject newpipe;
  lindex(newpipe, result, 0);            // "newpipe {evp1 evp2 evp3}"
  EQ(std::string("newpipe"), lindex(newpipe, 0));
  CTCLObject prcList; lindex(prcList, newpipe, 1);   // processor list.
  
  std::vector<std::string> processors = list2vec(prcList);
  EQ(size_t(0), processors.size());      // Empty pipe.
}
// Too few parameters is an error:

void pcmdTests::clone_3()
{
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman clone default"),
    CTCLException
  );
}
// Too many parameter is an error:

void pcmdTests::clone_4()
{
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman clone default newpipe junk"),
    CTCLException
  );
  
}
// Invalid source pipeline is an error:

void pcmdTests::clone_5()
{
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman clone dflt newpipe"),
    CTCLException
  );
}
//  Clone onto existing pipeline is an error too

void pcmdTests::clone_6()
{
  lsSetup();
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman clone default apipe"),
    CTCLException
  );
}

// pman command with no subcommand is an error:

void pcmdTests::pman_1()
{
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman"),
    CTCLException
  );
}
// pman command with an unknown subcommand is an error.

void pcmdTests::pman_2()
{
  CPPUNIT_ASSERT_THROW(
    m_pInterp->GlobalEval("pman no-such-subcommand"),
    CTCLException
  );
}