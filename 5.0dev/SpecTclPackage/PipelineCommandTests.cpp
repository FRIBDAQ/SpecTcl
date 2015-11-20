// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <tcl.h>

#define private public
#include "CPipelineCommand.h"
#include "CSpecTclInterpreter.h"
#include "CAnalysisPipeline.h"
#undef private

#include "CDecoder.h"
#include "CEventProcessor.h"
#include "TCLException.h"
#include "CParameter.h"


/// stub
void
CParameter::invalidateAll() {}


class PipelineCommandTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(PipelineCommandTest);
  CPPUNIT_TEST(registercmd);

  // Tests for decoder factory management.

  CPPUNIT_TEST(nodecoder);
  CPPUNIT_TEST(add1decodercount);
  CPPUNIT_TEST(add1decoderfind);
  CPPUNIT_TEST(adddupdecoderthrows); // TODO: add remove tests

  CPPUNIT_TEST(removedecodernox);
  CPPUNIT_TEST(removedecoderok);
  CPPUNIT_TEST(removedecodermultiple);
  CPPUNIT_TEST(removedecodercurrent);


  // Tests for event processor factory management

  CPPUNIT_TEST(noevprocessor);
  CPPUNIT_TEST(add1evpcount);
  CPPUNIT_TEST(add1evpfind);
  CPPUNIT_TEST(adddupevpthrows); 

  CPPUNIT_TEST(removeevpnoex);
  CPPUNIT_TEST(removeevpok);
  CPPUNIT_TEST(removeevpinpipe);
  

  // Tests to manage the analysis pipeline decoder.

  CPPUNIT_TEST(setdecodernosuch);
  CPPUNIT_TEST(setdecoderok);
  CPPUNIT_TEST(listdecodernone);
  CPPUNIT_TEST(listdecoder);

  // Tests to manage the event  pipeline event processors.

  // Adding analysis pipeline elements.

  CPPUNIT_TEST(addevpnosuch);
  CPPUNIT_TEST(addevpend);
  CPPUNIT_TEST(addevpbefore);
  CPPUNIT_TEST(addevpbeforenosuch);
  CPPUNIT_TEST(addtopipe);
  CPPUNIT_TEST(addfull);
  CPPUNIT_TEST(badkeyword);
  CPPUNIT_TEST(badpipeline);

  // Pipeline listing

  CPPUNIT_TEST(listemptypipeline);
  CPPUNIT_TEST(listpipeline);
  CPPUNIT_TEST(listmatching);
  CPPUNIT_TEST(listselected);
  CPPUNIT_TEST(listbadkeyword);
  CPPUNIT_TEST(listbadpipeline);

  // Removing pipeline elements.

  CPPUNIT_TEST(removeempty);
  CPPUNIT_TEST(removeok);
  CPPUNIT_TEST(removenx);
  CPPUNIT_TEST(removein);
  CPPUNIT_TEST(removebadkey);
  CPPUNIT_TEST(removebadpipe);

  // Tests to managethe analysis pipeline

  // Additions to analysis

   CPPUNIT_TEST(addanappend);
   CPPUNIT_TEST(addanbefore);
  
  // Listing elements in analysyis

  CPPUNIT_TEST(listana);
  CPPUNIT_TEST(listanamatching);
  
  // Removing elements (in analysys)

  CPPUNIT_TEST(removeanalysis);
  CPPUNIT_TEST(removeanalyzerinpipe);

  CPPUNIT_TEST_SUITE_END();


private:
  CTCLInterpreter* m_pInterp;
  Tcl_Interp*      m_pRawInterp;

public:
  void setUp() {
      // Setup needs to create the namespace:

    m_pRawInterp = Tcl_CreateInterp();
    CSpecTclInterpreter::setInterp(m_pRawInterp);

    m_pInterp = CSpecTclInterpreter::instance();
    Tcl_CreateNamespace(m_pInterp->getInterpreter(), "::spectcl", 0, 0);

  }
  void tearDown() {
    CPipelineCommand::m_pInstance->clearRegistrations();
    delete CPipelineCommand::m_pInstance;
    CPipelineCommand::m_pInstance = 0;

    delete CSpecTclInterpreter::m_pInstance;
    CSpecTclInterpreter::m_pInstance = 0;
    CSpecTclInterpreter::m_pInterp   = 0;

    if (!Tcl_InterpDeleted(m_pRawInterp)) {
      Tcl_DeleteInterp(m_pRawInterp);
    }

    delete CAnalysisPipeline::instance();
    CAnalysisPipeline::m_pInstance = 0;

  }
