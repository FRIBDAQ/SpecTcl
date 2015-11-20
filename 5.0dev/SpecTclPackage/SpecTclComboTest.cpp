// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#define private public
#include "CSpecTclCombo2dIncrementer.h"
#undef private

#include "CAxis.h"
#include "CSpectrum.h"
#include "CTrueCondition.h"
#include "CParameter.h"
#include "CSpecTclSpectrumAllocator.h"


class SpecTclComboTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SpecTclComboTest);
  CPPUNIT_TEST(construct);
  CPPUNIT_TEST(constructbound);
  CPPUNIT_TEST(constructpartbound);

  CPPUNIT_TEST(valid);
  // CPPUNIT_TEST(partiallyvalid);  // no need since construction tested this.


  CPPUNIT_TEST(incr1);
  CPPUNIT_TEST(incrmultiple);
  CPPUNIT_TEST(incrnotbound);

  CPPUNIT_TEST(getok);
  // CPPUNIT_TEST(getbadx);  // Redundant tests,
  // CPPUNIT_TEST(getbady);  // Tested in 2d tests.

  CPPUNIT_TEST(clear);

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

  void valid();

  void incr1();
  void incrmultiple();
  void incrnotbound();

  void getok();

  void clear();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SpecTclComboTest);

/**
 * construct
 *   Simple construction with no names that bind
 *   bound but empty.
 */
void SpecTclComboTest::construct() {
  std::vector<std::string> names;
  CSpecTclCombo2dIncrementer inc(names, names);



  EQ(static_cast<size_t>(0), inc.m_xNames.size());
  EQ(static_cast<size_t>(0), inc.m_yNames.size());
  EQ(static_cast<size_t>(0), inc.m_xParams.size());
  EQ(static_cast<size_t>(0), inc.m_yParams.size());

  ASSERT(inc.m_fullyBound);
}
/**
 * constructbound
 *
 * Constructing with paramters that exist results in a
 * bound spectrum
 */
void
SpecTclComboTest::constructbound()
{
  std::vector<std::string> x;
  std::vector<std::string> y;
  x.push_back((new CParameter("a"))->getName());
  x.push_back((new CParameter("b"))->getName());

  y.push_back((new CParameter("c"))->getName());
  y.push_back((new CParameter("d"))->getName());


  CSpecTclCombo2dIncrementer inc(x, y);
  
  EQ(static_cast<size_t>(2), inc.m_xNames.size());
  EQ(static_cast<size_t>(2), inc.m_yNames.size());
  EQ(static_cast<size_t>(2), inc.m_xParams.size());
  EQ(static_cast<size_t>(2), inc.m_yParams.size());
  ASSERT(inc.m_fullyBound);

}
/**
 * constructpartbound
 *
 *   If not all of the parameters get defined,
 *   then m_fullyBound will be false.
 */
void
SpecTclComboTest::constructpartbound()
{
  std::vector<std::string> x;
  std::vector<std::string> y;
  x.push_back((new CParameter("a"))->getName());
  x.push_back((new CParameter("b"))->getName());

  y.push_back((new CParameter("c"))->getName());
  y.push_back("d");		// No such parameter


  CSpecTclCombo2dIncrementer inc(x, y);

  ASSERT(!inc.m_fullyBound);
  
}
/**
 * valid
 *   First make the spectrum, then the parameters and
 *   then validate the spectrum.  If all parameters are
 *   created, the spectrum should get bound and the
 *   x/y params should point to the right data structs
 *   in the parameter dictionary.
 */
