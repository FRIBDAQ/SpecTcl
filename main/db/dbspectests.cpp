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

/** @file:  dbspectests.cpp
 *  @brief: Test the SpecTcl::DBSpectrum class.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#define private public    // allows testing of private statics.
#include "DBSpectrum.h"
#undef public

#include "SpecTclDatabase.h"
#include "SaveSet.h"
#include "CSqlite.h"
#include "CSqliteStatement.h"

#include <string>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <stdexcept>
#include <errno.h>

class dbspectest : public CppUnit::TestFixture {

    
private:
    std::string m_filename;
    CSqlite*           m_pDb;
    SpecTcl::CDatabase* m_pSDb;
    SpecTcl::SaveSet*  m_pSaveSet;
public:
    void setUp() {
        // Make the tempfile.
        
        const char* temptemplate="dbspectestXXXXXX";
        char filename[200];
        strcpy(filename, temptemplate);
        int fd = mkstemp(filename);
        if (fd < 0) {
            int e = errno;
            std::stringstream msg;
            msg << "Unable to create tempfile: "
                << filename << " : " << strerror(e);
            throw std::logic_error(msg.str());
        }
        close(fd);
        m_filename = filename;
        
        // Open it for sqlite3 and create the schema:
        
        m_pDb = new CSqlite(filename);
        SpecTcl::CDatabase::create(filename);
        
        // Create a database and a save set:
        
        m_pSDb = new SpecTcl::CDatabase(filename);
        m_pSaveSet = m_pSDb->createSaveSet("test-save-set");
    }
    void tearDown() {
        delete m_pSaveSet;
        delete m_pSDb;
        delete  m_pDb;
        unlink(m_filename.c_str());
    }
    // tests:
    CPPUNIT_TEST_SUITE(dbspectest);
    CPPUNIT_TEST(exists_1);
    CPPUNIT_TEST(exists_2);
    CPPUNIT_TEST(exists_3);
    CPPUNIT_TEST_SUITE_END();
protected:
    void exists_1();
    void exists_2();
    void exists_3();
private:
    void addDummySpectrum(
        const char* name, const char* type, const char* dtype
    );
};

void
dbspectest::addDummySpectrum(
    const char* name, const char* type, const char* dtype
)
{
    CSqliteStatement s(
        *m_pDb,
        "INSERT INTO spectrum_defs (save_id, name, type, datatype) \
            VALUES (?,?,?,?)"
    );
    s.bind(1, m_pSaveSet->getInfo().s_id);
    s.bind(2, name, -1, SQLITE_STATIC);
    s.bind(3, type, -1, SQLITE_STATIC);
    s.bind(4, dtype, -1, SQLITE_STATIC);
    ++s;
}

CPPUNIT_TEST_SUITE_REGISTRATION(dbspectest);

void dbspectest::exists_1()
{
    // Nonexistent spectrum returns false for no such saveset.
    
    EQ(false, SpecTcl::DBSpectrum::exists(*m_pDb, 1, "nope"));
}
void dbspectest::exists_2()
{
    // False even if the save set exists.
    
    EQ(
        false,
        SpecTcl::DBSpectrum::exists(*m_pDb, m_pSaveSet->getInfo().s_id, "nope")
    );
}
void dbspectest::exists_3()
{
    // Add a dummy spectrum (incomplete) .. should show existence.
    
    addDummySpectrum("test", "1", "long");
    EQ(
        true,
        SpecTcl::DBSpectrum::exists(*m_pDb, m_pSaveSet->getInfo().s_id, "test")
    );
}