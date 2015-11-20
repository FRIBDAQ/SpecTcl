// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#define private public
#include "CSpecTclSum1dIncrementer.h"
#undef private

#include "CParameter.h"
#include "CSpectrum.h"
#include "CTrueCondition.h"
#include "CAxis.h"
#include "CSpecTclSpectrumAllocator.h"

#include <string>
#include <stdint.h>
#include <vector>
#include <stdexcept>


class SpecTclSum1Test : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SpecTclSum1Test);
  CPPUNIT_TEST(construct);
  CPPUNIT_TEST(constructvalid);
  CPPUNIT_TEST(constructsomevalid);

  CPPUNIT_TEST(allvalid);
  CPPUNIT_TEST(somevalid);

  CPPUNIT_TEST(incall);
  CPPUNIT_TEST(incsome);
  CPPUNIT_TEST(incnotbound);
  CPPUNIT_TEST(rangecheck);

  CPPUNIT_TEST(getok);
  CPPUNIT_TEST(getchover);

  /*
   * Note:  After the grand refactoring of stuff into
   *        CSpecTcl1dBaseIncrementer, no more tests are needed
   *        as the code for set and clear got  delegated to
   *        that base class and was tested by the 1d tests.
   */

  
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
  void constructvalid();
  void constructsomevalid();

  void allvalid();
  void somevalid();

  void incall();
  void incsome();
  void incnotbound();
  void rangecheck();

  void getok();
  void getchover();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SpecTclSum1Test);

/**
 * construct
 *   Construction without any valid parameters should:
 *   - Result in a vector of names that is the same as the input vector.
 *   - Result in an emtpy m_parameters vector.
 *   - Result in false for completely bound.
 */
void SpecTclSum1Test::construct() {
  std::vector<std::string> params;
  params.push_back("a");
  params.push_back("b");
  params.push_back("c");	// none of these parameters actually exists.

  CSpecTclSum1dIncrementer inc(params);

  EQ(static_cast<size_t>(3), inc.m_parameterNames.size());
  for (int i = 0; i < 3; i++) {
    EQ(params[i], inc.m_parameterNames[i]);
  }
  EQ(static_cast<size_t>(0), inc.m_parameters.size());
  ASSERT(! inc.m_completelyBound);
  
}
/**
 * constructvalid
 *
 *   If the names passed in to the incrementer's constructor
 *   are all valid parameters, the resulting object is 
 *   completely bound.
 */
void
SpecTclSum1Test::constructvalid()
{
  std::vector<std::string> params;
  params.push_back("a");
  params.push_back("b");
  params.push_back("c");	// none of these parameters actually exists.
  CParameter a("a");
  CParameter b("b");
  CParameter c("c");

  CSpecTclSum1dIncrementer inc(params);

  CParameterDictionary* pDict = CParameterDictionary::instance();

  EQ(static_cast<size_t>(3), inc.m_parameters.size());
  for (int i = 0; i < 3; i++) {
    EQ(pDict->find(params[i])->second, inc.m_parameters[i]);
  }
  ASSERT(inc.m_completelyBound);

  

}
/**
 * constructsomevalid
 *   If some but not all names are valid, this is an incomplete binding.
 */
void
SpecTclSum1Test::constructsomevalid()
{
  std::vector<std::string> params;
  params.push_back("a");
  params.push_back("b");
  params.push_back("c");	// none of these parameters actually exists.
  CParameter a("a");
  CParameter c("c");

  CSpecTclSum1dIncrementer inc(params);


  ASSERT(!inc.m_completelyBound);

  

}
/**
 * allvalid
 *   Test validation when all are valid:
 */
