// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include <CSegmentUnpacker.h>
#include <CModuleDictionary.h>
#include <CPacket.h>
#include <Event.h>
#include <string>
#include <TCLInterpreter.h>
#include <TCLResult.h>
#include <Analyzer.h>
#include <Histogrammer.h>
#include <tcl.h>
#include "Asserts.h"
#include <NSCLBufferDecoder.h>
#include <TranslatorPointer.h>
#include <Exception.h>

extern void* gpEventSink;

//  Dummy module we can insert.

class DummyModule : public CSegmentUnpacker
{
  bool     m_fWasSetup;
  UShort_t m_nUnpacked;
public:
  DummyModule(const string& rname, CTCLInterpreter& rInterp) :
    CSegmentUnpacker(rname, rInterp),
    m_fWasSetup(false),
    m_nUnpacked(0xffff)
  {}

  // Minimal implementations.

  virtual void Setup(CAnalyzer& ra, CHistogrammer& rh) {
    m_fWasSetup = true;
    Reset();
  }
  virtual string getType() const {return string("Testsegment");  };
  virtual TranslatorPointer<UShort_t> Unpack(TranslatorPointer<UShort_t> p, 
					     CEvent& rEvent,
					     CAnalyzer& rAnalyzer,
					     CBufferDecoder& rDecoder)  
  {
    m_nUnpacked = *p;
    ++p;
    return p;
  }
  bool WasSetup() const {
    return m_fWasSetup;
  }
  UShort_t Got() const {
    return m_nUnpacked;
  }
  void Reset() {
    m_nUnpacked = 0xffff;
  }
};

// The test class.

class PacketTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(PacketTest);
  CPPUNIT_TEST(ConstructTest);	//!< Test construction.
  CPPUNIT_TEST(pgmAddRemoveTest); //!< Add modules programmatically.
  CPPUNIT_TEST(OnDeleteTest);	//!< Test on-delete functionality.
  CPPUNIT_TEST(SetupTest);	//!< Test that setup works.
  CPPUNIT_TEST(ConfigTest);	//!< Test that configuration works correctly.
  CPPUNIT_TEST(cmdAddRemoveTest); //!< Test command add/remove.
  CPPUNIT_TEST(cmdListTest);	//!< Test list command.
  CPPUNIT_TEST(UnpackTest);	//!< Unpack various types of bufferss.
  CPPUNIT_TEST_SUITE_END();


private:
  Tcl_Interp*        m_pRawInterpreter;
  CTCLInterpreter*   m_pInterp;
  CModuleDictionary* m_pDictionary;
  CPacket*           m_pTopPacket;

public:
  // Need to create an interpreter and its associated object.

  void setUp() {
    m_pRawInterpreter = Tcl_CreateInterp();
    m_pInterp         = new CTCLInterpreter(m_pRawInterpreter);
    m_pDictionary     = new CModuleDictionary;
    m_pTopPacket      = new CPacket("top", *m_pInterp, m_pDictionary);
  }
  // Destroy the interpreter and its associated object
  void tearDown() {
    delete m_pTopPacket;
    delete m_pDictionary;
    delete m_pInterp;
    Tcl_DeleteInterp(m_pRawInterpreter);
  }
  void TestUnpack(void* buffer, DummyModule* d1, DummyModule* d2);
protected:
  void ConstructTest();
  void pgmAddRemoveTest();
  void OnDeleteTest();
  void SetupTest();
  void ConfigTest();
  void cmdAddRemoveTest();
  void cmdListTest();
  void UnpackTest();
};

CPPUNIT_TEST_SUITE_REGISTRATION(PacketTest);



/// The tests:

//
// After construction:
//    m_nId = -1
//    m_fPacketize = false.
//    m_Decoders is empty
//    m_pModules == m_pDictionary.
//
void PacketTest::ConstructTest() {
  EQ(-1,    m_pTopPacket->m_nId);
  EQ(false, m_pTopPacket->m_fPacketize);
  EQ(0,     m_pTopPacket->NumDecoders());
  EQ(m_pDictionary, m_pTopPacket->m_pModules);
}

