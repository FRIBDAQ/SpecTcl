// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#define private public
#include "CSpecTcl1dIncrementer.h"
#include "CParameterDictionary.h"
#undef private
#include "CParameter.h"
#include "CSpectrum.h"
#include "CTrueCondition.h"
#include "CAxis.h"
#include "CSpecTclSpectrumAllocator.h"


#include <stdint.h>
#include <vector>
#include <stdexcept>


class SpecTcl1dIncTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SpecTcl1dIncTest);
  CPPUNIT_TEST(construct1);
  CPPUNIT_TEST(construct2);

  CPPUNIT_TEST(invalidtovalid);
  CPPUNIT_TEST(validtoinvalid);

  CPPUNIT_TEST(isclear);

  CPPUNIT_TEST(incrementok);
  CPPUNIT_TEST(incrementtoobig);
  CPPUNIT_TEST(incrementtoosmall);
  CPPUNIT_TEST(inchighboundary);
  CPPUNIT_TEST(inclowboundary);
  CPPUNIT_TEST(incunbound);

  CPPUNIT_TEST(setok);
  CPPUNIT_TEST(setbadch);

  CPPUNIT_TEST(getok);
  CPPUNIT_TEST(getbadch);

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
  void construct1();
  void construct2();

  void invalidtovalid();
  void validtoinvalid();

  void isclear();

  void incrementok();
  void incrementtoobig();
  void incrementtoosmall();
  void inchighboundary();
  void inclowboundary();
  void incunbound();

  void setok();
  void setbadch();

  void getok();
  void getbadch();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SpecTcl1dIncTest);

/**
 * construct1:
 *
 *  Construction on a non-existent parameter
 *  - Name is saved.
 *  - parameter info pointer is null.
 */

void SpecTcl1dIncTest::construct1() {
  CSpecTcl1dIncrementer s("Testing");
  EQ(std::string("Testing"), s.m_parameterName);
  EQ(reinterpret_cast<CParameterDictionary::ParameterInfo*>(0), s.m_parameter);
}

/**
 * construct2
 *
 *  If there is a parameter by the requested name in the dictionary,
 *  the  created spectrum should get bound to it:
 */
void
SpecTcl1dIncTest::construct2()
{
  CParameter* p = new CParameter("test");
  CParameterDictionary* pDict = CParameterDictionary::instance();

  CSpecTcl1dIncrementer s("test");
  EQ(std::string("test"), s.m_parameterName);
  
  CParameterDictionary::DictionaryIterator pIter = pDict->find("test");
  EQ(pIter->second, s.m_parameter);
}
/**
 * invalidtovalid
 *
 *  Create a spectrum that does not have a valid parameter.
 *  Create the parameter
 *  Invoke validate
 *  The spectrum should be connected.
 */
void
SpecTcl1dIncTest::invalidtovalid()
{
  CSpecTcl1dIncrementer s("test"); //  We already know this is disconnected.

  CParameter* p = new CParameter("test");
  CParameterDictionary* pDict = CParameterDictionary::instance();

  s.validate(*(reinterpret_cast<CSpectrum*>(0))); // white box -- spectrum not used.

  CParameterDictionary::DictionaryIterator pIter = pDict->find("test");
  EQ(pIter->second, s.m_parameter);
}
/**
 * validtoinvalid
 *
 *  Create a spectrum that does have a valid parameter.
 *  destroy the parameter.
 *  invoke validate
 *  The spectrunm should not be connected.
 */
void
SpecTcl1dIncTest::validtoinvalid()
{
  CParameter* p = new CParameter("test");
  CParameterDictionary* pDict = CParameterDictionary::instance();

  CSpecTcl1dIncrementer s("test"); // we already know this is connected.

  delete p;

  s.validate(*(reinterpret_cast<CSpectrum*>(0)));

  EQ(reinterpret_cast<CParameterDictionary::ParameterInfo*>(0), s.m_parameter);
  
}
/**
 * isclear
 *
 *  When we put together all the pieces needed to make a
 *  spectrum the storage should be entirely filled with 0's.
 */
void
SpecTcl1dIncTest::isclear()
{
  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl1dIncrementer*     pInc = new CSpecTcl1dIncrementer("test");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
 
  CSpectrum spec("test", &alloc, pInc, cond, axes);

  void* pData = spec.getHandle();
  
  // storage should be 512 longwords of 0:

  uint32_t* pLongs = reinterpret_cast<uint32_t*>(pData);
  for (int i=0; i < 512; i++) {
    EQ(static_cast<uint32_t>(0), *pLongs++);
  }
}
/**
 * incrementok
 *
 *  Getting an increment done smack in the middle of the spectrum
 *  should increment the right channel of data.
 */
void
SpecTcl1dIncTest::incrementok()
{
  CParameter*                pParam = new CParameter("test");

  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl1dIncrementer*     pInc = new CSpecTcl1dIncrementer("test");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
 
  CSpectrum spec("test", &alloc, pInc, cond, axes);
  *pParam = 128;

  spec();			// increment.

  void* pData = spec.getHandle();
  uint32_t* pLongs = reinterpret_cast<uint32_t*>(pData);
  
  EQ(static_cast<uint32_t>(1), pLongs[128]);
  
}
/**
 * incrementtoobig
 *  No increment should happpen if the value of the
 *  data maps above the spectrum's right end.
 */
void
SpecTcl1dIncTest::incrementtoobig()
{
  CParameter*                pParam = new CParameter("test");

  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl1dIncrementer*     pInc = new CSpecTcl1dIncrementer("test");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
 
  CSpectrum spec("test", &alloc, pInc, cond, axes);
  *pParam = 600;

  spec();			// increment.

  void* pData = spec.getHandle();
  uint32_t* pLongs = reinterpret_cast<uint32_t*>(pData);
    
  for (int i=0; i < 512; i++) {
    EQ(static_cast<uint32_t>(0), *pLongs++);
  }
}
/**
 * incrementtoosmall
 *
 * No increment should happen if the data is below channel 0.
 */