void
SpecTclSum1Test::allvalid()
{
  std::vector<std::string> params;
  params.push_back("a");
  params.push_back("b");
  params.push_back("c");	// none of these parameters actually exists.

  std::vector<CAxis*> axes;
  axes.push_back(new CAxis("x", 512, 0, 511));

  CTrueCondition* pCond = new CTrueCondition;
  CSpecTclSpectrumAllocator alloc;

  CSpecTclSum1dIncrementer* inc = new CSpecTclSum1dIncrementer(params); // Invalid
  
  CSpectrum spec("test", &alloc, inc, pCond, axes);

  CParameter a("a");
  CParameter b("b");
  CParameter c("c");
  
  spec.checkValidity();
  ASSERT(inc->m_completelyBound);

}
/**
 * somevalid
 *   If only some are valid, should not be completely bound.
 */
void
SpecTclSum1Test::somevalid()
{
  std::vector<std::string> params;
  params.push_back("a");
  params.push_back("b");
  params.push_back("c");	// none of these parameters actually exists.

  std::vector<CAxis*> axes;
  axes.push_back(new CAxis("x", 512, 0, 511));

  CTrueCondition* pCond = new CTrueCondition;
  CSpecTclSpectrumAllocator alloc;

  CSpecTclSum1dIncrementer* inc = new CSpecTclSum1dIncrementer(params); // Invalid
  
  CSpectrum spec("test", &alloc, inc, pCond, axes);

  CParameter a("a");
  CParameter c("c");
  
  spec.checkValidity();
  ASSERT(!inc->m_completelyBound);
}
/**
 * incall
 *  Increment when all parameters are defined and have values in this
 *  event.
 */
void
SpecTclSum1Test::incall()
{

  // Generate the spectrum:


  std::vector<std::string> params;
  params.push_back("a");
  params.push_back("b");
  params.push_back("c");	// none of these parameters actually exists.

  std::vector<CAxis*> axes;
  axes.push_back(new CAxis("x", 512, 0, 512));

  CTrueCondition* pCond = new CTrueCondition;
  CSpecTclSpectrumAllocator alloc;

  CSpecTclSum1dIncrementer* inc = new CSpecTclSum1dIncrementer(params); // Invalid
  
  CSpectrum spec("test", &alloc, inc, pCond, axes);

  CParameter a("a");
  CParameter b("b");
  CParameter c("c");
  
  spec.checkValidity();

  // Generate the event:

  a = 128;
  b = 256;
  c = 300;
  spec();			// Process the event.

  uint32_t* pChannels = reinterpret_cast<uint32_t*>(spec.getHandle());

  EQ(static_cast<uint32_t>(1), pChannels[128]);
  EQ(static_cast<uint32_t>(1), pChannels[256]);
  EQ(static_cast<uint32_t>(1), pChannels[300]);

}
/**
 * incsome
 *   Not all spectrum parameters need to be present to make
 *   increments happen:
 */
void
SpecTclSum1Test::incsome()
{
 // Generate the spectrum:


  std::vector<std::string> params;
  params.push_back("a");
  params.push_back("b");
  params.push_back("c");	// none of these parameters actually exists.

  std::vector<CAxis*> axes;
  axes.push_back(new CAxis("x", 512, 0, 512));

  CTrueCondition* pCond = new CTrueCondition;
  CSpecTclSpectrumAllocator alloc;

  CSpecTclSum1dIncrementer* inc = new CSpecTclSum1dIncrementer(params); // Invalid
  
  CSpectrum spec("test", &alloc, inc, pCond, axes);

  CParameter a("a");
  CParameter b("b");
  CParameter c("c");
  
  spec.checkValidity();
  
  // Generate the event:

  b = 256;
  c = 300;
  spec();			// Process the event.
  uint32_t* pChannels = reinterpret_cast<uint32_t*>(spec.getHandle());

  EQ(static_cast<uint32_t>(1), pChannels[256]);
  EQ(static_cast<uint32_t>(1), pChannels[300]);



}

/**
 * incnotbound
 *
 *   If the spectrum is not completely bound, no increments occur
 */