protected:
  void registercmd();

  void nodecoder();
  void add1decodercount();
  void add1decoderfind();
  void adddupdecoderthrows();
  void removedecodernox();
  void removedecoderok();
  void removedecodermultiple();
  void removedecodercurrent();

  void noevprocessor();
  void add1evpcount();
  void add1evpfind();
  void adddupevpthrows();

  void removeevpnoex();
  void removeevpok();
  void removeevpinpipe();

  void setdecodernosuch();
  void setdecoderok();
  void listdecodernone();
  void listdecoder();

  void addevpnosuch();
  void addevpend();
  void addevpbefore();
  void addevpbeforenosuch();
  void addtopipe();
  void addfull();
  void badkeyword();
  void badpipeline();

  void listemptypipeline();
  void listpipeline();
  void listmatching();
  void listselected();
  void listbadkeyword();
  void listbadpipeline();

  void removeempty();
  void removeok();
  void removenx();
  void removein();
  void removebadkey();
  void removebadpipe();

  void addanappend();
  void addanbefore();

  void listana();
  void listanamatching();

  void removeanalysis();
  void removeanalyzerinpipe();

};

CPPUNIT_TEST_SUITE_REGISTRATION(PipelineCommandTest);

/**
 * registercmd
 *   instantiating the command should registercmd it with the interp.
 */
void PipelineCommandTest::registercmd() {


  CPipelineCommand* pCmd    = CPipelineCommand::instance();

  // All of this should have been sufficent to make the command.
  
  
  bool thrown = false;
  try {
    m_pInterp->GlobalEval("::spectcl::pipeline showdecoder");
  }
  catch(...) {
    thrown = true;
  }
  ASSERT(!thrown);
}

/**
 * Unless the test adds decoders there should not be any:
 */
void
PipelineCommandTest::nodecoder()
{
  EQ(static_cast<size_t>(0), CPipelineCommand::instance()->decodersSize());
}
/**
 * add1decodercount
 *   After adding a decoder to the 'factory', we should have one
 */

class CNullDecoder : public CDecoder 
{
public:
  virtual void   onData(CDataSource* pSource)  {}
  virtual size_t getItemSize()                {return 0;}
  virtual void*  getItemPointer()              {return 0;}
  virtual bool   next()                        {return false;}
};

void
PipelineCommandTest::add1decodercount()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CNullDecoder*      pDecoder = new CNullDecoder;

  pCommand->defineDecoder("null", pDecoder);

  EQ(static_cast<size_t>(1), pCommand->decodersSize());
}
/**
 * add1decoderfind
 *
 *  After installing a decoder it should be findable:
 */
void
PipelineCommandTest::add1decoderfind()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CNullDecoder*      pDecoder = new CNullDecoder;

  pCommand->defineDecoder("null", pDecoder);
  
  ASSERT(pCommand->findDecoder("null") != pCommand->m_RegisteredDecoders.end());

}
/**
 * adddupdecoderthrows
 *
 *   Adding a duplicate decoder should throw a pipelinecommand_exception
 *
 */
void
PipelineCommandTest::adddupdecoderthrows()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CNullDecoder*      pDecoder = new CNullDecoder;
  CNullDecoder*      pDuplicate = new CNullDecoder;

  pCommand->defineDecoder("null", pDecoder);

  bool threw = false;
  bool rightThrow = false;

  try {
    pCommand->defineDecoder("null", pDuplicate);
  }
  catch(pipelinecommand_exception& e) {
    threw = true;
    rightThrow = true;
  } 
  catch(...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(rightThrow);
  
  delete pDuplicate;
}

/**
 * noevprocessor
 *
 *  Until one is registered there should be no event processors.
 */
void
PipelineCommandTest::noevprocessor()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  
  EQ(static_cast<size_t>(0), pCommand->eventProcessorsSize());
}
/**
 * add1evpcount
 *
 *  After adding an event processor, the count should
 *  reflect this
 */
class CNullEventProcessor : public CEventProcessor
{
public:
  CNullEventProcessor(const char* pName = "null") :
    CEventProcessor(pName) {}
  virtual bool onEvent(void* pEvent, size_t nBytes, 
		       CAnalysisPipeline* pipeline) { return true; }
};
void
PipelineCommandTest::add1evpcount()
{
  CEventProcessor*  pEp = new CNullEventProcessor("null");
  CPipelineCommand* pCommand = CPipelineCommand::instance();

  pCommand->defineElement(pEp);

  EQ(static_cast<size_t>(1), pCommand->eventProcessorsSize());
  
}
/**
 * add1evpfind
 *
 *  After adding an event processor, we should als be able to find it.
 */
void
PipelineCommandTest::add1evpfind()
{
  CEventProcessor*  pEp = new CNullEventProcessor("null");
  CPipelineCommand* pCommand = CPipelineCommand::instance();

  pCommand->defineElement(pEp);

  ASSERT(pCommand->findEventProcessor(pEp->getName()) != pCommand->m_RegisteredEventProcessors.end());
}
/**
 * adddupevpthrows
 *
 *   An attempt to add an event processor with a duplicate
 *  name will result in an exception.
 */