void
SpecTclComboTest::valid()
{
  std::vector<std::string> xNames;
  std::vector<std::string> yNames;

  xNames.push_back("a");
  xNames.push_back("b");

  yNames.push_back("c");
  yNames.push_back("d");

  CTrueCondition* t = new CTrueCondition;
  CSpecTclSpectrumAllocator alloc;
  CSpecTclCombo2dIncrementer* pInc = new CSpecTclCombo2dIncrementer(xNames, yNames);
  std::vector<CAxis*> axes;
  axes.push_back(new CAxis("x", 512, 0, 511));
  axes.push_back(new CAxis("y", 512, 0, 511));

  CSpectrum spec("test", &alloc, pInc, t, axes);
  
  // Now create the parameters and validate:

  CParameter a("a");
  CParameter b("b");
  CParameter c("c");
  CParameter d("d");
  spec.checkValidity();

  ASSERT(pInc->m_fullyBound);

  CParameterDictionary* pDict = CParameterDictionary::instance();
  CParameterDictionary::DictionaryIterator p = pDict->find("a");
  ASSERT( pInc->m_xParams.count(p->second));
  p = pDict->find("b");
  ASSERT( pInc->m_xParams.count(p->second));

  p = pDict->find("c");
  ASSERT( pInc->m_yParams.count(p->second));
  p = pDict->find("d");
  ASSERT( pInc->m_yParams.count(p->second));





}
/**
 * incr1
 *   A single increment of a fully bound channel:
 */
void
SpecTclComboTest::incr1()
{
  std::vector<std::string> xNames;
  std::vector<std::string> yNames;

  xNames.push_back("a");
  xNames.push_back("b");

  yNames.push_back("c");
  yNames.push_back("d");

  CTrueCondition* t = new CTrueCondition;
  CSpecTclSpectrumAllocator alloc;
  CSpecTclCombo2dIncrementer* pInc = new CSpecTclCombo2dIncrementer(xNames, yNames);
  std::vector<CAxis*> axes;
  axes.push_back(new CAxis("x", 512, 0, 511));
  axes.push_back(new CAxis("y", 512, 0, 511));

  CSpectrum spec("test", &alloc, pInc, t, axes);
  
  // Now create the parameters and validate:

  CParameter a("a");
  CParameter b("b");
  CParameter c("c");
  CParameter d("d");
  spec.checkValidity();		// Fully bound spectrum.

  a = 32;
  c = 64;
  spec();			// Increment (32,64).

  uint32_t* pData = reinterpret_cast<uint32_t*>(spec.getHandle());
  EQ(static_cast<uint32_t>(1), pData[32 + 64*512]);
}
/**
 * incrmultiple
 *    Should be able to multiply increment:
 */
void
SpecTclComboTest::incrmultiple()
{
  std::vector<std::string> xNames;
  std::vector<std::string> yNames;

  xNames.push_back("a");
  xNames.push_back("b");

  yNames.push_back("c");
  yNames.push_back("d");

  CTrueCondition* t = new CTrueCondition;
  CSpecTclSpectrumAllocator alloc;
  CSpecTclCombo2dIncrementer* pInc = new CSpecTclCombo2dIncrementer(xNames, yNames);
  std::vector<CAxis*> axes;
  axes.push_back(new CAxis("x", 512, 0, 511));
  axes.push_back(new CAxis("y", 1024, 0, 1023));

  CSpectrum spec("test", &alloc, pInc, t, axes);
  
  // Now create the parameters and validate:

  CParameter a("a");
  CParameter b("b");
  CParameter c("c");
  CParameter d("d");
  spec.checkValidity();		// Fully bound spectrum.

  a = 32;
  b = 200;
  c = 64;
  d = 100;
  spec();			

  uint32_t* pData= reinterpret_cast<uint32_t*>(spec.getHandle());
  EQ(static_cast<uint32_t>(1), pData[32 + 64*512]);
  EQ(static_cast<uint32_t>(1), pData[32 + 100*512]);
  EQ(static_cast<uint32_t>(1), pData[200 + 64*512]);
  EQ(static_cast<uint32_t>(1), pData[200 + 100*512]);


}
/**
 * incrnotbound
 *
 *   Unbound spectra don't increment:
 */
