#ifndef CPARAMETERMAPPINGTEST_H
#define CPARAMETERMAPPINGTEST_H
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/HelperMacros.h>


#include "CParameterMapping.h"

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
