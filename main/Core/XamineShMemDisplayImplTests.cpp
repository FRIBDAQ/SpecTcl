

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include "Asserts.h"
#include "XamineShMemDisplayImpl.h"

#include <memory>
#include <exception>

class XamineShMemDisplayImplTests : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(XamineShMemDisplayImplTests);
    CPPUNIT_TEST_SUITE_END();


private:
    std::unique_ptr<CXamineShMemDisplayImpl> m_pCol;

public:
    void setUp() {
        m_pCol.reset(new CXamineShMemDisplayImpl(10));
    }

    void tearDown() {
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(XamineShMemDisplayImplTests);
