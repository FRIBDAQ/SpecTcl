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

  CPPUNIT_TEST( compareContour_0 );
  CPPUNIT_TEST( compareSlice_0 );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:


  void compareSlice_0();
  void compareContour_0();
};

CPPUNIT_TEST_SUITE_REGISTRATION(GateInfoTest);


void GateInfoTest::compareSlice_0()
{
    using SpJs::Slice;

    Slice gate0("gate0", "a", 0, 1);
    Slice gate1("gate0", "a", 0, 1);
    CPPUNIT_ASSERT(gate0 == gate0 );

    // different name
    gate1.setName("gate1");
    CPPUNIT_ASSERT(gate0 != gate1 );

    // different type
    gate1.setName("gate0");
    gate1.setParameter("b");
    CPPUNIT_ASSERT( gate0 != gate1 );

    // different type
    gate1.setParameter( gate0.getParameter() );
    gate1.setLowerLimit(1);
    CPPUNIT_ASSERT( gate0 != gate1 );

    gate1.setLowerLimit( gate0.getLowerLimit() );
    gate1.setUpperLimit(0);
    CPPUNIT_ASSERT( gate0 != gate1 );

}

void GateInfoTest::compareContour_0()
{
    using SpJs::Contour;

    Contour gate0("gate0", "a", "b", {{1,1}} );
    Contour gate1("gate0", "a", "b", {{1,1}} );
    CPPUNIT_ASSERT(gate0 == gate0 );

    // different name
    gate1.setName("gate1");
    CPPUNIT_ASSERT(gate0 != gate1 );

    // different first param
    gate1.setName("gate0");
    gate1.setParameter0("b");
    CPPUNIT_ASSERT( gate1 != gate0 );

    // different second param
    gate1.setParameter0(gate0.getParameter0());
    gate1.setParameter1("c");
    CPPUNIT_ASSERT( gate1 != gate0 );

    // different lower limit
    gate1.setParameter1(gate1.getParameter0());
    gate1.setPoints({{0,0}});
    CPPUNIT_ASSERT( gate0 != gate1 );

}
