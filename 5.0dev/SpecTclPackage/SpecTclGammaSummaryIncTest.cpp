// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#define private public
#define protected public
#include "CSpecTclGammaSummaryIncrementer.h"
#undef private
#undef protected

#include "CParameter.h"
#include "CSpectrum.h"
#include "CTrueCondition.h"
#include "CAxis.h"
#include "CSpecTclSpectrumAllocator.h"


#include <stdint.h>
#include <vector>
#include <set>


class SpecTclGamSumIncTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SpecTclGamSumIncTest);
  CPPUNIT_TEST(construct);
  CPPUNIT_TEST(constructbound);
  CPPUNIT_TEST(constructpartbound);

  CPPUNIT_TEST(validate);

  CPPUNIT_TEST(inc);
  CPPUNIT_TEST(partialinc);


  CPPUNIT_TEST_SUITE_END();


private:
  
public:
  void setUp() {
  }
  void tearDown() {
    delete CParameterDictionary::m_pInstance;
    CParameterDictionary::m_pInstance = 0;
  }
protected:
  void construct();
  void constructbound();
  void constructpartbound();
  
  void validate();

  void inc();
  void partialinc();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SpecTclGamSumIncTest);

/**
 * construct
 *
 *  Construction will 
 *  1. Stock m_names.
 *  2. Set m_bound false
 *
 *  - Assuming all parameters exist.
 */
void SpecTclGamSumIncTest::construct() {

  // Channels of the spectrum:

  CSpecTclGammaSummaryIncrementer::ChannelParameterNames first, second, third;

  first.push_back("a"); first.push_back("b");
  second.push_back("c"); second.push_back("d"); second.push_back("e");
  third.push_back("last1"); third.push_back("last2");

  CSpecTclGammaSummaryIncrementer::SpectrumParameterNames names;
  names.push_back(first); names.push_back(second); names.push_back(third);

  CSpecTclGammaSummaryIncrementer inc(names);

  for (int i = 0; i < names.size(); i++) {
    ASSERT(!(inc.m_bound[i]));
    for(int j = 0; j < names[i].size(); j++) {
      EQ(names[i][j], inc.m_names[i][j]);
    }
  }

  
}
/**
 * constructbound
 *
 *    Construct an incrementer with parameters bound.
 *    - All elements of m_bound should be true.
 *    - Should be able to find the parameters in the m_params
 *      vector of vectors.
 */
void SpecTclGamSumIncTest::constructbound()
{
  const char* ns[] = {
    "p1", "p2", "p3", "p4", NULL,
    "a1", "a2", "a3", NULL,
    "v", "v2", NULL
  };
  size_t nNames = sizeof(ns)/sizeof(const char*);

  CSpecTclGammaSummaryIncrementer::ChannelParameterNames chnames;
  CSpecTclGammaSummaryIncrementer::SpectrumParameterNames names;

  for (int i = 0; i < nNames; i++) {
    const char* pName = ns[i];
    if (!pName) {
      names.push_back(chnames);
      chnames.clear();
    } else {
      chnames.push_back(pName);
      new CParameter(pName);
	
    }
  }
  CSpecTclGammaSummaryIncrementer inc(names);

  for (int ch = 0; ch < names.size(); ch++) {
    ASSERT(inc.m_bound[ch]);
    for (int i = 0; i < names[i].size(); i++) {
      EQ(names[ch][i], (inc.m_params)[ch][i]->s_references.front()->getName());
    }
  }

}
/**
 * constructpartbound
 *
 * Same as constructorbound except that we are only going
 * to create parameters for every other channel so every other
 * channel will have m_bound false...we don't really care what the
 * m_params for the unbound chans are:
 */
void SpecTclGamSumIncTest::constructpartbound()
{
  const char* ns[] = {
    "p1", "p2", "p3", "p4", NULL,
    "a1", "a2", "a3", NULL,
    "v", "v2", NULL
  };
  size_t nNames = sizeof(ns)/sizeof(const char*);

  CSpecTclGammaSummaryIncrementer::ChannelParameterNames chnames;
  CSpecTclGammaSummaryIncrementer::SpectrumParameterNames names;

  int ich = 0;
  for (int i = 0; i < nNames; i++) {
    const char* pName = ns[i];
    if (!pName) {
      names.push_back(chnames);
      chnames.clear();
      ich++;
    } else {
      chnames.push_back(pName);
      if ((ich% 2) == 0) new CParameter(pName);
	
    }
  }
  
  CSpecTclGammaSummaryIncrementer inc(names);
  for (int ch = 0; ch < names.size(); ch++) {
    ASSERT(((ch %2) == 0) ? inc.m_bound[ch] : !inc.m_bound[ch]);

  }


}
/**
 * validate
 *   Construct a spectrum and validate it.  This version should
 *   succeeed in getting all channels bound.
 *
 */
void SpecTclGamSumIncTest::validate()
{
  const char* ns[] = {
    "p1", "p2", "p3", "p4", NULL,
    "a1", "a2", "a3", NULL,
    "v", "v2", NULL
  };
  size_t nNames = sizeof(ns)/sizeof(const char*);

  CSpecTclGammaSummaryIncrementer::ChannelParameterNames chnames;
  CSpecTclGammaSummaryIncrementer::SpectrumParameterNames names;

  for (int i = 0; i < nNames; i++) {
    const char* pName = ns[i];
    if (!pName) {
      names.push_back(chnames);
      chnames.clear();
    } else {
      chnames.push_back(pName);	
    }
  } 

  CSpecTclGammaSummaryIncrementer inc(names); // not bound...
  
  for (int ch = 0; ch < names.size(); ch++) {
    ASSERT(!inc.m_bound[ch]);
  }
  

  for (int i = 0; i < nNames; i++) {
    const char* pName = ns[i];
    if (pName) {
      new CParameter(pName);	
    }
  } 

  // now its valid;  We happen to know that validate is not
  // going to use the spectrum so the following cheat is easier
  // than building

  inc.validate(*reinterpret_cast<CSpectrum*>(0));
  for (int ch = 0; ch < names.size(); ch++) {
    ASSERT(inc.m_bound[ch]);
  }
  
}
/**
 * inc - increment from a few parameter values.
 */
