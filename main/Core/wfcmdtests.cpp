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

class WFCmdTests : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(WFCmdTests);
    CPPUNIT_TEST_SUITE_END();

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