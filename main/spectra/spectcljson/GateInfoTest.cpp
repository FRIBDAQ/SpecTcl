#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>


#include "GateInfo.h"

#include "config.h"

#define private public
#define protected public
#include "GateInfo.h"
#undef protected
#undef private

#include <stdexcept>

using namespace std;

ostream& operator<<(ostream& str, const pair<double, double>& thePair)
{
    str << "(" << thePair.first << ", " << thePair.second << ")" << endl;
    return str;
}


class GateInfoTest : public CppUnit::TestFixture {
  public:
  CPPUNIT_TEST_SUITE(GateInfoTest);
  CPPUNIT_TEST( copySlice_0 );
  CPPUNIT_TEST( copyBand_0 );
  CPPUNIT_TEST( copyContour_0 );

  CPPUNIT_TEST( compareContour_0 );
  CPPUNIT_TEST( compareBand_0 );
  CPPUNIT_TEST( compareSlice_0 );

  CPPUNIT_TEST( contourFromBand_0 );
  CPPUNIT_TEST( bandFromContour_0 );

  CPPUNIT_TEST( bandSetPoint_0 );
  CPPUNIT_TEST( contourSetPoint_0 );
  CPPUNIT_TEST( bandSetPointOutOfRange_0 );
  CPPUNIT_TEST( contourSetPointOutOfRange_0 );

  CPPUNIT_TEST( bandGetPointOutOfRange_0 );
  CPPUNIT_TEST( contourGetPointOutOfRange_0 );
  
  // Gamma slices  (added by RF).
  
  CPPUNIT_TEST(copyGammaSlice_0);     // copy construction.
  CPPUNIT_TEST(compareGammaSlice_0);  // operator==, operator!=
  CPPUNIT_TEST(setGammaSliceParam_0);

  CPPUNIT_TEST(gamma2DConstruct_1);
  CPPUNIT_TEST(gamma2DConstruct_2);
  CPPUNIT_TEST(gamma2DConstruct_3);
  CPPUNIT_TEST(gamma2DConstruct_4);
  
  CPPUNIT_TEST(gamma2DClone_1);
  
  CPPUNIT_TEST(gamma2DSetParam0);               // set param 0.
  CPPUNIT_TEST(gamma2DSetParam1_1);             // set param 1 when param 0 exists
  CPPUNIT_TEST(gamma2DSetParam1_2);             // set param 1 when param 0 nonex.
  CPPUNIT_TEST(gamma2DSetParam_1);              // Replace existing parameter.
  CPPUNIT_TEST(gamma2DSetParam_2);              // replace nonex parameter
  CPPUNIT_TEST(gamma2DCompare_1);
  CPPUNIT_TEST(gamma2DCompare_2);
  
  CPPUNIT_TEST(gammaContourConstruct_1);         // Default constructor
  CPPUNIT_TEST(gammaContourConstruct_2);         // parameterized constructor.
  CPPUNIT_TEST(gammaContourConstrcut_3);         // Construct from GateInfo2D
  CPPUNIT_TEST(gammaContourConstruct_4);         // Copy construction.
  CPPUNIT_TEST(gammaContourClone);               // Test cloning.
  
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void copySlice_0();
  void copyGammaSlice_0();
  void copyBand_0();
  void copyContour_0();

  void compareSlice_0();
  void compareGammaSlice_0();
  void compareContour_0();
  void compareBand_0();
  template<class T> void compareGate2D(T& g0, T& g1);

  void contourFromBand_0();
  void bandFromContour_0();

  void bandSetPoint_0();
  void contourSetPoint_0();
  void bandSetPointOutOfRange_0();
  void contourSetPointOutOfRange_0();

  void bandGetPointOutOfRange_0();
  void contourGetPointOutOfRange_0();
  
  
  // Gamma slice only tests
  
  void setGammaSliceParam_0();
  
  // 2d gamma gates:
  
  void gamma2DConstruct_1();
  void gamma2DConstruct_2();
  void gamma2DConstruct_3();
  void gamma2DConstruct_4();
  
