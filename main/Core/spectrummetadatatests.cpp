#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <string>
#include <vector>
#include <CNoSuchObjectException.h>
#include <TCLException.h>

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

    CPPUNIT_TEST(cmdset_1);   // wrong param count.
    CPPUNIT_TEST(cmdset_2);   // No such spectrum.
    CPPUNIT_TEST(cmdset_3);   // good set.
    
    CPPUNIT_TEST(cmdget_1);   // wrong param count.
    CPPUNIT_TEST(cmdget_2);   // No such spectrum
    CPPUNIT_TEST(cmdget_3);   // No such metadata.
    CPPUNIT_TEST(cmdget_4);   // good get.

    CPPUNIT_TEST(cmddump_1);   // wrong # params.
    CPPUNIT_TEST(cmddump_2);   // no such spectrum.
    CPPUNIT_TEST(cmddump_3);   // good dump.
    CPPUNIT_TEST_SUITE_END();

protected:
    void set_1();

    void get_1();
    void get_2();

    void getall();

    void cmdset_1();
    void cmdset_2();
    void cmdset_3();

    void cmdget_1();
    void cmdget_2();
    void cmdget_3();
    void cmdget_4();

    void cmddump_1();
    void cmddump_2();
    void cmddump_3();
private:
    CParameter*   m_pParam;
    CSpectrum* m_pSpectrum;
    CTCLInterpreter* m_pInterp;
    CSpectrumCommand* m_pCommand;
public:
    void setUp() {
        m_pParam = SpecTcl::getInstance()->AddParameter("test", 0, "arg");
        m_pSpectrum = SpecTcl::getInstance()->Create1D("test", keLong, *m_pParam, 100);
        SpecTcl::getInstance()->AddSpectrum(*m_pSpectrum);

        m_pInterp = new CTCLInterpreter;
        m_pCommand = new CSpectrumCommand(m_pInterp, "spectrum");
    }
    void tearDown() {
        SpecTcl::getInstance()->RemoveSpectrum("test");
        SpecTcl::getInstance()->RemoveParameter("test");

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

void
TestMetadata::cmdset_1() {
    // incorrect command parameter count for spectrum -setmetadata

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectrum -setmetadata test a b c"),
        CTCLException
    );
}
void 
TestMetadata::cmdset_2() {
    // NO such spectrum:

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectrum -setmetadata testy a b"),
        CTCLException
    );
}
void
TestMetadata::cmdset_3() {
    // can set by command.

    CPPUNIT_ASSERT_NO_THROW(
        m_pInterp->GlobalEval("spectrum -setmetadata test a b ")
    );

    // Make sure it got set:

    std::string value = m_pSpectrum->getMetadata("a");
    EQ(std::string("b"), value);
}

void
TestMetadata::cmdget_1() {
    // wrong # of parameters.
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectrum -getmetadata test a b"),
        CTCLException
    );
}
void
TestMetadata::cmdget_2() {
    // No such spectrum.
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectrum -getmetadata testy a"),
        CTCLException
    );
}
void 
TestMetadata::cmdget_3() {
    // no such metadata:

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectrum -getmetadata test a"),
        CTCLException
    );
}
void 
TestMetadata::cmdget_4() {
    // good get:
    m_pSpectrum->setMetadata("a", "b");
    std::string value;
    CPPUNIT_ASSERT_NO_THROW(
        value = m_pInterp->GlobalEval("spectrum -getmetadata test a")
    );
    EQ(std::string("b"), value);
}

void 
TestMetadata::cmddump_1() {
    // Wrong # params

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectrum -dumpmetadata test a"),
        CTCLException
    );
}
void
TestMetadata::cmddump_2() {
    // no such spectrum.

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectrum -dumpmetadata testy"),
        CTCLException
    );
}
void 
TestMetadata::cmddump_3() {
    // ok - set some metadata first:

    m_pSpectrum->setMetadata("a", "b");
    m_pSpectrum->setMetadata("0", "1");
    m_pSpectrum->setMetadata("help", "me");

    std::string value;
    CPPUNIT_ASSERT_NO_THROW(
        value = m_pInterp->GlobalEval("spectrum -dumpmetadata test")
    );
    EQ(std::string("0 1 a b help me"), value);
}