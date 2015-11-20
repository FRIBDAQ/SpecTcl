// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#define private public
#include "CSpecTcl2dSumIncrementer.h"
#undef private
#include "CParameterDictionary.h"
#include "CParameter.h"
#include "CSpectrum.h"
#include "CAxis.h"
#include "CTrueCondition.h"
#include "CSpecTclSpectrumAllocator.h"


#include <stdexcept>
#include <string>
#include <set>

static bool in(std::set<std::pair<double, double> > coords, double x, double y) {
  std::pair<double, double> pt(x, y);
  return (coords.find(pt) != coords.end());
}


class SpecTcl2dSumIncTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SpecTcl2dSumIncTest);
  CPPUNIT_TEST(construct);
  CPPUNIT_TEST(constructok);
  CPPUNIT_TEST(constructmissing);
  CPPUNIT_TEST(constructmismatch);
  
  CPPUNIT_TEST(validate);

  CPPUNIT_TEST(increment1);	// Increment a single axis pair.
  CPPUNIT_TEST(incrementn);  // Increment all axis pairs.
  CPPUNIT_TEST(incrementcrossed); // 'crossed' param pairs don't increment.
  CPPUNIT_TEST(incunbound);   // Unbound spectra never increment.

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
  void constructok();
  void constructmissing();
  void constructmismatch();

  void validate();


  void increment1();
  void incrementn();
  void incrementcrossed();
  void incunbound();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SpecTcl2dSumIncTest);

/**
 * construct
 *    Construct the incrementer.
 *    - m_xNames is stocked from xNames
 *    - m_yNames is empty is stocked from yNames
 *    - m_xParams is empty.
 *    - m_yParams is empty.
 *    - m_bound -> false.
 */
void SpecTcl2dSumIncTest::construct() {
  std::vector<std::string> xNames;
  std::vector<std::string> yNames;

  xNames.push_back("a");
  xNames.push_back("b");
  xNames.push_back("c");

  yNames.push_back("x");
  yNames.push_back("y");
  yNames.push_back("z");

  CSpecTcl2dSumIncrementer inc(xNames, yNames);

  ASSERT(xNames == inc.m_xNames);
  ASSERT(yNames == inc.m_yNames);
  ASSERT(inc.m_xParams.empty());
  ASSERT(inc.m_yParams.empty());
  ASSERT(!inc.m_bound);
  
}
/**
 * constructok
 *   Constructing with parameters that are defined:
 * - m_xParams has parameters for the x names.
 * - m_yParams has parameters for the y names.
 * - m_bound -> true.
 */
void
SpecTcl2dSumIncTest::constructok()
{
  
  std::vector<std::string> xNames;
  std::vector<std::string> yNames;

  xNames.push_back((new CParameter("a"))->getName());
  xNames.push_back((new CParameter("b"))->getName());
  xNames.push_back((new CParameter("c"))->getName());



  yNames.push_back((new CParameter("x"))->getName());
  yNames.push_back((new CParameter("y"))->getName());
  yNames.push_back((new CParameter("z"))->getName());


  CSpecTcl2dSumIncrementer inc(xNames, yNames);

  ASSERT(inc.m_bound);
  EQ(xNames.size(), inc.m_xParams.size());
  EQ(yNames.size(), inc.m_yParams.size());

  CParameterDictionary* pDict = CParameterDictionary::instance();
  for (int i = 0; i < xNames.size(); i++) {
    CParameterDictionary::DictionaryIterator p = pDict->find(xNames[i]);
    EQ(p->second, inc.m_xParams[i]);
    p = pDict->find(yNames[i]);
    EQ(p->second, inc.m_yParams[i]);
  }
}

/**
 * constructmissing
 *    If a parameter is not defined, m_bound is not true.
 */
void
SpecTcl2dSumIncTest::constructmissing()
{
  
  std::vector<std::string> xNames;
  std::vector<std::string> yNames;

  xNames.push_back((new CParameter("a"))->getName());
  xNames.push_back("b");
  xNames.push_back((new CParameter("c"))->getName());



  yNames.push_back((new CParameter("x"))->getName());
  yNames.push_back((new CParameter("y"))->getName());
  yNames.push_back((new CParameter("z"))->getName());


  CSpecTcl2dSumIncrementer inc(xNames, yNames);
  ASSERT(!inc.m_bound);
}
/**
 * constructmismatch
 *  - If the xparams and yparams arrays are not the same length,
 *    a length_error should be thrown.
 */
