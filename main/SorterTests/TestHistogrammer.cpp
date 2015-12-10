

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include <Asserts.h>

#include "Histogrammer.h"

class TestHistogrammer : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(TestHistogrammer);
    CPPUNIT_TEST(testAddSpectrum_0);
    CPPUNIT_TEST_SUITE_END();


private:
    CHistogrammer* m_pHister;

public:
    void setUp() {
        m_pHister = new CHistogrammer(100);
    }

    void tearDown() {
        delete m_pHister;
    }


    void testAddSpectrum_0 () {

    }

};


CPPUNIT_TEST_SUITE_REGISTRATION(TestHistogrammer);
