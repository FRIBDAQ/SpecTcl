#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "CTreeParameter.h"
#include "CTreeException.h"
#define private public
#include "CTreeParameterVector.h"
#include "CVectorGates.h"
#undef private
#include <Event.h>
#include <Parameter.h>

// #include "TreeTestSupport.h"

#include <stdexcept>


class VGateTests : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(VGateTests);
    CPPUNIT_TEST_SUITE_END();


public:
    void tearDown() {
        CTreeParameterVector::ClearMap();
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(VGateTests);