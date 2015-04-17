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

class GateListTest : public CppUnit::TestFixture
{
  private:
    unique_ptr<GateList> m_pGateList;

  public:
    CPPUNIT_TEST_SUITE( GateListTest );
    CPPUNIT_TEST( addCut1D_0 );
    CPPUNIT_TEST( addCut2D_0 );
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

  // pass a new set of gates to the gate list...
  m_pGateList->synchronize(gates);

  // this takes ownership
  CPPUNIT_ASSERT( 2 == m_pGateList->size() );
  CPPUNIT_ASSERT( m_pGateList->find2D("test") != m_pGateList->end2d() );
  CPPUNIT_ASSERT( m_pGateList->find2D("test1") != m_pGateList->end2d() );

}