void
PipelineCommandTest::adddupevpthrows()
{
  CEventProcessor*  pEp = new CNullEventProcessor("null");
  CEventProcessor*  pDup = new CNullEventProcessor("null");
  CPipelineCommand* pCommand = CPipelineCommand::instance();

  pCommand->defineElement(pEp);

  bool threw(false);
  bool rightThrow(false);

  try {
    pCommand->defineElement(pDup);
  }
  catch (pipelinecommand_exception& e) {
    threw = true;
    rightThrow = true;
  }
  catch (...) {
    threw = true;
  }
  ASSERT(threw);
  ASSERT(rightThrow);

  delete pDup;

}
/**
 * setdecodernosuch
 *
 *  If I try to issue the ::spectcl::pipeline setdecoder some-name
 *  and that decoder is not registered (via CTCLInterpeter::GlobalEval)
 *  The command should fail and GlobalEval will throw a 
 *  CTCLException.  We should also be able to predict
 *  The result text to be: "no such decoder : 'some-name'"
 */
void
PipelineCommandTest::setdecodernosuch()
{
  bool threw(false);
  bool rightException(false);
  std::string resultText;

  CPipelineCommand* pCommand = CPipelineCommand::instance();

  try {
    m_pInterp->GlobalEval("::spectcl::pipeline setdecoder junk");
  }
  catch (CTCLException& e) {
    threw = true;
    rightException = true;
    resultText = e.ReasonText();
  }
  catch(...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(rightException);
  EQ(std::string("no such decoder : 'junk'"), resultText);
}
/**
 * setdecoderok
 *
 *  Setting the decoder to one that's known should work just fine
 */
void
PipelineCommandTest::setdecoderok()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CDecoder*         pDecoder = new CNullDecoder;
  pCommand->defineDecoder("null", pDecoder);

  m_pInterp->GlobalEval("::spectcl::pipeline setdecoder null");

  EQ(pDecoder, CAnalysisPipeline::instance()->m_pDecoder);

  
}
/**
 * listdecodernone
 *
 *   If there is no decoder set, then the list command
 *   should give nothing back.
 */
void
PipelineCommandTest::listdecodernone()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();

  std::string result = m_pInterp->GlobalEval("::spectcl::pipeline showdecoder");
  EQ(std::string(""), result);
}
/**
 * listdecoder
 *
 * If I set a named decoder in the analysis pipeline showdecoder
 * should show its name.
 */
void
PipelineCommandTest::listdecoder()
{

  // Create and insert the decoder:

  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CDecoder*         pDecoder = new CNullDecoder;
  pCommand->defineDecoder("null", pDecoder);
  m_pInterp->GlobalEval("::spectcl::pipeline setdecoder null");

  // Run the showdecoder command:

  std::string decoderType = 
      m_pInterp->GlobalEval("::spectcl::pipeline showdecoder");

  EQ(std::string("null"), decoderType);


}
/**
 ** addevpnosuch
 *
 *  Adding an event processor which has not been registered
 * (via ::spectcl::pipeline  add) returns an error and the result:
 *  "no such event processor : 'processor-name'"
 */
void
PipelineCommandTest::addevpnosuch()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();

  bool threw(false);
  bool threwRight(false);
  std::string msg;
  try {

    m_pInterp->GlobalEval("::spectcl::pipeline add no-such");

  }
  catch (CTCLException& e) {
    threw = true;
    threwRight = true;
    msg = e.ReasonText();
  }
  catch(...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(threwRight);

  EQ(std::string("no such event processor : 'no-such'"), msg);

}
/**
 * addevpend
 *
 * Simple version of the add event processor is just
 *  ::spectcl::pipeline add name
 *
 * That adds the event processor to the
 * back of the pipeline.
 *  we're going to add a pair and make sure they wind up in that order.
 */
void
PipelineCommandTest::addevpend()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CEventProcessor*  pEp1 = new CNullEventProcessor("null1");
  CEventProcessor*  pEp2 = new CNullEventProcessor("null2");

  pCommand->defineElement(pEp1);
  pCommand->defineElement(pEp2);

  // Add them to the pipeline:

  std::string name1 = m_pInterp->GlobalEval("::spectcl::pipeline add null1");
  std::string name2 = m_pInterp->GlobalEval("::spectcl::pipeline add null2");

  // name1, name2 should be the names of the added event processors:

  EQ(std::string("null1"), name1);
  EQ(std::string("null2"), name2);

  // Furthermore, the event analysis pipeline should have null1 and null2 in it
  // in that order...and nothing else.
 
  CAnalysisPipeline* pPipe  = CAnalysisPipeline::instance();
  CAnalysisPipeline::PipelineIterator p = pPipe->begin();

  EQ(pEp1, *p);
  p++;
  EQ(pEp2, *p);
  p++;
  ASSERT(p == pPipe->end());
}
/**
 * addevpbefore
 *
 *   Excercises the before clause;
 *   *   insert an element into the pipeline via pipeline add null1.
 *   *   insert the second element usnig pipeline add null2 before null1
 *   *   This should give me two element in the pipeline
 *   *   with null2 followed by null1 
 */