  void gamma2DClone_1();
  
  void gamma2DSetParam0();               // set param 0.
  void gamma2DSetParam1_1();             // set param 1 when param 0 exists
  void gamma2DSetParam1_2();
  void gamma2DSetParam_1();              // Replace existing parameter.
  void gamma2DSetParam_2();              // Replace nonex parameter
  void gamma2DCompare_1();
  void gamma2DCompare_2();
  
  // Gamma contour tests:
  
  void gammaContourConstruct_1();         // Default constructor
  void gammaContourConstruct_2();         // parameterized constructor.
  void gammaContourConstrcut_3();         // Construct from GateInfo2D
  void gammaContourConstruct_4();         // Copy construction.
  void gammaContourClone();               // Test cloning.
};

CPPUNIT_TEST_SUITE_REGISTRATION(GateInfoTest);

ostream& operator<<(ostream& os, std::vector<std::string> strings)
{
    os << "[";
    for (int i =0; i < strings.size(); i++) {
        os << strings[i] <<  " ";
    }
    os << "]";
    return os;
}

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

void GateInfoTest::copyGammaSlice_0()
{
    using SpJs::GammaSlice;
    std::vector<std::string> params = {"p1", "p2", "p3"};
    
    GammaSlice s0("gate0", params, 1, 2);
    GammaSlice s1(s0);
    
    CPPUNIT_ASSERT_EQUAL(s0.getParameters(), s1.getParameters());
    CPPUNIT_ASSERT_EQUAL(SpJs::GammaSliceGate, s0.getType());
    CPPUNIT_ASSERT_EQUAL(s0.getType(), s1.getType());
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

void GateInfoTest::compareGammaSlice_0()
{
    using SpJs::GammaSlice;
    std::vector<std::string> params = {"p1", "p2", "p3"};
    
    GammaSlice g0("slice0", params, 0, 1);
    GammaSlice g1(g0);
    CPPUNIT_ASSERT(g0 == g1);          // Don't want to implement << for gates.
    
    g1.setName("slice1");
    CPPUNIT_ASSERT(g0 != g1);
    g1.setName("slice0");
    
    // Lower and upper limit are fine due to common derivation from Cut.
    
    g1.appendParameter("p4");
    CPPUNIT_ASSERT(g0  != g1);          // Different parameters.
    g1.setAllParameters(params);        // in case we add more assertions.
    
}

void GateInfoTest::compareContour_0()
{
    using SpJs::Contour;

    Contour gate0("gate0", "a", "b", {{1,1}} );
    Contour gate1("gate0", "a", "b", {{1,1}} );
    CPPUNIT_ASSERT(gate0 == gate0 );

    compareGate2D(gate0, gate1);
}

void GateInfoTest::compareBand_0()
{
    using SpJs::Band;
    Band gate0("gate0", "a", "b", {{1,1}} );
    Band gate1("gate0", "a", "b", {{1,1}} );

    compareGate2D(gate0, gate1);
}

template<class T>
void GateInfoTest::compareGate2D(T& g0, T& g1)
{

    CPPUNIT_ASSERT(g0 == g0 );

    // different name
    g1.setName("g1");
    CPPUNIT_ASSERT(g0 != g1 );

    // different first param
    g1.setName("g0");
    g1.setParameter0("b");
    CPPUNIT_ASSERT( g1 != g0 );

    // different second param
    g1.setParameter0(g0.getParameter0());
    g1.setParameter1("c");
    CPPUNIT_ASSERT( g1 != g0 );

    // different lower limit
    g1.setParameter1(g1.getParameter0());
    g1.setPoints({{0,0}});
    CPPUNIT_ASSERT( g0 != g1 );
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

    // ensure that we cna set the value of point in a band
void GateInfoTest::bandSetPoint_0()
{
    using SpJs::Band;
    Band gate0("band", "a", "b", {{1, 1}});

    using dpair = pair<double, double>;
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Ensure construction succeeds",
                                  dpair(1, 1), gate0.getPoint(0));

    gate0.setPoint(0, 2, 3);

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Set point shoud have changed the value",
                                  dpair(2, 3), gate0.getPoint(0));
}

    // ensure that we can set the value of a point in a contour
