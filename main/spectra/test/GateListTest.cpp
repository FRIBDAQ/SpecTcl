//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321


#ifndef GGATETEST_H
#define GGATETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>


#define private public
#define protected public
#include "MasterGateList.h"
#undef protected 
#undef private

#include "GlobalSettings.h"

#include <GateInfo.h>
#include <iostream>
#include <algorithm>
#include <memory>

using namespace std;

// ensure that pair can be printed
ostream& operator<<(ostream& stream, const pair<double, double>& p)
{
  stream << "(" << p.first << ", " << p.second << ")";
  return stream;
}

namespace Viewer
{

class GateListTest : public CppUnit::TestFixture
{
  private:
    unique_ptr<MasterGateList> m_pGateList;

  public:
    CPPUNIT_TEST_SUITE( GateListTest );
    CPPUNIT_TEST( addCut1D_0 );
    CPPUNIT_TEST( addCut2D_0 );
    CPPUNIT_TEST( removeCut1D_0 );
    CPPUNIT_TEST( removeCut2D_0 );
    CPPUNIT_TEST( synchronize_0 );
    CPPUNIT_TEST( synchronize_1 );
    CPPUNIT_TEST( synchronize_2 );
    CPPUNIT_TEST( synchronize_3 );
    CPPUNIT_TEST( synchronize_4 );
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() {
      m_pGateList.reset(new MasterGateList);
    }
    void tearDown() {
    }

  protected:
    void addCut1D_0();
    void addCut2D_0();
    void removeCut1D_0();
    void removeCut2D_0();
    void synchronize_0();
    void synchronize_1();
    void synchronize_2();
    void synchronize_3();
    void synchronize_4();

};

CPPUNIT_TEST_SUITE_REGISTRATION(GateListTest);

#endif

// Test that we can synchronize to the SpecTcl data
void GateListTest::addCut1D_0()
{
  SpJs::Slice slice("test", "xparam", 0, 1);
  
  m_pGateList->addCut1D(slice);

  CPPUNIT_ASSERT(1 == m_pGateList->size());
}

void GateListTest::addCut2D_0()
{
  
  SpJs::Band band("test", "xparam", "yparam", {{0, 1}, {1, 2}, {2, 3}});
  
  m_pGateList->addCut2D(band);

  CPPUNIT_ASSERT(1 == m_pGateList->size());
}

// test that synchronizing does not delete a gate unless the gate
// should no longer exist
void GateListTest::synchronize_0()
{  
  using SpJs::Band;
  Band* pBand0(new Band("test", "xparam", "yparam", {{0, 1}, {1, 2}, {2, 3}}));
  Band* pBand1(new Band("test1", "xparam", "yparam", {{0, 1}, {1, 2}, {2, 3}}));
  
  m_pGateList->addCut2D(*pBand0);
  CPPUNIT_ASSERT( 1 == m_pGateList->size() );

  vector<SpJs::GateInfo*> gates = {pBand0, pBand1};

  auto pGate = m_pGateList->find2D("test");

  // pass a new set of gates to the gate list...
  m_pGateList->synchronize(gates);

  // this takes ownership
  //
  // Make sure that the exact same cuts remain and that we did not delete them
  // when synchronized
  CPPUNIT_ASSERT( 2 == m_pGateList->size() );
  CPPUNIT_ASSERT( pGate == m_pGateList->find2D("test") );
  CPPUNIT_ASSERT( m_pGateList->find2D("test1") != m_pGateList->end2d() );

  auto pGate1 = m_pGateList->find2D("test1");
 
  m_pGateList->synchronize(gates);

  CPPUNIT_ASSERT( 2 == m_pGateList->size() );
  CPPUNIT_ASSERT( pGate == m_pGateList->find2D("test") );
  CPPUNIT_ASSERT( pGate1== m_pGateList->find2D("test1") );


}

// test that synchronizing causes the points of a band to update if changed
void GateListTest::synchronize_1()
{
  using SpJs::Band;
  Band* pBand0(new Band("test", "xparam", "yparam", {{0, 1}, {1, 2}, {2, 3}}));

  m_pGateList->addCut2D(*pBand0);
  CPPUNIT_ASSERT( 1 == m_pGateList->size() );

  // create a band with the same name and change the value of a point
  Band* pBand1(new Band(*pBand0));
  pBand1->setPoint(0, 4, 5);
  vector<SpJs::GateInfo*> gates = {pBand1};

  // pass a new set of gates to the gate list...
  bool wasChanged = m_pGateList->synchronize(gates);

  // gate list takes ownership
  auto itGate0 = m_pGateList->find2D("test");
  auto& pGate0 = (*itGate0);

  using dpair = pair<double, double>;
  CPPUNIT_ASSERT_MESSAGE( "Gate pGate0 is in the gate list",
                          m_pGateList->end2d() != itGate0 );
  CPPUNIT_ASSERT_EQUAL_MESSAGE( "Gates list changed return value",
                          true, wasChanged );
  CPPUNIT_ASSERT_EQUAL_MESSAGE( "First point is same",
                                dpair(4, 5),
                                pGate0->getPoint(0) );
  CPPUNIT_ASSERT_EQUAL_MESSAGE( "Second point is same",
                                dpair(1, 2),
                                pGate0->getPoint(1) );
  CPPUNIT_ASSERT_EQUAL_MESSAGE( "Third point is same",
                                dpair(2, 3),
                                pGate0->getPoint(2) );
}

