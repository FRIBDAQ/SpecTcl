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

class GGateTest : public CppUnit::TestFixture
{
  public:
    CPPUNIT_TEST_SUITE( GGateTest );
    CPPUNIT_TEST( construct_0 );
    CPPUNIT_TEST( synchronize_0 );
    CPPUNIT_TEST( synchronize_1 );
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

} // end of namespace
