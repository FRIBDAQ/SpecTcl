#ifndef GGATETEST_H
#define GGATETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include "HistFiller.h"
#include <TH1.h>
#include "client.h"
#include "dispshare.h"

#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>

using namespace std;

extern spec_shared* xamine_shared;

  // ensure that pair can be printed
  template<class T1, class T2>
  ostream& operator<<(ostream& stream, const pair<T1, T2>& p)
  {
    stream << "(" << p.first << ", " << p.second << ")";
    return stream;
  }

class HistFillerTest : public CppUnit::TestFixture
{
  public:
    CPPUNIT_TEST_SUITE( HistFillerTest );
    CPPUNIT_TEST( fill_0 );
    CPPUNIT_TEST_SUITE_END();

  private:
    volatile Xamine_shared* m_pSharedMemory;

    int m_spec1d;
    int m_spec2d;

  public:
    void setUp() {
      Xamine_CreateSharedMemory(1024*1024, &m_pSharedMemory);
      Xamine_ManageMemory();
      int* pData = reinterpret_cast<int*>(Xamine_Allocate1d(&m_spec1d, 4, "test spectrum", 0)); // 32bit word
      vector<int> data = {0, 1, 2, 3};
      copy(begin(data), end(data), pData);
      Xamine_Allocate2d(&m_spec2d, 3, 3, "test spectrum", 0);
    }
    void tearDown() {
      Xamine_DetachSharedMemory();
    }

  protected:

    void fill_0() {
      TH1F hist("hist", "asdf", 4, 0, 3); 
      x2r::HistFiller filler;
      filler.fill(hist, m_spec1d);

    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(HistFillerTest);

#endif

