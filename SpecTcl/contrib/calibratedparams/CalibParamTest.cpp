// Template for a test suite.
#include <config.h>
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <CCalibratedParameter.h>
#include <Event.h>
#include <CLinearFit.h>
#include <math.h>


// Test for the calibrated parameter class.


class CalibratedParameterTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(CalibratedParameterTest);
  CPPUNIT_TEST(ConstructTest);
  CPPUNIT_TEST(FunctionCall);
  CPPUNIT_TEST(FitChange);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void ConstructTest();
  void FunctionCall();
  void FitChange();
};

CPPUNIT_TEST_SUITE_REGISTRATION(CalibratedParameterTest);

/*
Test Name/#	Construction
Test Objective	Test the constructors and canonical operations of the class 
                (Note this will also test the selectors.
Test Description	
    1.	Construct a Calibrated parameter
    2.	Inspect with selectors.
    3.	Do copy construction of the calibrated parameter
    4.	Test for equality
    5.	Test fits for equality.
    6.	Create another calibrated parameter (different parameters etc).
    7.	Compare for equality
    8.	Compare for inequality.
Expected Results	
    1.	N/A
    2.	Selectors should reflect constructor parameters.
    3.	NA
    4.	The two Calibrated parameters should compare equal.
    5.	The two fits contained by the parameters should compare equal
    6.	NA
    7.	Should not get equality
    8.	Should get inequality.

 */
void CalibratedParameterTest::ConstructTest() {
  // 1. Construct the initial calibrated parameter.

  CLinearFit  line;		// The fit I'm using.
  CFit::Point   pt;
  pt.x = 0.0; pt.y = 0.0;
  line.AddPoint(pt);
  pt.x = 1.0; pt.y = 2.0;
  line.AddPoint(pt);      // y = 2*x.
  line.Perform();

  CCalibratedParameter p1(1, 0, "TestFit", &line);

  // 2. Check the results of the construction.

  
  EQMSG("Target id", 1, p1.getTargetId());
  EQMSG("Raw id", 0, p1.getRawId());
  EQMSG("Fitname", string("TestFit"), p1.getFitName());
  const CFit* pFit = p1.getFit();

  if(line != *(const CLinearFit*)pFit) {
    FAIL("Fits not equal");
  }

  // 3. Copy construct parameter.

  CCalibratedParameter p2(p1);

  // 4. Calibrated parameters must be equal

 ASSERT(p1 == p2);			// Parameters must compare equal

  // 5. And their fits must be equal:

  const CFit* pFit2 = p2.getFit();
  ASSERT(*pFit == *pFit2);

  // 6. Create a different calibrated parameter.

  CCalibratedParameter p3(5, 7, "Testing", &line);

  // 7. SHould not be equal:

  if(p1 == p3) {
    FAIL("Unequal are equal");
  }
  // 8. Should be unequal.

  ASSERT(p1 != p3);   		// This is not the same test as above.

}
/*
Test Name	function call
Test Objective	Test the function call operator of CCalibratedParameter
Test Description	
   1.	Construct a fit and perform it such that it will yield y = 2*x+1
   2.	Construct a calibrated parameter with this fit.
   3.	Run several concocted events through the parameter checking that they all compute to y= 2*x+1.
        within +/- 0.5*slope due to the randomized undiscetation.
Expected Results	
   1.	NA
   2.	NA
   3.	All calibrated parameters evaluate to 2*x+1.

 */
void
CalibratedParameterTest::FunctionCall()
{
  CLinearFit line;
  CFit::Point pt;

  pt.x = 0.0; pt.y = 1.0;
  line.AddPoint(pt);
  pt.x = 1.0; pt.y = 3.0;
  line.AddPoint(pt);

  line.Perform();

  CCalibratedParameter Param(1, 0, "test", &line);

  CEvent event;

  for(int i =0; i < 10; i++) {
    event.clear();		// Reset the event.

    event[0] = i;		// Raw parameter.
    Param(event);		// Compute the calibrated param.
    ASSERT(event[1].isValid());	// Should have been set.
    
    float result = (double)i*2.0 + 1.0;
    float calibrated = event[1];
    float delta = fabs(result - calibrated);
    ASSERT(delta <= 0.5*2.0);

    //    EQMSG("comparison", result, calibrated);

  }
}
/*
Test Name	FitChange
Test Objective	Test ability to change from one fit to the other.
Test Description	
   1.	Construct a calibrated parameter.
   2.	Replace its fit.
   3.	Check returned fit for equality with initial fit.
   4.	Evaluate parameter for several data points with new fit.
Expected Results	
   4.	Initial fit should be equal to replaced fit.
   5.	Parameter should evaluate using new fit.
        note that since the input parameter gets
        spread across +/- .5 there will not be an
        exact comparison.. The comparison should be
        within +/- 0.5*slope.

 */
void
CalibratedParameterTest::FitChange()
{
  // 1. create initial parameter.

  CLinearFit line;
  CFit::Point pt;

  pt.x = 0.0; pt.y = 1.0;
  line.AddPoint(pt);
  pt.x = 1.0; pt.y = 3.0;
  line.AddPoint(pt);

  line.Perform();

  CCalibratedParameter Param(1, 0, "test", &line);


  // Make a secod fit and repalce the parameter's fit.

  CLinearFit line2;		// This will be y = 3x-5.

  pt.x = 0.0; pt.y = -5.0;
  line2.AddPoint(pt);
  pt.x = 1.0; pt.y = -2.0;
  line2.AddPoint(pt);
  line2.Perform();

  CFit* pOldFit = Param.ReplaceFit(line2);

  // 4 - check against old fit.

  if(line != *(CLinearFit*)pOldFit) {
    FAIL("Replaced fit != line");
  }
  delete pOldFit;

  // 5 Evaluate.

  CEvent event;
  for(int i =0; i < 100; i ++) {
    event.clear();
    event[0] = i;
    Param(event);

    float result = ((float)i)*3.0 - 5.0;
    float calib  = event[1];
    float delta  = fabs(result - calib);
    ASSERT(delta <= 0.5*3.0);

  }
}