void
SpecTcl2dSumIncTest::constructmismatch()
{
  std::vector<std::string> xNames;
  std::vector<std::string> yNames;

  xNames.push_back((new CParameter("a"))->getName());
  xNames.push_back((new CParameter("c"))->getName());



  yNames.push_back((new CParameter("x"))->getName());
  yNames.push_back((new CParameter("y"))->getName());
  yNames.push_back((new CParameter("z"))->getName());

  bool thrown = false;
  bool rthrow = false;
  std::string msg;
 
  try {
    CSpecTcl2dSumIncrementer inc(xNames, yNames);
  }
  catch (std::length_error l) {
    thrown = true;
    rthrow = true;
    msg    = l.what();
  }
  catch(...) {
    thrown = true;
  }
  ASSERT(thrown);
  ASSERT(rthrow);
  EQ(std::string("CSpecTcl2dSumIncrementer: Must be same number of x and y parameters"),
     msg);
}
/**
 * validate:
 *    Construct the spectrum, later all the parameters and then
 *    use validate to bind it:
 */
void
SpecTcl2dSumIncTest::validate()
{
  std::vector<CAxis*>        axes;
  CTrueCondition*            pCond = new CTrueCondition;
  std::vector<std::string>   xParams;
  std::vector<std::string>   yParams;
  CSpecTclSpectrumAllocator  alloc;

  axes.push_back(new CAxis("x", 512, 0, 511));
  axes.push_back(new CAxis("y", 512, 0, 511));

  xParams.push_back("a");
  xParams.push_back("b");
  xParams.push_back("c");
  
  yParams.push_back("x");
  yParams.push_back("y");
  yParams.push_back("z");
  CSpecTcl2dSumIncrementer* pInc = new CSpecTcl2dSumIncrementer(xParams, yParams);

  CSpectrum spec("test", &alloc, pInc, pCond, axes);

  // Create the parameters.

  new CParameter("a");
  new CParameter("b");
  new CParameter("c");
  new CParameter("x");
  new CParameter("y");
  new CParameter("z");

  spec.checkValidity();	       

  ASSERT(pInc->m_bound);
}
/**
 * increment1
 *
 *  Check the increment of a single axis pair...should 
 *  increment a single channel... once.
 */
void
SpecTcl2dSumIncTest::increment1()
{

  // Make a good valid spectrum:


  std::vector<CAxis*>        axes;
  CTrueCondition*            pCond = new CTrueCondition;
  std::vector<std::string>   xParams;
  std::vector<std::string>   yParams;
  CSpecTclSpectrumAllocator  alloc;

  axes.push_back(new CAxis("x", 512, 0, 511));
  axes.push_back(new CAxis("y", 512, 0, 511));

  xParams.push_back("a");
  xParams.push_back("b");
  xParams.push_back("c");
  
  yParams.push_back("x");
  yParams.push_back("y");
  yParams.push_back("z");

  CParameter* a = new CParameter("a");
  CParameter* b = new CParameter("b");
  CParameter* c = new CParameter("c");
  CParameter* x = new CParameter("x");
  CParameter* y = new CParameter("y");
  CParameter* z = new CParameter("z");

  CSpecTcl2dSumIncrementer* pInc = new CSpecTcl2dSumIncrementer(xParams, yParams);

 
  CSpectrum spec("test", &alloc, pInc, pCond, axes);

  // Set the a, x parameters and truigger the increment.

  *a = 100.0;
  *x = 50.0;

  spec.clear();
  spec();

  // Check the channel:

  void* pData = spec.getHandle();
  uint32_t* pChans = reinterpret_cast<uint32_t*>(pData);
  
  for(int x = 0; x < 512; x++) {
    for(int y = 0; y < 512; y++) {
      uint32_t expected = 0;
      if ((x == 100) && (y == 50)) {
	expected = 1;
      }
      EQ(expected, pChans[x + y * 512]);
    }
  }
}
/**
 * incrementn
 *    Set valid values for all axis pairs.. shouild get 3 increments:
 */
