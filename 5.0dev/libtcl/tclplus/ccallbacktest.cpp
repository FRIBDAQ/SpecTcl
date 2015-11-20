// Template for a test suite.

#include  <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <string.h>
#include "TCLInterpreter.h"

#include "CLanguageTraceCallbacks.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


class ccallbacktests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ccallbacktests);
  CPPUNIT_TEST(Callback);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void Callback();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ccallbacktests);

char*  acallback(Tcl_Interp* pInterp, char* name, char* element,
	       int flags, void* client)
{
  EQ(0, strcmp(name, "aname"));
  EQ(0, strcmp(element, "anelement"));
  EQ(TCL_TRACE_READS, flags);
  EQ((void*)0x01020304, client);
  return NULL;
}

void ccallbacktests::Callback() {
  CTCLInterpreter interp;
  CLanguageTraceCallback  cb(acallback, (void*)0x01020304);
  cb(&interp, (char*)"aname", (char*)"anelement", TCL_TRACE_READS);
  
}
