// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <CCalibratedParameter.h>
#include <CCalibratedParameterManager.h>
#include <CLinearFit.h>
#include <Event.h>
#include <Analyzer.h>
#include <BufferDecoder.h>

#include <stdio.h>

class CalibManager : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(CalibManager);
  CPPUNIT_TEST(AddTest);
  CPPUNIT_TEST(DeleteTest);
  CPPUNIT_TEST(STLJacketTest);
  CPPUNIT_TEST(EvalTest);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
    // empty out the manager:

    while (CCalibratedParameterManager::size()) {
      CCalibratedParameterManager::CalibratedParameterIterator i = 
	CCalibratedParameterManager::begin();
      string name = i->first;
      CCalibratedParameter* pParam = 
	CCalibratedParameterManager::DeleteParameter(name);
      delete pParam;
    }
  }
protected:
  void AddTest();
  void DeleteTest();
  void STLJacketTest();
  void EvalTest();
};

CPPUNIT_TEST_SUITE_REGISTRATION(CalibManager);
/*
Test Name	AddTest
Test Objective	Test ability to add calibrated parameters to the manager.
Test Description	
   1.	Create a pair of calibrated parameters
   2.	Check size of manager.
   3.	Add a parameter
   4.	Check size of manager
   5.	Find added parameter
   6.	Add second parameter
   7.	find first parameter
   8.	find second parameter
   9.	check size of manager.
Expected Results	
   1.	NA
   2.	SB 0
   3.	NA
   4.	SB 1
   5.	Should not be end and iterator.second should point to first 
        parameter
   6.	NA
   7.	Should not be end and iterator.second should point to first 
        parameter.
   8.	Should not be end and iterator.second should point to 
        second parameter.
   9.	Should be 2.

 */
void CalibManager::AddTest() 
{
  // 1.  Make the pair of parameters 

  CLinearFit line1;
  CLinearFit::Point pt;
  pt.x = 0.0; pt.y = 0.1;
  line1.AddPoint(pt);
  pt.x = 1.0; pt.y = 2.2;
  line1.AddPoint(pt);
  line1.Perform();

  CCalibratedParameter param1(1, 0, "line1", &line1);

  CLinearFit line2;
  pt.x = 0.0; pt.y = -1.0;
  line2.AddPoint(pt);
  pt.x = 5.0; pt.y = 15.0;
  line2.AddPoint(pt);
  line2.Perform();

  CCalibratedParameter param2(2,4, "line2", &line2);

  // 2. Check size of manager:

  EQMSG("Initial size", (size_t)0, CCalibratedParameterManager::size());

  // 3. Add the first parametrs:

  CCalibratedParameterManager::AddParameter("The First", 
					    new CCalibratedParameter(param1));

  // 4. Check size:

  EQMSG("After first add", (size_t)1, CCalibratedParameterManager::size());

  // 5. Check find:

  CCalibratedParameterManager::CalibratedParameterIterator i = 
    CCalibratedParameterManager::FindParameter("The First");
  ASSERT(i != CCalibratedParameterManager::end());
  ASSERT(param1 == *(i->second));

  // 6. Add second parameter:

  CCalibratedParameterManager::AddParameter("second one", 
					    new CCalibratedParameter(param2));

  // 7. Re-find 1.

  i = CCalibratedParameterManager::FindParameter("The First");
  ASSERT(i != CCalibratedParameterManager::end());
  ASSERT(param1 == *(i->second));

  // 8. Find 2:

  i = CCalibratedParameterManager::FindParameter("second one");
  ASSERT(i != CCalibratedParameterManager::end());
  ASSERT(param2 == *(i->second));

  // Size sb. 2:

  EQMSG("Size = 2",(size_t)2, CCalibratedParameterManager::size());


}

/*!
Test Name	DeleteTest
Test Objective	Test ability to delete a parameter.
Test Description	
   1.	Add 2 parameters as for prior test.
   2.	Check size.
   3.	Delete first parameter.
   4.	Check size
   5.	Find First
   6.	Find second.
Expected Results	
   1.	NA
   2.	2
   3.	NA
   4.	1
   5.	end()
   6.	not end, and *(i->second) == second parameter.

 */