void
SpecTclGamSumIncTest::inc()
{
  // for this we need to make a full spectrum:

  // Parameter definitions.

  const char* ns[] = {
    "p1", "p2", "p3", "p4", NULL,
    "a1", "a2", "a3", NULL,
    "v", "v2", NULL
  };
  size_t nNames = sizeof(ns)/sizeof(const char*);

  CSpecTclGammaSummaryIncrementer::ChannelParameterNames chnames;
  CSpecTclGammaSummaryIncrementer::SpectrumParameterNames names;

  for (int i = 0; i < nNames; i++) {
    const char* pName = ns[i];
    if (!pName) {
      names.push_back(chnames);
      chnames.clear();
    } else {
      chnames.push_back(pName);
      new CParameter(pName);
	
    }
  }
  CTrueCondition* pCond = new CTrueCondition;
  CSpecTclSpectrumAllocator alloc;
  CSpecTclGammaSummaryIncrementer* 
    pInc = new CSpecTclGammaSummaryIncrementer(names);
  CAxis*   px = new CAxis("channels", 3, 0, 2);
  CAxis*   py = new CAxis("y", 511, 0, 511);
  std::vector<CAxis*>axes;
  axes.push_back(px);
  axes.push_back(py);

  CSpectrum spec("Test", &alloc, pInc, pCond, axes);


  // Increment a few channels in each x channel:

  CParameter p1("p1");
  CParameter p3("p3"); p1 = 5; p3 = 100;

  CParameter a2("a2");
  CParameter a3("a3"); a2 = 123; a3 = 500;
  
  CParameter v("v"); v = 42;

  spec();			// process the 'event'.

  // build a set of channels with 1's in them

  std::set<std::pair<int, int> > nonzeros;
  nonzeros.insert(std::pair<int, int>(0, 5));
  nonzeros.insert(std::pair<int, int>(0, 100));
  nonzeros.insert(std::pair<int, int>(1, 123));
  nonzeros.insert(std::pair<int, int>(1, 500));
  nonzeros.insert(std::pair<int, int>(2, 42));
  

  for (int x = 0; x < 3; x++) {
    for (int y = 0; y < 511; y++) {
      EQ(static_cast<uint32_t>(nonzeros.count(std::pair<int, int>(x,y))),
			       *(pInc->pChannel(spec, x, y)));
    }
  }
}
/**
 * partialinc
 *
 *   Only params defined as of the last validation
 *   are incremented.
 */
void
SpecTclGamSumIncTest::partialinc()
{
 // for this we need to make a full spectrum:

  // Parameter definitions.

  const char* ns[] = {
    "p1", "p2", "p3", "p4", NULL,
    "a1", "a2", "a3", NULL,
    "v", "v2", NULL
  };
  size_t nNames = sizeof(ns)/sizeof(const char*);

  CSpecTclGammaSummaryIncrementer::ChannelParameterNames chnames;
  CSpecTclGammaSummaryIncrementer::SpectrumParameterNames names;

  int ch = 0;
  for (int i = 0; i < nNames; i++) {
    const char* pName = ns[i];
    if (!pName) {
      names.push_back(chnames);
      chnames.clear();
      ch++;
    } else {
      chnames.push_back(pName);
      if ((ch % 2) == 0)new CParameter(pName);
	
    }
  }
  CTrueCondition* pCond = new CTrueCondition;
  CSpecTclSpectrumAllocator alloc;
  CSpecTclGammaSummaryIncrementer* 
    pInc = new CSpecTclGammaSummaryIncrementer(names);
  CAxis*   px = new CAxis("channels", 3, 0, 2);
  CAxis*   py = new CAxis("y", 511, 0, 511);
  std::vector<CAxis*>axes;
  axes.push_back(px);
  axes.push_back(py);

  CSpectrum spec("Test", &alloc, pInc, pCond, axes);

  // Not defined in the spectrum.

  new CParameter("a2");
  new CParameter("a3");


  // Increment a few channels in each x channel:

  CParameter p1("p1");
  CParameter p3("p3"); p1 = 5; p3 = 100;

  CParameter a2("a2");
  CParameter a3("a3"); a2 = 123; a3 = 500;
  
  CParameter v("v"); v = 42;

  spec();			// process the 'event'.

  // build a set of channels with 1's in them

  std::set<std::pair<int, int> > nonzeros;
  nonzeros.insert(std::pair<int, int>(0, 5));
  nonzeros.insert(std::pair<int, int>(0, 100));
  //  nonzeros.insert(std::pair<int, int>(1, 123));
  //  nonzeros.insert(std::pair<int, int>(1, 500));
  nonzeros.insert(std::pair<int, int>(2, 42));
  

  for (int x = 0; x < 3; x++) {
    for (int y = 0; y < 511; y++) {
      EQ(static_cast<uint32_t>(nonzeros.count(std::pair<int, int>(x,y))),
			       *(pInc->pChannel(spec, x, y)));
    }
  }
}
