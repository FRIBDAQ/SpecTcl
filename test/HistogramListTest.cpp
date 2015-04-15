#ifndef GGATETEST_H
#define GGATETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>


#define private public
#define protected public
#include "HistogramList.h"
#undef protected 
#undef private

#include "GlobalSettings.h"
#include "GSlice.h"
#include "GGate.h"

#include <GateInfo.h>
#include <HistInfo.h>
#include <HistFactory.h>

#include <QList>
#include <QString>

#include <iostream>
#include <memory>
#include <iterator>
#include <algorithm>

using namespace std;

class HistogramListTest : public CppUnit::TestFixture
{
  private:
    unique_ptr<HistogramBundle> m_pHist0;
    unique_ptr<HistogramBundle> m_pHist1;
    unique_ptr<HistogramBundle> m_pHist2;
    unique_ptr<HistogramBundle> m_pHist3;

  public:
    CPPUNIT_TEST_SUITE( HistogramListTest );
    CPPUNIT_TEST( histNames_0 );
    CPPUNIT_TEST( removeSlice_0 );
    CPPUNIT_TEST( removeGate_0 );
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp();
    void tearDown();

  protected:
    void histNames_0();
    void removeSlice_0();
    void removeGate_0();

};

CPPUNIT_TEST_SUITE_REGISTRATION(HistogramListTest);

#endif

void HistogramListTest::setUp() 
{
  // create histogram bundles
  unique_ptr<QMutex> mutex0(new QMutex);
  SpJs::HistInfo info0   = {"hist0", 1, {"xparam"}, {{0, 10, 10}}, SpJs::Long};
  unique_ptr<TH1> pHist0 = SpJs::HistFactory().create(info0);
  m_pHist0.reset(new HistogramBundle(std::move(mutex0), 
                                     std::move(pHist0), 
                                     info0));

  unique_ptr<QMutex> mutex1(new QMutex);
  SpJs::HistInfo info1   = {"hist1", 1, {"xparam"}, {{0, 10, 10}}, SpJs::Long};
  unique_ptr<TH1> pHist1 = SpJs::HistFactory().create(info1);
  m_pHist1.reset(new HistogramBundle(std::move(mutex1), 
                                     std::move(pHist1), 
                                     info1));

  unique_ptr<QMutex> mutex2(new QMutex);
  SpJs::HistInfo info2   = {"hist2", 2, {"xparam", "yparam"}, {{0, 10, 10}}, SpJs::Long};
  unique_ptr<TH1> pHist2 = SpJs::HistFactory().create(info2);
  m_pHist2.reset(new HistogramBundle(std::move(mutex2), 
                                     std::move(pHist2), 
                                     info2));

  unique_ptr<QMutex> mutex3(new QMutex);
  SpJs::HistInfo info3   = {"hist3", 2, {"xparam", "yparam"}, {{0, 10, 10}}, SpJs::Long};
  unique_ptr<TH1> pHist3 = SpJs::HistFactory().create(info3);
  m_pHist3.reset(new HistogramBundle(std::move(mutex3), 
                                     std::move(pHist3), 
                                     info3));

}

void HistogramListTest::tearDown() 
{
  HistogramList::clear();

}

void HistogramListTest::histNames_0()
{
  HistogramList::addHist( std::move(m_pHist0) );
  HistogramList::addHist( std::move(m_pHist1) ); 
  HistogramList::addHist( std::move(m_pHist2) ); 
  HistogramList::addHist( std::move(m_pHist3) ); 

  CPPUNIT_ASSERT( 4 == HistogramList::size() );

  QList<QString> expNames;
  expNames.push_back("hist0");
  expNames.push_back("hist1");
  expNames.push_back("hist2");
  expNames.push_back("hist3");

  CPPUNIT_ASSERT( expNames == HistogramList::histNames() );
}

// Test that we can synchronize to the SpecTcl data
void HistogramListTest::removeSlice_0()
{
  // creata slice and add it to both of the histogram bundles
  GSlice slice;
  m_pHist0->addCut1D(&slice);
  m_pHist1->addCut1D(&slice);

  HistogramList::addHist(std::move(m_pHist0));
  HistogramList::addHist(std::move(m_pHist1));

  // make sure we have some cuts registered
  CPPUNIT_ASSERT( 1 == HistogramList::getHist("hist0")->getCut1Ds().size() ); 
  CPPUNIT_ASSERT( 1 == HistogramList::getHist("hist1")->getCut1Ds().size() ); 

  CPPUNIT_ASSERT( 2 == HistogramList::size() );

  // remove the slice
  HistogramList::removeSlice(slice);

  // Check whether the histogram list owned object contain their
  // gates
  auto it = HistogramList::begin();
  auto it_end = HistogramList::end();
  while (it != it_end ) {

    CPPUNIT_ASSERT( 0 == it->second->getCut1Ds().size() ); 

    ++it;
  }

}

// Test that we can synchronize to the SpecTcl data
void HistogramListTest::removeGate_0()
{

  // creata slice and add it to both of the histogram bundles
  GGate gate("gate", SpJs::Band());
  m_pHist2->addCut2D(&gate);
  m_pHist3->addCut2D(&gate);
  
  // Add these to the Histogram List... this copies the histograms
  // so that from here on out we are dealing with different objects
  // than were created above
  HistogramList::addHist( std::move(m_pHist2) );
  HistogramList::addHist( std::move(m_pHist3) );

  // make sure we have some cuts registered
  CPPUNIT_ASSERT( 1 == HistogramList::getHist("hist2")->getCut2Ds().size() ); 
  CPPUNIT_ASSERT( 1 == HistogramList::getHist("hist3")->getCut2Ds().size() ); 

  CPPUNIT_ASSERT( 2 == HistogramList::size() );

  // remove the slice
  HistogramList::removeGate(gate);

  // Check whether the histogram list owned object contain their
  // gates
  auto it = HistogramList::begin();
  auto it_end = HistogramList::end();
  while (it != it_end ) {

    CPPUNIT_ASSERT( 0 == it->second->getCut2Ds().size() ); 

    ++it;
  }

}

