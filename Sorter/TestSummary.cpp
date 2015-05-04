// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <vector>

#include "Parameter.h"
#include "Event.h"
#include "SummarySpectrumB.h"
#include "SummarySpectrumW.h"
#include "SummarySpectrumL.h"



class TestSummary : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TestSummary);
  CPPUNIT_TEST(allinB);
  CPPUNIT_TEST(allunderB);
  CPPUNIT_TEST(halfunderB);
  CPPUNIT_TEST(allOverB);
  CPPUNIT_TEST(halfOverB);
  
  CPPUNIT_TEST(allinW);
  CPPUNIT_TEST(allunderW);
  CPPUNIT_TEST(halfunderW);
  CPPUNIT_TEST(allOverW);
  CPPUNIT_TEST(halfOverW);
  
  CPPUNIT_TEST(allinL);
  CPPUNIT_TEST(allunderL);
  CPPUNIT_TEST(halfunderL);
  CPPUNIT_TEST(allOverL);
  CPPUNIT_TEST(halfOverL);
  CPPUNIT_TEST_SUITE_END();

  

private:
    std::pair<std::vector<unsigned>, std::vector<unsigned> >
        fillB(std::vector<int> chans, unsigned n);
    std::pair<std::vector<unsigned>, std::vector<unsigned> >
        fillW(std::vector<int> chans, unsigned n);
    std::pair<std::vector<unsigned>, std::vector<unsigned> >
        fillL(std::vector<int> chans, unsigned n);

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void allinB();
  void allunderB();
  void halfunderB();
  void allOverB();
  void halfOverB();
  
  void allinW();
  void allunderW();
  void halfunderW();
  void allOverW();
  void halfOverW();
  
  void allinL();
  void allunderL();
  void halfunderL();
  void allOverL();
  void halfOverL();

};



// Utilities to fill spectra and report the stats.
// for all of them parameter is a vector of channel increments, n the y chans.
// and the return is a pair where first is underflows and second overflows.
//

// Summary..B

std::pair<std::vector<unsigned>, std::vector<unsigned> >
TestSummary::fillB(std::vector<int> chans, unsigned n)
{
    // make the parameters-- no dict so we give them all the same name:
    
    std::vector<CParameter> parameters;
    CEvent                  e;
    for (int i = 0; i < chans.size(); i ++) {
        parameters.push_back(CParameter("test", i, "arb"));
        e[i] = chans[i];
    }
    CSummarySpectrumB spec("test", 1, parameters, n);
    
    spec(e);
    return std::pair<std::vector<unsigned>, std::vector<unsigned> >(
        spec.getUnderflows(), spec.getOverflows()
    );
}

// Summary..W

std::pair<std::vector<unsigned>, std::vector<unsigned> >
TestSummary::fillW(std::vector<int> chans, unsigned n)
{
    // make the parameters-- no dict so we give them all the same name:
    
    std::vector<CParameter> parameters;
    CEvent                  e;
    for (int i = 0; i < chans.size(); i ++) {
        parameters.push_back(CParameter("test", i, "arb"));
        e[i] = chans[i];
    }
    CSummarySpectrumW spec("test", 1, parameters, n);
    
    spec(e);
    return std::pair<std::vector<unsigned>, std::vector<unsigned> >(
        spec.getUnderflows(), spec.getOverflows()
    );
}


// Summmary..L


std::pair<std::vector<unsigned>, std::vector<unsigned> >
TestSummary::fillL(std::vector<int> chans, unsigned n)
{
    // make the parameters-- no dict so we give them all the same name:
    
    std::vector<CParameter> parameters;
    CEvent                  e;
    for (int i = 0; i < chans.size(); i ++) {
        parameters.push_back(CParameter("test", i, "arb"));
        e[i] = chans[i];
    }
    CSummarySpectrumL spec("test", 1, parameters, n);
    
    spec(e);
    return std::pair<std::vector<unsigned>, std::vector<unsigned> >(
        spec.getUnderflows(), spec.getOverflows()
    );
}
CPPUNIT_TEST_SUITE_REGISTRATION(TestSummary);

// Byte spectrum tests

// All channels are in range.

void TestSummary::allinB()
{
  std::vector<int> e;
  for (int i =0; i < 50; i++) {
    e.push_back(100);
  }
  std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
    fillB(e, 1024);
    
  // Both vectors are length 1 and have 0 in their 0'th element.
  
  EQ(size_t(1), result.first.size());
  EQ(size_t(1), result.second.size());
  
  EQ(unsigned(0), result.first[0]);
  EQ(unsigned(0), result.second[0]);
}

// all channels underflow:

void TestSummary::allunderB()
{
  std::vector<int> e;
  for (int i =0; i < 50; i++) {
    e.push_back(-1);
  }
  std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
    fillB(e, 1024);
    
  // SB  50 underflows.
  
  EQ(unsigned(50), result.first[0]);
  EQ(unsigned(0), result.second[0]);
}
// 1/2 of the channels are underflows:

