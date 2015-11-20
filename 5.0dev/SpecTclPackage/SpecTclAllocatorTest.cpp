// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#define private public		// To allow white box testing.
#include "CSpecTclSpectrumAllocator.h"
#undef private

#include "CSpectrum.h"
#include "CTrueCondition.h"
#include "CSpectrumIncrementer.h"
#include "CAxis.h"


class CNullIncrementer : public CSpectrumIncrementer
{
public:
  void validate(CSpectrum& spec) {}
  void operator()(CSpectrum& spec) {}
  uint32_t get(CSpectrum& spec, unsigned x, unsigned y) { return 0; }
  void     set(CSpectrum& spec, unsigned x, unsigned y, uint32_t value) {}
  void     clear(CSpectrum& spec) {}

};

class SpecTclAllocTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SpecTclAllocTest);
  CPPUNIT_TEST(oned);
  CPPUNIT_TEST(twod);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void oned();
  void twod();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SpecTclAllocTest);

/**
 * oned 
 *   test allocation of 1-d spectrum.
 */

void SpecTclAllocTest::oned() {
  CCondition*         pCond  = new CTrueCondition;
  CSpecTclSpectrumAllocator* pA;
  CSpectrumAllocator* pAlloc =  pA = new CSpecTclSpectrumAllocator;
  CSpectrumIncrementer* pInc = new CNullIncrementer;
  std::vector<CAxis*>   axes;
  axes.push_back(new CAxis("x", 100, 0.0, 1.0));

  // 1d spectrum 100 cells * sizeof(uint32_t)

  CSpectrum spec("Test", pAlloc, pInc, pCond, axes);

  ASSERT(pA->m_allocated);	// Did the allocation.
  EQ(static_cast<size_t>(100), pA->m_xDimension);
  EQ(static_cast<size_t>(1),   pA->m_yDimension); // 1-d spectrum has one y cell.
  EQ(static_cast<size_t>(100*sizeof(uint32_t)), pA->m_bytesAllocated);
}
/**
 * test allocation of 2d spectrum.
 */
void
SpecTclAllocTest::twod()
{
  CCondition*         pCond  = new CTrueCondition;
  CSpecTclSpectrumAllocator* pA;
  CSpectrumAllocator* pAlloc =  pA = new CSpecTclSpectrumAllocator;
  CSpectrumIncrementer* pInc = new CNullIncrementer;
  std::vector<CAxis*>   axes;
  axes.push_back(new CAxis("x", 100, 0.0, 1.0));
  axes.push_back(new CAxis("y", 250, 0.0, 1.0));

  // 2-d spectrum 100 x 250 channels.

  CSpectrum spec("Test", pAlloc, pInc, pCond, axes);

  ASSERT(pA->m_allocated);	// Did the allocation.
  EQ(static_cast<size_t>(100), pA->m_xDimension);
  EQ(static_cast<size_t>(250),   pA->m_yDimension); // 1-d spectrum has one y cell.
  EQ(static_cast<size_t>(100*250*sizeof(uint32_t)), pA->m_bytesAllocated);
}
