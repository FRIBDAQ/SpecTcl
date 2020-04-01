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
#define private public
#include "SaveSet.h"
#undef private
#include "CSqlite.h"
#include "CSqliteStatement.h"

#include <string>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdexcept>
#include <errno.h>
#include <sstream>
#include <time.h>

class savesettest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(savesettest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    CPPUNIT_TEST(construct_4);
    CPPUNIT_TEST(construct_5);
    CPPUNIT_TEST(construct_6);
    
    CPPUNIT_TEST(exists_1);
    CPPUNIT_TEST(exists_2);
    CPPUNIT_TEST_SUITE_END();
    
private:
    CSqlite*    m_pDatabase;
    std::string m_file;
    time_t      m_savesetTime;
public:
    void setUp() {
        makeTempFile();
        makeDatabase();
        m_pDatabase = new CSqlite(m_file.c_str());
        
    }
    void tearDown() {
        delete m_pDatabase;
        unlink(m_file.c_str());
    }
protected:
    void construct_1();   // Construct by name.
    void construct_2();
    void construct_3();
    void construct_4();   // construct by id.
    void construct_5();
    void construct_6();
    
    void exists_1();
    void exists_2();
private:
    void makeTempFile();
    void makeDatabase();
    void makeEmptySet(const char* name);
};
/**
 * use mkstemp to make temporary file.
 */
void
savesettest::makeTempFile()
{
    const char* dbtemplate="savesettestXXXXXX";
    char filename[200];
    strcpy(filename, dbtemplate);
    int fd = mkstemp(filename);
    if (fd < 0) {
        int e = errno;
        std::stringstream msg;
        msg << "Unable to make temp file: " << dbtemplate
            << " : " << strerror(e);
        throw std::logic_error(msg.str());
    }
    close(fd);
    m_file = filename;
}
/**
 * makeDatabase
 *    Create the database in the file m_file.
 */
void
savesettest::makeDatabase()
{
    SpecTcl::CDatabase::create(m_file.c_str());
}
/**
 * makeEmptySet
 *   Make an empty save set and squirrel away the timestamp.
 * @param const char* name - name of the saveset.
 */
void
savesettest::makeEmptySet(const char* name)
{
    time_t now = time(nullptr);
    CSqliteStatement s(
        *m_pDatabase,
        "INSERT INTO save_sets (name, timestamp) VALUES(?,?)"
    );
    s.bind(1, name, -1, SQLITE_STATIC);
    s.bind(2, now);
    ++s;
    m_savesetTime = now;
}
CPPUNIT_TEST_SUITE_REGISTRATION(savesettest);

void savesettest::construct_1()
{
    // Construcing on a nonexistent saveset fails:
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::SaveSet set(*m_pDatabase, m_file.c_str()),
        std::logic_error
    );
}
void savesettest::construct_2()
{
    // constructing an existing one is ok:
    
    makeEmptySet("set");
    CPPUNIT_ASSERT_NO_THROW(
        SpecTcl::SaveSet set(*m_pDatabase, "set");
    );
}
void savesettest::construct_3()
{
    // the info element of the save set is correctly built:
    
    makeEmptySet("set");
    SpecTcl::SaveSet set(*m_pDatabase, "set");
    EQ(std::string("set"), set.m_Info.s_name);
    EQ(1, set.m_Info.s_id);
    EQ(m_savesetTime, set.m_Info.s_stamp);
}
void savesettest::construct_4()
{
    // construct by id with no such fails.
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::SaveSet set(*m_pDatabase, 1),
        std::logic_error
    );
}
void savesettest::construct_5()
{
    // Construt by id for existing one does not fail:
    
    makeEmptySet("set");   // id=1.
    CPPUNIT_ASSERT_NO_THROW(
        SpecTcl::SaveSet  set(*m_pDatabase, 1)
    );
}
void savesettest::construct_6()
{
    // Construct by id loads the right info.
    
    makeEmptySet("set");
    SpecTcl::SaveSet set(*m_pDatabase, 1);
    EQ(std::string("set"), set.m_Info.s_name);
    EQ(1, set.m_Info.s_id);
    EQ(m_savesetTime, set.m_Info.s_stamp);
}
void savesettest::exists_1()
{
    // does not exist:
    
    ASSERT(!(SpecTcl::SaveSet::exists(*m_pDatabase, "set")));
}
void savesettest::exists_2()
{
    // does exist.
    
    makeEmptySet("set");
    ASSERT(SpecTcl::SaveSet::exists(*m_pDatabase, "set"));
}