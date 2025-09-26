/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  wfcmdtests
 *  @brief: Test the spectc::serial::waveform command.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "CWaveformCommand.h"
#include <TCLInterpreter.h>
#include <TCLException.h>
#define private public
#include "CWaveFormDictionary.h"
#undef private

#include "SpecTcl.h"
#include "CWaveForm.h"
#include <tcl.h>

class WFCmdTests : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(WFCmdTests);
    CPPUNIT_TEST(create_1);
    CPPUNIT_TEST(create_2);
    CPPUNIT_TEST(create_3);
    CPPUNIT_TEST(create_4);

    CPPUNIT_TEST(list_1);
    CPPUNIT_TEST(list_2);
    CPPUNIT_TEST(list_3);
    CPPUNIT_TEST(list_4);
    CPPUNIT_TEST(list_5);

    CPPUNIT_TEST(get_1);
    CPPUNIT_TEST(get_2);
    CPPUNIT_TEST(get_3);
    CPPUNIT_TEST_SUITE_END();

    // test methods
private:
    void create_1();
    void create_2();
    void create_3();
    void create_4();
    void create_5();

    void list_1();
    void list_2();
    void list_3();
    void list_4();
    void list_5();

    void get_1();
    void get_2();
    void get_3();

// Test objects:
private:
    CTCLInterpreter* m_pInterp;
    CWaveformCommand* m_pCommand;
public:
    void setUp() {
        m_pInterp = new CTCLInterpreter;
        m_pCommand = new CWaveformCommand(*m_pInterp);
    }
    void tearDown() {
        delete m_pCommand;
        delete m_pInterp;

        // Also destroy the waveform dict:

        delete CWaveFormDictionary::m_pInstance;
        CWaveFormDictionary::m_pInstance = nullptr;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(WFCmdTests);

// create tests:

void
WFCmdTests::create_1() {
    // explicit create subcommand:

    // successful.
    CPPUNIT_ASSERT_NO_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform create test 100")
    );  
    // The waveform was created and has the right size

    auto wf = SpecTcl::getInstance()->findWaveform("test");
    ASSERT(wf);
    EQ(size_t(100), wf->size());

}

void
WFCmdTests::create_2() {
    // implicit create:

    CPPUNIT_ASSERT_NO_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform test 100")
    );

    auto wf = SpecTcl::getInstance()->findWaveform("test");
    ASSERT(wf);
    EQ(size_t(100), wf->size());
}

void WFCmdTests::create_3() {
    // not enough parameters.

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcL::serial::waveform create test"),
        CTCLException
    );
}
void WFCmdTests::create_4() {
    // too many parameters.

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcL::serial::waveform create test 100 junk"),
        CTCLException
    );
}
void WFCmdTests::create_5() {
    // duplicate should fail:

    CPPUNIT_ASSERT_NO_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform create test 100")
    );

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spctcl::serial::waveform create test 150"),    // already exists.
        CTCLException
    );

    // THe old one is still there and has correct size.

    auto wf = SpecTcl::getInstance()->findWaveform("test");
    ASSERT(wf);
    EQ(size_t(100), wf->size());
}

/// tests for list:

void WFCmdTests::list_1() {
    // no waveforms gives empty list result both with and without pattern:

    CPPUNIT_ASSERT_NO_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform list *")
    );

    CTCLObject result;
    result.Bind(*m_pInterp);
    result = m_pInterp->GetResultString();
    EQ(0, result.llength());

    // NOw with the default "*" pattern:

    CPPUNIT_ASSERT_NO_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform list")
    );
    result = m_pInterp->GetResultString();
    EQ(0, result.llength());
}
void WFCmdTests::list_2() {
    // too many command params:

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("spectcl::serial::waveform list a b"),
        CTCLException
    );
}
void WFCmdTests::list_3() {
    // List when one waveform is present:

    m_pInterp->GlobalEval("spectcl::serial::waveform create test  100"); 
    m_pInterp->GlobalEval("spectcl::serial::waveform list");

    CTCLObject result;
    result.Bind(*m_pInterp);
    result = m_pInterp->GetResultString();
    EQ(1, result.llength());    // There's one dict.

    CTCLObject dict = result.lindex(0);
    dict.Bind(*m_pInterp);

    // Should look like:  {name test samples 100 metadata {}} since dicts preserve order.
    // shimmer into a list and see:

    EQ(6, dict.llength());
    EQ(std::string("name"), std::string(dict.lindex(0)));
    EQ(std::string("test"), std::string(dict.lindex(1)));
    EQ(std::string("samples"), std::string(dict.lindex(2)));
    EQ(std::string("100"), std::string(dict.lindex(3)));
    EQ(std::string("metadata"), std::string(dict.lindex(4)));
    EQ(std::string(""), std::string(dict.lindex(5)));

    // Assume that other than possibly the metadata, the list format is correct...
    // can we pattern match it?

    m_pInterp->GlobalEval("spectcl::serial::waveform list test");
    result = m_pInterp->GetResultString();
    EQ(1, result.llength());    // There's one dict.

    m_pInterp->GlobalEval("spectcl::serial::waveform list testing");
    result = m_pInterp->GetResultString();
    EQ(0, result.llength());    // No match.

}

void WFCmdTests::list_4() {
    // If there are two wf's I can see them both and select them.
    m_pInterp->GlobalEval("spectcl::serial::waveform create test1  100"); 
    m_pInterp->GlobalEval("spectcl::serial::waveform create test2  100"); 

    m_pInterp->GlobalEval("spectcl::serial::waveform list");   // Matches both:

    CTCLObject result;
    result.Bind(*m_pInterp);
    result = m_pInterp->GetResultString();
    EQ(2, result.llength());

    // Using partial pattern match:

    m_pInterp->GlobalEval("spectcl::serial::waveform list test*");   // Matches both:
    result = m_pInterp->GetResultString();
    EQ(2, result.llength());

    // match only test1:

    m_pInterp->GlobalEval("spectcl::serial::waveform list *1");  // only one match.
    result = m_pInterp->GetResultString();
    EQ(1, result.llength());

    CTCLObject dict;
    dict.Bind(*m_pInterp);
    dict = result.lindex(0);
    EQ(std::string("test1"),std::string(dict.lindex(1)));    // Name is correct.

}

void WFCmdTests::list_5() {
    // Waveform with metadata.
    // note we manually add it:

    m_pInterp->GlobalEval("spectcl::serial::waveform create test  100"); 
    auto wf = SpecTcl::getInstance()->findWaveform("test");
    wf->setMetadata("frequency", "250MHz");

    // Now the list:

    m_pInterp->GlobalEval("spectcl::serial::waveform list");
    CTCLObject result;
    result.Bind(*m_pInterp);
    result = m_pInterp->GetResultString();

    CTCLObject wfdict;
    wfdict.Bind(*m_pInterp);
    wfdict = result.lindex(0);   // The matched dict.

    CTCLObject mddict;
    mddict.Bind(*m_pInterp);
    mddict = wfdict.lindex(5);   // Metadata dict.

    EQ(2, mddict.llength());    // frequency 250MHz:
    EQ(std::string("frequency"), std::string(mddict.lindex(0)));
    EQ(std::string("250MHz"), std::string(mddict.lindex(1)));
}

// tests of the get subcommand.

void WFCmdTests::get_1() {}
void WFCmdTests::get_2() {}
void WFCmdTests::get_3() {}