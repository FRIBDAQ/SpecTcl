#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <string>
#include <vector>
#include <CNoSuchObjectException.h>

#include <TCLInterpreter.h>

#include "Parameter.h"
#define private public    // to get acess to m_metadata
#include "Spectrum.h"
#include "Spectrum1DL.h"
#include <SpectrumCommand.h>
#include <SpecTcl.h>
#undef private
#include "CMetadata.h"
class TestMetadata : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TestMetadata);
    // tests for CSpectrum.
    CPPUNIT_TEST(set_1);
    CPPUNIT_TEST(get_1);
    CPPUNIT_TEST(get_2);
    CPPUNIT_TEST(getall);

    // Tests for the spectrum command.
    CPPUNIT_TEST_SUITE_END();

protected:
    void set_1();

    void get_1();
    void get_2();

    void getall();
private:
    CParameter*   m_pParam;
    CSpectrum* m_pSpectrum;
    CTCLInterpreter* m_pInterp;
    CSpectrumCommand* m_pCommand;
public:
    void setUp() {
        m_pParam = SpecTcl::getInstance()->AddParameter("test", 0, "arg");
        m_pSpectrum = SpecTcl::getInstance()->Create1D("test", keLong, *m_pParam, 100);

        m_pInterp = new CTCLInterpreter;
        m_pCommand = new CSpectrumCommand(m_pInterp, "spectrum");
    }
    void tearDown() {
        delete SpecTcl::getInstance()->RemoveSpectrum("test");
        delete SpecTcl::getInstance()->RemoveParameter("test");

        delete m_pCommand;
        delete m_pInterp;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestMetadata);

void
TestMetadata::set_1() {
    m_pSpectrum->setMetadata("a", "b");
    std::string value;
    CPPUNIT_ASSERT_NO_THROW(
        value = m_pSpectrum->m_metadata.get("a")
    );
    EQ(std::string("b"), value);
}

void 
TestMetadata::get_1() {
    // get no such is exception.

    CPPUNIT_ASSERT_THROW(
        m_pSpectrum->getMetadata("a"),
        CNoSuchObjectException
    );
}
void
TestMetadata::get_2() {
    // get with actual metadata works

    m_pSpectrum->setMetadata("a", "b");
    std::string value;
    CPPUNIT_ASSERT_NO_THROW(
        value = m_pSpectrum->getMetadata("a")
    );
    EQ(std::string("b"), value);
}

void 
TestMetadata::getall() {
    m_pSpectrum->setMetadata("a", "b");
    m_pSpectrum->setMetadata("z", "x");
    m_pSpectrum->setMetadata("q", "r");
    m_pSpectrum->setMetadata("s", "t");

    // Stuff should come out in alpha order in the map:

    std::vector<std::pair<std::string, std::string>> sb = {
        {"a", "b"},  {"q", "r"}, {"s", "t"}, {"z", "x"}
    };

    const auto&  md = m_pSpectrum->getAllMetadata();

    int i;
    for(const auto& p : md) {
        EQ(p.first, sb[i].first);
        EQ(p.second, sb[i].second);
        i++;
    }
}