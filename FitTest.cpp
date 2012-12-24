// Template for a test suite.

#include <config.h>
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include <iostream>

#include <DesignByContract.h>
#include <./CFit.h>
#include <./CLinearFit.h>
#include <math.h>

class FitTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(FitTest);
  CPPUNIT_TEST(Construction);
  CPPUNIT_TEST(AddPoints);
  CPPUNIT_TEST(Fit);
  CPPUNIT_TEST_SUITE_END();


private:
  CCalibLinearFit* m_pFit;
public:
  void setUp() {
    m_pFit = new CCalibLinearFit;
    
  }
  void tearDown() {
    delete m_pFit;
  }
protected:
  void Construction();
  void AddPoints();
  void Fit();
};

CPPUNIT_TEST_SUITE_REGISTRATION(FitTest);


/*
Construction:
Ensure initial state is correct
1.      Get state
2.      Get number of points in fit data.
3.	Evaluate the fit
4.	Perform the Fit
5.	Get the fit parameters.
*/
void FitTest::Construction() 
{
  cerr << "FitTest\n";
  EQMSG("Fit state: ", CCalibFit::Accepting, m_pFit->GetState());
  EQMSG("# points:  ", (size_t)0, m_pFit->size());
 
  /*
  EXCEPTION(m_pFit->Perform(), 
	    DesignByContract::DesignByContractException&);
  */
  bool ok = false;
  try {
    m_pFit->Perform();
  }
  catch (DesignByContract::DesignByContractException&) {
    ok = true;
  }
  catch(...) {
    cerr << "Caught something!!\n";
    ok = true;
  }
  ASSERT(ok);


  CCalibFit::FitParameterList params;
  EXCEPTION(params = m_pFit->GetParameters(), 
	    DesignByContract::DesignByContractException&);
}


static CCalibFit::Point dummypoints[] = {
  {1, 2},
  {2, 3},
  {3, 4},
  {10, 42}
};
static const int numdummypoints = sizeof(dummypoints)/sizeof(CCalibFit::Point);

/*
Test Name	AddPoints
Test Objective	Ensure points can be added correctly.
Test Description	
1.	Add points to the fit.  
2.	Check the final number of points.
3.	Iterate through the points.

Expected Results	
1.	After each point the size() member should give the number of points 
        added.
2.	After all points are added, the total number of points should match 
        the number added.
3.	Iterating through the points should retrieve the same set of points 
        inserted, in the order inserted.
*/
void FitTest::AddPoints()
{
  for(int i =0; i < numdummypoints; i++) {
    m_pFit->AddPoint(dummypoints[i]);
    EQMSG("size in loop", (size_t)(i+1), m_pFit->size());
  }
  EQMSG("size after loop", 
	(size_t)numdummypoints, m_pFit->size());

  //

  int i = 0;
  CCalibFit::PointIterator p = m_pFit->begin();
  while(p != m_pFit->end()) {
    EQMSG("Xcompare: ", dummypoints[i].x, p->x);
    EQMSG("Ycompare: ", dummypoints[i].y, p->y);
    p++;
    i++;
  }
}

/*
Test Name	FitTest
Test Objective	
     Ensure that linear fits work, that parameters can be retrieved and 
     points evaluated.
Test Description	

1.	Insert 10 points that randomly vary along the line y=1.5x+7.3
2.	Perform the fit
3.	Get the fit state.
4.	Get the parameters of the fit
5.	Evaluate the 10 data points.

Expected Results	
3       The fit state should be Performed.
4.	The parameters should be within 10% of the actual (that's probably 
        generous).
5.	The 10 points should evaluate as described by the fit parameters.


 */

static const double testSlope     = 1.5;
static const double testIntercept = 7.3;
static const int    dataPoints    = 10;
static const double tolerance     = 0.1; // fraction of value.
static const double jitter        = 0.01; // jitter on data points.

void CheckLinearFit(CCalibFit* pFit)
{
  CCalibFit::Point pt;
  for(int i = 0; i < dataPoints; i++) {
    double x = (double)i;
    double y = x*testSlope + testIntercept; // exact value.
    y += (drand48() - 0.5)*jitter*y;        // random +/- jitter.

    pt.x = x;
    pt.y = y;
    pFit->AddPoint(pt);
  }
  // Peform the fit>

  pFit->Perform();

  EQMSG("state",CCalibFit::Performed,  pFit->GetState());

  CCalibFit::FitParameterList params = pFit->GetParameters();

  // there are three parameters named: slope, offset, chisquare.
  // We are only interested in slope and offset.

  EQ((size_t)3, params.size());
  
  CCalibFit::FitParameterIterator p  = params.begin();
  double slope;
  double intercept;
  int found = 0;
  while(p != params.end()) {
    if(p->first == string("slope")) {
      slope = p->second;
      found++;
    }
    if(p->first == string("offset")) {
      intercept = p->second;
      found++;
    }
    p++;
  }
  EQMSG("Not all params found", 2, found);

  // Check the fit quality.

  ASSERT(fabs(testSlope-slope) < tolerance*testSlope);
  ASSERT(fabs(testIntercept - intercept) < tolerance*testIntercept);

  // evaluate the fit:

  for(int i = 15; i < 25; i++) { // extrapolate past data...
    double x = (double)i;
    double y = (*pFit)(x);

    EQMSG("Point check: ", x*slope+intercept, y);
  }
}

void FitTest::Fit()
{
  CheckLinearFit(m_pFit);

}
