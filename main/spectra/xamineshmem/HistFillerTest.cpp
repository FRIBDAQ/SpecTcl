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
#include <cstdint>

using namespace std;

extern spec_shared* xamine_shared;

// ensure that pair can be printed
template<class T1, class T2>
ostream& operator<<(ostream& stream, const pair<T1, T2>& p)
{
    stream << "(" << p.first << ", " << p.second << ")";
    return stream;
}

template<class T> int spec1dtype()    { return -1;}
template<> int spec1dtype<uint32_t>() { return 0; }
template<> int spec1dtype<uint16_t>() { return 1; }

template<class T> int spec2dtype()    { return -1;}
template<> int spec2dtype<uint16_t>() { return 0; }
template<> int spec2dtype<uint8_t>()  { return 1; }
template<> int spec2dtype<uint32_t>() { return 2; }

class HistFillerTest : public CppUnit::TestFixture
{
  public:
    CPPUNIT_TEST_SUITE( HistFillerTest );
    CPPUNIT_TEST( fill1D_0 );
    CPPUNIT_TEST( fill1D_1 );
    CPPUNIT_TEST( fill1D_2 );
    CPPUNIT_TEST( fill2D_0 );
    CPPUNIT_TEST( fill2D_1 );
    CPPUNIT_TEST( fill2D_2 );
    CPPUNIT_TEST( fill2D_3 );
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

        m_spec1d = -1;
        m_spec2d = -1;

    }

    void tearDown() {
        if (m_spec1d > 0) Xamine_FreeSpectrum(m_spec1d);
        if (m_spec2d > 0) Xamine_FreeSpectrum(m_spec2d);

        Xamine_DetachSharedMemory();
    }

    template<class T>
    void create1DSpectrum() {
        T* pData = reinterpret_cast<T*>(Xamine_Allocate1d(&m_spec1d, 4, "test spectrum", spec1dtype<T>())); // 32bit word
        if (pData) {

            vector<T> data = {1, 2, 3, 4};
            copy(begin(data), end(data), pData);

            Xamine_SetMap1d(m_spec1d, 0, 3, "asdf");
        } else {
            throw std::runtime_error("HistFillerTest::setUp() Failed to allocate 1d spectrum");
        }
    }

    template<class T>
    void create2DSpectrum() {
        T* pData = reinterpret_cast<T*>(Xamine_Allocate2d(&m_spec2d, 3, 3, "spectrum2d", spec2dtype<T>())); // 0 = 32bit word
        if (pData) {

            vector<T> data = {1, 2, 3, 4, 5, 6, 7, 8, 9};
            copy(begin(data), end(data), pData);

            Xamine_SetMap2d(m_spec2d, 0, 2, "x", 0, 2, "y");
        } else {
            throw std::runtime_error("HistFillerTest::setUp() Failed to allocate 2d spectrum");
        }
    }

    void assert1DFillWorks(TH1& hist) {
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

    void assert2DFillWorks(TH2& hist) {

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

    // test the ability to fill a TH1F histogram with 32 bit data
    void fill1D_0() {
        create1DSpectrum<uint32_t>(); // 32 bit data

        TH1F hist("hist", "asdf", 4, 0, 3);

        assert1DFillWorks(hist);
    }

    // test filling of TH1F with 16-bit data
    void fill1D_1() {
        create1DSpectrum<uint16_t>(); // 16 bit data

        TH1F hist("hist", "asdf", 4, 0, 3);

        assert1DFillWorks(hist);
    }

    // test filling of TH1D with uint16_t data
    void fill1D_2() {
        create1DSpectrum<uint16_t>();

        TH1D hist("hist", "asdf", 4, 0, 3);

        assert1DFillWorks(hist);
    }

    //test the ability to fill a 2d histogram with 32-bit data
    void fill2D_0() {
        create2DSpectrum<uint32_t>(); // 32 bit data

        TH2F hist("hist", "asdf", 3, 0, 2, 3, 0, 2);

        assert2DFillWorks(hist);
    }

    //test the ability to fill a TH2F histogram with 16-bit data
    void fill2D_1() {
        create2DSpectrum<uint16_t>(); // 16-bit data

        TH2F hist("hist", "asdf", 3, 0, 2, 3, 0, 2);

        assert2DFillWorks(hist);
    }

    //test the ability to fill a TH2F histogram with 8-bit data
    void fill2D_2() {
        create2DSpectrum<uint8_t>(); // 16-bit data

        TH2F hist("hist", "asdf", 3, 0, 2, 3, 0, 2);

        assert2DFillWorks(hist);
    }

    //test the ability to fill a TH2D histogram with 8-bit data
    void fill2D_3() {
        create2DSpectrum<uint8_t>(); // 16-bit data

        TH2D hist("hist", "asdf", 3, 0, 2, 3, 0, 2);

        assert2DFillWorks(hist);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(HistFillerTest);

#endif