void
SpecTclSum1Test::incnotbound()
{
 std::vector<std::string> params;
  params.push_back("a");
  params.push_back("b");
  params.push_back("c");	// none of these parameters actually exists.

  std::vector<CAxis*> axes;
  axes.push_back(new CAxis("x", 512, 0, 512));

  CTrueCondition* pCond = new CTrueCondition;
  CSpecTclSpectrumAllocator alloc;

  CSpecTclSum1dIncrementer* inc = new CSpecTclSum1dIncrementer(params); // Invalid
  
  CSpectrum spec("test", &alloc, inc, pCond, axes);

  CParameter a("a");
  CParameter b("b");
  spec.checkValidity();		// Not completely bound until C is defined.
  CParameter c("c");
  
  a = 128;
  b = 256;
  c = 300;
  spec();			// Process the event.

  uint32_t* pChannels = reinterpret_cast<uint32_t*>(spec.getHandle());

  EQ(static_cast<uint32_t>(0), pChannels[128]);
  EQ(static_cast<uint32_t>(0), pChannels[256]);
  EQ(static_cast<uint32_t>(0), pChannels[300]);

}
/**
 * rangecheck
 *
 *  Ensure the software range checks.
 */
void
SpecTclSum1Test::rangecheck()
{
  // Generate the spectrum:


  std::vector<std::string> params;
  params.push_back("a");
  params.push_back("b");
  params.push_back("c");	// none of these parameters actually exists.

  std::vector<CAxis*> axes;
  axes.push_back(new CAxis("x", 512, 0, 512));

  CTrueCondition* pCond = new CTrueCondition;
  CSpecTclSpectrumAllocator alloc;

  CSpecTclSum1dIncrementer* inc = new CSpecTclSum1dIncrementer(params); // Invalid
  
  CSpectrum spec("test", &alloc, inc, pCond, axes);

  CParameter a("a");
  CParameter b("b");
  CParameter c("c");
  
  spec.checkValidity();

  // Generate the event:

  a = -100;
  b = 1024;
  c = 100;

  spec();

  EQ(1U, inc->m_overflows);
  EQ(1U, inc->m_underflows);

}
/**
 * getok
 *   Get of a channel value should be ok.
 */
void
SpecTclSum1Test::getok()
{
  // Generate the spectrum:


  std::vector<std::string> params;
  params.push_back("a");
  params.push_back("b");
  params.push_back("c");	// none of these parameters actually exists.

  std::vector<CAxis*> axes;
  axes.push_back(new CAxis("x", 512, 0, 512));

  CTrueCondition* pCond = new CTrueCondition;
  CSpecTclSpectrumAllocator alloc;

  CSpecTclSum1dIncrementer* inc = new CSpecTclSum1dIncrementer(params); // Invalid
  
  CSpectrum spec("test", &alloc, inc, pCond, axes);

  // Put a counting patterrn in the channels:

  uint32_t* pCh = reinterpret_cast<uint32_t*>(spec.getHandle());
  for(int i = 0; i < 512; i++) {
    *pCh++ = i;
  }

  // Should now be able to get them all:

  for (unsigned i = 0; i < 512; i++) {
    EQ(i, spec.get(i, 0));
  }
}
/**
 * getchover
 *
 *   Getting a channel past thenend of the spectrum throws a std::range_error
 */
void
SpecTclSum1Test::getchover()
{
  // Generate the spectrum:


  std::vector<std::string> params;
  params.push_back("a");
  params.push_back("b");
  params.push_back("c");	// none of these parameters actually exists.

  std::vector<CAxis*> axes;
  axes.push_back(new CAxis("x", 512, 0, 512));

  CTrueCondition* pCond = new CTrueCondition;
  CSpecTclSpectrumAllocator alloc;

  CSpecTclSum1dIncrementer* inc = new CSpecTclSum1dIncrementer(params); // Invalid
  
  CSpectrum spec("test", &alloc, inc, pCond, axes);

  bool thrown = false;
  bool rthrow = false;
  std::string msg;

  try {
    spec.get(1024, 0);
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
  EQ(std::string("Invalid channel number 1024 must be in the range [0..511]"),
     msg);
}