//
//  Create two dummy modules:
//    dum1 dum2.
//  Add them to the unpacker.
//   - Should be able to find them both.
//  Remove one (by pointer).
//   - Should be able to find remaining one.
//   - Should not be able to find removed one.
//   - Removed one is not owned.
//  Remove last by iterator.
//   - Should not have any modules left.
//
void
PacketTest::pgmAddRemoveTest()
{
  DummyModule* pm1 = new DummyModule("dum1", *m_pInterp);
  DummyModule* pm2 = new DummyModule("dum2", *m_pInterp);

  m_pTopPacket->AddModule(pm1);
  m_pTopPacket->AddModule(pm2);

  EQ(2, m_pTopPacket->NumDecoders());
  ASSERT(m_pTopPacket->BeginDecoders() != m_pTopPacket->EndDecoders());
  CPacket::ModuleIterator p = m_pTopPacket->FindDecoder("dum1");
  EQ(pm1, (DummyModule*)*p);
  p = m_pTopPacket->FindDecoder("dum2");
  EQ(pm2, (DummyModule*)*p);

  //Remove one...
  
  m_pTopPacket->RemoveModule(pm2); // Take out the second...
  EQ(pm1,(DummyModule*)*(m_pTopPacket->FindDecoder("dum1")));
  ASSERT(m_pTopPacket->EndDecoders() ==  m_pTopPacket->FindDecoder("dum2"));
  EQ(1, m_pTopPacket->NumDecoders());

  // Remove the last one via iterator:
  //
   p = m_pTopPacket->FindDecoder("dum1");
  m_pTopPacket->RemoveModule(p);
  EQ(0,m_pTopPacket->NumDecoders());
  ASSERT(!pm1->isOwned());
  ASSERT(!pm2->isOwned());

  delete pm1;
  delete pm2;

}
//
//  Check that on-delete operates correctly:
//  Create a pair of modules and add them.
//  Delete one module.
//  -   It should have been removed from the packet.
//  Call packet's OnDelete member:
//  - There should be 0 decoders.
//  - Neither module should be owned.
//
void
PacketTest::OnDeleteTest()
{
  DummyModule* pm1 = new DummyModule("dum1", *m_pInterp);
  DummyModule* pm2 = new DummyModule("dum2", *m_pInterp);

  m_pTopPacket->AddModule(pm1);
  m_pTopPacket->AddModule(pm2);

  delete pm1;

  EQ(1, m_pTopPacket->NumDecoders()); // only one left...
  ASSERT(m_pTopPacket->EndDecoders() == m_pTopPacket->FindDecoder("dum1"));

  // Call onDelete for the packet..pm2 should get released:

  m_pTopPacket->OnDelete();
  ASSERT(!(pm2->isOwned()));

  delete pm2;
}
//
// Test setup function delegation to member decoders..
// Insert a pair of dummys in to the packet.
// Invoke the Setup member.
//  Both dummies should indicate they were setup.
//
void
PacketTest::SetupTest()
{
  DummyModule* pm1 = new DummyModule("dum1", *m_pInterp);
  DummyModule* pm2 = new DummyModule("dum2", *m_pInterp);
  gpEventSink   = new CHistogrammer(1024*1024);
  CAnalyzer* pan= new CAnalyzer;

  m_pTopPacket->AddModule(pm1);
  m_pTopPacket->AddModule(pm2);

  m_pTopPacket->Setup(*pan, *(CHistogrammer*)gpEventSink);

  ASSERT(pm1->WasSetup());
  ASSERT(pm2->WasSetup());

  delete pm1;
  delete pm2;
  delete pan;
  delete (CHistogrammer*)gpEventSink;
}
//
// Setup the packet:
//   - m_fPacketize is false.
//   - m_nId        is -1.
// Configure to packetize true.
// - Setup should throw a string exception.
// Configure to id 1234
// Setup the packet.
//   - m_fPacketize is true.
//   - m_nId    is 1234
//
void
PacketTest::ConfigTest()
{

  gpEventSink   = new CHistogrammer(1024*1024);
  CAnalyzer* pan= new CAnalyzer;
  
  m_pTopPacket->Setup(*pan, *(CHistogrammer*)gpEventSink);
  ASSERT(!m_pTopPacket->m_fPacketize);
  EQ(-1, m_pTopPacket->m_nId);

  //  Only get 1/2 the job done... th esetup should be pissed.

  m_pInterp->Eval("top config packetize true\n");
  bool ok;
  try {
    m_pTopPacket->Setup(*pan, *(CHistogrammer*)gpEventSink);
    ok = false;
  }
  catch (string msg) {
    ok = true;
  }
  ASSERT(ok);

  // Do the rest of the job:

  m_pInterp->Eval("top config id 1234\n");
  m_pTopPacket->Setup(*pan, *(CHistogrammer*)gpEventSink);
  EQ(1234, m_pTopPacket->m_nId);
  ASSERT(m_pTopPacket->m_fPacketize);

  delete pan;
  delete (CHistogrammer*)gpEventSink;
}
//
// Create two dummys.  Add them to the dictionary and then add them
// via tcl commands.
//  They should both be findable.
// Remove one of them.
//   It should not be findable any more.
void
PacketTest::cmdAddRemoveTest()
{
  DummyModule* pm1 = new DummyModule("dum1", *m_pInterp);
  DummyModule* pm2 = new DummyModule("dum2", *m_pInterp);
  m_pDictionary->Add(pm1);
  m_pDictionary->Add(pm2);

  // Add the modules via TCL command:

  m_pInterp->Eval("top add dum1 dum2\n");
  CPacket::ModuleIterator p = m_pTopPacket->FindDecoder("dum1");
  EQ(pm1, (DummyModule*)*p);
  p = m_pTopPacket->FindDecoder("dum2");
  EQ(pm2, (DummyModule*)*p);

  // Remove dum2:

  m_pInterp->Eval("top remove dum2\n");
  p = m_pTopPacket->FindDecoder("dum1");
  EQ(pm1, (DummyModule*)*p);
  p = m_pTopPacket->FindDecoder("dum2");
  ASSERT(m_pTopPacket->EndDecoders() == p);
     
}
//
// Create the dummies and add them as before.  Then ask for a listing.
// should get:
// "{dum1 Testsegment} {dum2 Testsegment}"
//
void 
PacketTest::cmdListTest()
{
  DummyModule* pm1 = new DummyModule("dum1", *m_pInterp);
  DummyModule* pm2 = new DummyModule("dum2", *m_pInterp);
  m_pDictionary->Add(pm1);
  m_pDictionary->Add(pm2);

  // Add the modules via TCL command: and list...

  m_pInterp->Eval("top add dum1 dum2\n");
  m_pInterp->Eval("top list\n");

  CTCLResult r(m_pInterp);
  
  string result((const char*)r);
  EQ(string("{dum1 Testsegment} {dum2 Testsegment}"), result);
  
  delete pm1;
  delete pm2;
}
//
// Below is a buffer segment that has a non id'd packet.  Each
// dummy will 'unpack' one word from the buffer at the current
// position..we have a non-id'd packet with 2 dummies.
//
UShort_t nonidbuf[] = {
  3,1,2
};

