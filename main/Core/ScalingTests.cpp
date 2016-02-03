#include <config.h>
#include <cppunit/TextTestResult.h>
#include <cppunit/TestSuite.h>

#include <iostream>
#include "CParameterMappingTest.h"
#include "CAxisTests.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif
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
