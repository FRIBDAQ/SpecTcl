// Template for a test suite.
#include <config.h>
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include <./CLinearFitCreator.h>
#include <./CFitFactory.h>

#include <iostream>

extern void CheckLinearFit(CCalibFit* pFit);


class FactoryTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(FactoryTest);
  CPPUNIT_TEST(CreatorList);
  CPPUNIT_TEST(FitCreation);
  CPPUNIT_TEST(FitManipulation);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void CreatorList();
  void FitCreation();
  void FitManipulation();
};

CPPUNIT_TEST_SUITE_REGISTRATION(FactoryTest);

/*
Test Name
	 CreatorList
Test Objective	
     Ensure that functions on the creator list operate correctly.
Test Description	
1.	Check number of fit creators. (sizecreators).
2.	Add a CCalibLinearFitCreator to the factory.
3.	Check # fit creators4.	iterate through the fit creators.
5.	Find the CCalibLinearFitCreator.
Test Conditions	
Expected Results	
1.	0
2.	N/A
3.	1
4.	Should see the linear fit creator we added.
5.	Should be able to find it.

 */
void FactoryTest::CreatorList() 
{
  cerr << "Factory test\n";

  CCalibLinearFitCreator* pCreator = new CCalibLinearFitCreator();

  EQ(0, CCalibFitFactory::sizeCreators());
  CCalibFitFactory::AddFitType("linear", pCreator);
  EQ(1, CCalibFitFactory::sizeCreators());

  CCalibFitFactory::FitCreatorIterator c = CCalibFitFactory::FindFitCreator("linear");

  ASSERT(c != CCalibFitFactory::endCreators());

  EQMSG("pointer compare", (CCalibFitCreator*)pCreator, c->second);
  
}
/*
Test Name
         FitCreation
Test Objective
	 Ensure a named fit can be created.
Test Description	
1.	Check number of fits.
2.	Create a linear fit.
3.	check # of fits.
4.	iterate fits
5.	Find the fit.
6.	Delete the fit
7.	number of fits?

Expected Results	
1.	0
2.	N/A
3.	1
4.	Should have exactly the fit I created.
5.	Should find the fit I created.
6.	N/A
7.	0.
 */
void FactoryTest::FitCreation()
{
  CCalibFit* pFit = CCalibFitFactory::Create("linear", "testfit");
  ASSERT(pFit);

  EQMSG("1 fit", 1, CCalibFitFactory::size());

  CCalibFitFactory::FitIterator f = CCalibFitFactory::begin();
  ASSERT(f != CCalibFitFactory::end());
  EQMSG("fit in factory map", pFit, f->second);
  f++;
  ASSERT( f ==  CCalibFitFactory::end());

  f = CCalibFitFactory::FindFit("testfit");
  ASSERT(f != CCalibFitFactory::end());
  EQMSG("Fit found is mine", pFit, f->second);
  ASSERT(CCalibFitFactory::Delete("testfit"));

  EQMSG("Fit factory empty", 0, CCalibFitFactory::size());

	 
}


/*
Test Name/#	 FitManipulation
Test Objective	Ensure a created fit can be manipulated.
Test Description	
1.	Create a linear fit.
2.	Add points to it
3.	Perform the fit
4.	Get the fit parameters
5.	Evaluate the fit at some points
Test Conditions	
Expected Results	
1.	N/A2.	true
3.	true
4.	Fit parameters are good to 10% of the line I'm putting in.
5.	Fit evaulates as it should according to its parameters.


Note the tests are identical to those done by FitTest::Fit so the actual
test is factored out into CheckLinearFit.

*/
void FactoryTest::FitManipulation()
{
  CCalibFit* pFit = CCalibFitFactory::Create("linear", "testfit");
  CheckLinearFit(pFit);
  CCalibFitFactory::Delete("testfit");
  
}
