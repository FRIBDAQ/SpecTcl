#ifndef GGATETEST_H
#define GGATETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <Asserts.h>

#include "HistFiller.h"
#include <TH1.h>
#include <TH2.h>
#include "client.h"
#include "dispshare.h"

#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
#include <stdexcept>

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
    CPPUNIT_TEST( fill1D_0 );
    CPPUNIT_TEST( fill2D_0 );
    CPPUNIT_TEST_SUITE_END();

  private:
    volatile Xamine_shared* m_pSharedMemory;

    int m_spec1d;
    int m_spec2d;

  public:
    void setUp() {
      Xamine_CreateSharedMemory(1024*1024, &m_pSharedMemory);
      Xamine_ManageMemory();
      Xamine_initspectra(); // sets up access to spectra

      int* pData = reinterpret_cast<int*>(Xamine_Allocate1d(&m_spec1d, 4, "test spectrum", 0)); // 32bit word
      vector<int> data = {1, 2, 3, 4};
      if (pData) {
          copy(begin(data), end(data), pData);
          Xamine_SetMap1d(m_spec1d, 0, 3, "asdf");
      } else {
          throw std::runtime_error("HistFillerTest::setUp() Failed to allocate 1d spectrum");
      }

      data = {1, 2, 3, 4, 5, 6, 7, 8, 9};
      pData = reinterpret_cast<int*>(Xamine_Allocate2d(&m_spec2d, 3, 3, "spectrum2d", 2)); // 0 = 32bit word
      if (pData) {
          copy(begin(data), end(data), pData);
          Xamine_SetMap2d(m_spec2d, 0, 2, "x", 0, 2, "y");
      } else {
          throw std::runtime_error("HistFillerTest::setUp() Failed to allocate 2d spectrum");
      }
    }

    void tearDown() {
      Xamine_FreeSpectrum(m_spec1d);
      Xamine_FreeSpectrum(m_spec2d);
      Xamine_DetachSharedMemory();
    }

  protected:

    void fill1D_0() {
      TH1F hist("hist", "asdf", 4, 0, 3); 
      x2r::HistFiller filler;
      filler.fill(hist, m_spec1d);

      EQMSG("Fill sets the correct content for first bin",
            double(1), hist.GetBinContent(1));
      EQMSG("Fill sets the correct content for bin 1",
            double(2), hist.GetBinContent(2));
      EQMSG("Fill sets the correct content for bin 2",
            double(3), hist.GetBinContent(3));
      EQMSG("Fill sets the correct content for last bin",
            double(4), hist.GetBinContent(4));
    }

    void fill2D_0() {
        TH2F hist("hist", "asdf", 3, 0, 2, 3, 0, 2);

        x2r::HistFiller filler;
        filler.fill(hist, m_spec2d);

        for (int xb=1; xb<=3; ++xb) {
            for (int yb=1; yb<=3; ++yb) {
                string msg = "fill sets correct content for bin (";
                msg += to_string(xb) + "," + to_string(yb) + ")";
                EQMSG(msg.c_str(), double((yb-1)*3+xb), hist.GetBinContent(xb, yb));
            }
        }
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(HistFillerTest);

#endif

