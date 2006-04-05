// This implementation of TreeParameter is based on the ideas and original code of::
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//
// Template for a test suite.

#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CTreeParameter.h"
#include "CTreeParameterArray.h"
#include <string>
#include <stdio.h>

#include <Event.h>
#include "TreeTestSupport.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

class TreeParamArrayTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TreeParamArrayTests);
  CPPUNIT_TEST(Construction);
  CPPUNIT_TEST(Destruction);
  CPPUNIT_TEST(Reset);
  CPPUNIT_TEST(Iteration);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
    TreeTestSupport::ClearMap();
  }
protected:
  void Construction();
  void Destruction();
  void Reset();
  void Iteration();
};

/// Re-use CheckConstructed from TreeParameterTest:
 

static void
CheckArray(string basename, const char* comment, int low, unsigned int num,
	   CTreeParameterArray& array,
	   unsigned int channels, double first, double last, double slope,
	   string units)
{
  for(int i =0; i < num; i++) {
    string name = MakeElementName(basename, low, num, low+i);
    multimap<string, CTreeParameter*>::iterator pParam = CTreeParameter::find(name);
    ASSERT(pParam != CTreeParameter::end());
    CTreeParameter& element(array[i+low]);
    ASSERT(&element == pParam->second);

    CheckConstructed(element, comment,
		      name, channels,first, last, slope, units, false);
  }
}

CPPUNIT_TEST_SUITE_REGISTRATION(TreeParamArrayTests);

void TreeParamArrayTests::Construction() {
  // Default construction:

  CTreeParameterArray Default;

  EQMSG("default low", 0, Default.lowIndex());
  EQMSG("default size", 0U, Default.size());

  // Construction of the array given resolution, size/ base only.

  CTreeParameterArray resonly("resonly", 12, 10, 5);
  EQMSG("resonly low", 5, resonly.lowIndex());
  EQMSG("resonly size", 10U, resonly.size()); 
  CheckArray("resonly", "resolution only", 5, 10,
	     resonly, 
	     1 << 12, 0.0, 4096.0, 1, "unknown");
  // Construction of an array given res, low, high, units, count, baseindex).

  CTreeParameterArray resfull("resfull", 12, 0.0, 4096.0, "channels", false,
			   10, 0);
  CheckArray("resfull", "resolution full", 0, 10,
	    resfull, 
	     1 << 12, 0.0, 4096.0, 1.0, "channels");

  // Construct an array using full defaults:

  CTreeParameterArray fulldefaults("fulldefault", 10, -1);
  CheckArray("fulldefault", "Full default constrution", -1, 10,
	     fulldefaults,
	     100, 1.0, 100.0, 99.0/100.0, "unknown");

  // Construct defaults but with units:

  CTreeParameterArray defaultswithunits("def+units", "mm",
					100, -50);
  CheckArray("def+units", "Default with units", -50, 100,
	     defaultswithunits,
	     100, 1.0, 100.0, 99.0/100.0, "mm");

  // Construct with a range:

  CTreeParameterArray rangegiven("ranged", 0.0, 100.0, "cm",
				 10, 4);
  CheckArray("ranged", "With range but no channelcount",  4, 10,
	     rangegiven, 
	     100, 0, 100, 1, "cm");

  // Construct with range and channel count

  CTreeParameterArray rangeandchans("range+chans", 200, 0.0, 100.0,
				    "inches", 10, 0);
  CheckArray("range+chans", "Range and channel count", 0, 10,
	     rangeandchans,
	     200, 0, 100, 0.5, "inches");
}

//
void
TreeParamArrayTests::Destruction()
{

  CTreeParameterArray* a = new CTreeParameterArray("george",
						   100, 0);
  delete a;			// Should get rid of all the georges:

  for(int i = 0; i < 100; i++) {
    string name = MakeElementName("george", 0, 100, i);
    multimap<string, CTreeParameter*>::iterator p = CTreeParameter::find(name);
    ASSERT(p == CTreeParameter::end());
  }
}
//
void
TreeParamArrayTests::Reset()
{
  // Construct/bind and then set values for all elements.
  // Then reset the array and ensure that all elements are invalid.

  CTreeParameterArray a("george",100,0);
  CTreeParameter::BindParameters();
  CEvent event;
  CTreeParameter::setEvent(event);

  for (int i =0; i < 100; i++) {
    a[i] = (double)i;
  }
  a.Reset();

  for (int i = 0; i < 100; i++) {
    ASSERT(!a[i].isValid());
  }
}
//
void
TreeParamArrayTests::Iteration()
{
  CTreeParameterArray a("george", 100, 0);

  int iteration = 0;
  vector<CTreeParameter*>::iterator p = a.begin();
  while(p != a.end()) {
    CTreeParameter* pparam = *p;
    string name = MakeElementName("george", 0, 100, iteration);
    EQMSG("Name: ", name, pparam->getName());
    iteration++;
    p++;
  }
  EQMSG("Final count ", 100, iteration);

  
}
