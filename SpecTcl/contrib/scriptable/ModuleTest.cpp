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
#include <RangeError.h>
#include <Parameter.h>
#include <tcl.h>
#include "Asserts.h"
#include <CModule.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

extern void* gpEventSink;

// Segment unpackers are abstract base classes we need to
// derive a minimally functional class to be able
// to test the segment unpacker.
//
class ConcreteModule : public CModule
{
public:
  ConcreteModule(const string& rname, CTCLInterpreter& rInterp) :
    CModule(rname, rInterp) {}

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


class ModuleTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ModuleTest);
  CPPUNIT_TEST(ConstructTest);	//!< Test construction.
  CPPUNIT_TEST(CreateTest);	//!< Test map creation/init.
  CPPUNIT_TEST(MapTest);	//!< Test map with entries.
  CPPUNIT_TEST_SUITE_END();


private:
  Tcl_Interp*       m_pRawInterpreter;
  CTCLInterpreter*  m_pInterp;
  ConcreteModule*   m_pModule;

public:
  // Need to create an interpreter and its associated object.

  void setUp() {
    m_pRawInterpreter = Tcl_CreateInterp();
    m_pInterp         = new CTCLInterpreter(m_pRawInterpreter);
    m_pModule         = new ConcreteModule("Map", *m_pInterp);
    gpEventSink       = new CHistogrammer(1024*1024);
  }
  // Destroy the interpreter and its associated object
  void tearDown() {
    delete m_pModule;
    delete m_pInterp;
    Tcl_DeleteInterp(m_pRawInterpreter);
    delete (CHistogrammer*)gpEventSink;
  }
protected:
  void ConstructTest();
  void CreateTest();
  void MapTest();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ModuleTest);



/// The tests:

//
// On construction ,the parameter map must be empty, so all
// Id requests will fail.  We try < 0 and 0.
//
void ModuleTest::ConstructTest() {

  bool ok;

  // Try negative.

  try {
    m_pModule->Id(-1);
    ok = false;
  }
  catch(CRangeError& re) {
    ok = true;
  }
  catch (...) {
    ok = false;
  }
  ASSERT(ok);

  // Try the smallest possible.

  try {
    m_pModule->Id(0);
    ok = false;
  }
  catch(CRangeError& re) {
    ok = true;
  }
  catch (...) {
    ok = false;
  }
  ASSERT(ok);

}
//
// On map creation:
//   All map entries must be initialized to -1.
//   Id's of entries off the end of the map will throw.
//
void
ModuleTest::CreateTest()
{
  bool ok;

  m_pModule->CreateMap(10);	// entries 0-9.

  
  //  Negative should always throw:

  try {
    m_pModule->Id(-1);
    ok = false;
  }
  catch(CRangeError& re) {
    ok = true;
  }
  catch (...) {
    ok = false;
  }
  ASSERT(ok);

  // 10 should throw.

  try {
    m_pModule->Id(10);
    ok = false;
  }
  catch(CRangeError& re) {
    ok = true;
  }
  catch (...) {
    ok = false;
  }
  ASSERT(ok);

  // All entries should be populated with -1:

  for(int i =0; i < 10; i++) {
    EQ(-1, m_pModule->Id(i));
  }

}
//
// Insert parameters in the map:
//  - Parameters should Id correctly in the right spot.
//  - Parameters that are not mapped should be -1.
//
void
ModuleTest::MapTest()
{
  CHistogrammer* pHist = (CHistogrammer*)gpEventSink;
  pHist->AddParameter("p1", 0, "");
  pHist->AddParameter("p2", 1, "");


  //  Make a map entry with items at 0, and 9:

  m_pModule->CreateMap(10);
  m_pModule->MapElement(0, "p1");
  m_pModule->MapElement(9, "p2");

  // Ensure that these entries worked:

  EQ(0, m_pModule->Id(0));
  EQ(1, m_pModule->Id(9));

  // Ensure no extras got made:

  for(int i =1; i < 9; i++) {
    EQ(-1, m_pModule->Id(i));
  }

  // clean up.


  delete pHist->RemoveParameter("p1");
  delete pHist->RemoveParameter("p2");
}
