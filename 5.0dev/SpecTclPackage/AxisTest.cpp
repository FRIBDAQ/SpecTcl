// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#define private public
#include "CAxis.h"
#undef private

class AxisTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(AxisTest);
  CPPUNIT_TEST(construct);
  CPPUNIT_TEST(getname);
  CPPUNIT_TEST(getbins);
  CPPUNIT_TEST(getlow);
  CPPUNIT_TEST(gethigh);	// don't you wish ;-)
  CPPUNIT_TEST(tochannels);
  CPPUNIT_TEST(toaxis);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void construct();
  void getname();
  void getbins();
  void getlow();
  void gethigh();
  void tochannels();
  void toaxis();
};

CPPUNIT_TEST_SUITE_REGISTRATION(AxisTest);

/**
 * construct (whitebox)
 *   Construction should fill in member variables.
 */
void AxisTest::construct() {

  CAxis x("x-axis", 1024, 1.0, 1024.0);
  EQ(std::string("x-axis"), x.m_name);
  EQ(1024U, x.m_channels);
  EQ(1.0, x.m_low);
  EQ(1024.0, x.m_high);
}
/**
 * getname - test m_name selector.
 */
void
AxisTest::getname()
{
  const char* name="x-axis";
  CAxis x(name, 1024, 1.0, 10240.0);

  EQ(std::string(name), x.name());
}
/** 
 * getbins - test m_channels selector.
 */
void
AxisTest::getbins()
{
  const char* name="x-axis";
  unsigned bins = 1024;
  CAxis x(name, bins, 1.0, 10240.0);
  EQ(bins, x.channels());

}
/**
 * getlow - test m_low selector.
 */
void
AxisTest::getlow()
{
  const char* name="x-axis";
  unsigned bins = 1024;
  double   low  = 1.0;
  CAxis x(name, bins, low, 10240.0);
  EQ(low, x.low());
}
/**
 * test m_high selector.
 */
void
AxisTest::gethigh()
{
  const char* name="x-axis";
  unsigned bins = 1024;
  double   low  = 1.0;
  double  high  = 10240.0;
  CAxis x(name, bins, low, high);
  EQ(high, x.high());

}
/**
 * tochannels
 *
 * Test conversion from parameter to axis space.
 */
void
AxisTest::tochannels()
{
  const char* name="x-axis";
  unsigned bins = 1024;
  double   low  = 0.0;
  double  high  = 1023.0;
  CAxis x(name, bins, low, high);

  EQ(0, x.toChannels(low));
  EQ((int)bins, x.toChannels(high-.1));
  EQ(512, x.toChannels(high/2.0));

  CAxis y(name, bins, -1.0, 1.0);
  EQ(0, y.toChannels(-1.0));
  EQ(512, y.toChannels(0));
  EQ(1024, y.toChannels(.999999));
}
/**
 * toaxis
 *   Test conversion to axis coords from bins.
 */
void
AxisTest::toaxis()
{
  const char* name="x-axis";
  unsigned bins = 1024;
  double   low  = 0.0;
  double  high  = 1023.0;
  CAxis x(name, bins, low, high);

  EQ(low, x.toAxis(0));
  EQ(high, x.toAxis(bins));
  EQ((low+high)/2.0, x.toAxis(bins/2));
}
