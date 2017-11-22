// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <vector>

#include "Parameter.h"
#include "Event.h"

#include "CGammaSummarySpectrum.h"

// Note gamma summary spectra _are_ templated (hoorah) so we only test the longs 


class TestGammaS : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TestGammaS);
  CPPUNIT_TEST(allin);
  CPPUNIT_TEST(allunder);
  CPPUNIT_TEST(allover);
  CPPUNIT_TEST(mixed1);
  CPPUNIT_TEST(mixed2);
  CPPUNIT_TEST_SUITE_END();


private:
    std::pair<std::vector<unsigned>, std::vector<unsigned> >
        fill(std::vector<std::vector<int> > values, unsigned n);
public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void allin();
  void allunder();
  void allover();
  void mixed1();
  void mixed2();
};

// fill
//   Note with root, gamma summaries are 2-d spectra but only y over/unders count
std::pair<std::vector<unsigned>, std::vector<unsigned> >
TestGammaS::fill(std::vector<std::vector<int> > values, unsigned n)
{
    CEvent e;
    std::vector<std::vector<CParameter> > params;
    int pno = 0;
    for (int i =0; i < values.size(); i++) {
        std::vector<int> chvalues = values[i];
        std::vector<CParameter> plist;
        for (int j= 0; j < chvalues.size(); j++) {
            plist.push_back(CParameter("test", pno, "arb"));
            e[pno] = chvalues[j];
            pno++;
        }
        params.push_back(plist);
    }
    CGammaSummarySpectrumL spec("test", 0, n, &params);
    
    spec(e);
    
    return std::pair<std::vector<unsigned>, std::vector<unsigned> >(
        spec.getUnderflows(), spec.getOverflows()
    );
    
}

CPPUNIT_TEST_SUITE_REGISTRATION(TestGammaS);

    // All values are in the spectrum.
    
void TestGammaS::allin()
{
    // Each channel has 5 parameters and there are 100 channels.
    
    std::vector<std::vector<int> > values;
    for (int i =0; i < 100; i++) {
        std::vector<int> colValues;
        for (int j = 0; j < 5; j++) {
            colValues.push_back(50);
        }
        values.push_back(colValues);
    }
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fill(values, 1024);
        
    EQ(size_t(2), result.first.size());
    EQ(size_t(2), result.second.size());
    
    EQ(unsigned(0), result.first[1]);
    EQ(unsigned(0), result.second[1]);
    
}
        // All values are underflows
        
void TestGammaS::allunder() 
{
    // Each channel has 5 parameters and there are 100 channels.
    
    std::vector<std::vector<int> > values;
    for (int i =0; i < 100; i++) {
        std::vector<int> colValues;
        for (int j = 0; j < 5; j++) {
            colValues.push_back(-1);
        }
        values.push_back(colValues);
    }
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fill(values, 1024);
        
    
    
    EQ(unsigned(500), result.first[1]);
    EQ(unsigned(0), result.second[1]);
    
}
    // All values are overflows.
    
void TestGammaS::allover()
{
    std::vector<std::vector<int> > values;
    for (int i =0; i < 100; i++) {
        std::vector<int> colValues;
        for (int j = 0; j < 5; j++) {
            colValues.push_back(1024);
        }
        values.push_back(colValues);
    }
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fill(values, 1024);
        
    
    
    EQ(unsigned(0), result.first[1]);
    EQ(unsigned(500), result.second[1]);
    
}
//   mixed by x channel 1/2 over 1/2 under.

void TestGammaS::mixed1()
{
    std::vector<std::vector<int> > values;
    for (int i =0; i < 100; i++) {
        std::vector<int> colValues;
        for (int j = 0; j < 5; j++) {
            colValues.push_back((i % 2) ? 1024 : -1);
        }
        values.push_back(colValues);
    }
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fill(values, 1024);
        
    
    
    EQ(unsigned(250), result.first[1]);
    EQ(unsigned(250), result.second[1]);   
}
// Mixed over/under/in within an individual x.

void TestGammaS::mixed2()
{
    std::vector<std::vector<int> > values;
    int vTable[3] = {1024, -1, 100};      // over, under, in
    int counters[3] = {0, 0, 0};
    
    for (int i =0; i < 100; i++) {
        std::vector<int> colValues;
        for (int j = 0; j < 5; j++) {
            int selector = (i+j) % 3;
            colValues.push_back(vTable[selector]);
            counters[selector]++;
        }
        values.push_back(colValues);
    }
    
    std::pair<std::vector<unsigned>, std::vector<unsigned> > result =
        fill(values, 1024);
        
    
    
    EQ(unsigned(counters[1]), result.first[1]);
    EQ(unsigned(counters[0]), result.second[1]);      
}