// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <vector>

#include "Parameter.h"
#include "Event.h"
#include "CSpectrum2DmB.h"
#include "CSpectrum2DmW.h"
#include "CSpectrum2DmL.h"


class Test2dM : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(Test2dM);
  CPPUNIT_TEST(allinB);
  CPPUNIT_TEST(xundersB);
  CPPUNIT_TEST(yundersB);
  CPPUNIT_TEST(xoversB);
  CPPUNIT_TEST(yoversB);
  CPPUNIT_TEST(mixedB);
  
  CPPUNIT_TEST(allinW);
  CPPUNIT_TEST(xundersW);
  CPPUNIT_TEST(yundersW);
  CPPUNIT_TEST(xoversW);
  CPPUNIT_TEST(yoversW);
  CPPUNIT_TEST(mixedW);
  
  CPPUNIT_TEST(allinL);
  CPPUNIT_TEST(xundersL);
  CPPUNIT_TEST(yundersL);
  CPPUNIT_TEST(xoversL);
  CPPUNIT_TEST(yoversL);
  CPPUNIT_TEST(mixedL);
  CPPUNIT_TEST_SUITE_END();


private:
    std::pair<std::vector<unsigned>, std::vector<unsigned> >
        fillB(std::vector<int> xs, std::vector<int> ys, unsigned nx, unsigned ny);
    std::pair<std::vector<unsigned>, std::vector<unsigned> >
        fillW(std::vector<int> xs, std::vector<int> ys, unsigned nx, unsigned ny);
    std::pair<std::vector<unsigned>, std::vector<unsigned> >
        fillL(std::vector<int> xs, std::vector<int> ys, unsigned nx, unsigned ny);
public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void allinB();
  void xundersB();
  void yundersB();
  void xoversB();
  void yoversB();
  void mixedB();
  
  void allinW();
  void xundersW();
  void yundersW();
  void xoversW();
  void yoversW();
  void mixedW();
  
  void allinL();
  void xundersL();
  void yundersL();
  void xoversL();
  void yoversL();
  void mixedL();
};

//  Fill byte spectrum:
//  @param xs - vector of x values.
//  @param ys - vector of y valuse (must be same sized.)
//  @param nx - Number of x channels.
//  @param ny - Number of y channels.
//  @return statistics (first underflows, second overflows).

std::pair<std::vector<unsigned>, std::vector<unsigned> >
Test2dM::fillB(std::vector<int> xs, std::vector<int> ys, unsigned nx, unsigned ny)
{
    std::vector<CParameter> ps;
    CEvent e;
    int pno;
    
    // Create the parameters and event
    
    for (int i = 0; i < xs.size(); i++) {
        ps.push_back(CParameter("x", pno, "arb"));
        ps.push_back(CParameter("y", pno+1, "arb"));
        e[pno] = xs[i];
        e[pno+1] = ys[i];
        pno += 2;
    }
    
    // Create the spectrum and process the event:
    
    CSpectrum2DmB spec("test", 0, ps, nx, ny);
    spec(e);
    
    return std::pair<std::vector<unsigned>, std::vector<unsigned> >(spec.getUnderflows(), spec.getOverflows());
}

// Fill word spectrum:

std::pair<std::vector<unsigned>, std::vector<unsigned> >
Test2dM::fillW(std::vector<int> xs, std::vector<int> ys, unsigned nx, unsigned ny)
{
    std::vector<CParameter> ps;
    CEvent e;
    int pno;
    
    // Create the parameters and event
    
    for (int i = 0; i < xs.size(); i++) {
        ps.push_back(CParameter("x", pno, "arb"));
        ps.push_back(CParameter("y", pno+1, "arb"));
        e[pno] = xs[i];
        e[pno+1] = ys[i];
        pno += 2;
    }
    
    // Create the spectrum and process the event:
    
    CSpectrum2DmW spec("test", 0, ps, nx, ny);
    spec(e);
    
    return std::pair<std::vector<unsigned>, std::vector<unsigned> >(spec.getUnderflows(), spec.getOverflows());
}

