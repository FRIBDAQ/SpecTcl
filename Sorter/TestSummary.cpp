// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <vector>

#include "Parameter.h"
#include "Event.h"
#include "SummarySpectrumB.h"
#include "SummarySpectrumW.h"
#include "SummarySpectrumL.h"



class TestSummary : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TestSummary);
  CPPUNIT_TEST(aTest);
  CPPUNIT_TEST_SUITE_END();

  

private:
    std::pair<std::vector<unsigned>, std::vector<unsigned> >
        fillB(std::vector<int> chans, unsigned n);
public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void aTest();
};

// Utilities to fill spectra and report the stats.
// for all of them parameter is a vector of channel increments, n the y chans.
// and the return is a pair where first is underflows and second overflows.
//

// Summary..B

std::pair<std::vector<unsigned>, std::vector<unsigned> >
TestSummary::fillB(std::vector<int> chans, unsigned n)
{
    // make the parameters-- no dict so we give them all the same name:
    
    std::vector<CParameter> parameters;
    CEvent                  e;
    for (int i = 0; i < chans.size(); i ++) {
        parameters.push_back(CParameter("test", i, "arb"));
        e[i] = chans[i];
    }
    CSummarySpectrumB spec("test", 1, parameters, n);
    
    spec(e);
    return std::pair<std::vector<unsigned>, std::vector<unsigned> >(
        spec.getUnderflows(), spec.getOverflows()
    );
}


CPPUNIT_TEST_SUITE_REGISTRATION(TestSummary);

void TestSummary::aTest() {
}