void
PipelineCommandTest::addevpbefore()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CEventProcessor*  pEp1 = new CNullEventProcessor("null1");
  CEventProcessor*  pEp2 = new CNullEventProcessor("null2");

  pCommand->defineElement(pEp1);
  pCommand->defineElement(pEp2);

  // Add them to the pipeline:

  std::string name1 = m_pInterp->GlobalEval("::spectcl::pipeline add null1");
  std::string name2 = m_pInterp->GlobalEval("::spectcl::pipeline add null2 before null1");

  // name1, name2 should be the names of the added event processors:

  EQ(std::string("null1"), name1);
  EQ(std::string("null2"), name2);

  // Furthermore, the event analysis pipeline should have null1 and null2 in it
  // in that order...and nothing else.


 
  CAnalysisPipeline* pPipe  = CAnalysisPipeline::instance();
  EQ(static_cast<size_t>(2), pPipe->size()); // both should have been inserted.


  CAnalysisPipeline::PipelineIterator p = pPipe->begin();
  

  EQ(pEp2, *p);
  p++;
  EQ(pEp1, *p);
  p++;
  ASSERT(p == pPipe->end());
}
/**
 * addevpbeforenosuch
 *
 *   If I do an add pipe thing before nosuch
 *   and nosuch is not in the pipeline, 
 *   *   GlobalEval throws a CTCLException
 *   *   The result text is "event pipeline has no element named : 'nosuch'"
 */
void
PipelineCommandTest::addevpbeforenosuch()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CEventProcessor* pEp      = new CNullEventProcessor("null");
  pCommand->defineElement(pEp);

  bool threw(false);
  bool rightThrow(false);
  std::string result;

  try {
    m_pInterp->GlobalEval("::spectcl::pipeline  add null before null1");
  }
  catch (CTCLException& e) {
    threw = true;
    rightThrow = true;
    result = e.ReasonText();
  }
  catch(...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(rightThrow);
  EQ(std::string("event pipeline has no element named : 'null1'"), result);
}
/**
 * addtopipe
 *    Can specify to clause in add as well
 */
void
PipelineCommandTest::addtopipe()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CEventProcessor*  pEp1 = new CNullEventProcessor("null1");
  CEventProcessor*  pEp2 = new CNullEventProcessor("null2");

  pCommand->defineElement(pEp1);
  pCommand->defineElement(pEp2);

  // Add them to the pipeline:

  std::string name1 = m_pInterp->GlobalEval("::spectcl::pipeline add null1 to event");
  std::string name2 = m_pInterp->GlobalEval("::spectcl::pipeline add null2 to event");

  // name1, name2 should be the names of the added event processors:

  EQ(std::string("null1"), name1);
  EQ(std::string("null2"), name2);

  // Furthermore, the event analysis pipeline should have null1 and null2 in it
  // in that order...and nothing else.
 
  CAnalysisPipeline* pPipe  = CAnalysisPipeline::instance();
  CAnalysisPipeline::PipelineIterator p = pPipe->begin();

  EQ(pEp1, *p);
  p++;
  EQ(pEp2, *p);
  p++;
  ASSERT(p == pPipe->end());
}
/**
 * addfull
 *
 *   Should be able to be completely explicit as well:
 *   "pipline add null to event before null1
 */
void 
PipelineCommandTest::addfull()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CEventProcessor*  pEp1 = new CNullEventProcessor("null1");
  CEventProcessor*  pEp2 = new CNullEventProcessor("null2");

  pCommand->defineElement(pEp1);
  pCommand->defineElement(pEp2);

  // Add them to the pipeline:

  std::string name1 = m_pInterp->GlobalEval("::spectcl::pipeline add null1");
  std::string name2 = 
   m_pInterp->GlobalEval("::spectcl::pipeline add null2 to event before null1");

  // name1, name2 should be the names of the added event processors:

  EQ(std::string("null1"), name1);
  EQ(std::string("null2"), name2);

  // Furthermore, the event analysis pipeline should have null1 and null2 in it
  // in that order...and nothing else.


 
  CAnalysisPipeline* pPipe  = CAnalysisPipeline::instance();
  EQ(static_cast<size_t>(2), pPipe->size()); // both should have been inserted.


  CAnalysisPipeline::PipelineIterator p = pPipe->begin();
  

  EQ(pEp2, *p);
  p++;
  EQ(pEp1, *p);
  p++;
  ASSERT(p == pPipe->end()); 
}
/**
 * badkeyword
 *
 *  pipeline add null1 frogfull slithey
 *
 *  Is a bad clause.. must be "to" or "before"
 */
void
PipelineCommandTest::badkeyword()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CEventProcessor* p         = new CNullEventProcessor("null");
  pCommand->defineElement(p);

  bool threw(false);
  bool rightThrow(false);
  std::string msg;


  try {
    m_pInterp->GlobalEval("::spectcl::pipeline add null slithy toves");
  }
  catch(CTCLException& e) {
    threw = true;
    rightThrow = true;
    msg = e.ReasonText();
  } 
  catch(...) {
    threw = true;
  }
  ASSERT(threw);
  ASSERT(rightThrow);
  EQ(std::string("invalid keyword: 'slithy' must be 'before' or 'to'"), 
     msg);
}
/**
 * badpipeline
 *
 *   to must have 'analysis' or 'event' keywords.
 */
