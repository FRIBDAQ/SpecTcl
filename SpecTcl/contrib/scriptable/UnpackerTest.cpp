// Template for a test suite.
#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <CPacket.h>
#include <CPacketCreator.h>
#include <CModuleCommand.h>
#include <CSegmentUnpacker.h>
#include <tcl.h>
#include <TCLInterpreter.h>
#include <CModuleDictionary.h>
#include <CScriptableUnpacker.h>
#include <TCLAnalyzer.h>
#include <string>
#include <NSCLBufferDecoder.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

extern void* gpInterpreter;

class UnpackerTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(UnpackerTest);
  CPPUNIT_TEST(ConstructTest);
  CPPUNIT_TEST(RunTransitionTest);
  CPPUNIT_TEST(OnOtherTest);
  CPPUNIT_TEST(SetupTest);
  CPPUNIT_TEST(IntegrationTest);
  CPPUNIT_TEST_SUITE_END();


private:
  Tcl_Interp*          m_pRawInterp;
  CTCLInterpreter*     m_pInterp;
  CScriptableUnpacker* m_pSUnpacker;

public:
  void setUp() {
    m_pRawInterp = Tcl_CreateInterp();
    gpInterpreter = m_pInterp
                  = new CTCLInterpreter(m_pRawInterp);
    m_pSUnpacker  = new CScriptableUnpacker;
  }
  void tearDown() {
    delete m_pSUnpacker;
    delete m_pInterp;
    Tcl_DeleteInterp(m_pRawInterp);

  }
protected:
  void ConstructTest();
  void RunTransitionTest();
  void OnOtherTest();
  void SetupTest();
  void IntegrationTest();
};

CPPUNIT_TEST_SUITE_REGISTRATION(UnpackerTest);

//
// On creation, the creator should be able to locate the correct
// module dictionary.
//  The only thing we have going for us is null tests:
//  I should not yet have made either the module or the unpack command
// Then if I attach the module it will make these commands:
void 
UnpackerTest::ConstructTest() {
  
  bool ok;

  try {
    m_pInterp->Eval("module -types\n");
    ok = false;
  }
  catch (...) {
    ok = true;
  }
  ASSERT(ok);

  try {
    m_pInterp->Eval("unpack cget \n");
    ok = false;
  }
  catch (...) {
    ok = true;
  }
  ASSERT(ok);

  // Now 'attach' it to an anaylzer and see if the commands get registered.

  CTclAnalyzer analyzer(*m_pInterp, 10, 100);
  m_pSUnpacker->OnAttach(analyzer);

  try {
    m_pInterp->Eval("module -types\n");
    ok = true;
  }
  catch (...) {
    ok = false;
  }
  ASSERT(ok);

  try {
    m_pInterp->Eval("unpack cget \n");
    ok = true;
  }
  catch (...) {
    ok = false;
  }
  ASSERT(ok);

  EQ(false, m_pSUnpacker->isSetup());
}
//
//  Check that run transitions do the right thing to the isSetup state.
//
void
UnpackerTest::RunTransitionTest()
{
  CTclAnalyzer analyzer(*m_pInterp, 10, 100);
  m_pSUnpacker->OnAttach(analyzer);
  
  // We should be not setup:

  EQ(false, m_pSUnpacker->isSetup());

  // Starting a run sets us up...

  CNSCLBufferDecoder decoder;
  m_pSUnpacker->OnBegin(analyzer, decoder);
  EQ(true, m_pSUnpacker->isSetup());

  // Stopping a run unsets us up..

  m_pSUnpacker->OnEnd(analyzer, decoder);
  EQ(false, m_pSUnpacker->isSetup());

  // Resuming sets up again..

  m_pSUnpacker->OnResume(analyzer, decoder);
  EQ(true, m_pSUnpacker->isSetup());

  // Pausing unsets

  m_pSUnpacker->OnPause(analyzer, decoder);
  EQ(false, m_pSUnpacker->isSetup());
}
//  Run the on other member and be sure that it sets up the module.
//
void
UnpackerTest::OnOtherTest()
{
  CTclAnalyzer analyzer(*m_pInterp, 10, 100);
  m_pSUnpacker->OnAttach(analyzer);
  CNSCLBufferDecoder   decoder;

  EQ(false, m_pSUnpacker->isSetup());

  m_pSUnpacker->OnOther(analyzer, decoder);

  EQ(true, m_pSUnpacker->isSetup());
}

// Ensure that setup really really works:
// After building and attaching the unpacker, we configure
// the "unpack" object to enable packetizing
// This configuration does not do anything until after the next
// Begin like operation, OnOther, or operator() call.
// So:
//  - Verify that the unpacker thinks packetization is off.
//  - OnBegin.
//  - Verify that the unpacker thinks packetization is on.
void
UnpackerTest::SetupTest()
{
  CTclAnalyzer analyzer(*m_pInterp, 10, 100);
  m_pSUnpacker->OnAttach(analyzer);
  CNSCLBufferDecoder   decoder;

  CPacket* pPacket = m_pSUnpacker->getTopLevel();

  EQ(false, pPacket->Packetized()); // The default state...

  m_pInterp->Eval("unpack config packetize true\n");
  EQ(false, pPacket->Packetized()); // Configured but config not processed.

  m_pSUnpacker->OnBegin(analyzer, decoder);

  EQ(true, pPacket->Packetized()); // Now config was processed


}
//
// The stuff below sets up for a full integration test where
// we exercise module creation, unpacker registration and
// buffer unpacking.
//
// The Events we will unpack looks like this:

UShort_t Event1[] = {
  7, 3, 1234, 111, 3, 4321, 222}; // Packet 1 and 2 there.
