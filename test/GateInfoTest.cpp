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
  CPPUNIT_TEST( copySlice_0 );
  CPPUNIT_TEST( copyBand_0 );
  CPPUNIT_TEST( copyContour_0 );
  CPPUNIT_TEST( compareContour_0 );
  CPPUNIT_TEST( compareSlice_0 );

  CPPUNIT_TEST( contourFromBand_0 );
  CPPUNIT_TEST( bandFromContour_0 );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void copySlice_0();
  void copyBand_0();
  void copyContour_0();

  void compareSlice_0();
  void compareContour_0();

  void contourFromBand_0();
  void bandFromContour_0();
};

CPPUNIT_TEST_SUITE_REGISTRATION(GateInfoTest);

void GateInfoTest::copySlice_0()
{
    using SpJs::Slice;

    Slice gate0 ("gate0", "a", 1, 2);
    Slice gate1(gate0);

    CPPUNIT_ASSERT_EQUAL(gate0.getParameter(), gate1.getParameter());
    CPPUNIT_ASSERT_EQUAL(gate0.getLowerLimit(), gate1.getLowerLimit());
    CPPUNIT_ASSERT_EQUAL(gate0.getUpperLimit(), gate1.getUpperLimit());
    CPPUNIT_ASSERT_EQUAL(gate0.getType(), gate1.getType());
}

void GateInfoTest::copyBand_0()
{
    using SpJs::Band;

    Band gate0 ("gate0", "a", "b", {{0, 1}, {1, 2}});
    Band gate1(gate0);

    CPPUNIT_ASSERT_EQUAL(gate0.getParameter0(), gate1.getParameter0());
    CPPUNIT_ASSERT_EQUAL(gate0.getParameter1(), gate1.getParameter1());
    CPPUNIT_ASSERT(gate0.getPoints() == gate1.getPoints());
    CPPUNIT_ASSERT_EQUAL(gate0.getType(), gate1.getType());
}

void GateInfoTest::copyContour_0()
{
    using SpJs::Contour;

    Contour gate0 ("gate0", "a", "b", {{0, 1}, {1, 2}});
    Contour gate1(gate0);

    CPPUNIT_ASSERT_EQUAL(gate0.getParameter0(), gate1.getParameter0());
    CPPUNIT_ASSERT_EQUAL(gate0.getParameter1(), gate1.getParameter1());
    CPPUNIT_ASSERT(gate0.getPoints() == gate1.getPoints());
    CPPUNIT_ASSERT_EQUAL(gate0.getType(), gate1.getType());
}

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

void GateInfoTest::contourFromBand_0()
{
    using SpJs::Band;
    using SpJs::Contour;

    Band gate0("band", "a", "b", {{1,1}} );
    Contour cont(gate0);

    CPPUNIT_ASSERT_EQUAL( SpJs::ContourGate, cont.getType());
    CPPUNIT_ASSERT_EQUAL( gate0.getName(), cont.getName());
    CPPUNIT_ASSERT_EQUAL( gate0.getParameter0(), cont.getParameter0());
    CPPUNIT_ASSERT_EQUAL( gate0.getParameter1(), cont.getParameter1());
    CPPUNIT_ASSERT( gate0.getPoints() == cont.getPoints());

}

void GateInfoTest::bandFromContour_0()
{
    using SpJs::Band;
    using SpJs::Contour;

    Contour gate0("band", "a", "b", {{1,1}} );
    Band cont(gate0);

    CPPUNIT_ASSERT_EQUAL( SpJs::BandGate, cont.getType());
    CPPUNIT_ASSERT_EQUAL( gate0.getName(), cont.getName());
    CPPUNIT_ASSERT_EQUAL( gate0.getParameter0(), cont.getParameter0());
    CPPUNIT_ASSERT_EQUAL( gate0.getParameter1(), cont.getParameter1());
    CPPUNIT_ASSERT( gate0.getPoints() == cont.getPoints());

}