void
PipelineCommandTest::badpipeline()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CEventProcessor* p         = new CNullEventProcessor("null");
  pCommand->defineElement(p);

  bool threw(false);
  bool rightThrow(false);
  std::string msg;


  try {
    m_pInterp->GlobalEval("::spectcl::pipeline add null to something");
  }
  catch(CTCLException& e) {
    threw = true;
    rightThrow = true;
    msg = e.ReasonText();
  } 
  catch(...) {
    threw = true;
  }
  ASSERT(threw);
  ASSERT(rightThrow);
  EQ(std::string("no such pipeline: 'something' must be 'event' or 'analysis'"),
     msg);
}
/**
 * listemptypipeline
 *
 *   Should be an empty list.
 */
void
PipelineCommandTest::listemptypipeline()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  
  std::string result = m_pInterp->GlobalEval("::spectcl::pipeline list");
  EQ(std::string(""), result);
}
/**
 * listpipeline
 *
 *  List with some element in it should also be in pipeline order.
 */
void
PipelineCommandTest::listpipeline()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CEventProcessor*  pEp1 = new CNullEventProcessor("null1");
  CEventProcessor*  pEp2 = new CNullEventProcessor("null2");

  pCommand->defineElement(pEp1);
  pCommand->defineElement(pEp2);

  // Add them to the pipeline:

  std::string name1 = m_pInterp->GlobalEval("::spectcl::pipeline add null1");
  std::string name2 = 
   m_pInterp->GlobalEval("::spectcl::pipeline add null2 to event before null1");


  std::string list = m_pInterp->GlobalEval("::spectcl::pipeline list");
  EQ(std::string("null2 null1"), list);
}
/**
 * listmatching
 *    Exercises list matching glob-pattern.
 *    * Create two event processors null1 and null2 
 *    * Define them as elements known to the pipeline command.
 *    * Insert them in the event pipeline.
 *    * execute:  pipeline list matching *2
 *    * shouild only list null2.
 */
void
PipelineCommandTest::listmatching()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CEventProcessor*  pEp1 = new CNullEventProcessor("null1");
  CEventProcessor*  pEp2 = new CNullEventProcessor("null2");

  pCommand->defineElement(pEp1);
  pCommand->defineElement(pEp2);

  // Add them to the pipeline:

  std::string name1 = m_pInterp->GlobalEval("::spectcl::pipeline add null1");
  std::string name2 = m_pInterp->GlobalEval("::spectcl::pipeline add null2");


  // Execute the list command

  std::string result = m_pInterp->GlobalEval("::spectcl::pipeline list matching *2");

  EQ(std::string("null2"), result);
}
/**
 * listselected
 *
 *   This the in keyword in conjunction with the matching keyword.
 */
void
PipelineCommandTest::listselected()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CEventProcessor*  pEp1 = new CNullEventProcessor("null1");
  CEventProcessor*  pEp2 = new CNullEventProcessor("null2");

  pCommand->defineElement(pEp1);
  pCommand->defineElement(pEp2);

  // Add them to the pipeline:

  std::string name1 = m_pInterp->GlobalEval("::spectcl::pipeline add null1");
  std::string name2 = m_pInterp->GlobalEval("::spectcl::pipeline add null2");


  // Execute the list command

  std::string result = m_pInterp->GlobalEval("::spectcl::pipeline list in event matching *2");

  EQ(std::string("null2"), result);
}
/**
 * listbadkeyword
 *
 *  provide a bad keyword to the list command.
 *  e.g. only 'in' and 'matching' are allowed now.
 */
void
PipelineCommandTest::listbadkeyword()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  
  bool thrown = false;
  bool rightThrow = false;
  std::string msg;

  try {
    m_pInterp->GlobalEval("::spectcl::pipeline list frothy waves");
  }
  catch (CTCLException& e) {
    thrown = true;
    rightThrow = true;
    msg = e.ReasonText();
  } 
  catch(...) {
    thrown = true;
  }
  ASSERT(thrown);
  ASSERT(rightThrow);
  EQ(std::string("invalid keyword: 'frothy' must be 'in' or 'matching'"), msg);
}
/**
 * listbadpipeline
 *
 *   tests e.g. list in george which is not a valid pipeline name.
 */
void
PipelineCommandTest::listbadpipeline()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  
  bool thrown = false;
  bool rightThrow = false;
  std::string msg;

  try {
    m_pInterp->GlobalEval("::spectcl::pipeline list in slimy-pipeline");
  }
  catch (CTCLException& e) {
    thrown = true;
    rightThrow = true;
    msg = e.ReasonText();
  } 
  catch(...) {
    thrown = true;
  }
  ASSERT(thrown);
  ASSERT(rightThrow);
  EQ(std::string("invalid pipeline name 'slimy-pipeline' must be 'event' or 'analysis'"), msg);
}
/**
 * removeempty
 *
 *  clearly removing an element from an empty pipeline will fail:
 *   "no such element 'george'"
 */
void
PipelineCommandTest::removeempty()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  
  bool threw(false);
  bool rightThrow(false);
  std::string msg;
  try {
    m_pInterp->GlobalEval("::spectcl::pipeline remove george");
  }
  catch (CTCLException& e) {
    threw = true;
    rightThrow = true;
    msg = e.ReasonText();
  }
  catch (...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(rightThrow);
  EQ(std::string("no such element 'george'"), msg);
}
/**
 * removeok
 *
 *   Insert elements in the pipeline remove one and ensure
 *   * an element was removed.
 *   * the correct element remains.
 */
