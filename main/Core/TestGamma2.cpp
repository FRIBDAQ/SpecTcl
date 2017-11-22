// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <vector>

#include "Parameter.h"
#include "Event.h"

#include "Gamma2DB.h"
#include "Gamma2DW.h"
#include "Gamma2DL.h"

// Commented tests need to be rethought as Root won't log both over/under in x & y
// only one of them.

class TestGamma2 : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TestGamma2);
  CPPUNIT_TEST(allinB);
  //CPPUNIT_TEST(allunderB);
  //CPPUNIT_TEST(alloverB);
  //CPPUNIT_TEST(mixedB);
  
  CPPUNIT_TEST(allinW);
  //CPPUNIT_TEST(allunderW);
  //CPPUNIT_TEST(alloverW);
  //CPPUNIT_TEST(mixedW);
  
  CPPUNIT_TEST(allinL);
  //CPPUNIT_TEST(allunderL);
  //CPPUNIT_TEST(alloverL);
  //CPPUNIT_TEST(mixedL);
  CPPUNIT_TEST_SUITE_END();


private:
    std::pair<std::vector<unsigned>, std::vector<unsigned> >
        fillB(std::vector<int> v, unsigned  nx, unsigned ny);
    std::pair<std::vector<unsigned>, std::vector<unsigned> >
        fillW(std::vector<int> v, unsigned  nx, unsigned ny);
   std::pair<std::vector<unsigned>, std::vector<unsigned> >
        fillL(std::vector<int> v, unsigned  nx, unsigned ny);

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void allinB();
  void allunderB();
  void alloverB();
  void mixedB();
  
  void allinW();
  void allunderW();
  void alloverW();
  void mixedW();
  
  void allinL();
  void allunderL();
  void alloverL();
  void mixedL();
};

// Spectrum fillers B,W,L
//
// @param v  - vector of values.
// @param nx X dimension
// @param ny Y dimension
// @return pair first is undeflows stats, second overflow stats.

std::pair<std::vector<unsigned>, std::vector<unsigned> >
TestGamma2::fillB(std::vector<int> v, unsigned nx, unsigned ny)
{
    CEvent e;
    std::vector<CParameter> ps;
    
    for (int i =0; i < v.size(); i++) {
        ps.push_back(CParameter("Test", i, "arb"));
        e[i] = v[i];
    }
    
    CGamma2DB spec("test", 0, ps, nx, ny);
    spec(e);
    
    return std::pair<std::vector<unsigned>, std::vector<unsigned> >(
        spec.getUnderflows(), spec.getOverflows()
    );
}



std::pair<std::vector<unsigned>, std::vector<unsigned> >
TestGamma2::fillW(std::vector<int> v, unsigned nx, unsigned ny)
{
    CEvent e;
    std::vector<CParameter> ps;
    
    for (int i =0; i < v.size(); i++) {
        ps.push_back(CParameter("Test", i, "arb"));
        e[i] = v[i];
    }
    
    CGamma2DW spec("test", 0, ps, nx, ny);
    spec(e);
    
    return std::pair<std::vector<unsigned>, std::vector<unsigned> >(
        spec.getUnderflows(), spec.getOverflows()
    );
}

std::pair<std::vector<unsigned>, std::vector<unsigned> >
TestGamma2::fillL(std::vector<int> v, unsigned nx, unsigned ny)
{
    CEvent e;
    std::vector<CParameter> ps;
    
    for (int i =0; i < v.size(); i++) {
        ps.push_back(CParameter("Test", i, "arb"));
        e[i] = v[i];
    }
    
    CGamma2DL spec("test", 0, ps, nx, ny);
    spec(e);
    
    return std::pair<std::vector<unsigned>, std::vector<unsigned> >(
        spec.getUnderflows(), spec.getOverflows()
    );
}

CPPUNIT_TEST_SUITE_REGISTRATION(TestGamma2);

// Byte spectrum tests.

void TestGamma2::allinB()
{
    std::vector<int> values;
    for (int i =0; i < 50; i++) {
        values.push_back(50);
    }
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillB(values, 1024, 1024);
    
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(0), result.first[0]);
    EQ(unsigned(0), result.first[1]);
    
    EQ(unsigned(0), result.second[0]);
    EQ(unsigned(0), result.second[1]);
}


void TestGamma2::allunderB()
{
    std::vector<int> values;
    for (int i =0; i < 50; i++) {
        values.push_back(-1);
    }
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillB(values, 1024, 1024);
    
    
    EQ(unsigned(50*49/2), result.first[0]);
    EQ(unsigned(50*49/2), result.first[1]);
    
    EQ(unsigned(0), result.second[0]);
    EQ(unsigned(0), result.second[1]);
}

