// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#define private public
#include "CSpecTcl2dIncrementer.h"
#undef private
#include "CParameter.h"
#include "CParameterDictionary.h"
#include "CTrueCondition.h"
#include "CSpecTclSpectrumAllocator.h"
#include "CAxis.h"
#include <CSpectrum.h>


#include <vector>
#include <string>
#include <stdexcept>


class SpecTcl2dIncTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SpecTcl2dIncTest);
  CPPUNIT_TEST(construct);
  CPPUNIT_TEST(constructbound);

  CPPUNIT_TEST(validateok);
  CPPUNIT_TEST(validateyok);
  CPPUNIT_TEST(validatexok);
  
  CPPUNIT_TEST(cleared);

  CPPUNIT_TEST(inc);
  CPPUNIT_TEST(incbadx);
  CPPUNIT_TEST(inconlyx);
  CPPUNIT_TEST(inconlyy);

  CPPUNIT_TEST(setok);
  CPPUNIT_TEST(setbadxcoord);
  CPPUNIT_TEST(setbadycoord);

  CPPUNIT_TEST(getok);
  // CPPUNIT_TEST(getbadx);
  // CPPUNIT_TEST(getbady);


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

  void validateok();
  void validateyok();
  void validatexok();

  void cleared();

  void inc();
  void incbadx();
  void inconlyx();
  void inconlyy();

  void setok();
  void setbadxcoord();
  void setbadycoord();

  void getok();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SpecTcl2dIncTest);

/**
 * construction
 *  - The parameter names should be set.
 *  - the parameter info ptrs should be null.
 *  - The bound flag should be false.
 */
void SpecTcl2dIncTest::construct() {
  CSpecTcl2dIncrementer s("xpar", "ypar");

  EQ(std::string("xpar"), s.m_xParameterName);
  EQ(std::string("ypar"), s.m_yParameterName);
  EQ(reinterpret_cast<CParameterDictionary::ParameterInfo*>(0),
     s.m_xParameter);  
  EQ(reinterpret_cast<CParameterDictionary::ParameterInfo*>(0),
     s.m_yParameter);
  ASSERT(! s.m_bound);
}
/**
 * constructbound
 *
 *  If the parameter names passed to the constructor represent
 *  valid parameters, the m_xParametrer and m_yParameter
 *  sb.non null and m_bound <- true.
 */
void
SpecTcl2dIncTest::constructbound()
{
  CParameter& p1(*(new CParameter("xpar")));
  CParameter& p2(*(new CParameter("ypar")));

  CSpecTcl2dIncrementer s("xpar", "ypar");
  CParameterDictionary* pDict = CParameterDictionary::instance();
  CParameterDictionary::DictionaryIterator pX = pDict->find("xpar");
  CParameterDictionary::DictionaryIterator pY = pDict->find("ypar");

  EQ(pX->second, s.m_xParameter);
  EQ(pY->second, s.m_yParameter);
  ASSERT(s.m_bound);
}
/**
 * validateok
 *
 * Make a spectrum using the incrementer.
 * Make the parameters.
 * validate the spectrum.
 *
 * Should have stuff marked valid.
 *

 */