void
PipelineCommandTest::removeok()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CEventProcessor*  pEp1 = new CNullEventProcessor("null1");
  CEventProcessor*  pEp2 = new CNullEventProcessor("null2");

  pCommand->defineElement(pEp1);
  pCommand->defineElement(pEp2);

  m_pInterp->GlobalEval("::spectcl::pipeline add null1");
  m_pInterp->GlobalEval("::spectcl::pipeline add null2");

  // Remove one:

  m_pInterp->GlobalEval("::spectcl::pipeline remove null1");

  // Should be one left and it's null2:

  CAnalysisPipeline* pPipe = CAnalysisPipeline::instance();
  EQ(static_cast<size_t>(1), pPipe->size());
  
  CAnalysisPipeline::PipelineIterator p = pPipe->begin();
  EQ(std::string("null2"), (*p)->getName());
}
/**
 * removenx()
 *   This is essentially the same as removeempty() except there
 *   are elements in the pipeline but we ask to remove one that isn't.
 */
void
PipelineCommandTest::removenx()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CEventProcessor*  pEp1 = new CNullEventProcessor("null1");
  CEventProcessor*  pEp2 = new CNullEventProcessor("null2");

  pCommand->defineElement(pEp1);
  pCommand->defineElement(pEp2);

  m_pInterp->GlobalEval("::spectcl::pipeline add null1");
  m_pInterp->GlobalEval("::spectcl::pipeline add null2");
  
  bool threw(false);
  bool rightThrow(false);
  std::string msg;
  try {
    m_pInterp->GlobalEval("::spectcl::pipeline remove george");
  }
  catch (CTCLException& e) {
    threw = true;
    rightThrow = true;
    msg = e.ReasonText();
  }
  catch (...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(rightThrow);
  EQ(std::string("no such element 'george'"), msg);
}
/**
 * removein
 *   Use the in keyword to specify which pipe.
 */
void
PipelineCommandTest::removein()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CEventProcessor*  pEp1 = new CNullEventProcessor("null1");
  CEventProcessor*  pEp2 = new CNullEventProcessor("null2");

  pCommand->defineElement(pEp1);
  pCommand->defineElement(pEp2);

  m_pInterp->GlobalEval("::spectcl::pipeline add null1");
  m_pInterp->GlobalEval("::spectcl::pipeline add null2");

  // Remove one:

  m_pInterp->GlobalEval("::spectcl::pipeline remove null1 in event");

  // Should be one left and it's null2:

  CAnalysisPipeline* pPipe = CAnalysisPipeline::instance();
  EQ(static_cast<size_t>(1), pPipe->size());
  
  CAnalysisPipeline::PipelineIterator p = pPipe->begin();
  EQ(std::string("null2"), (*p)->getName());  
}
/**
 * removebadkey
 * 
 *  A bad keyword should throw with a predictable error message
 */
void
PipelineCommandTest::removebadkey()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  
  bool threw(false);
  bool threwRight(false);
  std::string msg;

  try {
    m_pInterp->GlobalEval("::spectcl::pipeline remove george from abc");
  }
  catch (CTCLException& e) {
    threw = true;
    threwRight = true;
    msg = e.ReasonText();
  } catch(...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(threwRight);
  EQ(std::string("invalid keyword: 'from' must be 'in'"), msg);
}
/**
 *  removebadpipe
 *
 *   remove with bad pipe in 'in' clause
 */
void
PipelineCommandTest::removebadpipe()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  
  bool threw(false);
  bool threwRight(false);
  std::string msg;

  try {
    m_pInterp->GlobalEval("::spectcl::pipeline remove george in abc");
  }
  catch (CTCLException& e) {
    threw = true;
    threwRight = true;
    msg = e.ReasonText();
  } catch(...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(threwRight);
  EQ(std::string("invalid pipeline: 'abc' must be 'analysis' or 'event'"), msg);
}
/**
 * removedecodernox
 *
 * remove a nonexistent decodeer definition.
 */
void
PipelineCommandTest::removedecodernox()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CNullDecoder*      pDecoder = new CNullDecoder;

  pCommand->defineDecoder("null", pDecoder);

  // Remove a different decoder (that does not exist).

  bool threw(false);
  bool rightThrow(false);
  std::string msg;

  try {
    pCommand->removeDecoder("goofy");
  }
  catch (pipelinecommand_exception& e) {
    threw = true;
    rightThrow = true;
    msg = e.what();
  }
  catch(...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(rightThrow);
  EQ(std::string("CPipelineCommand::removeDecoder no such decoder 'goofy'"), msg);
}
/**
 * removedecoderok
 *
 *   Remove a decoder that has been defined (the only one defined).
 */
