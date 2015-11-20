// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include <vector>

#include "Parameter.h"
#include "Event.h"


#include "Gamma1DL.h"
#include "Gamma1DW.h"

class TestGamma1 : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TestGamma1);
  CPPUNIT_TEST(allinL);
  CPPUNIT_TEST(allunderL);
  CPPUNIT_TEST(allOverL);
  CPPUNIT_TEST(mixedL);
  
  CPPUNIT_TEST(allinW);
  CPPUNIT_TEST(allunderW);
  CPPUNIT_TEST(allOverW);
  CPPUNIT_TEST(mixedW);
  CPPUNIT_TEST_SUITE_END();


private:
    std::pair<std::vector<unsigned>, std::vector<unsigned> >
        fillL(std::vector<int> values, int n);
    std::pair<std::vector<unsigned>, std::vector<unsigned> >
        fillW(std::vector<int> values, int n);
public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void allinL();
  void allunderL();
  void allOverL();
  void mixedL();
  
  void allinW();
  void allunderW();
  void allOverW();
  void mixedW();
};


// Fill methods, L, W:
// @param values - a set of values to increment with.
// @param n      - Number of channels.
// @return pair  first - underflow stats, second overflow.

std::pair<std::vector<unsigned>, std::vector<unsigned> >
TestGamma1::fillL(std::vector<int> values, int n)
{
    CEvent e;
    std::vector<CParameter> ps;
    
    for (int i =0; i < values.size(); i++) {
        e[i] = values[i];
        ps.push_back(CParameter("test", i, "arb"));
    }
    CGamma1DL spec("Test", 0, ps, n);
    spec(e);
    
    return std::pair<std::vector<unsigned>, std::vector<unsigned> >(
        spec.getUnderflows(), spec.getOverflows()
    );
}

std::pair<std::vector<unsigned>, std::vector<unsigned> >
TestGamma1::fillW(std::vector<int> values, int n)
{
    CEvent e;
    std::vector<CParameter> ps;
    
    for (int i =0; i < values.size(); i++) {
        e[i] = values[i];
        ps.push_back(CParameter("test", i, "arb"));
    }
    CGamma1DW spec("Test", 0, ps, n);
    spec(e);
    
    return std::pair<std::vector<unsigned>, std::vector<unsigned> >(
        spec.getUnderflows(), spec.getOverflows()
    );
}

CPPUNIT_TEST_SUITE_REGISTRATION(TestGamma1);


// Longword tests

void TestGamma1::allinL()
{
    std::vector<int> v;
    for (int i = 0; i < 50; i++) {
        v.push_back(i);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillL(v, 1023);
    
    EQ(size_t(1), result.first.size());
    EQ(size_t(1), result.second.size());
    
    EQ(unsigned(0), result.first[0]);
    EQ(unsigned(0), result.second[0]);
    
    
}

void TestGamma1::allunderL()
{
    std::vector<int> v;
    for (int i = 0; i < 50; i++) {
        v.push_back(-1);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillL(v, 1023);
    
    EQ(unsigned(50), result.first[0]);
    EQ(unsigned(0), result.second[0]);
    
}

void TestGamma1::allOverL()
{
    std::vector<int> v;
    for (int i = 0; i < 50; i++) {
        v.push_back(1023);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillL(v, 1023);
    
    EQ(unsigned(0), result.first[0]);
    EQ(unsigned(50), result.second[0]);
    
}

void TestGamma1::mixedL()
{
    std::vector<int> v;
    int values[3] = {-1, 1023, 50};
    int ctrs[3]   = {0, 0, 0};
    
    for (int i = 0; i < 50; i++) {
        int idx = i % 3;
        v.push_back(values[idx]);
        ctrs[idx]++;
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillL(v, 1023);
    
    EQ(unsigned(ctrs[0]), result.first[0]);
    EQ(unsigned(ctrs[1]), result.second[0]);
}

// Word tests:


void TestGamma1::allinW()
{
    std::vector<int> v;
    for (int i = 0; i < 50; i++) {
        v.push_back(i);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillW(v, 1023);
    
    EQ(size_t(1), result.first.size());
    EQ(size_t(1), result.second.size());
    
    EQ(unsigned(0), result.first[0]);
    EQ(unsigned(0), result.second[0]);
    
    
}

void TestGamma1::allunderW()
{
    std::vector<int> v;
    for (int i = 0; i < 50; i++) {
        v.push_back(-1);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillW(v, 1023);
    
    EQ(unsigned(50), result.first[0]);
    EQ(unsigned(0), result.second[0]);
    
}

void TestGamma1::allOverW()
{
    std::vector<int> v;
    for (int i = 0; i < 50; i++) {
        v.push_back(1023);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillW(v, 1023);
    
    EQ(unsigned(0), result.first[0]);
    EQ(unsigned(50), result.second[0]);
    
}

void TestGamma1::mixedW()
{
    std::vector<int> v;
    int values[3] = {-1, 1023, 50};
    int ctrs[3]   = {0, 0, 0};
    
    for (int i = 0; i < 50; i++) {
        int idx = i % 3;
        v.push_back(values[idx]);
        ctrs[idx]++;
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillW(v, 1023);
    
    EQ(unsigned(ctrs[0]), result.first[0]);
    EQ(unsigned(ctrs[1]), result.second[0]);
}