// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#define private public
#include "CSpecTclSummaryIncrementer.h"
#undef private

#include "CParameter.h"
#include "CParameterDictionary.h"
#include "CTrueCondition.h"
#include "CAxis.h"
#include "CSpecTclSpectrumAllocator.h"
#include "CSpectrum.h"


class SpecTclSummaryTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SpecTclSummaryTest);
  CPPUNIT_TEST(construct);
  CPPUNIT_TEST(constructbound);
  CPPUNIT_TEST(constructunbound);

  CPPUNIT_TEST(bound);
  CPPUNIT_TEST(notbound);

  CPPUNIT_TEST(inc1);
  CPPUNIT_TEST(inc2);
  CPPUNIT_TEST(incunbound);
  CPPUNIT_TEST(rangecheck);

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
  void constructunbound();

  void bound();
  void notbound();
  
  void inc1();
  void inc2();
  void incunbound();
  void rangecheck();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SpecTclSummaryTest);

/**
 * construct
 *
 *   In construction with an empty parameter name vector,
 *    - m_names is empty
 *    - m_params is empty.
 *    - m_bound = true   since all names got looked up ok.
 */
void SpecTclSummaryTest::construct() {
  std::vector<std::string> names;
  CSpecTclSummaryIncrementer i(names);

  EQ(static_cast<size_t>(0), i.m_Names.size());
  EQ(static_cast<size_t>(0), i.m_Params.size());
  ASSERT(i.m_bound);
}

/**
 * constructbound
 *
 *  Constructing with existing parameters:
 *  - Fills in m_Names
 *  - Fills in m_Params in the same order as m_Names
 *  - Sets m_bound -> true
 */
void
SpecTclSummaryTest::constructbound()
{
  std::vector<std::string> names;
  names.push_back((new CParameter("a"))->getName());
  names.push_back((new CParameter("b"))->getName());
  names.push_back((new CParameter("c"))->getName());

  CSpecTclSummaryIncrementer inc(names);
  EQ(static_cast<size_t>(3), inc.m_Names.size());
  EQ(static_cast<size_t>(3), inc.m_Params.size());
  ASSERT(inc.m_bound);

  CParameterDictionary* pDict = CParameterDictionary::instance();

  for(int i = 0; i < 3; i++) {
    CParameterDictionary::DictionaryIterator p = pDict->find(inc.m_Names[i]);
    EQ(p->second, inc.m_Params[i]);
  }
 
}
/**
 * constructunbound
 *
 *  If one of the parameters does not exist, the incrementer winds up
 *  unbound:
 */
void
SpecTclSummaryTest::constructunbound()
{
 std::vector<std::string> names;
  names.push_back("a");
  names.push_back((new CParameter("b"))->getName());
  names.push_back((new CParameter("c"))->getName());

  CSpecTclSummaryIncrementer inc(names);

  ASSERT(!inc.m_bound);

}
/**
 * bound
 *    validate is bound if all parameters are currently defined.
 */
void
SpecTclSummaryTest::bound()
{
  std::vector<std::string>  names;
  std::vector<CAxis*>       axes;
  CSpecTclSpectrumAllocator alloc;

  names.push_back("a");
  names.push_back("b");
  names.push_back("c");
  names.push_back("d");
  axes.push_back(new CAxis("parameters", 4, 0, 3));
  axes.push_back(new CAxis("spectra",   4096, 0, 4095));
  CTrueCondition* pTrue = new CTrueCondition;
  CSpecTclSummaryIncrementer* pInc;

  CSpectrum spec("test", &alloc, pInc = new CSpecTclSummaryIncrementer(names), pTrue, axes);

  new CParameter("a");
  new CParameter("b");
  new CParameter("c");
  new CParameter("d");
  spec.checkValidity();

  // Now we should be bound:

  ASSERT(pInc->m_bound);

  // Check that we're bound to the right stuff:


  CParameterDictionary* pDict = CParameterDictionary::instance();
  for (int i = 0; i < names.size(); i++) {
    CParameterDictionary::DictionaryIterator p = pDict->find(names[i]);
    EQ(p->second, pInc->m_Params[i]);
  }
  
  
}
/**
 * notbound
 *   Validate with parameters that don't exist flips to unbound
 */
void
SpecTclSummaryTest::notbound()
{
  std::vector<std::string>  names;
  std::vector<CAxis*>       axes;
  CSpecTclSpectrumAllocator alloc;

  names.push_back("a");
  names.push_back("b");
  names.push_back("c");
  names.push_back("d");
  axes.push_back(new CAxis("parameters", 4, 0, 3));
  axes.push_back(new CAxis("spectra",   4096, 0, 4095));
  CTrueCondition* pTrue = new CTrueCondition;
  CSpecTclSummaryIncrementer* pInc;

  CSpectrum spec("test", &alloc, pInc = new CSpecTclSummaryIncrementer(names), pTrue, axes);

  new CParameter("a");
  new CParameter("b");
  CParameter* c = new CParameter("c");
  new CParameter("d");
  spec.checkValidity();

  // Now we should be bound:

  ASSERT(pInc->m_bound);

  // Kill off c and check again

  delete c;
  spec.checkValidity();
  ASSERT(!pInc->m_bound);
  
}
/**
 * inc1
 *    Single increment test.
 */
