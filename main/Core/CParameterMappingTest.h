#ifndef __CPARAMETERMAPPINGTEST_H
#define __CPARAMETERMAPPINGTEST_H
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>


#ifndef __CPARAMETERMAPPING_H
#include "CParameterMapping.h"
#endif

class CParameterMappingTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(CParameterMappingTest);
  CPPUNIT_TEST(testConstructors);
  CPPUNIT_TEST(testIdentityTransform);
  CPPUNIT_TEST(testUnitInterval);
  CPPUNIT_TEST(testSlideTransform);
  CPPUNIT_TEST_SUITE_END();
private:
  CParameterMapping m_UnMapped;
  CParameterMapping m_UnitInterval;
  CParameterMapping m_Displaced;
  
public:
  CParameterMappingTest();
  void testConstructors();
  void testIdentityTransform();
  void testUnitInterval();
  void testSlideTransform();
};

CPPUNIT_TEST_SUITE_REGISTRATION(CParameterMappingTest);

#endif
