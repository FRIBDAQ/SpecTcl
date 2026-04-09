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

#include "TreeTestSupport.h"

#include <stdexcept>


class VGateTests : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(VGateTests);
    CPPUNIT_TEST_SUITE_END();

private:
    CTreeParameterVector* m_pVector;
public:
    void setUp() {
        m_pVector = new CTreeParameterVector("test-vector");
    }
    void tearDown() {
        delete m_pVector;
        CTreeParameterVector::ClearMap();
        TreeTestSupport::ClearMap();
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(VGateTests);