// Fill long spectrum.
std::pair<std::vector<unsigned>, std::vector<unsigned> >
Test2dM::fillL(std::vector<int> xs, std::vector<int> ys, unsigned nx, unsigned ny)
{
    std::vector<CParameter> ps;
    CEvent e;
    int pno;
    
    // Create the parameters and event
    
    for (int i = 0; i < xs.size(); i++) {
        ps.push_back(CParameter("x", pno, "arb"));
        ps.push_back(CParameter("y", pno+1, "arb"));
        e[pno] = xs[i];
        e[pno+1] = ys[i];
        pno += 2;
    }
    
    // Create the spectrum and process the event:
    
    CSpectrum2DmL spec("test", 0, ps, nx, ny);
    spec(e);
    
    return std::pair<std::vector<unsigned>, std::vector<unsigned> >(spec.getUnderflows(), spec.getOverflows());
}


CPPUNIT_TEST_SUITE_REGISTRATION(Test2dM);

// Byte spectrum tests:

void Test2dM::allinB() {
    std::vector<int> x;
    std::vector<int> y;
    
    // 50x 50y
    
    for (int i =0; i < 50; i++) {
        x.push_back(i);
        y.push_back(i);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillB(x, y, 1024, 1024);
        
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(0), result.first[0]);   // x underflows
    EQ(unsigned(0), result.first[1]);   // y underflows.
    
    EQ(unsigned(0), result.second[0]);  // x overflows.
    EQ(unsigned(0), result.second[1]);  // y overflows.
    
}

void Test2dM::xundersB()
{
    std::vector<int> x;
    std::vector<int> y;
    
    // 50x 50y
    
    for (int i =0; i < 50; i++) {
        x.push_back(-1);
        y.push_back(i);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillB(x, y, 1024, 1024);
        
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(50), result.first[0]);   // x underflows
    EQ(unsigned(0), result.first[1]);   // y underflows.
    
    EQ(unsigned(0), result.second[0]);  // x overflows.
    EQ(unsigned(0), result.second[1]);  // y overflows.   
}
void Test2dM::yundersB()
{
    std::vector<int> x;
    std::vector<int> y;
    
    // 50x 50y
    
    for (int i =0; i < 50; i++) {
        x.push_back(i);
        y.push_back(-1);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillB(x, y, 1024, 1024);
        
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(0), result.first[0]);   // x underflows
    EQ(unsigned(50), result.first[1]);   // y underflows.
    
    EQ(unsigned(0), result.second[0]);  // x overflows.
    EQ(unsigned(0), result.second[1]);  // y overflows.   
}
void Test2dM::xoversB()
{
    std::vector<int> x;
    std::vector<int> y;
    
    // 50x 50y
    
    for (int i =0; i < 50; i++) {
        x.push_back(1024);
        y.push_back(i);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillB(x, y, 1024, 1024);
        
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(0), result.first[0]);   // x underflows
    EQ(unsigned(0), result.first[1]);   // y underflows.
    
    EQ(unsigned(50), result.second[0]);  // x overflows.
    EQ(unsigned(0), result.second[1]);  // y overflows.   
}
void Test2dM::yoversB()
{
    std::vector<int> x;
    std::vector<int> y;
    
    // 50x 50y
    
    for (int i =0; i < 50; i++) {
        x.push_back(i);
        y.push_back(1024);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillB(x, y, 1024, 1024);
        
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(0), result.first[0]);   // x underflows
    EQ(unsigned(0), result.first[1]);   // y underflows.
    
    EQ(unsigned(0), result.second[0]);  // x overflows.
    EQ(unsigned(50), result.second[1]);  // y overflows.   
}