  // test that synchronizing causes the points of a band to update if changed
  void GateListTest::synchronize_2()
  {
    using SpJs::Contour;
    Contour* pBand0(new Contour("test", "xparam", "yparam", {{0, 1}, {1, 2}, {2, 3}}));

    m_pGateList->addCut2D(*pBand0);
    CPPUNIT_ASSERT( 1 == m_pGateList->size() );

    // create a contour with the same name and change the value of a point
    Contour* pBand1(new Contour(*pBand0));
    pBand1->setPoint(0, 4, 5);
    vector<SpJs::GateInfo*> gates = {pBand1};

    // pass a new set of gates to the gate list...
    m_pGateList->synchronize(gates);

    // get the gate known by the list
    auto itGate0 = m_pGateList->find2D("test");
    // it is a unique_ptr so we have to take its reference
    auto& pGate0 = (*itGate0);

    using dpair = pair<double, double>;
    CPPUNIT_ASSERT_MESSAGE( "Gate pGate0 is in the gate list",
                            m_pGateList->end2d() != itGate0 );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "First point is same",
                                  dpair(4, 5),
                                  pGate0->getPoint(0) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Second point is same",
                                  dpair(1, 2),
                                  pGate0->getPoint(1) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Third point is same",
                                  dpair(2, 3),
                                  pGate0->getPoint(2) );
  }

// test that synchronizing causes the low limit of a slice to update
void GateListTest::synchronize_3()
{
      using SpJs::Slice;
      Slice* pBand0(new Slice("test", "xparam", 2, 3));

      m_pGateList->addCut1D(*pBand0);
      CPPUNIT_ASSERT( 1 == m_pGateList->size() );

      // create a slice with the same name and change the value of a point
      Slice* pBand1(new Slice(*pBand0));
      pBand1->setLowerLimit(5);
      pBand1->setUpperLimit(6);
      vector<SpJs::GateInfo*> gates = {pBand1};

      // pass a new set of gates to the gate list...
      m_pGateList->synchronize(gates);

      // get the gate known by the list
      auto itGate0 = m_pGateList->find1D("test");
      // it is a unique_ptr so we have to take its reference
      auto& pGate0 = (*itGate0);

      using dpair = pair<double, double>;
      CPPUNIT_ASSERT_MESSAGE( "Gate pGate0 is in the gate list",
                              m_pGateList->end1d() != itGate0 );
      CPPUNIT_ASSERT_EQUAL_MESSAGE( "Lower limit was updated",
                                    5.0, pGate0->getXLow() );

}

// test that synchronizing causes the high limits of a slice to update
void GateListTest::synchronize_4()
{
      using SpJs::Slice;
      Slice* pBand0(new Slice("test", "xparam", 2, 3));

      m_pGateList->addCut1D(*pBand0);
      CPPUNIT_ASSERT( 1 == m_pGateList->size() );

      // create a slice with the same name and change the value of a point
      Slice* pBand1(new Slice(*pBand0));
      pBand1->setLowerLimit(5);
      pBand1->setUpperLimit(6);
      vector<SpJs::GateInfo*> gates = {pBand1};

      // pass a new set of gates to the gate list...
      m_pGateList->synchronize(gates);

      // get the gate known by the list
      auto itGate0 = m_pGateList->find1D("test");
      // it is a unique_ptr so we have to take its reference
      auto& pGate0 = (*itGate0);

      using dpair = pair<double, double>;
      CPPUNIT_ASSERT_MESSAGE( "Gate pGate0 is in the gate list",
                              m_pGateList->end1d() != itGate0 );

      CPPUNIT_ASSERT_EQUAL_MESSAGE( "Upper limit was updated",
                                    6.0, pGate0->getXHigh() );

}

void GateListTest::removeCut1D_0()
{
  using SpJs::Slice;
  Slice* pSlice(new Slice("test", "xparam", 0, 1));
  
  m_pGateList->addCut1D(*pSlice);
  CPPUNIT_ASSERT( 1 == m_pGateList->size() );
  CPPUNIT_ASSERT( m_pGateList->find1D("test") != m_pGateList->end1d() );
  CPPUNIT_ASSERT( m_pGateList->find2D("test") == m_pGateList->end2d() );

  // pass a new set of gates to the gate list...
  m_pGateList->removeCut1D("test");

  // this takes ownership
  CPPUNIT_ASSERT( 0 == m_pGateList->size() );
  CPPUNIT_ASSERT( m_pGateList->find1D("test") == m_pGateList->end1d() );
  CPPUNIT_ASSERT( m_pGateList->find2D("test") == m_pGateList->end2d() );
}


void GateListTest::removeCut2D_0() {
  using SpJs::Band;
  Band* pGate(new Band("test", "xparam", "yparam", {{0, 1}, {1, 2}, {2, 3}}));

  m_pGateList->addCut2D(*pGate);
  CPPUNIT_ASSERT( 1 == m_pGateList->size() );
  CPPUNIT_ASSERT( m_pGateList->find1D("test") == m_pGateList->end1d() );
  CPPUNIT_ASSERT( m_pGateList->find2D("test") != m_pGateList->end2d() );

  // pass a new set of gates to the gate list...
  m_pGateList->removeCut2D("test");

  // this takes ownership
  CPPUNIT_ASSERT( 0 == m_pGateList->size() );
  CPPUNIT_ASSERT( m_pGateList->find1D("test") == m_pGateList->end1d() );
  CPPUNIT_ASSERT( m_pGateList->find2D("test") == m_pGateList->end2d() );
}

} // end of namespace
