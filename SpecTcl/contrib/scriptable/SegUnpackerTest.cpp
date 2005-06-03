// Template for a test suite.
#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include <CSegmentUnpacker.h>
#include <CModuleDictionary.h>
#include <CPacket.h>
#include <string>
#include <TCLInterpreter.h>
#include <Analyzer.h>
#include <Histogrammer.h>
#include <tcl.h>
#include "Asserts.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Segment unpackers are abstract base classes we need to
// derive a minimally functional class to be able
// to test the segment unpacker.
//
class ConcreteSegUnpacker : public CSegmentUnpacker
{
public:
  ConcreteSegUnpacker(const string& rname, CTCLInterpreter& rInterp) :
    CSegmentUnpacker(rname, rInterp) {}

  // Minimal implementations.

  virtual void Setup(CAnalyzer& ra, CHistogrammer& rh) {}
  virtual string getType() const {return string("Testsegment");  };
  virtual TranslatorPointer<UShort_t> Unpack(TranslatorPointer<UShort_t> p, 
					     CEvent& rEvent,
					     CAnalyzer& rAnalyzer,
					     CBufferDecoder& rDecoder)  
  {
    return p;
  }
};
// The test class:


class SegUnpackTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SegUnpackTest);
  CPPUNIT_TEST(ConstructTest);	//!< Test construction.
  CPPUNIT_TEST(OwnershipTest);	//!< Test ownership functions.
  CPPUNIT_TEST_SUITE_END();


private:
  Tcl_Interp*       m_pRawInterpreter;
  CTCLInterpreter*  m_pInterp;
  ConcreteSegUnpacker* m_pSegment;

public:
  // Need to create an interpreter and its associated object.

  void setUp() {
    m_pRawInterpreter = Tcl_CreateInterp();
    m_pInterp         = new CTCLInterpreter(m_pRawInterpreter);
    m_pSegment        = new  ConcreteSegUnpacker("testing", *m_pInterp);
  }
  // Destroy the interpreter and its associated object
  void tearDown() {
    delete m_pSegment;
    delete m_pInterp;
    Tcl_DeleteInterp(m_pRawInterpreter);
  }
protected:
  void ConstructTest();
  void OwnershipTest();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SegUnpackTest);



/// The tests:

//
// After construction:
//   - there must be a command named after the unpacker.
//   - The segment is not owned.
//   - The owner is null.
// We trust that the base class properly initializes since it's
// completely stolen from the Readout software which has tests for
// it.
//
void SegUnpackTest::ConstructTest() {

  bool ok;
  try {
    m_pInterp->Eval("testing help\n");
    ok = true;
  }
  catch(...) {
    ok = false;
  }
  ASSERT(ok);

  // 
  ASSERT(!m_pSegment->isOwned());
  EQ((CPacket*)0, m_pSegment->getOwner());
}
//
// Test the ownership function.
// Construct a ConcreteSegUnpacker and a packet.
// This will be hard to test indepenedent of the
// CPacket...we'll really be testing both together
// for a bit.
//  Add the segment to the unpacker.
//   - segment should be owned and by packet.
//  Remove the segment from the unpacker.
//   - segement should not be owned and m_pOwner ==  0.
//  Add back to the unpacker. and call OnDelete.
//   - Segment should not be owned and m_pOwner == 0.
//
void
SegUnpackTest::OwnershipTest()
{
  CModuleDictionary dict;
  CPacket           pkt("packet", *m_pInterp, &dict);

  // Adding should imply ownership...

  pkt.AddModule(m_pSegment);
  ASSERT(m_pSegment->isOwned());
  EQ(&pkt, m_pSegment->getOwner());

  // Removing should revoke ownership.

  try {
    pkt.RemoveModule(m_pSegment);
    ASSERT(!m_pSegment->isOwned());
    EQ((CPacket*)0, m_pSegment->getOwner());
  } 
  catch (string msg) {
    cerr << "Remove module exception: " << msg << endl;
  }


  // OnDelete should also imply revoking ownership:

  try {
    pkt.AddModule(m_pSegment);
    m_pSegment->OnDelete();
    ASSERT(!m_pSegment->isOwned());
    EQ((CPacket*)0, m_pSegment->getOwner());  }
  catch(string msg) {
    cerr << "Add module exception" << msg << endl;
  }



   
}
