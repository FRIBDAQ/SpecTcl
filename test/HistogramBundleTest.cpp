#ifndef HISTOGRAMBUNDLETEST_H
#define HISTOGRAMBUNDLETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>


#define private public
#define protected public
#include "HistogramBundle.h"
#undef protected 
#undef private

#include "GlobalSettings.h"
#include "GSlice.h"
#include "GGate.h"
#include "GateList.h"

#include <GateInfo.h>
#include <QHistInfo.h>
#include <HistFactory.h>

#include <QList>
#include <QString>

#include <iostream>
#include <memory>
#include <iterator>
#include <algorithm>

using namespace std;

namespace Viewer
{

class HistogramBundleTest : public CppUnit::TestFixture
{
  private:
    unique_ptr<HistogramBundle> m_pHist0;
    unique_ptr<GateList> m_pGateList;

  public:
    CPPUNIT_TEST_SUITE( HistogramBundleTest );
    CPPUNIT_TEST( synchronizeGates_0 );
    CPPUNIT_TEST( synchronizeGates_1 );
    CPPUNIT_TEST( synchronizeGates_2 );
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp();
    void tearDown();

  protected:
    void synchronizeGates_0();
    void synchronizeGates_1();
    void synchronizeGates_2();

};

CPPUNIT_TEST_SUITE_REGISTRATION(HistogramBundleTest);

#endif

void HistogramBundleTest::setUp() 
{
  // create histogram bundles
  unique_ptr<QMutex> mutex0(new QMutex);
  SpJs::HistInfo info0   = {"hist0", 1, {"xparam"}, {{0, 10, 10}}, SpJs::Long};
  unique_ptr<TH1> pHist0 = SpJs::HistFactory().create(info0);
  m_pHist0.reset(new HistogramBundle(std::move(mutex0), 
                                     std::move(pHist0), 
                                     info0));

  m_pGateList.reset(new GateList);

}


void HistogramBundleTest::tearDown() 
{
}

void HistogramBundleTest::synchronizeGates_0()
{
  m_pGateList->addCut1D(SpJs::Slice("slice0", "xparam", 0, 1));
  m_pGateList->addCut2D(SpJs::Band("gate0", "x", "y", {{0, 1}, {1, 2}, {2, 3}}));

  bool changed = m_pHist0->synchronizeGates(m_pGateList.get());

  CPPUNIT_ASSERT_EQUAL_MESSAGE("Histogram has one cut", 
      size_t(1), 
      m_pHist0->getCut1Ds().size());

  auto& cuts = m_pHist0->getCut1Ds();
  CPPUNIT_ASSERT_MESSAGE("Histogram registered slice", 
      cuts.end() != cuts.find("slice0"));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("Returns true if gate was added", 
      true, changed);
}

void HistogramBundleTest::synchronizeGates_1()
{
  m_pGateList->addCut2D(SpJs::Band("gate0", "x", "y", {{0, 1}, {1, 2}, {2, 3}}));

  bool changed = m_pHist0->synchronizeGates(m_pGateList.get());

  auto& cuts = m_pHist0->getCut2Ds();
  CPPUNIT_ASSERT_MESSAGE("1D Histogram did not registered 2d gate", 
      cuts.end() == cuts.find("gate0"));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("Returns false if nothing changed", 
      false, changed);
}

void HistogramBundleTest::synchronizeGates_2()
{
  auto& cuts = m_pHist0->getCut1Ds();
  GSlice slice(SpJs::Slice("test", "x", 0, 1));

  cuts.insert(make_pair(QString("test"), &slice));

  m_pGateList->clear();

  CPPUNIT_ASSERT_EQUAL_MESSAGE("B4 sync there is one slice",
      size_t(1), cuts.size());

  bool changed = m_pHist0->synchronizeGates(m_pGateList.get());

  CPPUNIT_ASSERT_EQUAL_MESSAGE("After sync all 1d gates removed",
      size_t(0), cuts.size());
  CPPUNIT_ASSERT_EQUAL_MESSAGE("Removal of a gate is a change", 
      true, changed);
}


} // end of namespace
