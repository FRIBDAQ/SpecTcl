// Template for a test suite.

#include <config.h>
#include <histotypes.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include <Asserts.h>
#include <stdexcept>
#define private public
#include "GateFactory.h"
#include "GateContainer.h"   // so we can get to m_metadta e.g.
#undef private
#include <CNoSuchObjectException.h>
#include <vector>
#include <TCLInterpreter.h>
#include <SpecTcl.h>
#include <GateCommand.h>
#include <TCLException.h>
#include <CGateDictionarySingleton.h>

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

  CPPUNIT_TEST(SetMeta_1);  // Good set...
  CPPUNIT_TEST(GetMeta_1);  // no such
  CPPUNIT_TEST(GetMeta_2);  // Ok get.
  CPPUNIT_TEST(DumpMeta_1); // Dup is correct.

  CPPUNIT_TEST(CmdSetMeta_1); // Invalid # parameters.
  CPPUNIT_TEST(CmdSetMeta_2); // No such gate.
  CPPUNIT_TEST(CmdSetMeta_3); // good set.

  CPPUNIT_TEST(CmdGetMeta_1); // Invalid # params.
  CPPUNIT_TEST(CmdGetMeta_2); // No such gate
  CPPUNIT_TEST(CmdGetMeta_3); // No such metadata.
  CPPUNIT_TEST(CmdGetMeta_4); // OK.
  CPPUNIT_TEST(CmdDumpMeta_1); // incorrect # params.
  CPPUNIT_TEST(CmdDumpMeta_2); // No such gate.
  CPPUNIT_TEST(CmdDumpMeta_3); // Ok.
  CPPUNIT_TEST_SUITE_END();

public:
    

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

    // Metadata tests Issue #229

    void SetMeta_1();
    void GetMeta_1();
    void GetMeta_2();
    void DumpMeta_1();

    // Gate command metadata tests (Issue #229)
    void CmdSetMeta_1();
    void CmdSetMeta_2();
    void CmdSetMeta_3();

    void CmdGetMeta_1();
    void CmdGetMeta_2();
    void CmdGetMeta_3();
    void CmdGetMeta_4();

    void CmdDumpMeta_1(); 
    void CmdDumpMeta_2(); 
    void CmdDumpMeta_3(); 

private:
    CTCLInterpreter* m_pInterp;
    CGateCommand* m_pCommand; 
    CGate*        m_pGate;

public:
    void setUp() {
        m_pInterp = new CTCLInterpreter();
        m_pCommand = new CGateCommand(m_pInterp);
        m_pGate   = SpecTcl::getInstance()->CreateTrueGate();
        SpecTcl::getInstance()->AddGate("true", m_pGate);
    }

    void tearDown() {
        delete m_pCommand;
        delete m_pInterp;
        auto pDict = CGateDictionarySingleton::getInstance();

        // Leak memory for now:
        pDict->clear();
    }
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

void 
GateTests::SetMeta_1() {
    CGateContainer c;
    c.setMetadata("a", "b");

    std::string value;
    CPPUNIT_ASSERT_NO_THROW(
        value = c.m_metadata.get("a")
    );
    EQ(std::string("b"), value);
}

void GateTests::GetMeta_1() {
    CGateContainer c;

    CPPUNIT_ASSERT_THROW(
        c.getMetadata("a"),
        CNoSuchObjectException
    );
}
void GateTests::GetMeta_2() {
    CGateContainer c;
    c.setMetadata("a", "b");

    std::string value;
    CPPUNIT_ASSERT_NO_THROW(
        value = c.getMetadata("a")
    );
    EQ(std::string("b"), value);
}
void GateTests::DumpMeta_1() {
    CGateContainer c;
    c.setMetadata("z", "x");
    c.setMetadata("a", "b");
    c.setMetadata("q", "r");

    // Comes out in order by key:

    std::vector<std::pair<std::string, std::string>> sb = {
        {"a", "b"}, {"q", "r"}, {"z", "x"}
    };
    const auto& md = c.getAllMetadata();

    int i;
    for (const auto& p : md) {
        EQ(p.first, sb[i].first);
        EQ(p.second, sb[i].second);
        i++;
    }
}

void
GateTests::CmdSetMeta_1() {
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("::spectcl::serial::gate -setmetadata true a b c"),
        CTCLException       
    );
}
void
GateTests::CmdSetMeta_2() {
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("::spectcl::serial::gate -setmetadata false a b"),
        CTCLException
    );
}

void
GateTests::CmdSetMeta_3() {
    CPPUNIT_ASSERT_NO_THROW(
        m_pInterp->GlobalEval("::spectcl::serial::gate -setmetadata true a b")
    );
    EQ(std::string("b"), SpecTcl::getInstance()->FindGate("true")->getMetadata("a"));
}

void
GateTests::CmdGetMeta_1() {
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("::spectcl::serial::gate -getmetadata true a b"),
        CTCLException
    );
}
void
GateTests::CmdGetMeta_2() {
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("::spectcl::serial::gate -getmetadata false a"),
        CTCLException
    );
}
void
GateTests::CmdGetMeta_3() {
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("::spectcl::serial::gate -getmetadata true a"),
        CTCLException
    );
}
void
GateTests::CmdGetMeta_4() {
    SpecTcl::getInstance()->FindGate("true")->setMetadata("a", "b");
    std::string value;
    CPPUNIT_ASSERT_NO_THROW(
        value = m_pInterp->GlobalEval("::spectcl::serial::gate -getmetadata true a")
    );
    EQ(std::string("b"), value);
}

void
GateTests::CmdDumpMeta_1() {
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("::spectcl::serial::gate -dumpmetadata true junk"),
        CTCLException
    );
}
void 
GateTests::CmdDumpMeta_2() {
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("::spectcl::serial::gate -dumpmetadata trues"),
        CTCLException
    );
}
void
GateTests::CmdDumpMeta_3() {
    auto c = SpecTcl::getInstance()->FindGate("true");
    c->setMetadata("z", "q");
    c->setMetadata("q", "zzz");
    c->setMetadata("a", "b");
    std::string value;
    CPPUNIT_ASSERT_NO_THROW(
        value = m_pInterp->GlobalEval("::spectcl::serial::gate -dumpmetadata true")
    );
    EQ(std::string("a b q zzz z q"), value);

}