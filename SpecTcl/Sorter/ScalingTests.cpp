#include <cppunit/TextTestResult.h>
#include <cppunit/TestSuite.h>

#include <iostream.h>
#include "CParameterMappingTest.h"
#include "CAxisTests.h"

int
main(int argc, char** argv)
{
  CppUnit::TestSuite suite;

  CppUnit::TextTestResult res;

  suite.addTest(CParameterMappingTest::suite());
  suite.addTest(CAxisTests::suite());

  suite.run(&res);

  std::cout << res << std::endl;

  return 0;
}