void
SpecTcl2dIncTest::validateok()
{
  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl2dIncrementer*     pInc = new CSpecTcl2dIncrementer("testx", "testy");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  CAxis*                     pY   = new CAxis("y", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
  axes.push_back(pY);

  CSpectrum spec("test", &alloc, pInc, cond, axes);

  CParameter* x = new CParameter("testx");
  CParameter* y = new CParameter("testy");

  spec.checkValidity();

  CParameterDictionary* pDict = CParameterDictionary::instance();
  CParameterDictionary::DictionaryIterator pIx   = pDict->find("testx");
  CParameterDictionary::DictionaryIterator pIy   = pDict->find("testy");

  EQ(pIx->second, pInc->m_xParameter);
  EQ(pIy->second, pInc->m_yParameter);

  ASSERT(pInc->m_bound);


 
}
/**
 * validateyok
 *
 *  The y parameter is valid but not x:
 */
void
SpecTcl2dIncTest::validateyok()
{
  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl2dIncrementer*     pInc = new CSpecTcl2dIncrementer("testx", "testy");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  CAxis*                     pY   = new CAxis("y", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
  axes.push_back(pY);

  CSpectrum spec("test", &alloc, pInc, cond, axes);
  CParameter* y = new CParameter("testy");

  spec.checkValidity();

  CParameterDictionary* pDict = CParameterDictionary::instance();
  CParameterDictionary::DictionaryIterator pIy   = pDict->find("testy");


  EQ(reinterpret_cast<CParameterDictionary::ParameterInfo*>(0),
     pInc->m_xParameter);
  EQ(pIy->second, pInc->m_yParameter);
  ASSERT(! pInc->m_bound);
}
/**
 * validatexok
 *   Validation where only the x parameter is defined.
 */
void
SpecTcl2dIncTest::validatexok()
{
  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl2dIncrementer*     pInc = new CSpecTcl2dIncrementer("testx", "testy");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  CAxis*                     pY   = new CAxis("y", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
  axes.push_back(pY);

  CSpectrum spec("test", &alloc, pInc, cond, axes);
  CParameter* x = new CParameter("testx");

  spec.checkValidity();

  CParameterDictionary* pDict = CParameterDictionary::instance();
  CParameterDictionary::DictionaryIterator pIx   = pDict->find("testx");


  EQ(reinterpret_cast<CParameterDictionary::ParameterInfo*>(0),
     pInc->m_yParameter);
  EQ(pIx->second, pInc->m_xParameter);
  ASSERT(! pInc->m_bound);
}
/**
 * cleared
 *
 *   Invoking cleared (happens at Spectrum construction time)
 *   clears the channels in the spectrum.
 */
void
SpecTcl2dIncTest::cleared()
{
  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl2dIncrementer*     pInc = new CSpecTcl2dIncrementer("testx", "testy");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  CAxis*                     pY   = new CAxis("y", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
  axes.push_back(pY);

  CSpectrum spec("test", &alloc, pInc, cond, axes);
  CParameter* x = new CParameter("testx");

  void* pData = spec.getHandle();
  uint32_t* pChans = reinterpret_cast<uint32_t*>(pData);

  pChans[10] = 100;		// in case malloc gave me zeroed memory:

  spec.clear();

  for(int i =0; i < 512*512; i++) {
    EQ(static_cast<uint32_t>(0), pChans[i]);
  }
}
/**
 * inc
 *
 *  Increment a cell of the spectrum.
 */
void
SpecTcl2dIncTest::inc()
{

  CParameter* x = new CParameter("testx");
  CParameter* y = new CParameter("testy");

  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl2dIncrementer*     pInc = new CSpecTcl2dIncrementer("testx", "testy");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  CAxis*                     pY   = new CAxis("y", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
  axes.push_back(pY);



  CSpectrum spec("test", &alloc, pInc, cond, axes);


  (*x) = 100;
  (*y) = 200;
  spec();			// Do the increment.

  uint32_t* p = reinterpret_cast<uint32_t*>(spec.getHandle());
  EQ(static_cast<uint32_t>(1), p[100 + 200*512]);
}
/**
 *  incbadx - increment when the X parameters is not defined.
 */
void
SpecTcl2dIncTest::incbadx()
{
  CParameter* x = new CParameter("testx");
  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl2dIncrementer*     pInc = new CSpecTcl2dIncrementer("testx", "testy");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  CAxis*                     pY   = new CAxis("y", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
  axes.push_back(pY);



  CSpectrum spec("test", &alloc, pInc, cond, axes);


  (*x) = 100;

  spec();			// Do the increment.
  void* pData = spec.getHandle();
  uint32_t* pChans = reinterpret_cast<uint32_t*>(pData);

  for(int i =0; i < 512*512; i++) {
    EQ(static_cast<uint32_t>(0), pChans[i]);
  }
}
/**
 * inconlyx - increment but the event only has an x parameter value.
 */
void
SpecTcl2dIncTest::inconlyx()
{
  CParameter* x = new CParameter("testx");
  CParameter* y = new CParameter("testy");

  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl2dIncrementer*     pInc = new CSpecTcl2dIncrementer("testx", "testy");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  CAxis*                     pY   = new CAxis("y", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
  axes.push_back(pY);



  CSpectrum spec("test", &alloc, pInc, cond, axes);


  (*x) = 100;

  spec();			// Do the increment.
  void* pData = spec.getHandle();
  uint32_t* pChans = reinterpret_cast<uint32_t*>(pData);

  for(int i =0; i < 512*512; i++) {
    EQ(static_cast<uint32_t>(0), pChans[i]);
  }
}
/**
 * inconlyy - increment the event but only y parameter is valid.
 */
void
SpecTcl2dIncTest::inconlyy()
{
  CParameter* x = new CParameter("testx");
  CParameter* y = new CParameter("testy");

  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl2dIncrementer*     pInc = new CSpecTcl2dIncrementer("testx", "testy");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  CAxis*                     pY   = new CAxis("y", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
  axes.push_back(pY);



  CSpectrum spec("test", &alloc, pInc, cond, axes);


  (*y) = 100;

  spec();			// Do the increment.
  void* pData = spec.getHandle();
  uint32_t* pChans = reinterpret_cast<uint32_t*>(pData);

  for(int i =0; i < 512*512; i++) {
    EQ(static_cast<uint32_t>(0), pChans[i]);
  }
}
/**
 * setok
 *
 *   Set a spectrum channel value
 */
void
SpecTcl2dIncTest::setok()
{
  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl2dIncrementer*     pInc = new CSpecTcl2dIncrementer("testx", "testy");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  CAxis*                     pY   = new CAxis("y", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
  axes.push_back(pY);



  CSpectrum spec("test", &alloc, pInc, cond, axes);

  spec.set(100, 200, 1234);

  uint32_t* p = reinterpret_cast<uint32_t*>(spec.getHandle());

  EQ(static_cast<uint32_t>(1234), p[100 + 200*512]);
}
/**
 * setbadxcoord
 *
 *   Do a set with a bad x coordinate... shoult throw an out_of_range exception.
 */
void
SpecTcl2dIncTest::setbadxcoord()
{
  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl2dIncrementer*     pInc = new CSpecTcl2dIncrementer("testx", "testy");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  CAxis*                     pY   = new CAxis("y", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
  axes.push_back(pY);



  CSpectrum spec("test", &alloc, pInc, cond, axes);

  bool thrown = false;
  bool rthrow = false;
  std::string msg;
  
  try {
    spec.set(1234, 10, 1234);

  }
  catch (std::out_of_range& e) {
    thrown = true;
    rthrow = true;
    msg = e.what();
  }
  catch(...) {
    thrown = true;
  }

  ASSERT(thrown);
  ASSERT(rthrow);
  EQ(std::string("Invalid channel number 1234 must be in the range [0..511]"),
     msg);
  
}
/**
 * setbadycoord - same as above but the y coordinate is out of range.
 */
void
SpecTcl2dIncTest::setbadycoord()
{
  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl2dIncrementer*     pInc = new CSpecTcl2dIncrementer("testx", "testy");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  CAxis*                     pY   = new CAxis("y", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
  axes.push_back(pY);



  CSpectrum spec("test", &alloc, pInc, cond, axes);

  bool thrown = false;
  bool rthrow = false;
  std::string msg;
  
  try {
    spec.set(123, 1234, 1234);

  }
  catch (std::out_of_range& e) {
    thrown = true;
    rthrow = true;
    msg = e.what();
  }
  catch(...) {
    thrown = true;
  }

  ASSERT(thrown);
  ASSERT(rthrow);
  EQ(std::string("Invalid channel number 1234 must be in the range [0..511]"),
     msg);
}
/**
 * getok  Get should return the value of a chanenl.
 */
void
SpecTcl2dIncTest::getok()
{
  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl2dIncrementer*     pInc = new CSpecTcl2dIncrementer("testx", "testy");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  CAxis*                     pY   = new CAxis("y", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
  axes.push_back(pY);



  CSpectrum spec("test", &alloc, pInc, cond, axes);

  spec.set(100, 200, 1234);

  EQ(static_cast<uint32_t>(1234), spec.get(100, 200));
}
