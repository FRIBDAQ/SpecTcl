#ifndef __CAXISTESTS_H
#define __CAXISTESTS_H
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>

#ifndef __CAXIS_H
#include "CAxis.h"
#endif


#ifndef __CPARAMETERMAPPING_H
#include "CParameterMapping.h"
#endif

class CAxisTests : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(CAxisTests);
  CPPUNIT_TEST(testIdentityConstructor);
  CPPUNIT_TEST(testUnmappedUnitConstructor);
  CPPUNIT_TEST(testMappedUnitConstructor);
  CPPUNIT_TEST(testIdentityTransforms);
  CPPUNIT_TEST(testUnmappedUnitTransforms);
  CPPUNIT_TEST(testMappedUnitTransforms);
  CPPUNIT_TEST_SUITE_END();
private:
  CAxis m_Identity;		// Identity axis.
  CAxis m_UnmappedUnit;		// Unit interval on unmapped param.
  CAxis m_MappedUnit;           // Unit interval on mapped parameter.
public:
  CAxisTests();			// Constructor.
  void testIdentityConstructor();
  void testUnmappedUnitConstructor();
  void testMappedUnitConstructor();
  void testIdentityTransforms();
  void testUnmappedUnitTransforms();
  void testMappedUnitTransforms();
};

#endif