void 
SpecTclSummaryTest::inc1()
{
  std::vector<std::string>  names;
  std::vector<CAxis*>       axes;
  CSpecTclSpectrumAllocator alloc;

  names.push_back("a");
  names.push_back("b");
  names.push_back("c");
  names.push_back("d");
  axes.push_back(new CAxis("parameters", 4, 0, 3));
  axes.push_back(new CAxis("spectra",   4096, 0, 4095));
  CTrueCondition* pTrue = new CTrueCondition;
  CSpecTclSummaryIncrementer* pInc;

  CSpectrum spec("test", &alloc, pInc = new CSpecTclSummaryIncrementer(names), pTrue, axes);

  new CParameter("a");
  new CParameter("b");
  CParameter* c = new CParameter("c");
  new CParameter("d");
  spec.checkValidity();

  *c = 1234;
  spec();
  EQ(static_cast<uint32_t>(1), spec.get(2, 1234));
}
/**
 * inc2
 *   check multiple increments.
 */
void 
SpecTclSummaryTest::inc2()
{
  std::vector<std::string>  names;
  std::vector<CAxis*>       axes;
  CSpecTclSpectrumAllocator alloc;

  names.push_back("a");
  names.push_back("b");
  names.push_back("c");
  names.push_back("d");
  axes.push_back(new CAxis("parameters", 4, 0, 3));
  axes.push_back(new CAxis("spectra",   4096, 0, 4095));
  CTrueCondition* pTrue = new CTrueCondition;
  CSpecTclSummaryIncrementer* pInc;

  CSpectrum spec("test", &alloc, pInc = new CSpecTclSummaryIncrementer(names), pTrue, axes);

  CParameter* a = new CParameter("a");
  CParameter* b = new CParameter("b");
  CParameter* c = new CParameter("c");
  CParameter* d = new CParameter("d");
  spec.checkValidity();

  *a = 42;
  *b = 3.1415;
  *c = 1234;
  *d = 100;

  spec();

  EQ(static_cast<uint32_t>(1), spec.get(0, 42));
  EQ(static_cast<uint32_t>(1), spec.get(1, 3));
  EQ(static_cast<uint32_t>(1), spec.get(2, 1234));
  EQ(static_cast<uint32_t>(1), spec.get(3, 100));
}
/**
 * incunbound 
 *   Nothing increments if spectrum unbound.
 */
void
SpecTclSummaryTest::incunbound()
{
  std::vector<std::string>  names;
  std::vector<CAxis*>       axes;
  CSpecTclSpectrumAllocator alloc;

  names.push_back("a");
  names.push_back("b");
  names.push_back("c");
  names.push_back("d");
  axes.push_back(new CAxis("parameters", 4, 0, 3));
  axes.push_back(new CAxis("spectra",   4096, 0, 4095));
  CTrueCondition* pTrue = new CTrueCondition;
  CSpecTclSummaryIncrementer* pInc;

  CSpectrum spec("test", &alloc, pInc = new CSpecTclSummaryIncrementer(names), pTrue, axes);

  CParameter* a = new CParameter("a");
  //  CParameter* b = new CParameter("b");
  CParameter* c = new CParameter("c");
  CParameter* d = new CParameter("d");
  spec.checkValidity();

  *a = 42;
  // *b = 3.1415;
  *c = 1234;
  *d = 100;

  spec();

  for (int y = 0; y < 4096; y++) {
    for(int x = 0; x < 4; x++) {
      EQ(static_cast<uint32_t>(0), spec.get(x,y));
    }
  }

}
/**
 * rangecheck
 *
 *  Out of range does not increment rather than spilling over into 
 *  another channel:
 */
void
SpecTclSummaryTest::rangecheck()
{
  std::vector<std::string>  names;
  std::vector<CAxis*>       axes;
  CSpecTclSpectrumAllocator alloc;

  names.push_back("a");
  names.push_back("b");
  names.push_back("c");
  names.push_back("d");
  axes.push_back(new CAxis("parameters", 4, 0, 3));
  axes.push_back(new CAxis("spectra",   4096, 0, 4095));
  CTrueCondition* pTrue = new CTrueCondition;
  CSpecTclSummaryIncrementer* pInc;

  CSpectrum spec("test", &alloc, pInc = new CSpecTclSummaryIncrementer(names), pTrue, axes);

  CParameter* a = new CParameter("a");
  CParameter* b = new CParameter("b");
  CParameter* c = new CParameter("c");
  CParameter* d = new CParameter("d");
  spec.checkValidity();

  *a = 10000;

  spec();

  for (int y = 0; y < 4096; y++) {
    for(int x = 0; x < 4; x++) {
      EQ(static_cast<uint32_t>(0), spec.get(x,y));
    }
  }

}