void GateInfoTest::contourSetPoint_0()
{
    using SpJs::Contour;
    Contour gate0("contour", "a", "b", {{1, 1}});

    using dpair = pair<double, double>;
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Ensure construction succeeds",
                                  dpair(1, 1), gate0.getPoint(0));

    gate0.setPoint(0, 2, 3);

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Set point shoud have changed the value",
                                  dpair(2, 3), gate0.getPoint(0));
}

// ensure that you cannot set a nonexistent point...
void GateInfoTest::bandSetPointOutOfRange_0()
{
    using SpJs::Band;
    Band gate0("band", "a", "b", {{1, 1}});

    CPPUNIT_ASSERT_THROW( gate0.setPoint(1, 2, 3), std::out_of_range );
}

// ensure that you cannot set a nonexistent point
void GateInfoTest::contourSetPointOutOfRange_0()
{
    using SpJs::Contour;
    Contour gate0("contour", "a", "b", {{1, 1}});

    CPPUNIT_ASSERT_THROW( gate0.setPoint(1, 2 ,3), std::out_of_range );
}

void GateInfoTest::bandGetPointOutOfRange_0()
{
    using SpJs::Band;
    Band gate0("band", "a", "b", {{1, 1}});

    CPPUNIT_ASSERT_THROW( gate0.getPoint(20), std::out_of_range );
}

void GateInfoTest::contourGetPointOutOfRange_0()
{
    using SpJs::Contour;
    Contour gate0("contour", "a", "b", {{1, 1}});

    CPPUNIT_ASSERT_THROW( gate0.getPoint(20), std::out_of_range );
}

void GateInfoTest::setGammaSliceParam_0()
{
    using SpJs::GammaSlice;
    std::vector<std::string> params={"p1", "p1", "p3"};
    
    GammaSlice s("slice", params, 0, 1);
    s.setParameter("p2", 1);
    params[1] = "p2";
    CPPUNIT_ASSERT_EQUAL(params, s.getParameters());
    for (int i = 0; i < params.size(); i++) {
        CPPUNIT_ASSERT_EQUAL(params[i], s.getParameter(i));
    }
    
    s.appendParameter("p4");
    CPPUNIT_ASSERT(params != s.getParameters());
    CPPUNIT_ASSERT_EQUAL(std::string("p4"), s.getParameter(3));
    
    s.setAllParameters(params);
    CPPUNIT_ASSERT_EQUAL(params, s.getParameters());
    
    //  These can throw:
    
    CPPUNIT_ASSERT_THROW(s.setParameter("junk", 1234), std::out_of_range);
    CPPUNIT_ASSERT_THROW(s.getParameter(1234), std::out_of_range);
}