void
SpecTcl1dIncTest::incrementtoosmall()
{
  CParameter*                pParam = new CParameter("test");

  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl1dIncrementer*     pInc = new CSpecTcl1dIncrementer("test");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
 
  CSpectrum spec("test", &alloc, pInc, cond, axes);
  *pParam = -100;

  spec();			// increment.

  void* pData = spec.getHandle();
  uint32_t* pLongs = reinterpret_cast<uint32_t*>(pData);
    
  for (int i=0; i < 512; i++) {
    EQ(static_cast<uint32_t>(0), *pLongs++);
  }
}
/**
 * inchighboundary
 * 
 *   An increment that's just over the upper boundary:
 */
void
SpecTcl1dIncTest::inchighboundary()
{
  CParameter*                pParam = new CParameter("test");

  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl1dIncrementer*     pInc = new CSpecTcl1dIncrementer("test");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
 
  CSpectrum spec("test", &alloc, pInc, cond, axes);
  *pParam = 511.1;

  spec();			// increment.

  void* pData = spec.getHandle();
  uint32_t* pLongs = reinterpret_cast<uint32_t*>(pData);
    
  for (int i=0; i < 512; i++) {
    EQ(static_cast<uint32_t>(0), *pLongs++);
  }
}
/**
 * inclowboundary
 *   Just below the low limit won't increment.
 */
void
SpecTcl1dIncTest::inclowboundary()
{
  CParameter*                pParam = new CParameter("test");

  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl1dIncrementer*     pInc = new CSpecTcl1dIncrementer("test");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
 
  CSpectrum spec("test", &alloc, pInc, cond, axes);
  *pParam = -0.5;

  spec();			// increment.

  void* pData = spec.getHandle();
  uint32_t* pLongs = reinterpret_cast<uint32_t*>(pData);
    
  for (int i=0; i < 512; i++) {
    EQ(static_cast<uint32_t>(0), *pLongs++);
  }   
}
/**
 * incunbound
 *
 * Can't increment an unbound spectrum either:
 */
void
SpecTcl1dIncTest::incunbound()
{
  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl1dIncrementer*     pInc = new CSpecTcl1dIncrementer("test");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
 
  CSpectrum spec("test", &alloc, pInc, cond, axes);

  spec();			// increment.

  void* pData = spec.getHandle();
  uint32_t* pLongs = reinterpret_cast<uint32_t*>(pData);
    
  for (int i=0; i < 512; i++) {
    EQ(static_cast<uint32_t>(0), *pLongs++);
  }   
}
/**
 * setok 
 *   Test the set function with coordinates that are ok.
 */
void 
SpecTcl1dIncTest::setok()
{
  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl1dIncrementer*     pInc = new CSpecTcl1dIncrementer("test");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
 
  CSpectrum spec("test", &alloc, pInc, cond, axes);

  spec.set(128, 0, 1234);

  void* pData = spec.getHandle();
  uint32_t* pLongs = reinterpret_cast<uint32_t*>(pData);
 
  EQ(static_cast<uint32_t>(1234), pLongs[128]);
}
/**
 * setbadch
 *
 *  Setting a bad channel number results in an
 *  out_of_range error:
 */
void
SpecTcl1dIncTest::setbadch()
{
  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl1dIncrementer*     pInc = new CSpecTcl1dIncrementer("test");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
 
  CSpectrum spec("test", &alloc, pInc, cond, axes);

  bool thrown(false);
  bool rthrow(false);
  std::string msg;

  try {
    spec.set(768,0, 1234);
  }
  catch (std::out_of_range& e) {
    thrown = true;
    rthrow = true;
    msg    = e.what();
  }
  catch (...) {
    thrown = true;
  }
  ASSERT(thrown);
  ASSERT(rthrow);
  EQ(std::string("Invalid channel number 768 must be in the range [0..511]"),
     msg);

    
}
/**
 * getok
 *
 *  Check that we can get a specified channel value.
 *  Given that the channel number is in range.
 */
void
SpecTcl1dIncTest::getok()
{
  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl1dIncrementer*     pInc = new CSpecTcl1dIncrementer("test");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);
 
  CSpectrum spec("test", &alloc, pInc, cond, axes);

  spec.set(128, 0, 1234);

  EQ(static_cast<uint32_t>(1234), spec.get(128,0));
  EQ(static_cast<uint32_t>(0), spec.get(100,0));

}
/**
 * setbadch
 *
 *   Get with invalid channel throws an exception.
 */
void
SpecTcl1dIncTest::getbadch()
{
  CTrueCondition*            cond  = new CTrueCondition;
  CSpecTclSpectrumAllocator  alloc;
  CSpecTcl1dIncrementer*     pInc = new CSpecTcl1dIncrementer("test");
  CAxis*                     pX   = new CAxis("x", 512, 0, 511);
  std::vector<CAxis*>        axes;
  axes.push_back(pX);

  CSpectrum spec("test", &alloc, pInc, cond, axes);


  bool thrown(false);
  bool rthrow(false);
  std::string msg;

  try {
    spec.get(1024, 0);
  }
  catch (std::out_of_range& e) { 
    thrown = true;
    rthrow = true;
    msg = e.what();
  }
  catch (...) {
    thrown = true;
  }
  ASSERT(thrown);
  ASSERT(rthrow);
  EQ(std::string("Invalid channel number 1024 must be in the range [0..511]"),
     msg);
}
