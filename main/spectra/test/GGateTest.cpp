#ifndef GGATETEST_H
#define GGATETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>


#define private public
#define protected public
#include "GGate.h"
#undef protected 
#undef private

#include "GlobalSettings.h"

#include <GateInfo.h>
#include <iostream>
#include <algorithm>

using namespace std;

namespace Viewer
{

  // ensure that pair can be printed
  ostream& operator<<(ostream& stream, const pair<double, double>& p)
  {
    stream << "(" << p.first << ", " << p.second << ")";
    return stream;
  }

class GGateTest : public CppUnit::TestFixture
{
  public:
    CPPUNIT_TEST_SUITE( GGateTest );
    CPPUNIT_TEST( construct_0 );
    CPPUNIT_TEST( synchronize_0 );
    CPPUNIT_TEST( synchronize_1 );
    CPPUNIT_TEST( getPoint_0 );
    CPPUNIT_TEST( equality_0 );
    CPPUNIT_TEST( inequality_0 );
    CPPUNIT_TEST( inequality_1 );
    CPPUNIT_TEST( inequality_2 );
    CPPUNIT_TEST( inequality_3 );
    CPPUNIT_TEST( closeContour_0 );
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() {
    }
    void tearDown() {
    }

  protected:
    void construct_0();
    void synchronize_0();
    void synchronize_1();
    void getPoint_0();
    void equality_0();
    void inequality_0();
    void inequality_1();
    void inequality_2();
    void inequality_3();

    void closeContour_0();
};

CPPUNIT_TEST_SUITE_REGISTRATION(GGateTest);

#endif

void GGateTest::construct_0()
{
  SpJs::Band band0("mycut", "x", "y", {{0, 1}, {1, 2}, {2, 3}});

  GGate gate(band0);
  auto pCut = gate.getGraphicObject();
  CPPUNIT_ASSERT( QString("mycut") == gate.getName() );
  CPPUNIT_ASSERT( QString("x") == gate.getParameterX() );
  CPPUNIT_ASSERT( QString("y") == gate.getParameterY() );


  double x[3] = {0, 1, 2};
  double y[3] = {1, 2, 3};
  CPPUNIT_ASSERT( 3 == pCut->GetN() );
  CPPUNIT_ASSERT( std::equal(x, x+3, pCut->GetX()) );
  CPPUNIT_ASSERT( std::equal(y, y+3, pCut->GetY()) );
  
}

// Test that we can synchronize to the SpecTcl data
void GGateTest::synchronize_0()
{
  SpJs::Band band0("mycut", "x", "y", {{0, 1}, {1, 2}, {2, 3}});

  GGate gate(band0);
  auto pCut = gate.getGraphicObject();
  pCut->RemovePoint(0);
  CPPUNIT_ASSERT( 2 == pCut->GetN() );

  // do the synchronization
  gate.synchronize(GGate::SpecTcl);

  double x[3] = {0, 1, 2};
  double y[3] = {1, 2, 3};
  CPPUNIT_ASSERT( 3 == pCut->GetN() );
  CPPUNIT_ASSERT( std::equal(x, x+3, pCut->GetX()) );
  CPPUNIT_ASSERT( std::equal(y, y+3, pCut->GetY()) );
  
}

// Ensure we can synchronize to the GUI data
void GGateTest::synchronize_1()
{
  SpJs::Band band0("mycut", "x", "y", {{0, 1}, {1, 2}, {2, 3}});

  GGate gate(band0);
  auto pCut = gate.getGraphicObject();
  pCut->Set(4);
  pCut->SetPoint(0, 4, 4);
  pCut->SetPoint(1, 5, 5);
  pCut->SetPoint(2, 6, 6);
  pCut->SetPoint(3, 7, 7);
  CPPUNIT_ASSERT( 4 == pCut->GetN() );

  // do the synchronization
  gate.synchronize(GGate::GUI);

  vector<pair<double, double> > points = {{4, 4}, {5, 5}, {6, 6}, {7, 7}};
  CPPUNIT_ASSERT( 4 == pCut->GetN() );
  CPPUNIT_ASSERT( points == gate.getPoints() );
  
}

void GGateTest::getPoint_0()
{
  using dpair = pair<double, double>;
  // create a band gate and enable access to the points...
  SpJs::Band band0("mycut", "x", "y", {{0, 1}, {1, 2}, {2, 3}});
  GGate gate(band0);
  CPPUNIT_ASSERT( dpair(0, 1) == gate.getPoint(0) );
  CPPUNIT_ASSERT( dpair(1, 2) == gate.getPoint(1) );
  CPPUNIT_ASSERT( dpair(2, 3) == gate.getPoint(2) );
}

void GGateTest::equality_0()
{
  GGate g0(SpJs::Band("mycut", "x", "y", {{0, 1}, {1, 2}, {2, 3}}));
  GGate g1(SpJs::Band("mycut", "x", "y", {{0, 1}, {1, 2}, {2, 3}}));

  CPPUNIT_ASSERT_MESSAGE( "Gates should be equal", g0 == g1 );
}

void GGateTest::inequality_0()
{
  GGate g0(SpJs::Band("mycut", "x", "y", {{0, 1}, {1, 2}, {2, 3}}));
  GGate g1(SpJs::Band("mycut2", "x", "y", {{0, 1}, {1, 2}, {2, 3}}));

  CPPUNIT_ASSERT_MESSAGE( "Gates with different names are not equal", g0 != g1 );
}

  void GGateTest::inequality_1()
  {
    GGate g0(SpJs::Band("mycut", "x", "y", {{0, 1}, {1, 2}, {2, 3}}));
    GGate g1(SpJs::Band("mycut", "x1", "y", {{0, 1}, {1, 2}, {2, 3}}));

  CPPUNIT_ASSERT_MESSAGE( "Gates with different x parameters are not equal", g0 != g1 );
  }

  void GGateTest::inequality_2()
  {
    GGate g0(SpJs::Band("mycut", "x", "y", {{0, 1}, {1, 2}, {2, 3}}));
    GGate g1(SpJs::Band("mycut", "x", "y2", {{0, 1}, {1, 2}, {2, 3}}));

  CPPUNIT_ASSERT_MESSAGE( "Gates with different y parameters are not equal", g0 != g1 );
  }

  void GGateTest::inequality_3()
  {
    GGate g0(SpJs::Band("mycut", "x", "y", {{0, 1}, {1, 2}, {2, 3}}));
  GGate g1(SpJs::Band("mycut", "x", "y", {{1, 2}, {2, 3}}));

  CPPUNIT_ASSERT_MESSAGE( "Gates with different points are not equal", g0 != g1 );
  }

  void GGateTest::closeContour_0() 
{

  // if we pass in info without the data points forming a closed entity,
  // the graphical object should be closed
    GGate g0(SpJs::Contour("mycut", "x", "y", {{0, 1}, {1, 2}, {2, 3}}));
    auto pCut = g0.getGraphicObject();

    int nPoints = pCut->GetN();
    auto pX = pCut->GetX();
    auto pY = pCut->GetY();

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "First and last x values should be equal",
                                    pX[0], pX[nPoints-1] );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "First and last y values should be equal",
                                    pY[0], pY[nPoints-1] );
}

} // end of namespace
