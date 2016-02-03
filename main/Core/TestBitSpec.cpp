// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <vector>

#include "Parameter.h"
#include "Event.h"

#include "BitSpectrumL.h"
#include "BitSpectrumW.h"



class TestBitSpec : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TestBitSpec);
  CPPUNIT_TEST(inspecL);
  CPPUNIT_TEST(uflowL);
  CPPUNIT_TEST(oflowL);
  CPPUNIT_TEST(bothL);
  
  CPPUNIT_TEST(inspecW);
  CPPUNIT_TEST(uflowW);
  CPPUNIT_TEST(oflowW);
  CPPUNIT_TEST(bothW);
  CPPUNIT_TEST_SUITE_END();


private:
    std::pair<std::vector<unsigned>, std::vector<unsigned> >
        fillL(unsigned mask, unsigned low, unsigned high);
    std::pair<std::vector<unsigned>, std::vector<unsigned> >
        fillW(unsigned mask, unsigned low, unsigned high);
public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void inspecL();
  void uflowL();
  void oflowL();
  void bothL();
  
  void inspecW();
  void uflowW();
  void oflowW();
  void bothW();
};

// Fill spectrum and return statistics .first is underflor, .second over.

std::pair<std::vector<unsigned>, std::vector<unsigned> >
TestBitSpec::fillL(unsigned mask, unsigned low, unsigned high)
{
    CParameter p("test", 0, "arb");
    CEvent    e;
    CBitSpectrumL spec("Test", 0, p, low, high);
    
    e[0] = mask;
    
    spec(e);
    
    return std::pair<std::vector<unsigned>, std::vector<unsigned> >(spec.getUnderflows(), spec.getOverflows());
}

std::pair<std::vector<unsigned>, std::vector<unsigned> >
TestBitSpec::fillW(unsigned mask, unsigned low, unsigned high)
{
    CParameter p("test", 0, "arb");
    CEvent    e;
    CBitSpectrumW spec("Test", 0, p, low, high);
    
    e[0] = mask;
    
    spec(e);
    
    return std::pair<std::vector<unsigned>, std::vector<unsigned> >(spec.getUnderflows(), spec.getOverflows());
}

CPPUNIT_TEST_SUITE_REGISTRATION(TestBitSpec);

// Tests for longword spectra:

// All bits are in range.

void TestBitSpec::inspecL() {
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillL(0xfff0, 4, 16);
        
    EQ(size_t(1), result.first.size());
    EQ(size_t(1), result.second.size());
    
    EQ(unsigned(0), result.first[0]);
    EQ(unsigned(0), result.second[0]);
}

// Underflow bits:

void TestBitSpec::uflowL()
{
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillL(0xffff, 4, 16);
        
    
    EQ(unsigned(4), result.first[0]);
    EQ(unsigned(0), result.second[0]);
}
// Overflow bits:

void TestBitSpec::oflowL()
{
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillL(0xffff0, 4, 16);
        
    
    EQ(unsigned(0), result.first[0]);
    EQ(unsigned(4), result.second[0]);
}
// Both over and under are possible for bits:

void TestBitSpec::bothL()
{
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillL(0xfffff, 4, 16);
        
    
    EQ(unsigned(4), result.first[0]);
    EQ(unsigned(4), result.second[0]);
}

// word spectrum tests:


// All bits are in range.

void TestBitSpec::inspecW() {
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillW(0xfff0, 4, 16);
        
    EQ(size_t(1), result.first.size());
    EQ(size_t(1), result.second.size());
    
    EQ(unsigned(0), result.first[0]);
    EQ(unsigned(0), result.second[0]);
}

// Underflow bits:

void TestBitSpec::uflowW()
{
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillW(0xffff, 4, 16);
        
    
    EQ(unsigned(4), result.first[0]);
    EQ(unsigned(0), result.second[0]);
}
// Overflow bits:

void TestBitSpec::oflowW()
{
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillW(0xffff0, 4, 16);
        
    
    EQ(unsigned(0), result.first[0]);
    EQ(unsigned(4), result.second[0]);
}
// Both over and under are possible for bits:

void TestBitSpec::bothW()
{
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fillW(0xfffff, 4, 16);
        
    
    EQ(unsigned(4), result.first[0]);
    EQ(unsigned(4), result.second[0]);
}