void GateInfoTest::gamma2DConstruct_1()
{
    using SpJs::Gamma2DGate;
    Gamma2DGate gate;
    CPPUNIT_ASSERT_EQUAL(std::string(""), gate.getName());
    SpJs::GateType t = gate.getType();
    CPPUNIT_ASSERT_EQUAL(SpJs::GammaContourGate, t);
    std::vector<std::string> ps;
    CPPUNIT_ASSERT_EQUAL(ps, gate.getParameters());
    
}
void GateInfoTest::gamma2DConstruct_2()
{
    using SpJs::Gamma2DGate;
    std::vector<std::string> params = {"p1", "p2", "p3"};
    
    Gamma2DGate gate("test-gate", params, SpJs::GammaContourGate);
    
    CPPUNIT_ASSERT_EQUAL(std::string("test-gate"), gate.getName());
    CPPUNIT_ASSERT_EQUAL(SpJs::GammaContourGate, gate.getType());
    CPPUNIT_ASSERT_EQUAL(params[0], gate.getParameter0());
    CPPUNIT_ASSERT_EQUAL(params[1], gate.getParameter1());
    CPPUNIT_ASSERT_EQUAL(params, gate.getParameters());
    CPPUNIT_ASSERT_EQUAL(params[2], gate.getParameter(2));
    CPPUNIT_ASSERT_THROW(
        gate.getParameter(3),
        std::out_of_range
    );
    
    
}
void GateInfoTest::gamma2DConstruct_3()
{
    std::vector<std::string> params = {"p1", "p2", "p3"};
    SpJs::Gamma2DGate g1("test-gate", params, SpJs::GammaContourGate);
    SpJs::GateInfo2D& gref(g1);
    
    SpJs::Gamma2DGate gate(gref);
    
    CPPUNIT_ASSERT_EQUAL(std::string("test-gate"), gate.getName());
    CPPUNIT_ASSERT_EQUAL(SpJs::GammaContourGate, gate.getType());
    CPPUNIT_ASSERT_EQUAL(params[0], gate.getParameter0());
    CPPUNIT_ASSERT_EQUAL(params[1], gate.getParameter1());
    CPPUNIT_ASSERT_EQUAL(params, gate.getParameters());
    CPPUNIT_ASSERT_EQUAL(params[2], gate.getParameter(2));
    CPPUNIT_ASSERT_THROW(
        gate.getParameter(3),
        std::out_of_range
    );    
}
void GateInfoTest::gamma2DConstruct_4()
{
    std::vector<std::string> params = {"p1", "p2", "p3"};
    SpJs::Gamma2DGate g1("test-gate", params, SpJs::GammaContourGate);
    SpJs::Gamma2DGate gate(g1);
    CPPUNIT_ASSERT_EQUAL(std::string("test-gate"), gate.getName());
    CPPUNIT_ASSERT_EQUAL(SpJs::GammaContourGate, gate.getType());
    CPPUNIT_ASSERT_EQUAL(params[0], gate.getParameter0());
    CPPUNIT_ASSERT_EQUAL(params[1], gate.getParameter1());
    CPPUNIT_ASSERT_EQUAL(params, gate.getParameters());
    CPPUNIT_ASSERT_EQUAL(params[2], gate.getParameter(2));
    CPPUNIT_ASSERT_THROW(
        gate.getParameter(3),
        std::out_of_range
    );    
}
void GateInfoTest::gamma2DClone_1()
{
    std::vector<std::string> params = {"p1", "p2", "p3"};
    SpJs::Gamma2DGate g1("test-gate", params, SpJs::GammaContourGate);
    std::unique_ptr<SpJs::GateInfo> pGate1 = g1.clone();
    SpJs::Gamma2DGate* pGate = dynamic_cast<SpJs::Gamma2DGate*>(pGate1.get());
    CPPUNIT_ASSERT(pGate);              // Ensures dyncast worked.
    
    CPPUNIT_ASSERT_EQUAL(std::string("test-gate"), pGate->getName());
    CPPUNIT_ASSERT_EQUAL(SpJs::GammaContourGate, pGate->getType());
    CPPUNIT_ASSERT_EQUAL(params[0], pGate->getParameter0());
    CPPUNIT_ASSERT_EQUAL(params[1], pGate->getParameter1());
    CPPUNIT_ASSERT_EQUAL(params, pGate->getParameters());
    CPPUNIT_ASSERT_EQUAL(params[2], pGate->getParameter(2));
    CPPUNIT_ASSERT_THROW(
        pGate->getParameter(3),
        std::out_of_range
    );    
}
void GateInfoTest::gamma2DSetParam0()
{
    SpJs::Gamma2DGate g("test-gate", {"p1", "p2"}, SpJs::GammaContourGate);
    g.setParameter0("param1");
    
    CPPUNIT_ASSERT_EQUAL(std::string("param1"), g.getParameter0());
}