// Below is the same buffer segment, but it contains an id'd packet:
//

UShort_t idbuf[] = {
  5, 4, 1234, 1, 2};
//
// Below is a buffer that contains 2 id'd packets in it (test hierarchical
// unpack.
//
UShort_t hierbuf[] = {
  7, 3, 1234, 1, 3, 4321, 2
};
//
// Common testing code for unpack test:  The top level is set up
// and we get a buffer pointer to work with, and a pair of dummies.
void 
PacketTest::TestUnpack(void* buffer, DummyModule* d1, DummyModule* d2)
{
  NonSwappingBufferTranslator ns(buffer);
  TranslatorPointer<UShort_t> p(ns);
  CEvent                      Event;
  CNSCLBufferDecoder          Decode;
  CAnalyzer                   Analyzer;  
  gpEventSink   = new CHistogrammer(1024*1024);

  try {
    d1->Reset();
    d2->Reset();
    m_pTopPacket->Setup(Analyzer, *(CHistogrammer*)gpEventSink);
    
    m_pTopPacket->Unpack(p, Event, Analyzer, Decode);
    EQ((UShort_t)1, d1->Got());
    EQ((UShort_t)2, d2->Got());
  }
  catch(string msg) {
    cerr << "Caught string exception in testunpack: " << msg << endl;
    delete (CHistogrammer*)gpEventSink;
    ASSERT(0);
  }
  catch (CException& rexcept) {
    cerr << "Caught an nscl exception: " << rexcept.ReasonText() << endl;
    delete (CHistogrammer*)gpEventSink;
    ASSERT(0);
  }
  catch(...) {
    delete (CHistogrammer*)gpEventSink;
    throw;
  }
  delete (CHistogrammer*)gpEventSink;

}
// 1.  Unpack nonidbuf with 2 dummies the first should get 1, the second 2.
// 2.  Unpack idbuf having configured m_pTopPacket to be an id packet
//     with id 1234.
// 3. Reconfig toplevel to nonpacketizing.
//    Create a new packet with packetizing , id 1234 and dummy1,2,
//    should decode too.
// 4. Setup toplevel with packet 1, packet2 each with a dummy packetized
//    and 1234, 4321 ids. Decode hierbuf.
void
PacketTest::UnpackTest()
  
{
  DummyModule* pm1 = new DummyModule("dum1", *m_pInterp);
  DummyModule* pm2 = new DummyModule("dum2", *m_pInterp);
  CPacket* pack1 = new CPacket("first", *m_pInterp, m_pDictionary);
  CPacket* pack2 = new CPacket("second", *m_pInterp, m_pDictionary);

  try {
    
    m_pTopPacket->AddModule(pm1);
    m_pTopPacket->AddModule(pm2);
    TestUnpack(nonidbuf, pm1, pm2);
    
    // Turn on packetizing correctly:
    
    m_pInterp->Eval("top config packetize true id 1234\n");
    TestUnpack(idbuf, pm1, pm2);
    
    // Turn off packetizing, make a new packet and put pm1, pm2 in it instead..
    m_pTopPacket->OnDelete();	// Clear the decoder list.
    m_pInterp->Eval("top config packetize false\n");
    


    pack1->AddModule(pm1);
    pack1->AddModule(pm2);
    m_pTopPacket->AddModule(pack1);
    
    m_pInterp->Eval("first config packetize true id 1234\n");
    TestUnpack(idbuf, pm1, pm2);
    
    // NOw do the hierarchical unpack.
    
    
    pack1->OnDelete();		// empty pack1.
    pack1->AddModule(pm1);
    pack2->AddModule(pm2);
    m_pTopPacket->AddModule(pack1);
    m_pTopPacket->AddModule(pack2);
    m_pInterp->Eval("second config packetize true id 4321\n");
    TestUnpack(hierbuf, pm1, pm2);
    
  }
  catch (...) {
    delete pack1;
    delete pack2;
    delete pm1;
    delete pm2;
    throw;
  }
  delete pm1;
  delete pm2;
  delete pack1;
  delete pack2;

  
}
