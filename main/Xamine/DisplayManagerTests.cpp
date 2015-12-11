

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include <Asserts.h>

#include <DisplayManager.h>

class DisplayManagerTests : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(DisplayManagerTests);
    CPPUNIT_TEST(addDisplay_0);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {}


    void addDisplay_0 () {
        FAIL("addDisplay_0 not implemented");
    }

};
