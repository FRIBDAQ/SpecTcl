

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <Asserts.h>

#include <DisplayManager.h>

class DisplayInterfaceTests : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(DisplayInterfaceTests);
    CPPUNIT_TEST(addDisplay_0);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {}


    void addDisplay_0 () {

    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(DisplayInterfaceTests);
