// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <vector>

#include "Parameter.h"
#include "Event.h"


#include "Gamma2DD.h"


class TestDeluxe : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TestDeluxe);
  CPPUNIT_TEST(allIn);
  CPPUNIT_TEST(xUnder);
  CPPUNIT_TEST(yUnder);
  CPPUNIT_TEST(xOver);
  CPPUNIT_TEST(yOver);
  CPPUNIT_TEST(mixed);
  CPPUNIT_TEST_SUITE_END();


private:
    std::pair<std::vector<unsigned>, std::vector<unsigned> >
        fill(std::vector<int> x, std::vector<int> y, unsigned nx, unsigned ny);
public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void allIn();
  void xUnder();
  void yUnder();
  void xOver();
  void yOver();
  void mixed();
};


std::pair<std::vector<unsigned>, std::vector<unsigned> >
TestDeluxe::fill(std::vector<int> x, std::vector<int> y, unsigned nx, unsigned ny)
{
    CEvent e;
    std::vector<CParameter> xs;
    std::vector<CParameter> ys;
    
    for (int i = 0; i < x.size(); i++) {
        xs.push_back(CParameter("x", i, "arb"));
        e[i] = x[i];
    }
    for (int i = 0; i < y.size(); i++) {
        int n = i+x.size();
        ys.push_back(CParameter("y", n, "arb"));
        e[n] = y[i];
    }
    
    CGamma2DDL spec("test", 0, xs, ys, nx, ny);
    spec(e);
    
    return std::pair<std::vector<unsigned>, std::vector<unsigned> >(
        spec.getUnderflows(), spec.getOverflows()
    );
}

CPPUNIT_TEST_SUITE_REGISTRATION(TestDeluxe);

void TestDeluxe::allIn() {
    std::vector<int> x;
    std::vector<int> y;
    
    for (int i =0; i < 50; i++) {
        x.push_back(12);
        y.push_back(50);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fill(x,y, 1024, 1024);
    
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(0), result.first[0]);
    EQ(unsigned(0), result.first[1]);
    
    EQ(unsigned(0), result.second[0]);
    EQ(unsigned(0), result.second[1]);
}

void TestDeluxe::xUnder() {
    std::vector<int> x;
    std::vector<int> y;
    
    for (int i =0; i < 50; i++) {
        x.push_back(-1);
        y.push_back(50);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fill(x,y, 1024, 1024);
    
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(50*50), result.first[0]);
    EQ(unsigned(0), result.first[1]);
    
    EQ(unsigned(0), result.second[0]);
    EQ(unsigned(0), result.second[1]);
}

void TestDeluxe::yUnder() {
    std::vector<int> x;
    std::vector<int> y;
    
    for (int i =0; i < 50; i++) {
        x.push_back(12);
        y.push_back(-1);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fill(x,y, 1024, 1024);
    
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(0), result.first[0]);
    EQ(unsigned(50*50), result.first[1]);
    
    EQ(unsigned(0), result.second[0]);
    EQ(unsigned(0), result.second[1]);
}

void TestDeluxe::xOver() {
    std::vector<int> x;
    std::vector<int> y;
    
    for (int i =0; i < 50; i++) {
        x.push_back(1024);
        y.push_back(50);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fill(x,y, 1024, 1024);
    
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(0), result.first[0]);
    EQ(unsigned(0), result.first[1]);
    
    EQ(unsigned(50*50), result.second[0]);
    EQ(unsigned(0), result.second[1]);
}
void TestDeluxe::yOver() {
    std::vector<int> x;
    std::vector<int> y;
    
    for (int i =0; i < 50; i++) {
        x.push_back(12);
        y.push_back(1024);
    }
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fill(x,y, 1024, 1024);
    
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(0), result.first[0]);
    EQ(unsigned(0), result.first[1]);
    
    EQ(unsigned(0), result.second[0]);
    EQ(unsigned(50*50), result.second[1]);
}

void TestDeluxe::mixed()
{
    int v[3] = {-1, 1024, 50};
    unsigned xc[3] = {0,0,0};
    unsigned yc[3] = {0,0,0};
    
    std::vector<int> x;
    std::vector<int> y;
    
    for (int i = 0; i < 50; i++) {
        int xidx = i % 3;
        int yidx = (i+1) %3;
        x.push_back(v[xidx]);
        xc[xidx]++;
        
        y.push_back(v[yidx]);
        yc[yidx]++;
    }
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result = fill(x,y, 1024, 1024);
    
    EQ((xc[0])*50, result.first[0]);
    EQ((yc[0])*50, result.first[1]);
    
    EQ(xc[1] * 50, result.second[0]);
    EQ(yc[1] * 50, result.second[1]);
}