UShort_t Event2[] = {
  4, 3, 4321, 333};		// Only packet 2 there.
UShort_t Event3[] = {
  4, 3, 1234, 444};		// Only packet 1 there.
UShort_t Event4[] = {		// oho...empty event hehehe.
  1, 3, 1234, 666		// With some fake stuff at the end!!
};

// I'm also going to need a proto-buffer to initialize my buffer
// decoder so the correct translator will be produced.
//

UShort_t FakeBuffer[] = {
  16, 2, 0, 0, 0,0, 0, 0,0,0, 4, 0x0102, 0x0304,0x0102, 0,0};


// The top level unpacker will have packetization off.
// We'll make two packets with packetization for
// packet ids 1234 and 4321 respectively.
// Each of these will have an unpacker of the
// class implemented below that will just
// unpack a single word and store it for recovery.
//
class DummyUnpacker : public CSegmentUnpacker
{
  UShort_t m_nWord;
public:
  DummyUnpacker(const string rName, CTCLInterpreter& rInterp) :
    CSegmentUnpacker(rName, rInterp)
  {m_nWord = 0xffff; }

  virtual void Setup(CAnalyzer& rana, CHistogrammer& rHist) {
    m_nWord = 0xffff;
  }
  virtual string getType() const {return string("Test"); }
  virtual TranslatorPointer<UShort_t> Unpack(TranslatorPointer<UShort_t> pEvent, 
					     CEvent& rEvent, 
					     CAnalyzer& rAnalyzer, 
					     CBufferDecoder& rDecoder)
  {
    m_nWord = *pEvent;
    ++pEvent;
    return pEvent;
  }
  void Reset() {
    m_nWord = 0xffff;
  }
  UShort_t getWord() const {
    return m_nWord;
  }
};
// To make a dummy unpacker we need a dummy unpacker creator:
//
class DummyCreator : public CModuleCreator
{
public:
  DummyCreator(const string& rType,
	       CModuleCommand* pCreatorCommand) :
    CModuleCreator(rType, pCreatorCommand)
  {}
  virtual string TypeInfo() const {
    return string("test");
  }
  virtual CSegmentUnpacker* Create(CTCLInterpreter& rInterp, 
				   const string& rName)
  {
    return new DummyUnpacker(rName, rInterp);
  }
};


// Now the test itself:

void
UnpackerTest::IntegrationTest()
{
  CTclAnalyzer analyzer(*m_pInterp, 10, 100);
  m_pSUnpacker->OnAttach(analyzer);
  CNSCLBufferDecoder   decoder;

  m_pSUnpacker->RegisterCreator("test", 
				new DummyCreator("test",
					     m_pSUnpacker->getModuleCommand()));
				


  // Use scripts to setup the decode:

  // The packet decoders...

  m_pInterp->Eval("module pk1 packet packetize true id 1234\n");
  m_pInterp->Eval("module pk2 packet packetize true id 4321\n");

  // The two dummies:

  m_pInterp->Eval("module t1 test\n");
  m_pInterp->Eval("module t2 test\n");

  // Put the dummies in their packets and the packets in the unpacker.

  m_pInterp->Eval("pk1 add t1\n");
  m_pInterp->Eval("pk2 add t2\n");
  m_pInterp->Eval("unpack add pk1 pk2\n");

  // To do the test, we're going to need to get pointers to the t1 and t2
  // items to be able to manipulate them directly.

  CPacket*            pTop = m_pSUnpacker->getTopLevel();
  CModuleDictionary* pDict = pTop->getDictionary();
  CModuleDictionary::ModuleIterator p;
  p  = pDict->Find("t1");
  DummyUnpacker *t1   = (DummyUnpacker*)p->second;
  p  = pDict->Find("t2");
  DummyUnpacker *t2   = (DummyUnpacker*)p->second;

  // Before each event we'll do an on-begin to be sure everything is configed
  // and initted.  
  // The first event should give 111 to t1 and 222 to t2:

  m_pSUnpacker->OnEnd(analyzer, decoder);
  m_pSUnpacker->OnBegin(analyzer, decoder);
  decoder(32, FakeBuffer, analyzer);
  CEvent event;
  m_pSUnpacker->operator()(Event1, event, analyzer, decoder);

  EQ((UShort_t)111, t1->getWord());
  EQ((UShort_t)222, t2->getWord());

  // The second event should give 0xffff to t1 and 3333 to t2

  m_pSUnpacker->OnEnd(analyzer, decoder);
  m_pSUnpacker->OnBegin(analyzer, decoder);
  m_pSUnpacker->operator()(Event2, event, analyzer, decoder);

  EQ((UShort_t)0xffff, t1->getWord());
  EQ((UShort_t)333,    t2->getWord());

  // The third event should give 444 to t1 and 0xffff to t2:

  m_pSUnpacker->OnEnd(analyzer, decoder);
  m_pSUnpacker->OnBegin(analyzer, decoder);
  m_pSUnpacker->operator()(Event3, event, analyzer, decoder);

  EQ((UShort_t)444, t1->getWord());
  EQ((UShort_t)0xffff,    t2->getWord());

  // The last one should give 0xffff to both:

  m_pSUnpacker->OnEnd(analyzer, decoder);
  m_pSUnpacker->OnBegin(analyzer, decoder);
  m_pSUnpacker->operator()(Event4, event, analyzer, decoder);

  EQ((UShort_t)0xffff, t1->getWord());
  EQ((UShort_t)0xffff,    t2->getWord());
  m_pSUnpacker->OnEnd(analyzer, decoder);

}
