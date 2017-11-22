// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <vector>

#include "Parameter.h"
#include "Spectrum1DL.h"
#include "Spectrum1DW.h"
#include <Event.h>

class Test1d : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(Test1d);
  CPPUNIT_TEST(inspec1DL);
  CPPUNIT_TEST(under1DL);
  CPPUNIT_TEST(over1DL);
  
  CPPUNIT_TEST(inspec1DW);
  CPPUNIT_TEST(under1DW);
  CPPUNIT_TEST(over1DW);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void inspec1DL();
  void under1DL();
  void over1DL();
  
  void inspec1DW();
  void under1DW();
  void over1DW();
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test1d);

// Neither under nor over flow
void Test1d::inspec1DL()
{
    CParameter   p("test", 0, "arb");
    CSpectrum1DL test("test", 1, p, 100);
    CEvent       e;
    e[0] = 50;
    test(e);

    std::vector<unsigned> stats = test.getOverflows();
    EQ(size_t(1), stats.size());
    EQ(unsigned(0), stats[0]);
    
    stats = test.getUnderflows();
    EQ(size_t(1), stats.size());
    EQ(unsigned(0), stats[0]);
    
}
// Underflow
void Test1d::under1DL()
{
    CParameter   p("test", 0, "arb");
    CSpectrum1DL test("test", 1, p, 100);
    CEvent       e;
    
    e[0] = -1;   // Underflow.
    test(e);
    
    std::vector<unsigned> stats = test.getOverflows();
    EQ(size_t(1), stats.size());
    EQ(unsigned(0), stats[0]);
    
    stats = test.getUnderflows();
    EQ(unsigned(1), stats[0]);
}
// Overflow
void Test1d::over1DL()
{
    CParameter   p("test", 0, "arb");
    CSpectrum1DL test("test", 1, p, 100);
    CEvent       e;
    
    e[0] = 100;   // Underflow.
    test(e);
    
    std::vector<unsigned> stats = test.getOverflows();
    EQ(size_t(1), stats.size());
    EQ(unsigned(1), stats[0]);
    
    stats = test.getUnderflows();
    EQ(size_t(1), stats.size());
    EQ(unsigned(0), stats[0]);
    
}


// Neither under nor over flow
void Test1d::inspec1DW()
{
    CParameter   p("test", 0, "arb");
    CSpectrum1DW test("test", 1, p, 100);
    CEvent       e;
    e[0] = 50;
    test(e);

    std::vector<unsigned> stats = test.getOverflows();
    EQ(size_t(1), stats.size());
    EQ(unsigned(0), stats[0]);
    
    stats = test.getUnderflows();
    EQ(size_t(1), stats.size());
    EQ(unsigned(0), stats[0]);
    
}
// Underflow
void Test1d::under1DW()
{
    CParameter   p("test", 0, "arb");
    CSpectrum1DW test("test", 1, p, 100);
    CEvent       e;
    
    e[0] = -1;   // Underflow.
    test(e);
    
    std::vector<unsigned> stats = test.getOverflows();
    EQ(size_t(1), stats.size());
    EQ(unsigned(0), stats[0]);
    
    stats = test.getUnderflows();
    EQ(size_t(1), stats.size());
    EQ(unsigned(1), stats[0]);
}
// Overflow
void Test1d::over1DW()
{
    CParameter   p("test", 0, "arb");
    CSpectrum1DW test("test", 1, p, 100);
    CEvent       e;
    
    e[0] = 100;   // Underflow.
    test(e);
    
    std::vector<unsigned> stats = test.getOverflows();
    EQ(size_t(1), stats.size());
    EQ(unsigned(1), stats[0]);
    
    stats = test.getUnderflows();
    EQ(size_t(1), stats.size());
    EQ(unsigned(0), stats[0]);
    
}