void TestGamma2::alloverB()
{
    std::vector<int> values;
    for (int i =0; i < 50; i++) {
        values.push_back(1024);
    }
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillB(values, 1024, 1024);
    
    
    EQ(unsigned(0), result.first[0]);
    EQ(unsigned(0), result.first[1]);
    
    EQ(unsigned(50*49/2), result.second[0]);
    EQ(unsigned(50*49/2), result.second[1]);
}

void TestGamma2::mixedB()
{
    int v[3] = {-1, 1024, 50};
    int ctrs[3] = {0, 0, 0};
    std::vector<int> values;
    
    for (int i =0;  i < 50; i++) {
        int idx = i%3;
        values.push_back(v[idx]);
        ctrs[idx]++;
    }
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillB(values, 1024, 1024);
    
    
    EQ(unsigned(ctrs[0]*50/2), result.first[0]);
    EQ(unsigned(ctrs[0]*48/2), result.first[1]);
    
    EQ(unsigned(ctrs[1]*48/2), result.second[0]);
    EQ(unsigned(ctrs[1]*50/2), result.second[1]);
}

// Word spectra tests:


void TestGamma2::allinW()
{
    std::vector<int> values;
    for (int i =0; i < 50; i++) {
        values.push_back(50);
    }
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillW(values, 1024, 1024);
    
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(0), result.first[0]);
    EQ(unsigned(0), result.first[1]);
    
    EQ(unsigned(0), result.second[0]);
    EQ(unsigned(0), result.second[1]);
}


void TestGamma2::allunderW()
{
    std::vector<int> values;
    for (int i =0; i < 50; i++) {
        values.push_back(-1);
    }
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillW(values, 1024, 1024);
    
    
    EQ(unsigned(50*49/2), result.first[0]);
    EQ(unsigned(50*49/2), result.first[1]);
    
    EQ(unsigned(0), result.second[0]);
    EQ(unsigned(0), result.second[1]);
}

void TestGamma2::alloverW()
{
    std::vector<int> values;
    for (int i =0; i < 50; i++) {
        values.push_back(1024);
    }
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillW(values, 1024, 1024);
    
    
    EQ(unsigned(0), result.first[0]);
    EQ(unsigned(0), result.first[1]);
    
    EQ(unsigned(50*49/2), result.second[0]);
    EQ(unsigned(50*49/2), result.second[1]);
}

void TestGamma2::mixedW()
{
    int v[3] = {-1, 1024, 50};
    int ctrs[3] = {0, 0, 0};
    std::vector<int> values;
    
    for (int i =0;  i < 50; i++) {
        int idx = i%3;
        values.push_back(v[idx]);
        ctrs[idx]++;
    }
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillW(values, 1024, 1024);
    
    
    EQ(unsigned(ctrs[0]*50/2), result.first[0]);
    EQ(unsigned(ctrs[0]*48/2), result.first[1]);
    
    EQ(unsigned(ctrs[1]*48/2), result.second[0]);
    EQ(unsigned(ctrs[1]*50/2), result.second[1]);
}

// Longword spectra



void TestGamma2::allinL()
{
    std::vector<int> values;
    for (int i =0; i < 50; i++) {
        values.push_back(50);
    }
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillL(values, 1024, 1024);
    
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(0), result.first[0]);
    EQ(unsigned(0), result.first[1]);
    
    EQ(unsigned(0), result.second[0]);
    EQ(unsigned(0), result.second[1]);
}


void TestGamma2::allunderL()
{
    std::vector<int> values;
    for (int i =0; i < 50; i++) {
        values.push_back(-1);
    }
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillL(values, 1024, 1024);
    
    
    EQ(unsigned(50*49/2), result.first[0]);
    EQ(unsigned(50*49/2), result.first[1]);
    
    EQ(unsigned(0), result.second[0]);
    EQ(unsigned(0), result.second[1]);
}

void TestGamma2::alloverL()
{
    std::vector<int> values;
    for (int i =0; i < 50; i++) {
        values.push_back(1024);
    }
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillL(values, 1024, 1024);
    
    
    EQ(unsigned(0), result.first[0]);
    EQ(unsigned(0), result.first[1]);
    
    EQ(unsigned(50*49/2), result.second[0]);
    EQ(unsigned(50*49/2), result.second[1]);
}

void TestGamma2::mixedL()
{
    int v[3] = {-1, 1024, 50};
    int ctrs[3] = {0, 0, 0};
    std::vector<int> values;
    
    for (int i =0;  i < 50; i++) {
        int idx = i%3;
        values.push_back(v[idx]);
        ctrs[idx]++;
    }
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillL(values, 1024, 1024);
    
    
    EQ(unsigned(ctrs[0]*50/2), result.first[0]);
    EQ(unsigned(ctrs[0]*48/2), result.first[1]);
    
    EQ(unsigned(ctrs[1]*48/2), result.second[0]);
    EQ(unsigned(ctrs[1]*50/2), result.second[1]);
}