void Test2dM::mixedB()
{
    int xtbl[3] = {100, -1, 1024};
    int ytbl[3] = {-1, 1024, 100};
    unsigned xcounters[3] = {0,0,0};
    unsigned ycounters[3] = {0,0,0};
    
    std::vector<int> x;
    std::vector<int> y;
    
    // 50x 50y
    
    for (int i =0; i < 50; i++) {
        int xv = xtbl[i % 3];
        int yv = ytbl[i % 3];
        x.push_back(xv);
        y.push_back(yv);
        xcounters[i % 3]++;
        ycounters[i % 3]++;
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillB(x, y, 1024, 1024);
        
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(xcounters[1], result.first[0]);   // x underflows
    EQ(ycounters[0], result.first[1]);   // y underflows.
    
    EQ(xcounters[2], result.second[0]);  // x overflows.
    EQ(ycounters[1], result.second[1]);  // y overflows.   
}

// Word spectrum tests:


void Test2dM::allinW() {
    std::vector<int> x;
    std::vector<int> y;
    
    // 50x 50y
    
    for (int i =0; i < 50; i++) {
        x.push_back(i);
        y.push_back(i);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillW(x, y, 1024, 1024);
        
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(0), result.first[0]);   // x underflows
    EQ(unsigned(0), result.first[1]);   // y underflows.
    
    EQ(unsigned(0), result.second[0]);  // x overflows.
    EQ(unsigned(0), result.second[1]);  // y overflows.
    
}

void Test2dM::xundersW()
{
    std::vector<int> x;
    std::vector<int> y;
    
    // 50x 50y
    
    for (int i =0; i < 50; i++) {
        x.push_back(-1);
        y.push_back(i);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillW(x, y, 1024, 1024);
        
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(50), result.first[0]);   // x underflows
    EQ(unsigned(0), result.first[1]);   // y underflows.
    
    EQ(unsigned(0), result.second[0]);  // x overflows.
    EQ(unsigned(0), result.second[1]);  // y overflows.   
}
void Test2dM::yundersW()
{
    std::vector<int> x;
    std::vector<int> y;
    
    // 50x 50y
    
    for (int i =0; i < 50; i++) {
        x.push_back(i);
        y.push_back(-1);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillW(x, y, 1024, 1024);
        
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(0), result.first[0]);   // x underflows
    EQ(unsigned(50), result.first[1]);   // y underflows.
    
    EQ(unsigned(0), result.second[0]);  // x overflows.
    EQ(unsigned(0), result.second[1]);  // y overflows.   
}
void Test2dM::xoversW()
{
    std::vector<int> x;
    std::vector<int> y;
    
    // 50x 50y
    
    for (int i =0; i < 50; i++) {
        x.push_back(1024);
        y.push_back(i);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillW(x, y, 1024, 1024);
        
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(0), result.first[0]);   // x underflows
    EQ(unsigned(0), result.first[1]);   // y underflows.
    
    EQ(unsigned(50), result.second[0]);  // x overflows.
    EQ(unsigned(0), result.second[1]);  // y overflows.   
}
void Test2dM::yoversW()
{
    std::vector<int> x;
    std::vector<int> y;
    
    // 50x 50y
    
    for (int i =0; i < 50; i++) {
        x.push_back(i);
        y.push_back(1024);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillW(x, y, 1024, 1024);
        
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(0), result.first[0]);   // x underflows
    EQ(unsigned(0), result.first[1]);   // y underflows.
    
    EQ(unsigned(0), result.second[0]);  // x overflows.
    EQ(unsigned(50), result.second[1]);  // y overflows.   
}

void Test2dM::mixedW()
{
    int xtbl[3] = {100, -1, 1024};
    int ytbl[3] = {-1, 1024, 100};
    unsigned xcounters[3] = {0,0,0};
    unsigned ycounters[3] = {0,0,0};
    
    std::vector<int> x;
    std::vector<int> y;
    
    // 50x 50y
    
    for (int i =0; i < 50; i++) {
        int xv = xtbl[i % 3];
        int yv = ytbl[i % 3];
        x.push_back(xv);
        y.push_back(yv);
        xcounters[i % 3]++;
        ycounters[i % 3]++;
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillW(x, y, 1024, 1024);
        
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(xcounters[1], result.first[0]);   // x underflows
    EQ(ycounters[0], result.first[1]);   // y underflows.
    
    EQ(xcounters[2], result.second[0]);  // x overflows.
    EQ(ycounters[1], result.second[1]);  // y overflows.   
}

// Long spectra tests:



void Test2dM::allinL() {
    std::vector<int> x;
    std::vector<int> y;
    
    // 50x 50y
    
    for (int i =0; i < 50; i++) {
        x.push_back(i);
        y.push_back(i);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillL(x, y, 1024, 1024);
        
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(0), result.first[0]);   // x underflows
    EQ(unsigned(0), result.first[1]);   // y underflows.
    
    EQ(unsigned(0), result.second[0]);  // x overflows.
    EQ(unsigned(0), result.second[1]);  // y overflows.
    
}

void Test2dM::xundersL()
{
    std::vector<int> x;
    std::vector<int> y;
    
    // 50x 50y
    
    for (int i =0; i < 50; i++) {
        x.push_back(-1);
        y.push_back(i);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillL(x, y, 1024, 1024);
        
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(50), result.first[0]);   // x underflows
    EQ(unsigned(0), result.first[1]);   // y underflows.
    
    EQ(unsigned(0), result.second[0]);  // x overflows.
    EQ(unsigned(0), result.second[1]);  // y overflows.   
}
void Test2dM::yundersL()
{
    std::vector<int> x;
    std::vector<int> y;
    
    // 50x 50y
    
    for (int i =0; i < 50; i++) {
        x.push_back(i);
        y.push_back(-1);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillL(x, y, 1024, 1024);
        
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(0), result.first[0]);   // x underflows
    EQ(unsigned(50), result.first[1]);   // y underflows.
    
    EQ(unsigned(0), result.second[0]);  // x overflows.
    EQ(unsigned(0), result.second[1]);  // y overflows.   
}
void Test2dM::xoversL()
{
    std::vector<int> x;
    std::vector<int> y;
    
    // 50x 50y
    
    for (int i =0; i < 50; i++) {
        x.push_back(1024);
        y.push_back(i);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillL(x, y, 1024, 1024);
        
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(0), result.first[0]);   // x underflows
    EQ(unsigned(0), result.first[1]);   // y underflows.
    
    EQ(unsigned(50), result.second[0]);  // x overflows.
    EQ(unsigned(0), result.second[1]);  // y overflows.   
}
void Test2dM::yoversL()
{
    std::vector<int> x;
    std::vector<int> y;
    
    // 50x 50y
    
    for (int i =0; i < 50; i++) {
        x.push_back(i);
        y.push_back(1024);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillL(x, y, 1024, 1024);
        
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(0), result.first[0]);   // x underflows
    EQ(unsigned(0), result.first[1]);   // y underflows.
    
    EQ(unsigned(0), result.second[0]);  // x overflows.
    EQ(unsigned(50), result.second[1]);  // y overflows.   
}

void Test2dM::mixedL()
{
    int xtbl[3] = {100, -1, 1024};
    int ytbl[3] = {-1, 1024, 100};
    unsigned xcounters[3] = {0,0,0};
    unsigned ycounters[3] = {0,0,0};
    
    std::vector<int> x;
    std::vector<int> y;
    
    // 50x 50y
    
    for (int i =0; i < 50; i++) {
        int xv = xtbl[i % 3];
        int yv = ytbl[i % 3];
        x.push_back(xv);
        y.push_back(yv);
        xcounters[i % 3]++;
        ycounters[i % 3]++;
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillL(x, y, 1024, 1024);
        
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(xcounters[1], result.first[0]);   // x underflows
    EQ(ycounters[0], result.first[1]);   // y underflows.
    
    EQ(xcounters[2], result.second[0]);  // x overflows.
    EQ(ycounters[1], result.second[1]);  // y overflows.   
}

