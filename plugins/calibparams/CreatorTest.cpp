// Template for a test suite.
#include <config.h>
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <CLinearFitCreator.h>
#include <CFit.h>

#include <iostream>

class CreatorTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(CreatorTest);
  CPPUNIT_TEST(Create);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void Create();
};

CPPUNIT_TEST_SUITE_REGISTRATION(CreatorTest);

/* 
Test Name	 Creation
Test Objective	Ensure I can create a linear fit.
Test Description	
1.	Create a CLinearFitCreator
2.	Ask it to create a fit.
3.	Check the state of the fit.

Expected Results	
3. Fit state should be accepting.

 */
void CreatorTest::Create() 
{
  cerr << "CreatorTest\n";
  CLinearFitCreator creator;

  CFit* pFit = creator();

  EQ(CFit::Accepting, pFit->GetState());
}
