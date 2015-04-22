#ifndef GGATETEST_H
#define GGATETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>


#define private public
#define protected public
#include "GateList.h"
#undef protected 
#undef private

#include "GlobalSettings.h"

#include <GateInfo.h>
#include <iostream>
#include <algorithm>
#include <memory>

using namespace std;

namespace Viewer
{

class GateListTest : public CppUnit::TestFixture
{
  private:
    unique_ptr<GateList> m_pGateList;

  public:
    CPPUNIT_TEST_SUITE( GateListTest );
    CPPUNIT_TEST( addCut1D_0 );
    CPPUNIT_TEST( addCut2D_0 );
    CPPUNIT_TEST( removeCut1D_0 );
    CPPUNIT_TEST( removeCut2D_0 );
    CPPUNIT_TEST( synchronize_0 );
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() {
      m_pGateList.reset(new GateList);
    }
    void tearDown() {
    }

  protected:
    void addCut1D_0();
    void addCut2D_0();
    void removeCut1D_0();
    void removeCut2D_0();
    void synchronize_0();
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