void TestSummary::halfunderB()
{
  std::vector<int> e;
  for (int i =0; i < 50; i++) {
    e.push_back((i % 2) ? -1 : 123);
  }
  std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
    fillB(e, 1024);
    
  // SB  25 underflows.
  
  EQ(unsigned(25), result.first[0]);
  EQ(unsigned(0), result.second[0]);
}
// All channels are overflows:

void TestSummary::allOverB()
{
  std::vector<int> e;
  for (int i =0; i < 50; i++) {
    e.push_back(1024);
  }
  std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
    fillB(e, 1024);
    
  // SB  50 underflows.
  
  EQ(unsigned(0), result.first[0]);
  EQ(unsigned(50), result.second[0]);
}
 // 1/2 channels overflow:
 
 void TestSummary::halfOverB()
{
  std::vector<int> e;
  for (int i =0; i < 50; i++) {
    e.push_back((i % 2) ? 1024 : 123);
  }
  std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
    fillB(e, 1024);
    
  // SB  25 underflows.
  
  EQ(unsigned(0), result.first[0]);
  EQ(unsigned(25), result.second[0]);
}

// Word spectrum tests::


// All channels are in range.

void TestSummary::allinW()
{
  std::vector<int> e;
  for (int i =0; i < 50; i++) {
    e.push_back(100);
  }
  std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
    fillW(e, 1024);
    
  // Both vectors are length 1 and have 0 in their 0'th element.
  
  EQ(size_t(1), result.first.size());
  EQ(size_t(1), result.second.size());
  
  EQ(unsigned(0), result.first[0]);
  EQ(unsigned(0), result.second[0]);
}

// all channels underflow:

void TestSummary::allunderW()
{
  std::vector<int> e;
  for (int i =0; i < 50; i++) {
    e.push_back(-1);
  }
  std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
    fillW(e, 1024);
    
  // SB  50 underflows.
  
  EQ(unsigned(50), result.first[0]);
  EQ(unsigned(0), result.second[0]);
}
// 1/2 of the channels are underflows:

void TestSummary::halfunderW()
{
  std::vector<int> e;
  for (int i =0; i < 50; i++) {
    e.push_back((i % 2) ? -1 : 123);
  }
  std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
    fillW(e, 1024);
    
  // SB  25 underflows.
  
  EQ(unsigned(25), result.first[0]);
  EQ(unsigned(0), result.second[0]);
}
// All channels are overflows:

void TestSummary::allOverW()
{
  std::vector<int> e;
  for (int i =0; i < 50; i++) {
    e.push_back(1024);
  }
  std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
    fillW(e, 1024);
    
  // SB  50 underflows.
  
  EQ(unsigned(0), result.first[0]);
  EQ(unsigned(50), result.second[0]);
}
 // 1/2 channels overflow:
 
 void TestSummary::halfOverW()
{
  std::vector<int> e;
  for (int i =0; i < 50; i++) {
    e.push_back((i % 2) ? 1024 : 123);
  }
  std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
    fillW(e, 1024);
    
  // SB  25 underflows.
  
  EQ(unsigned(0), result.first[0]);
  EQ(unsigned(25), result.second[0]);
}


// Longword SPectra.


// All channels are in range.

void TestSummary::allinL()
{
  std::vector<int> e;
  for (int i =0; i < 50; i++) {
    e.push_back(100);
  }
  std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
    fillL(e, 1024);
    
  // Both vectors are length 1 and have 0 in their 0'th element.
  
  EQ(size_t(1), result.first.size());
  EQ(size_t(1), result.second.size());
  
  EQ(unsigned(0), result.first[0]);
  EQ(unsigned(0), result.second[0]);
}

// all channels underflow:

void TestSummary::allunderL()
{
  std::vector<int> e;
  for (int i =0; i < 50; i++) {
    e.push_back(-1);
  }
  std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
    fillL(e, 1024);
    
  // SB  50 underflows.
  
  EQ(unsigned(50), result.first[0]);
  EQ(unsigned(0), result.second[0]);
}
// 1/2 of the channels are underflows:

void TestSummary::halfunderL()
{
  std::vector<int> e;
  for (int i =0; i < 50; i++) {
    e.push_back((i % 2) ? -1 : 123);
  }
  std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
    fillL(e, 1024);
    
  // SB  25 underflows.
  
  EQ(unsigned(25), result.first[0]);
  EQ(unsigned(0), result.second[0]);
}
// All channels are overflows:

void TestSummary::allOverL()
{
  std::vector<int> e;
  for (int i =0; i < 50; i++) {
    e.push_back(1024);
  }
  std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
    fillL(e, 1024);
    
  // SB  50 underflows.
  
  EQ(unsigned(0), result.first[0]);
  EQ(unsigned(50), result.second[0]);
}
 // 1/2 channels overflow:
 
 void TestSummary::halfOverL()
{
  std::vector<int> e;
  for (int i =0; i < 50; i++) {
    e.push_back((i % 2) ? 1024 : 123);
  }
  std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
    fillL(e, 1024);
    
  // SB  25 underflows.
  
  EQ(unsigned(0), result.first[0]);
  EQ(unsigned(25), result.second[0]);
}