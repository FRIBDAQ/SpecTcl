// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include <Asserts.h>

#define private public
#include "TreeBuilder.h"
#undef private


#include <Event.h>
#include <cmath>

class marshalltests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(marshalltests);
  CPPUNIT_TEST(construction);
  CPPUNIT_TEST(pointer);
  
  CPPUNIT_TEST(marshall_1);
  CPPUNIT_TEST(marshall_2);
  CPPUNIT_TEST(marshall_3);
  CPPUNIT_TEST(marshall_4);
  
  CPPUNIT_TEST(reset);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void construction();
  void pointer();
  
  void marshall_1();
  void marshall_2();
  void marshall_3();
  void marshall_4();
  
  void reset();
};

CPPUNIT_TEST_SUITE_REGISTRATION(marshalltests);

// Construction gives me something filled with nans:
void marshalltests::construction() {
  ParameterMarshaller m(100);
  EQ(size_t(100), m.m_nParamCount);
  for (int i = 0; i < 100; i++) {
    ASSERT(std::isnan(m.m_pParameters[i]));
  }
}
// Pointer method:

void marshalltests::pointer()
{
  ParameterMarshaller m(100);
  EQ(m.m_pParameters, m.pointer());
}
// Marshall with empty event does nothing:

void marshalltests::marshall_1()
{
  ParameterMarshaller m(100);
  CEvent              e;
  
  m.marshall(e);
  Double_t* pMarshalled = m.pointer();
  for (int i = 0; i < 100; i++) {
    ASSERT(std::isnan(pMarshalled[i]));
  }
}
// Marshall with nonempty event but nothing valid:

void marshalltests::marshall_2()
{
ParameterMarshaller m(100);
  CEvent              e(100);
  
  m.marshall(e);
  Double_t* pMarshalled = m.pointer();
  for (int i = 0; i < 100; i++) {
    ASSERT(std::isnan(pMarshalled[i]));
  }  
}
// Marshall with an event with parameter 50 set to some value:
void marshalltests::marshall_3()
{
  ParameterMarshaller m(100);
  CEvent            e;
  e[50] = 1234;
  
  m.marshall(e);
  Double_t* p = m.pointer();
  for (int i = 0; i < 100; i++) {
    if (i != 50) {
      ASSERT(std::isnan(p[i]));
    } else {
      EQ(Double_t(1234), p[i]);
    }
  }
}
// Marshall all event parameters equal to their parameter number:

void marshalltests::marshall_4()
{
  ParameterMarshaller m(100);
  CEvent              e;
  for (int i =0; i < 100; i+= 2) {
    e[i] = i;
  }
  m.marshall(e);
  Double_t* p = m.pointer();
  for (int i = 0; i < 100; i++) {
    if (i % 2) {
      ASSERT(std::isnan(p[i]));
    } else {
      EQ(Double_t(i), p[i]);
    }
  }
}
// After marshalling a non empty event, reset makes everythin nan again.

void marshalltests::reset()
{
  ParameterMarshaller m(100);
  CEvent              e;
  for (int i =0; i < 100; i+= 2) {
    e[i] = i;
  }
  m.marshall(e);
  Double_t* p = m.pointer();
  m.reset(e);
  for (int i = 0; i < 100; i++) {
    ASSERT(std::isnan(p[i]));
  }
}