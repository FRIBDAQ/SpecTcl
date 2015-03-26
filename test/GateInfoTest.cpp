#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>


#include "GateInfo.h"

#include "config.h"

#define private public
#define protected public
#include "GateInfo.h"
#undef protected
#undef private

using namespace std;

class GateInfoTest : public CppUnit::TestFixture {
  public:
  CPPUNIT_TEST_SUITE(GateInfoTest);
  CPPUNIT_TEST( compareBase_0 );
  CPPUNIT_TEST( parseSlice_0 );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:

  void compareBase_0();
  void parseSlice_0();
};

CPPUNIT_TEST_SUITE_REGISTRATION(GateInfoTest);

void GateInfoTest::compareBase_0()
{
    using SpJs::GateInfo;

    GateInfo gate0("gate0", SpJs::SliceGate);
    GateInfo gate1("gate0", SpJs::SliceGate);
    CPPUNIT_ASSERT(gate0 == gate0 );

    // different name
    gate1.s_name = "gate1";
    CPPUNIT_ASSERT(gate0 != gate1 );

    // different type
    gate1.s_name = "gate0";
    gate1.s_type = SpJs::BandGate;
    CPPUNIT_ASSERT( gate0 != gate1 );

}

void GateInfoTest::parseSlice_0()
{
}