void
SpecTclComboTest::incrnotbound()
{
  std::vector<std::string> xNames;
  std::vector<std::string> yNames;

  xNames.push_back("a");
  xNames.push_back("b");

  yNames.push_back("c");
  yNames.push_back("d");

  CTrueCondition* t = new CTrueCondition;
  CSpecTclSpectrumAllocator alloc;
  CSpecTclCombo2dIncrementer* pInc = new CSpecTclCombo2dIncrementer(xNames, yNames);
  std::vector<CAxis*> axes;
  axes.push_back(new CAxis("x", 512, 0, 511));
  axes.push_back(new CAxis("y", 1024, 0, 1023));

  CSpectrum spec("test", &alloc, pInc, t, axes);
  
  // Now create the parameters and validate:

  CParameter a("a");
  CParameter b("b");
  CParameter c("c");
  // CParameter d("d");
  spec.checkValidity();		// Fully bound spectrum.

  a = 32;
  b = 200;
  c = 64;
  //  d = 100;
  spec();			

  uint32_t* pData= reinterpret_cast<uint32_t*>(spec.getHandle());
  EQ(static_cast<uint32_t>(0), pData[32 + 64*512]);
  EQ(static_cast<uint32_t>(0), pData[200 + 64*512]);
}
/**
 * getok
 *    See that we can get a channel given valid x/y channel numbers.
 */
void
SpecTclComboTest::getok()
{
  std::vector<std::string> xNames;
  std::vector<std::string> yNames;

  xNames.push_back("a");
  xNames.push_back("b");

  yNames.push_back("c");
  yNames.push_back("d");
  CTrueCondition* t = new CTrueCondition;
  CSpecTclSpectrumAllocator alloc;
  CSpecTclCombo2dIncrementer* pInc = new CSpecTclCombo2dIncrementer(xNames, yNames);
  std::vector<CAxis*> axes;
  axes.push_back(new CAxis("x", 512, 0, 511));
  axes.push_back(new CAxis("y", 1024, 0, 1023));

  CSpectrum spec("test", &alloc, pInc, t, axes);
  
  // Now create the parameters and validate:

  CParameter a("a");
  CParameter b("b");
  CParameter c("c");
  CParameter d("d");
  spec.checkValidity();		// Fully bound spectrum.
		

  uint32_t* pData= reinterpret_cast<uint32_t*>(spec.getHandle());
  for (int y = 0; y < 1024; y++) {
    for (int x = 0; x < 512; x++) {
      *pData++ = x + 512*y;
    }
  }

  uint32_t v = spec.get(5,8);
  EQ(static_cast<uint32_t>(8*512 + 5), v);
}
/** 
 * clear
 *   Clear a spectrum.
 */
void
SpecTclComboTest::clear()
{
  std::vector<std::string> xNames;
  std::vector<std::string> yNames;

  xNames.push_back("a");
  xNames.push_back("b");

  yNames.push_back("c");
  yNames.push_back("d");
  CTrueCondition* t = new CTrueCondition;
  CSpecTclSpectrumAllocator alloc;
  CSpecTclCombo2dIncrementer* pInc = new CSpecTclCombo2dIncrementer(xNames, yNames);
  std::vector<CAxis*> axes;
  axes.push_back(new CAxis("x", 512, 0, 511));
  axes.push_back(new CAxis("y", 1024, 0, 1023));

  CSpectrum spec("test", &alloc, pInc, t, axes);
  
  // Now create the parameters and validate:

  CParameter a("a");
  CParameter b("b");
  CParameter c("c");
  CParameter d("d");
  spec.checkValidity();		// Fully bound spectrum.
		

  uint32_t* pData= reinterpret_cast<uint32_t*>(spec.getHandle());
  for (int y = 0; y < 1024; y++) {
    for (int x = 0; x < 512; x++) {
      *pData++ = x + 512*y;
    }
  }

  spec.clear();
  pData= reinterpret_cast<uint32_t*>(spec.getHandle());
  for (int y = 0; y < 1024; y++) {
    for (int x = 0; x < 512; x++) {
      EQ(static_cast<uint32_t>(0), *pData);
      pData++;
    }
  }
 
}
