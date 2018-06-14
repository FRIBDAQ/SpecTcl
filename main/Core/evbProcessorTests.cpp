// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Histogrammer.h"              // Since this has parameter dicts.
#include "SpecTcl.h"
#include "Globals.h"
#include "TreeParameter.h"
#include "Asserts.h"

// Open box testing:

#define private public
#include "CEventBuilderEventProcessor.h"
#undef private

class Testname : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(Testname);
  CPPUNIT_TEST(testInit);
  CPPUNIT_TEST_SUITE_END();


private:
    CHistogrammer*               m_pHistogrammer;
    CEventBuilderEventProcessor* m_processor;    
public:
  void setUp() {
    m_pHistogrammer = new CHistogrammer;
    gpEventSink     = m_pHistogrammer;
    m_processor    = new CEventBuilderEventProcessor(100.0, "evb");
  }
  void tearDown() {
    delete m_pHistogrammer;
    gpEventSink = nullptr;
    delete m_processor;
    
  }
protected:
  void testInit();
};

CPPUNIT_TEST_SUITE_REGISTRATION(Testname);

/** When the tree parameters have been registered:
 *  - the clock MHz should be right, as is the basename.  m_nEvnts is 0
 *  - handler map and time difference matrix is empty.
 *  - The tree parameters should be non null, have the right names
 *    and result in parameters.
 */

void Testname::testInit() {
    CEventBuilderEventProcessor& p(*m_processor);  // I'm typing lazy.
    
    EQ(100.0, p.m_ClockMHz);
    EQ(std::string("evb"), p.m_baseName);
    EQ(unsigned(0), p.m_nEvents);
}
