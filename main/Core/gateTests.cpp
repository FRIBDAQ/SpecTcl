// Template for a test suite.

#include <config.h>
#include <histotypes.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include <Asserts.h>
#include <stdexcept>
#include "GateFactory.h"


class GateTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(GateTests);
  CPPUNIT_TEST(And);
  CPPUNIT_TEST(Band);
  CPPUNIT_TEST(Bands2Contour);
  CPPUNIT_TEST(Cut);
  CPPUNIT_TEST(False);
  CPPUNIT_TEST(Not);
  CPPUNIT_TEST(Or);
  CPPUNIT_TEST(True);
  CPPUNIT_TEST(GammaCut);
  CPPUNIT_TEST(GammaBand);
  CPPUNIT_TEST(GammaContour);
  CPPUNIT_TEST(EqMask);
  CPPUNIT_TEST(AndMask);
  CPPUNIT_TEST(NotMask);
  CPPUNIT_TEST(Invalid);
  CPPUNIT_TEST_SUITE_END();
  
protected:
    void And();
    void Band();
    void Bands2Contour();
    void Cut();
    void False();
    void Not();
    void Or();
    void True();
    void GammaCut();
    void GammaBand();
    void GammaContour();
    void EqMask();
    void AndMask();
    void NotMask();
    void Invalid();
private:

};

CPPUNIT_TEST_SUITE_REGISTRATION(GateTests);

void GateTests::And()
{
    EQ(CGateFactory::And, CGateFactory::stringToGateType("*"));
}
void GateTests::Band()
{
    EQ(CGateFactory::band, CGateFactory::stringToGateType("b"));
}
void GateTests::Bands2Contour()
{
    EQ(CGateFactory::bandcontour, CGateFactory::stringToGateType("c2band"));
}
void GateTests::Cut()
{
    EQ(CGateFactory::cut, CGateFactory::stringToGateType("s"));
}
void GateTests::False()
{
    EQ(CGateFactory::falseg, CGateFactory::stringToGateType("F"));
}

void GateTests::Not()
{
    EQ(CGateFactory::Not, CGateFactory::stringToGateType("-"));
}
void GateTests::Or()
{
    EQ(CGateFactory::Or, CGateFactory::stringToGateType("+"));
}
void GateTests::True()
{
    EQ(CGateFactory::trueg, CGateFactory::stringToGateType("T"));
}
void GateTests::GammaCut()
{
    EQ(CGateFactory::gammacut, CGateFactory::stringToGateType("gs"));
}
void GateTests::GammaBand()
{
    EQ(CGateFactory::gammaband, CGateFactory::stringToGateType("gb"));
}
void GateTests::GammaContour()
{
    EQ(CGateFactory::gammacontour, CGateFactory::stringToGateType("gc"));
}
void GateTests::EqMask()
{
    EQ(CGateFactory::em, CGateFactory::stringToGateType("em"));
}
void GateTests::AndMask()
{
    EQ(CGateFactory::am, CGateFactory::stringToGateType("am"));
}
void GateTests::NotMask()
{
    EQ(CGateFactory::nm, CGateFactory::stringToGateType("nm"));
}
void GateTests::Invalid()
{
    CPPUNIT_ASSERT_THROW(
        CGateFactory::stringToGateType("nosuch"),
        std::invalid_argument
    );
}