void
PipelineCommandTest::removedecoderok()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CDecoder*      pDecoder = new CNullDecoder;

  pCommand->defineDecoder("null", pDecoder);

  CDecoder* pD = pCommand->removeDecoder("null");
  
  EQ(static_cast<size_t>(0), pCommand->decodersSize());
  EQ(pDecoder, pD);
  delete pDecoder;
}
/**
 * removedecodermultiple
 *
 *   Remove the right decoder from among more than one.
 */
void
PipelineCommandTest::removedecodermultiple()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  CDecoder*      pD1 = new CNullDecoder;
  CDecoder*      pD2 = new CNullDecoder;
  CDecoder*      pD3 = new CNullDecoder;

  pCommand->defineDecoder("null1", pD1);
  pCommand->defineDecoder("null2", pD2);
  pCommand->defineDecoder("null3", pD3);


  // Remove the middle one:

  CDecoder* pRemoved = pCommand->removeDecoder("null2");

  // Right counts and right one back:

  EQ(static_cast<size_t>(2), pCommand->decodersSize());
  EQ(pD2, pRemoved);

  // Right ones remaining.

  CPipelineCommand::pDecoder pD = pCommand->findDecoder("null1");
  ASSERT(pD != pCommand->m_RegisteredDecoders.end());
  EQ(pD1, pD->second);

  pD  = pCommand->findDecoder("null3");
  ASSERT(pD != pCommand->m_RegisteredDecoders.end());
  EQ(pD3, pD->second);

  delete pD2;
}
/**
 * removedecodercurrent
 * 
 *  Should give us an exception.
 */
void
PipelineCommandTest::removedecodercurrent()
{
 CPipelineCommand* pCommand = CPipelineCommand::instance();
  CDecoder*      pD1 = new CNullDecoder;
  CDecoder*      pD2 = new CNullDecoder;
  CDecoder*      pD3 = new CNullDecoder;

  pCommand->defineDecoder("null1", pD1);
  pCommand->defineDecoder("null2", pD2);
  pCommand->defineDecoder("null3", pD3);

  // Set null2 as the current decoder

  m_pInterp->GlobalEval("::spectcl::pipeline setdecoder null2");

  bool threw(false);
  bool threwCorrect(false);
  std::string msg;

  try {
    pCommand->removeDecoder("null2");
  }
  catch (pipelinecommand_exception& e) {
    threw = true;
    threwCorrect = true;
    msg = e.what();
  } 
  catch(...) {
    threw = true;
  }
  
  ASSERT(threw);
  ASSERT(threwCorrect);
  EQ(std::string("CPipelineCommand::removeDecoder 'null2' is the current decoder"),
     msg);
  
}
/**
 * removeevpnoex
 *
 *   Remove an event processor from the registry when it's not in the registry.
 */
void
PipelineCommandTest::removeevpnoex()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();

  bool threw = false;
  bool threwRight = false;
  std::string msg;
  
  try {
    pCommand->removeElementDefinition("george");
  }
  catch (pipelinecommand_exception& e) {
    threw = true;
    threwRight = true;
    msg = e.what();
  }
  catch(...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(threwRight);
  EQ(std::string("CPipelineCommand::removeElementDefinition no such element 'george'"),
     msg);


  
}
/**
 * removeevpok
 *  
 *  Remove an event processor definition that is registered (ok).
 *  * The number of event processors should indicate the removal.
 *  * The return value from the method should point to our event processor.
 *  * The remaining event processors should be named correctly.
 */
void
PipelineCommandTest::removeevpok()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();

  CEventProcessor* pEp1 = new CNullEventProcessor("null1");
  CEventProcessor* pEp2 = new CNullEventProcessor("removeme");
  CEventProcessor* pEp3 = new CNullEventProcessor("null2");

  pCommand->defineElement(pEp1);
  pCommand->defineElement(pEp2);
  pCommand->defineElement(pEp3);

  // Remove 'removeme'

  CEventProcessor* pRemoved = pCommand->removeElementDefinition("removeme");
  
  // Check count and removed one.

  EQ(static_cast<size_t>(2), pCommand->m_RegisteredEventProcessors.size());
  EQ(pEp2, pRemoved);

  // Check remaining elements; names and pointers.

  CPipelineCommand::pEventProcessor p = pCommand->m_RegisteredEventProcessors.begin();
  EQ(std::string("null1"), p->first);
  EQ(pEp1, p->second);
  
  p++;
  EQ(std::string("null2"), p->first);
  EQ(pEp3, p->second);

  p++;

  ASSERT(p == pCommand->m_RegisteredEventProcessors.end());
  delete pEp2;
}
/**
 * removeevpinpipe
 *
 *  Removing an event processor that is in the pipeline is an error too:
 */
void
PipelineCommandTest::removeevpinpipe()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();

  CEventProcessor* pEp1 = new CNullEventProcessor("null1");
  pCommand->defineElement(pEp1);
  
  m_pInterp->GlobalEval("::spectcl::pipeline add null1"); //  add it to the pipeline.
 
  bool threw = false;
  bool threwRight = false;
  std::string msg;

  try {
    pCommand->removeElementDefinition("null1");
  }
  catch (pipelinecommand_exception& e) {
    threw = true;
    threwRight = true;
    msg = e.what();
  }
  catch (...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(threwRight);
  EQ(std::string("CPipelineCommand::removeElementDefinition 'null1' is still in the pipeline and cannot be removed"),
     msg);
}
/**
 * addanappend
 *
 *  Add to the end of the analysis pipeline:
 */
