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
    CPPUNIT_TEST_SUITE_END();

    // test methods
private:
    void create_1();
    void create_2();
    void create_3();
    void create_4();
    void create_5();
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