void GateInfoTest::gamma2DSetParam1_1()
{
    SpJs::Gamma2DGate g("test-gate", {"p1", "p2"}, SpJs::GammaContourGate);
    g.setParameter1("param1");
    CPPUNIT_ASSERT_EQUAL(std::string("param1"), g.getParameter1());
}
void GateInfoTest::gamma2DSetParam1_2()
{
    SpJs::Gamma2DGate g;
    g.setParameter1("param1");
    CPPUNIT_ASSERT_EQUAL(std::string(""), g.getParameter0());
    CPPUNIT_ASSERT_EQUAL(std::string("param1"), g.getParameter1());
}

void GateInfoTest::gamma2DSetParam_1()
{
    SpJs::Gamma2DGate g("test-gate", {"p1", "p2"}, SpJs::GammaContourGate);
    g.setParameter(1, "NewParam");
    CPPUNIT_ASSERT_EQUAL(std::string("NewParam"), g.getParameter1());
}
void GateInfoTest::gamma2DSetParam_2()
{
    SpJs::Gamma2DGate g("test-gate", {"p1", "p2"}, SpJs::GammaContourGate);
    CPPUNIT_ASSERT_THROW(
        g.setParameter(2, "ShouldToss"),
        std::out_of_range
    );
}
void GateInfoTest::gamma2DCompare_1()
{
    std::vector<std::string> params={"p1", "p2"};
    SpJs::Gamma2DGate g1("test-gate", params, SpJs::GammaContourGate);
    SpJs::Gamma2DGate g2(g1);
    
    CPPUNIT_ASSERT(g1 == g2);
}
void GateInfoTest::gamma2DCompare_2()
{
    std::vector<std::string> params={"p1", "p2"};
    SpJs::Gamma2DGate g1("test-gate", params, SpJs::GammaContourGate);
    SpJs::Gamma2DGate g2(g1);
    std::string newp("newParam");
    g2.setParameter0(newp);
    CPPUNIT_ASSERT(g1 != g2);
}

void GateInfoTest::gammaContourConstruct_1()         // Default constructor
{
    SpJs::GammaContour g;
    CPPUNIT_ASSERT_EQUAL(std::string(""), g.getName());
    CPPUNIT_ASSERT_EQUAL(SpJs::GammaContourGate, g.getType());
    CPPUNIT_ASSERT(g.getParameters().empty());
    
}
void GateInfoTest::gammaContourConstruct_2()         // parameterized constructor.
{
    std::vector<std::string> params={"p1", "p2", "p3"};
    SpJs::GammaContour g("test-gate", params);
    CPPUNIT_ASSERT_EQUAL(std::string("test-gate"), g.getName());
    CPPUNIT_ASSERT_EQUAL(SpJs::GammaContourGate, g.getType());
    CPPUNIT_ASSERT_EQUAL(params, g.getParameters());
}
void GateInfoTest::gammaContourConstrcut_3()         // Construct from GateInfo2D
{
    std::vector<std::string> params = {"p1", "p2", "p3", "p4"};
    SpJs::GammaContour g1("test-gate", params);
    SpJs::GateInfo2D& gref(g1);
    SpJs::GammaContour g2(gref);
    
    CPPUNIT_ASSERT(g1 == g2);
}
void GateInfoTest::gammaContourConstruct_4()         // Copy construction.
{
    std::vector<std::string> params = {"p1", "p2", "p3", "p4"};
    SpJs::GammaContour g1("test-gate", params);
    SpJs::GammaContour g2(g1);
    
    CPPUNIT_ASSERT( g1 == g2);
}
void GateInfoTest::gammaContourClone()
{
    std::vector<std::string> params = {"p1", "p2", "p3", "p4"};
    SpJs::GammaContour g1("test-gate", params);
    std::unique_ptr<SpJs::GateInfo> p = g1.clone();
    SpJs::GammaContour* pG2 = dynamic_cast<SpJs::GammaContour*>(p.get());
    CPPUNIT_ASSERT(pG2);
    CPPUNIT_ASSERT(*pG2 == g1);
}