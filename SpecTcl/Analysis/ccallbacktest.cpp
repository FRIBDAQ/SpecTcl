#include <config.h>
#include <buffer.h>
#include <buftypes.h>


#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "CLanguageCallback.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

class ccallbacktests : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(ccallbacktests);
  CPPUNIT_TEST(callright);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp()
  {
  }
  void tearDown() {
  }

protected:
  void callright();
};


CPPUNIT_TEST_SUITE_REGISTRATION(ccallbacktests);

unsigned int valueGotten;
const void*  pbuffergotten;
const void*  pdatagotten;

void
callback(unsigned int nvalue, const void* pbuffer, void* pdata)
{
  valueGotten  = nvalue;
  pbuffergotten = pbuffer;
  pdatagotten   = pdata;
}




int abc;
int def;

void
ccallbacktests::callright()
{
  CLanguageCallback cb(callback, &def);

  cb(1, &abc);

  EQ((unsigned)1, valueGotten);
  EQ((const void*)&abc, pbuffergotten);
  EQ((const void*)&def, pdatagotten);
}
