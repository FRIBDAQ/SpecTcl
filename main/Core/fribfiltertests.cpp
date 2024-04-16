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
#include "AnalysisRingItems.h"
#define private public
#include "CTreeVariable.h"
#undef private

#include <v12/RingItemFactory.h>
#include <CRingItem.h>

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
    CPPUNIT_TEST(params_1);
    CPPUNIT_TEST(vars_1);
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
    void params_1();
    void vars_1();

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
    EQ(frib::analysis::PARAMETER_DEFINITIONS, pitem->type());
    const frib::analysis::ParameterDefinitions* ppItem = 
        reinterpret_cast<const frib::analysis::ParameterDefinitions*>(pitem->getItemPointer());
    EQ(std::uint32_t(0), ppItem->s_numParameters);


    // get and verify the variable description/value item:


    std::unique_ptr<CRingItem> vitem(fact.getRingItem(m_fd));
    ASSERT(vitem.get() != 0);
    EQ(frib::analysis::VARIABLE_VALUES, vitem->type());
    const frib::analysis::VariableItem* pvItem = 
        reinterpret_cast<const frib::analysis::VariableItem*>(vitem->getItemPointer());
    EQ(std::uint32_t(0), pvItem->s_numVars);

    // Should be at end of file:

    std::unique_ptr<CRingItem> none(fact.getRingItem(m_fd));
    ASSERT(none.get() == 0);

}

void FribFilterTests::params_1() {
    // Try some parameter definitions and see if they make it into the ring item:

    std::vector<std::string> names = { "p1", "p2", "p3", "the_last"};
    std::vector<UInt_t>     ids    = {   1,   3,    5,       2};

    CFRIBFilterFormat filter;
    filter.open(m_tempfile);
    filter.DescribeEvent(names, ids);
    filter.close();

    if (lseek(m_fd, 0, SEEK_SET) < 0) {
        throw std::runtime_error("FribFilterTests::params_2 failed to rewind test file");
    }

    v12::RingItemFactory fact;
    // Get and verify the parameter description item.
    std::unique_ptr<CRingItem> pitem(fact.getRingItem(m_fd));
    ASSERT(pitem.get() != 0);
    EQ(frib::analysis::PARAMETER_DEFINITIONS, pitem->type());
    const frib::analysis::ParameterDefinitions* ppItem = 
        reinterpret_cast<const frib::analysis::ParameterDefinitions*>(pitem->getItemPointer());
    EQ(std::uint32_t(names.size()), ppItem->s_numParameters);

    const frib::analysis::ParameterDefinition* pDef = ppItem ->s_parameters;
    for (int i =0; i < names.size(); i++) {
        EQ(std::uint32_t(ids[i]), pDef->s_parameterNumber);
        std::string aname = pDef->s_parameterName;
        EQ(names[i], aname);

        // Next one:

        const std::uint8_t* p = reinterpret_cast<const std::uint8_t*>(pDef);
        p += sizeof(std::uint32_t) + aname.size() + 1;
        pDef = reinterpret_cast<const frib::analysis::ParameterDefinition*>(p);
    }


}

void FribFilterTests::vars_1() {
    // Make some tree variables and ensure they are also stored properly in the second ring item:

    CTreeVariable v1("pi", 3.1416, "radians");
    CTreeVariable v2("pi_over_two", 3.1416/2.0, "radians");
    CTreeVariable v3("forty-five", 45.0, "degrees");
    CTreeVariable v4("inch", 2.54, "cm");

    // These will come out of the dict in alpha order:

    std::vector<std::string> names = {"forty-five", "inch", "pi", "pi-over_two"};
    std::vector<double> values= {45.0, 2.54, 3.1416, 3.1416/2.0};
    std::vector<std::string> units = {"degrees", "cm", "radians", "radians"};

    //we already verified parameters and that should be orthogonal to this.

    std::vector<std::string> pnames;
    std::vector<UInt_t> pids;

    CFRIBFilterFormat filter;
    filter.open(m_tempfile);
    filter.DescribeEvent(pnames, pids);
    filter.close();

    if (lseek(m_fd, 0, SEEK_SET) < 0) {
        throw std::runtime_error("FribFilterTests::params_2 failed to rewind test file");
    }

    v12::RingItemFactory fact;
    // Skip the parameter def.
    std::unique_ptr<CRingItem> pitem(fact.getRingItem(m_fd));

    // Get/verify the variable values:

    std::unique_ptr<CRingItem> vitem(fact.getRingItem(m_fd));
    ASSERT(vitem.get() != 0);
    EQ(frib::analysis::VARIABLE_VALUES, vitem->type());
    const frib::analysis::VariableItem* pvItem = 
        reinterpret_cast<const frib::analysis::VariableItem*>(vitem->getItemPointer());
    EQ(std::uint32_t(names.size()), pvItem->s_numVars);

    const frib::analysis::Variable* pVar = pvItem->s_variables;
    for (int i =0; i < names.size(); i++) {
        std::string vunits = pVar->s_variableUnits;
        std::string aname = pVar->s_variableName;

        EQ(names[i], aname);
        EQ(values[i], pVar->s_value);
        EQ(units[i], vunits);

        const std::uint8_t* p = reinterpret_cast<const std::uint8_t*>(pVar);
        p += sizeof(double) + frib::analysis::MAX_UNITS_LENGTH + aname.size() + 1;
        pVar = reinterpret_cast<const frib::analysis::Variable*>(p);
    }
}