void
PipelineCommandTest::addanappend()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();

  CEventProcessor* pEp1 = new CNullEventProcessor("null1");
  pCommand->defineElement(pEp1);
  
  m_pInterp->GlobalEval("::spectcl::pipeline add null1 to analysis");

  // Should be able to find it in the analysis pipeline as the first element.

  CAnalysisPipeline* pPipe = CAnalysisPipeline::instance();
  CAnalysisPipeline::PipelineIterator p = pPipe->analysisFind("null1");
  ASSERT(p != pPipe->analysisEnd());
  EQ(pEp1, *p);

}
/**
 * addanbefore
 *
 *  Adds to the analysis pipeline before a named element.
 */
void
PipelineCommandTest::addanbefore()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();

  CEventProcessor* pEp1 = new CNullEventProcessor("null1");
  CEventProcessor* pEp2 = new CNullEventProcessor("null2");
  pCommand->defineElement(pEp1);
  pCommand->defineElement(pEp2);

  
  m_pInterp->GlobalEval("::spectcl::pipeline add null1 to analysis");
  m_pInterp->GlobalEval("::spectcl::pipeline add null2 to analysis before null1");

  // The analysis pipeline should be null2, null1:

  CAnalysisPipeline* pPipe = CAnalysisPipeline::instance();
  CAnalysisPipeline::PipelineIterator p = pPipe->analysisBegin();

  EQ(pEp2, *p);
  p++;
  EQ(pEp1, *p);
  p++;
  ASSERT(p == pPipe->analysisEnd());

}
/**
 * listana
 *   List in analysis should show the elements in he analysis pipeline:
 */
void 
PipelineCommandTest::listana()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();

  // stock the pipeline

  CEventProcessor* pEp1 = new CNullEventProcessor("null1");
  CEventProcessor* pEp2 = new CNullEventProcessor("null2");
  pCommand->defineElement(pEp1);
  pCommand->defineElement(pEp2);

  
  m_pInterp->GlobalEval("::spectcl::pipeline add null1 to analysis");
  m_pInterp->GlobalEval("::spectcl::pipeline add null2 to analysis before null1");

  std::string listing = m_pInterp->GlobalEval("::spectcl::pipeline list in analysis");
  EQ(std::string("null2 null1"), listing);
}
/**
 * listanamatching
 *   list in analysis matching a pattern.
 */
void
PipelineCommandTest::listanamatching()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();

  // stock the pipeline

  CEventProcessor* pEp1 = new CNullEventProcessor("null1");
  CEventProcessor* pEp2 = new CNullEventProcessor("null2");
  pCommand->defineElement(pEp1);
  pCommand->defineElement(pEp2);

  
  m_pInterp->GlobalEval("::spectcl::pipeline add null1 to analysis");
  m_pInterp->GlobalEval("::spectcl::pipeline add null2 to analysis before null1");

  std::string listing = m_pInterp->GlobalEval("::spectcl::pipeline list matching *2 in analysis");
  EQ(std::string("null2"), listing);
}
/**
 * removeanalysis
 *
 *  Check that I can remove an element fromthe analysis pipeline
 */
void
PipelineCommandTest::removeanalysis()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();

  // stock the pipeline

  CEventProcessor* pEp1 = new CNullEventProcessor("null1");
  CEventProcessor* pEp2 = new CNullEventProcessor("null2");
  pCommand->defineElement(pEp1);
  pCommand->defineElement(pEp2);

  
  m_pInterp->GlobalEval("::spectcl::pipeline add null1 to analysis");
  m_pInterp->GlobalEval("::spectcl::pipeline add null2 to analysis before null1");

  m_pInterp->GlobalEval("::spectcl::pipeline remove null2 in analysis");

  std::string listing = m_pInterp->GlobalEval("::spectcl::pipeline list in analysis");
  EQ(std::string("null1"), listing);
}
/**
 * removeanalyzerinpipe
 *
 *  It is an error to remove the analyzer definition when it's in the analysis
 *  pipe.
 */
void
PipelineCommandTest::removeanalyzerinpipe()
{
  CPipelineCommand* pCommand = CPipelineCommand::instance();
  
  CEventProcessor* pEp1 = new CNullEventProcessor("null");
  pCommand->defineElement(pEp1);
  m_pInterp->GlobalEval("::spectcl::pipeline add null to analysis");

  bool threw = false;
  bool rightThrow = false;
  std::string msg;
  try {
    pCommand->removeElementDefinition("null");
  }
  catch(pipelinecommand_exception& e) {
    threw = true;
    rightThrow = true;
    msg = e.what();
  }
  catch(...) {
    threw = true;
  }
  ASSERT(threw);
  ASSERT(rightThrow);
  EQ(std::string("CPipelineCommand::removeElementDefinition 'null' is still in the pipeline and cannot be removed"),
     msg);

}
