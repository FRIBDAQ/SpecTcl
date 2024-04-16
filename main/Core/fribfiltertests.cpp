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

#include <v12/RingItemFactory.h>
#include <v12/CRingItem.h>

#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include <string.h>
#include <vector>
#include <stdexcept>
#include <memory>

const char* TempPlate = "fribvfilterXXXXXX";   // Temp file Template (get it?).

class FribFilterTests : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(FribFilterTests);
    CPPUNIT_TEST(empty);
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
    void empty();

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

void FribFilterTests::empty() {
    // If I make  filter open/describe_parameters/close it I shouild have 2 ring items
    // First is a param def item, the second variable values but both will be empty.

    // empty parameter arrays:

    std::vector<std::string> names;
    std::vector<UInt_t>      ids;

    CFRIBFilterFormat filter;
    filter.open(m_tempfile);
    filter.DescribeEvent(names, ids);
    filter.close();

    // Rewind the file and verify the contents:

    if (lseek(m_fd, 0, SEEK_SET) < 0) {
        throw std::runtime_error("FribFilterTests::empty - failed to rewind filter file");
    }


    v12::RingItemFactory fact;
    // Get and verify the parameter description item.
    std::unique_ptr<CRingItem> pitem(fact.getRingItem(m_fd));
    ASSERT(pitem.get() != 0);

    // get and verify the variable description/value item:


    std::unique_ptr<CRingItem> vitem(fact.getRingItem(m_fd));
    ASSERT(vitem.get() != 0);

    // Should be at end of file:

    std::unique_ptr<CRingItem> none(fact.getRingItem(m_fd));
    ASSERT(none.get() == 0);


}