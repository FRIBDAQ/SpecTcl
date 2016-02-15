

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include "Asserts.h"
#include "XamineShMemDisplayImpl.h"
#include "TestXamineShMem.h"

#include <memory>
#include <exception>

using namespace std;

class XamineShMemDisplayImplTests : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(XamineShMemDisplayImplTests);
    CPPUNIT_TEST(addSpectrum_0);
    CPPUNIT_TEST_SUITE_END();


private:
    std::unique_ptr<CXamineShMemDisplayImpl> m_pCol;

public:
    void setUp() {
        shared_ptr<CXamineSharedMemory> pShMem(new CTestXamineShMem);
        m_pCol.reset(new CXamineShMemDisplayImpl( pShMem ) );
    }

    void tearDown() {

    }

    void addSpectrum_0() {

    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(XamineShMemDisplayImplTests);
