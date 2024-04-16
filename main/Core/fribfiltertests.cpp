/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  
 *  @brief: 
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "FRIBFilterFormatter.h"
#include "Event.h"
#define private public
#include "CTreeVariable.h"
#undef private


#include <string>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>
#include <stdexcept>

const char* TempPlate = "fribvfilterXXXXXX";   // Temp file Template (get it?).

class FribFilterTests : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(FribFilterTests);
    CPPUNIT_TEST(dummy);
    CPPUNIT_TEST_SUITE_END();

    // Data:
private:
    // For the temp file:

    std::string m_tempfile;
    int m_fd;

public: 
    void setUp() {
        makeTemp();
    }
    void tearDown() {
        CTreeVariable::CleanMap();
        cleanTemp();
    }

protected:
    void dummy();

private:
    void makeTemp();
    void cleanTemp();

};
/// Utilities:

void
FribFilterTests::makeTemp() {
    // Make the temp file, save the name/fd:

    char filename[100];
    strcpy(filename, TempPlate);
    m_fd = mkstemp(filename);

    if (m_fd < 0) {
        throw std::runtime_error("Could not make temp file for filter tests");
    }
    m_tempfile = filename;

}

void
FribFilterTests::cleanTemp() {
    close(m_fd);
    unlink(m_tempfile.c_str());
}

/// The tests

void FribFilterTests::dummy() {}