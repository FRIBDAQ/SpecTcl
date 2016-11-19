// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <vector>

#include "Parameter.h"
#include "Spectrum2DL.h"
#include "Spectrum2DW.h"
#include "Spectrum2DB.h"
#include <Event.h>


class Test2D : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(Test2D);
  CPPUNIT_TEST(inspec2DB);
  CPPUNIT_TEST(underX2DB);
  CPPUNIT_TEST(underY2DB);
  CPPUNIT_TEST(overX2DB);
  CPPUNIT_TEST(overY2DB);
  
  CPPUNIT_TEST(inspec2DW);
  CPPUNIT_TEST(underX2DW);
  CPPUNIT_TEST(underY2DW);
  CPPUNIT_TEST(overX2DW);
  CPPUNIT_TEST(overY2DW);
  
  CPPUNIT_TEST(inspec2DL);
  CPPUNIT_TEST(underX2DL);
  CPPUNIT_TEST(underY2DL);
  CPPUNIT_TEST(overX2DL);
  CPPUNIT_TEST(overY2DL);
  
  CPPUNIT_TEST_SUITE_END();


private:
    std::pair<std::vector<unsigned>, std::vector<unsigned> > fillSpecB(int x, int y);
    std::pair<std::vector<unsigned>, std::vector<unsigned> > fillSpecW(int x, int y);
    std::pair<std::vector<unsigned>, std::vector<unsigned> > fillSpecL(int x, int y);
public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void inspec2DB();
  void underX2DB();
  void underY2DB();
  void overX2DB();
  void overY2DB();
  
  void inspec2DW();
  void underX2DW();
  void underY2DW();
  void overX2DW();
  void overY2DW();

  void inspec2DL();
  void underX2DL();
  void underY2DL();
  void overX2DL();
  void overY2DL();
};

// fillSpecB - fill a byte spectrum
// @param x value
// @param y value
// @return std::pair<std::vector<std::unsigned> > - .first - underflows .second overflows.

std::pair<std::vector<unsigned>, std::vector<unsigned> >
Test2D::fillSpecB(int x, int y)
{
    CParameter px("x", 0, "arb");
    CParameter py("y", 1, "arb");
    CSpectrum2DB test("test", 1, px, py, 100, 100);
    
    CEvent e;
    
    e[0] = x;
    e[1] = y;
    test(e);
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result(test.getUnderflows(), test.getOverflows());
    
    return result;
}


// fillSpecW - fill a word spectrum
// @param x value
// @param y value
// @return std::pair<std::vector<std::unsigned> > - .first - underflows .second overflows.

std::pair<std::vector<unsigned>, std::vector<unsigned> >
Test2D::fillSpecW(int x, int y)
{
    CParameter px("x", 0, "arb");
    CParameter py("y", 1, "arb");
    CSpectrum2DW test("test", 1, px, py, 100, 100);
    
    CEvent e;
    
    e[0] = x;
    e[1] = y;
    test(e);
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result(test.getUnderflows(), test.getOverflows());
    
    return result;
}

// fillSpecL - fill a word spectrum
// @param x value
// @param y value
// @return std::pair<std::vector<std::unsigned> > - .first - underflows .second overflows.

std::pair<std::vector<unsigned>, std::vector<unsigned> >
Test2D::fillSpecL(int x, int y)
{
    CParameter px("x", 0, "arb");
    CParameter py("y", 1, "arb");
    CSpectrum2DL test("test", 1, px, py, 100, 100);
    
    CEvent e;
    
    e[0] = x;
    e[1] = y;
    test(e);
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result(test.getUnderflows(), test.getOverflows());
    
    return result;
}
CPPUNIT_TEST_SUITE_REGISTRATION(Test2D);

// Inside the spectrum.

void Test2D::inspec2DB() {

    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fillSpecB(50, 75);
    
    std::vector<unsigned> stats = result.first;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    stats = result.second;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    
}


// X underflow;

void Test2D::underX2DB()
{
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fillSpecB(-1, 75);
    std::vector<unsigned> stats = result.first;
    EQ(size_t(2), stats.size());
    EQ(unsigned(1), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    stats = result.second;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    
}

void Test2D::underY2DB() {
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fillSpecB(50, -1);
    
    std::vector<unsigned> stats = result.first;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(1), stats[1]);
    
    stats = result.second;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    
}

void Test2D::overX2DB()
{
    
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fillSpecB(100, 75);
    
    std::vector<unsigned> stats = result.first;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    stats = result.second;
    EQ(size_t(2), stats.size());
    EQ(unsigned(1), stats[0]);
    EQ(unsigned(0), stats[1]);
        
}

void Test2D::overY2DB()
{
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fillSpecB(50, 100);
    
    std::vector<unsigned> stats = result.first;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    stats = result.second;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(1), stats[1]);
}



/// Word tests:


void Test2D::inspec2DW() {

    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fillSpecW(50, 75);
    
    std::vector<unsigned> stats = result.first;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    stats = result.second;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    
}


// X underflow;

void Test2D::underX2DW()
{
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fillSpecW(-1, 75);
    std::vector<unsigned> stats = result.first;
    EQ(size_t(2), stats.size());
    EQ(unsigned(1), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    stats = result.second;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    
}

void Test2D::underY2DW() {
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fillSpecW(50, -1);
    
    std::vector<unsigned> stats = result.first;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(1), stats[1]);
    
    stats = result.second;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    
}

void Test2D::overX2DW()
{
    
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fillSpecW(100, 75);
    
    std::vector<unsigned> stats = result.first;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    stats = result.second;
    EQ(size_t(2), stats.size());
    EQ(unsigned(1), stats[0]);
    EQ(unsigned(0), stats[1]);
        
}

void Test2D::overY2DW()
{
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fillSpecW(50, 100);
    
    std::vector<unsigned> stats = result.first;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    stats = result.second;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(1), stats[1]);
}

// Long tests:


void Test2D::inspec2DL() {

    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fillSpecL(50, 75);
    
    std::vector<unsigned> stats = result.first;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    stats = result.second;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    
}


// X underflow;

void Test2D::underX2DL()
{
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fillSpecL(-1, 75);
    std::vector<unsigned> stats = result.first;
    EQ(size_t(2), stats.size());
    EQ(unsigned(1), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    stats = result.second;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    
}

void Test2D::underY2DL() {
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fillSpecL(50, -1);
    
    std::vector<unsigned> stats = result.first;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(1), stats[1]);
    
    stats = result.second;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    
}

void Test2D::overX2DL()
{
    
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fillSpecL(100, 75);
    
    std::vector<unsigned> stats = result.first;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    stats = result.second;
    EQ(size_t(2), stats.size());
    EQ(unsigned(1), stats[0]);
    EQ(unsigned(0), stats[1]);
        
}

void Test2D::overY2DL()
{
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fillSpecL(50, 100);
    
    std::vector<unsigned> stats = result.first;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(0), stats[1]);
    
    stats = result.second;
    EQ(size_t(2), stats.size());
    EQ(unsigned(0), stats[0]);
    EQ(unsigned(1), stats[1]);
}