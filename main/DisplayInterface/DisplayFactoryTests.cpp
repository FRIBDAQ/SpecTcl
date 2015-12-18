

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <Asserts.h>

#include <DisplayFactory.h>

class DisplayFactoryTests : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(DisplayFactoryTests);
    CPPUNIT_TEST(addDisplay_0);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
    }

    void tearDown() {}


    void addDisplay_0 () {

    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(DisplayFactoryTests);