void
SpecTcl2dSumIncTest::incrementn()
{
  // Make a good valid spectrum:


  std::vector<CAxis*>        axes;
  CTrueCondition*            pCond = new CTrueCondition;
  std::vector<std::string>   xParams;
  std::vector<std::string>   yParams;
  CSpecTclSpectrumAllocator  alloc;

  axes.push_back(new CAxis("x", 512, 0, 511));
  axes.push_back(new CAxis("y", 512, 0, 511));

  xParams.push_back("a");
  xParams.push_back("b");
  xParams.push_back("c");
  
  yParams.push_back("x");
  yParams.push_back("y");
  yParams.push_back("z");

  CParameter* a = new CParameter("a");
  CParameter* b = new CParameter("b");
  CParameter* c = new CParameter("c");
  CParameter* x = new CParameter("x");
  CParameter* y = new CParameter("y");
  CParameter* z = new CParameter("z");

  CSpecTcl2dSumIncrementer* pInc = new CSpecTcl2dSumIncrementer(xParams, yParams);

 
  CSpectrum spec("test", &alloc, pInc, pCond, axes);

  // Set the parameter values (saved as a set of pairs too).
  // trigger the increment:

  std::set<std::pair<double, double> >  coords;

  *a = 50;  *x = 50; coords.insert(std::pair<double, double>(50, 50));

  *b = 100; *y = 125; coords.insert(std::pair<double, double>(100, 125));

  *c = 128; *z = 128; coords.insert(std::pair<double, double>(128, 128));

  spec.clear();
  spec();			// Trigger the increment.

  // Spectrum should be zero except for the points int he set:

  void* pData = spec.getHandle();
  uint32_t* pChans = reinterpret_cast<uint32_t*>(pData);
  
  for(int x = 0; x < 512; x++) {
    for(int y = 0; y < 512; y++) {
      uint32_t expected = 0;
      if (in(coords, x,y)) {
	expected = 1;
      }
      EQ(expected, pChans[x + y * 512]);
    }
  }

}
/**
 * incrementcrossed
 *
 *  Data must be on matching parameter pairs:
 */
void SpecTcl2dSumIncTest::incrementcrossed()
{
  // Make a good valid spectrum:


  std::vector<CAxis*>        axes;
  CTrueCondition*            pCond = new CTrueCondition;
  std::vector<std::string>   xParams;
  std::vector<std::string>   yParams;
  CSpecTclSpectrumAllocator  alloc;

  axes.push_back(new CAxis("x", 512, 0, 511));
  axes.push_back(new CAxis("y", 512, 0, 511));

  xParams.push_back("a");
  xParams.push_back("b");
  xParams.push_back("c");
  
  yParams.push_back("x");
  yParams.push_back("y");
  yParams.push_back("z");

  CParameter* a = new CParameter("a");
  CParameter* b = new CParameter("b");
  CParameter* c = new CParameter("c");
  CParameter* x = new CParameter("x");
  CParameter* y = new CParameter("y");
  CParameter* z = new CParameter("z");

  CSpecTcl2dSumIncrementer* pInc = new CSpecTcl2dSumIncrementer(xParams, yParams);

 
  CSpectrum spec("test", &alloc, pInc, pCond, axes);

  // Set the a, x parameters and truigger the increment.

  *a = 123.0;
  *y = 100.0;			// a pairs with x not y.

  spec.clear();
  spec();

  // All channels must be zero:

  void* pData = spec.getHandle();
  uint32_t* pChans = reinterpret_cast<uint32_t*>(pData);
  
  for(int x = 0; x < 512; x++) {
    for(int y = 0; y < 512; y++) {
      uint32_t expected = 0;
      EQ(expected, pChans[x + y * 512]);
    }
  }
}
/**
 * incunbound
 *
 *  Never increment if unbound.
 */
void
SpecTcl2dSumIncTest::incunbound()
{
  // Make a good valid spectrum:


  std::vector<CAxis*>        axes;
  CTrueCondition*            pCond = new CTrueCondition;
  std::vector<std::string>   xParams;
  std::vector<std::string>   yParams;
  CSpecTclSpectrumAllocator  alloc;

  axes.push_back(new CAxis("x", 512, 0, 511));
  axes.push_back(new CAxis("y", 512, 0, 511));

  xParams.push_back("a");
  xParams.push_back("b");
  xParams.push_back("c");
  
  yParams.push_back("x");
  yParams.push_back("y");
  yParams.push_back("z");

  CParameter* a = new CParameter("a");
  CParameter* b = new CParameter("b");
  CParameter* c = new CParameter("c");
  CParameter* x = new CParameter("x");
  // CParameter* y = new CParameter("y");
  CParameter* z = new CParameter("z");

  CSpecTcl2dSumIncrementer* pInc = new CSpecTcl2dSumIncrementer(xParams, yParams);

 
  CSpectrum spec("test", &alloc, pInc, pCond, axes);

  // Set the a, x parameters and truigger the increment.

  *a = 100.0;
  *x = 50.0;

  spec.clear();
  spec();

  // Check the channel:

  void* pData = spec.getHandle();
  uint32_t* pChans = reinterpret_cast<uint32_t*>(pData);
  
  for(int x = 0; x < 512; x++) {
    for(int y = 0; y < 512; y++) {
      uint32_t expected = 0;
      EQ(expected, pChans[x + y * 512]);
    }
  }
}
