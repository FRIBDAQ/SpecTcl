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
#include "MasterGateList.h"

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
    unique_ptr<HistogramBundle> m_pHist0;    // 1-d histogram
    unique_ptr<HistogramBundle> m_pHist1;    // 2-d histogram
    unique_ptr<HistogramBundle> m_pHist2;    // 1d Gamma histogram.
    
    unique_ptr<MasterGateList> m_pGateList;

  public:
    CPPUNIT_TEST_SUITE( HistogramBundleTest );
    CPPUNIT_TEST( synchronizeGates_0 );
    CPPUNIT_TEST( synchronizeGates_1 );
    CPPUNIT_TEST( synchronizeGates_2 );
    
    CPPUNIT_TEST(synchronizeGates_3);   // Gamma gate matches multi param 1d
    CPPUNIT_TEST(synchronizeGates_4);   // Gamma gate doesn't match ord. 1d.
    CPPUNIT_TEST(synchronizeGates_5);   // Ordinary slice no match in  gamma spec.
    
    CPPUNIT_TEST(synchronizeGates_6);   // 2d histogram matches for band, contour not slice.
    CPPUNIT_TEST(synchronizeGates_7);   // 2d Histogram doesn't match for gamma slice with same params.
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp();
    void tearDown();

  protected:
    void synchronizeGates_0();
    void synchronizeGates_1();
    void synchronizeGates_2();
    
    void synchronizeGates_3();
    void synchronizeGates_4();
    void synchronizeGates_5();
    void synchronizeGates_6();
    void synchronizeGates_7();
};

CPPUNIT_TEST_SUITE_REGISTRATION(HistogramBundleTest);

#endif

void HistogramBundleTest::setUp() 
{
  // create histogram bundles  1-D:
  
  unique_ptr<QMutex> mutex0(new QMutex);
  SpJs::HistInfo info0   = {"hist0", "1", {"xparam"}, {{0, 10, 10}}, SpJs::Long};
  unique_ptr<TH1> pHist0 = SpJs::HistFactory().create(info0);
  m_pHist0.reset(new HistogramBundle(std::move(mutex0), 
                                     std::move(pHist0), 
                                     info0));

  // 2-d bundle.
                                     
  SpJs::HistInfo info1 = {"hist1", "2", {"xparam", "yparam"}, {{0, 10, 10}, {0, 10, 10}}, SpJs::Long};
  unique_ptr<TH1> pHist1 = SpJs::HistFactory().create(info1);
  m_pHist1.reset(new HistogramBundle(std::move(mutex0), std::move(pHist1), info1));
  
  // Gamma spectrum bundle:
  
  SpJs::HistInfo info2 = {"hist3", "g1", {"param1", "param2", "param3"}, {{0, 10, 10}}, SpJs::Long};
  unique_ptr<TH1> pHist2 = SpJs::HistFactory().create(info2);
  m_pHist2.reset(new HistogramBundle(std::move(mutex0), std::move(pHist2), info2));
  
  m_pGateList.reset(new MasterGateList);
  
  

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
//    CPPUNIT_TEST(synchronizeGates_3);   // Gamma gate matches multi param 1d

void HistogramBundleTest::synchronizeGates_3()
{

  SpJs::GammaSlice slice("test",
      {std::string("param1"), std::string("param2"), std::string("param3")}, 0, 1);
  m_pGateList->addCut1D(slice);
  
  CPPUNIT_ASSERT(m_pHist2->synchronizeGates(m_pGateList.get()));
  auto& cuts = m_pHist2->getCut1Ds();
  CPPUNIT_ASSERT_EQUAL(size_t(1), cuts.size());
  
  CPPUNIT_ASSERT(cuts.end() != cuts.find("test"));
}



//    CPPUNIT_TEST(synchronizeGates_4);   // Gamma gate doesn't match ord. 1d.

void HistogramBundleTest::synchronizeGates_4()
{
  SpJs::GammaSlice slice("test",
      {std::string("param1"), std::string("param2"), std::string("param3")}, 0, 1);
  m_pGateList->addCut1D(slice);
  
  CPPUNIT_ASSERT(!m_pHist0->synchronizeGates(m_pGateList.get()));  
}

//    CPPUNIT_TEST(synchronizeGates_5);   // Ordinary slice no match in  gamma spec.
  
void HistogramBundleTest::synchronizeGates_5()
{
  SpJs::Slice slice("test", "param1", 0, 1);
  m_pGateList->addCut1D(slice);
  CPPUNIT_ASSERT(!m_pHist2->synchronizeGates(m_pGateList.get()));
}
//    CPPUNIT_TEST(synchronizeGates_6);   // 2d histogram matches for band, contour not slice.

void HistogramBundleTest::synchronizeGates_6()
{
  std::vector<std::pair<double, double>> points={{0,0}, {10,10}, {20,20}};
  SpJs::Band band("test", "xparam", "yparam", points);
  
  m_pGateList->addCut2D(band);
  CPPUNIT_ASSERT(m_pHist1->synchronizeGates(m_pGateList.get()));   // Should add.
  
  auto& cuts = m_pHist1->getCut2Ds();
  CPPUNIT_ASSERT_EQUAL(size_t(1), cuts.size());
  CPPUNIT_ASSERT(cuts.end() != cuts.find("test"));

}

//     CPPUNIT_TEST(synchronizeGates_7);   // 2d Histogram doesn't match for gamma slice with same params.

void HistogramBundleTest::synchronizeGates_7()
{
  SpJs::GammaSlice slice("test",
    {std::string("xparam"), std::string("yparam")}, 0, 1);
  m_pGateList->addCut1D(slice);
  
  CPPUNIT_ASSERT(!m_pHist1->synchronizeGates(m_pGateList.get()));
}

} // end of namespace
