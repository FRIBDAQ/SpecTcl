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

/** @file:  
 *  @brief: 
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "SpecTclDatabase.h"
#include "CSqlite.h"
#include "CSqliteStatement.h"
#include "CSqliteException.h"

#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <stdexcept>
#include <errno.h>
#include <sstream>
#include <set>
#include <stdexcept>

// Database should have these tables:

static const char* expectedTables[] {
        "save_sets",
        "parameter_defs",
        "spectrum_defs",
        "axis_defs",
        "spectrum_params",
        "spectrum_contents",
        "gate_defs",
        "gate_points",
        "gate_parameters",
        "component_gates",
        "gate_masks",
        "gate_applications",
        "treevariables",
        "runs",
        "events",
        "scaler_readouts",
        "scaler_channels",
    nullptr
};

// Database should have these indices:

static const char* expectedIndices[] {
        "pdef_save_id",
        "pdef_name",
        "sdef_save_id",
        "adef_specid",
        "sparams_spectrum_id",
        "scontents_spectrum_id",
        "gate_points_gatidx",
        "gate_params_parentidx",
        "gate_params_paramidx",
        "component_gates_parentidx",
        "gate_mask_parentix",
        "treevariables_saveidx",
        "run_num_idx",
        nullptr
};

class specdbtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(specdbtest);
    CPPUNIT_TEST(create_1);
    CPPUNIT_TEST(create_2);
    CPPUNIT_TEST(create_3);
    CPPUNIT_TEST(create_4);
    CPPUNIT_TEST(create_5);
    
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST_SUITE_END();
    
private:
    std::string m_dbfile;
public:
    void setUp() {
        const char* fileTemplate="specdbtestXXXXXX";
        char dbfile[200];
        strcpy(dbfile, fileTemplate);
        int fd = mkstemp(dbfile);
        if (fd < 0) {
            int e = errno;
            std::stringstream s;
            s <<  "Unable to make temp file " << fileTemplate
                << " : " << strerror(e);
            throw std::invalid_argument(s.str());
        }
        close(fd);
        unlink(dbfile);
        m_dbfile = dbfile;
    }
    void tearDown() {
        unlink(m_dbfile.c_str());
    }
protected:
    void create_1();
    void create_2();
    void create_3();
    void create_4();
    void create_5();
    
    void construct_1();
    void construct_2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(specdbtest);

void specdbtest::create_1()
{
    // Creating an impossible one should throw
    
    CPPUNIT_ASSERT_THROW(
        SpecTclDB::CDatabase::create("/should/not/be/able/to/create"),
        CSqliteException
    );
}
void specdbtest::create_2()
{
    // Creating in good file doesn't throw.
    CPPUNIT_ASSERT_NO_THROW(
        SpecTclDB::CDatabase::create(m_dbfile.c_str())
    );
}
void specdbtest::create_3()
{
    // create in existing db file doesn't throw.
    
    CPPUNIT_ASSERT_NO_THROW(
        SpecTclDB::CDatabase::create(m_dbfile.c_str())
    );
    CPPUNIT_ASSERT_NO_THROW(
        SpecTclDB::CDatabase::create(m_dbfile.c_str())
    );
    
}
void specdbtest::create_4()
{
    // make sure all the tables are present.  We'll trust that
    // the structures are correct for now:
    
    SpecTclDB::CDatabase::create(m_dbfile.c_str());
    
    // Throw all the table names in set and then look for them:
    
    CSqlite c(m_dbfile.c_str());
    CSqliteStatement s(
        c,
        "SELECT name FROM sqlite_master WHERE type='table'"
    );
    ++s;
    std::set<std::string> tables;
    while(!s.atEnd()) {
        tables.insert(
            std::string(reinterpret_cast<const char*>(s.getText(0)))
        );
        ++s;
    }
    const char** t = expectedTables;    
    while (*t) {
        std::string table = *t;
        EQMSG(table, size_t(1), tables.count(table));
        t++;
    }
}
void specdbtest::create_5()
{
    // make sure all the indices are present.  We'll trust that
    // the structures are correct for now:
    
    SpecTclDB::CDatabase::create(m_dbfile.c_str());
    
    // Throw all the indices names in set and then look for them:
    
    CSqlite c(m_dbfile.c_str());
    CSqliteStatement s(
        c,
        "SELECT name FROM sqlite_master WHERE type='index'"
    );
    ++s;
    std::set<std::string> indices;
    while(!s.atEnd()) {
        indices.insert(
            std::string(reinterpret_cast<const char*>(s.getText(0)))
        );
        ++s;
    }
    const char** t = expectedIndices;    
    while (*t) {
        std::string index = *t;
        EQMSG(index, size_t(1), indices.count(index));
        t++;
    }
}
void specdbtest::construct_1()
{
    // Can't construct on nonexistent:
    
    CPPUNIT_ASSERT_THROW(
        SpecTclDB::CDatabase nosuch(m_dbfile.c_str()),
        std::logic_error
    );
}
void specdbtest::construct_2()
{
    // Can construct on existing though:
    
    SpecTclDB::CDatabase::create(m_dbfile.c_str());
    CPPUNIT_ASSERT_NO_THROW(SpecTclDB::CDatabase ok(m_dbfile.c_str()));
}