void CalibManager::DeleteTest()
{
  // 1.  Make the pair of parameters and add them.

  CLinearFit line1;
  CLinearFit::Point pt;
  pt.x = 0.0; pt.y = 0.1;
  line1.AddPoint(pt);
  pt.x = 1.0; pt.y = 2.2;
  line1.AddPoint(pt);
  line1.Perform();

  CCalibratedParameter param1(1, 0, "line1", &line1);

  CLinearFit line2;
  pt.x = 0.0; pt.y = -1.0;
  line2.AddPoint(pt);
  pt.x = 5.0; pt.y = 15.0;
  line2.AddPoint(pt);
  line2.Perform();

  CCalibratedParameter param2(2,4, "line2", &line2);
  CCalibratedParameterManager::AddParameter("1", new CCalibratedParameter(param1));
  CCalibratedParameterManager::AddParameter("2", new CCalibratedParameter(param2));


  //2. Check size:

  EQMSG("Sizecheck2: ", (size_t)2, CCalibratedParameterManager::size());

  // 3. delete first.

  CCalibratedParameter* p = CCalibratedParameterManager::DeleteParameter("1");
  delete p;

  // 4. Size check:

  EQMSG("Sizecheck1: ", (size_t)1, CCalibratedParameterManager::size());

  // 5. Find deleted:

  CCalibratedParameterManager::CalibratedParameterIterator i;
  i = CCalibratedParameterManager::FindParameter("1");
  ASSERT(i == CCalibratedParameterManager::end());

  // 6. Locate second one:

  i = CCalibratedParameterManager::FindParameter("2");
  ASSERT(i != CCalibratedParameterManager::end());
  ASSERT(param2 == *(i->second));


}
/*!
Test Name	STLJacketTest - 
Test Objective	Test STL Jackets esp. for begin() end() itertion.
Test Description	
   1.	Create a pair of parameters, and add them.
   2.	Iterate through the manager.
Expected Results	
   Both parameters should appear in the iteration exactly once.and the number of 
iterations should be 2.

 */
void CalibManager::STLJacketTest()
{
  // 1.  Make the pair of parameters and add them.

  CLinearFit line1;
  CLinearFit::Point pt;
  pt.x = 0.0; pt.y = 0.1;
  line1.AddPoint(pt);
  pt.x = 1.0; pt.y = 2.2;
  line1.AddPoint(pt);
  line1.Perform();

  CCalibratedParameter param1(1, 0, "line1", &line1);

  CLinearFit line2;
  pt.x = 0.0; pt.y = -1.0;
  line2.AddPoint(pt);
  pt.x = 5.0; pt.y = 15.0;
  line2.AddPoint(pt);
  line2.Perform();

  CCalibratedParameter param2(2,4, "line2", &line2);
  CCalibratedParameterManager::AddParameter("1", new CCalibratedParameter(param1));
  CCalibratedParameterManager::AddParameter("2", new CCalibratedParameter(param2));

  // The structure below is used to keep track of iteration:

  struct {
    CCalibratedParameter* pParam;
    int                   nVisits;
  } parameters[2] = {
    {&param1, 0},
    {&param2, 0}
  };

  // Iterate and scoreboard.

  int iterations(0);
  CCalibratedParameterManager::CalibratedParameterIterator i =
    CCalibratedParameterManager::begin();

  while(i != CCalibratedParameterManager::end()) {
    CCalibratedParameter* p = i->second;
    for(int j= 0; j < 2; j++) {
      if(*(parameters[j].pParam) == *p) {
	parameters[j].nVisits++;
      }
    }
    i++;
    iterations++;
  }

  // Check it out:

  EQMSG("Iteration count", 2, iterations);
  for(int j = 0; j < 2; j++) {
    char msg[100];
    sprintf(msg, "Checking param %d", j);
    EQMSG(msg, 1,parameters[j].nVisits);
  }
}
/*
Test Name	EvalTest
Test Objective	Test ability to evaluate parameters.
Test Description	
   1.	Create and insert 2 calibrated parameters with known fits.
   2.	Evaluate for several events.Note we will be dirty about the 
        analyzer and the buffer decoder references since those are 
        not used by the software.
Expected Results	
       Resulting calibrated parameters should match .
  
 */
void CalibManager::EvalTest()
{
  CCalibratedParameterManager manager; // need an object now.
  // 1.  Make the pair of parameters and add them.

  CLinearFit line1;
  CLinearFit::Point pt;
  pt.x = 0.0; pt.y = 0.0;
  line1.AddPoint(pt);
  pt.x = 1.0; pt.y = 2.0;
  line1.AddPoint(pt);
  line1.Perform();		// y = 2x

  CCalibratedParameter param1(1, 0, "line1", &line1);

  CLinearFit line2;
  pt.x = 0.0; pt.y = 5.0;
  line2.AddPoint(pt);
  pt.x = 1.0; pt.y = 7;		// y = 2x+5
  line2.AddPoint(pt);
  line2.Perform();

  CCalibratedParameter param2(2,0, "line2", &line2);
  CCalibratedParameterManager::AddParameter("1", new CCalibratedParameter(param1));
  CCalibratedParameterManager::AddParameter("2", new CCalibratedParameter(param2));


  // Here's where it gets a bit dirty:

  CBufferDecoder* pDecoder(0);
  CAnalyzer*      pAnalyzer(0);	// Fortunately these get passed around by reference.

  CEvent          anEvent;

  for(int i= 0; i < 100; i++) {
    anEvent.clear();		// Clear the event's validity flags.
    anEvent[0] = (float)i;
    manager((Address_t)NULL,	// Don't use
	    anEvent,		// Do use
	    *pAnalyzer,		// Don't use.
	    *pDecoder);		// Don't use.
    // rEvent[1,2] should be valid:

    ASSERT(anEvent[1].isValid());
    ASSERT(anEvent[2].isValid());

    // and the elements should have correct values:

    float y      = (float)i*2.0;
    float actual = anEvent[1]; 
    EQMSG("Element 1: ", y, actual);

    y      = (float)i*2.0 + 5.0;
    actual = anEvent[2]; 
    EQMSG("Element2: ", y, actual);
  }
}

