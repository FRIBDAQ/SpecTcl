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
    
    CPPUNIT_TEST(create_1);
    CPPUNIT_TEST(create_2);
    
    CPPUNIT_TEST(list_1);
    CPPUNIT_TEST(list_2);
    CPPUNIT_TEST(list_3);
    
    CPPUNIT_TEST(info_1);
    
    // CR via a SpecTclDB::Database.
    
    CPPUNIT_TEST(db_1);      // Create
    CPPUNIT_TEST(db_2);      // retrieve by name.
    CPPUNIT_TEST(db_3);      // retrieve by id.
    CPPUNIT_TEST(db_4);      // list all savesets.
    CPPUNIT_TEST_SUITE_END();
    
private:
    CSqlite*    m_pDatabase;
    SpecTclDB::CDatabase* m_pSpecDb;
    std::string m_file;
    time_t      m_savesetTime;
public:
    void setUp() {
        makeTempFile();
        makeDatabase();
        m_pDatabase = new CSqlite(m_file.c_str());
        m_pSpecDb   = new SpecTclDB::CDatabase(m_file.c_str());
    }
    void tearDown() {
        delete m_pDatabase;
        delete m_pSpecDb;
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
    
    void create_1();
    void create_2();
    
    void list_1();
    void list_2();
    void list_3();
    
    void info_1();

    void db_1();
    void db_2();
    void db_3();
    void db_4();
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
    SpecTclDB::CDatabase::create(m_file.c_str());
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
        SpecTclDB::SaveSet set(*m_pDatabase, m_file.c_str()),
        std::logic_error
    );
}
void savesettest::construct_2()
{
    // constructing an existing one is ok:
    
    makeEmptySet("set");
    CPPUNIT_ASSERT_NO_THROW(
        SpecTclDB::SaveSet set(*m_pDatabase, "set");
    );
}
void savesettest::construct_3()
{
    // the info element of the save set is correctly built:
    
    makeEmptySet("set");
    SpecTclDB::SaveSet set(*m_pDatabase, "set");
    EQ(std::string("set"), set.m_Info.s_name);
    EQ(1, set.m_Info.s_id);
    EQ(m_savesetTime, set.m_Info.s_stamp);
}
void savesettest::construct_4()
{
    // construct by id with no such fails.
    
    CPPUNIT_ASSERT_THROW(
        SpecTclDB::SaveSet set(*m_pDatabase, 1),
        std::logic_error
    );
}
void savesettest::construct_5()
{
    // Construt by id for existing one does not fail:
    
    makeEmptySet("set");   // id=1.
    CPPUNIT_ASSERT_NO_THROW(
        SpecTclDB::SaveSet  set(*m_pDatabase, 1)
    );
}
void savesettest::construct_6()
{
    // Construct by id loads the right info.
    
    makeEmptySet("set");
    SpecTclDB::SaveSet set(*m_pDatabase, 1);
    EQ(std::string("set"), set.m_Info.s_name);
    EQ(1, set.m_Info.s_id);
    EQ(m_savesetTime, set.m_Info.s_stamp);
}
void savesettest::exists_1()
{
    // does not exist:
    
    ASSERT(!(SpecTclDB::SaveSet::exists(*m_pDatabase, "set")));
}
void savesettest::exists_2()
{
    // does exist.
    
    makeEmptySet("set");
    ASSERT(SpecTclDB::SaveSet::exists(*m_pDatabase, "set"));
}
void savesettest::create_1()
{
    // no problem if it does not already exist:
    
    SpecTclDB::SaveSet* pSet;
    CPPUNIT_ASSERT_NO_THROW(
        pSet = SpecTclDB::SaveSet::create(*m_pDatabase, "set")
    );
    EQ(std::string("set"), pSet->m_Info.s_name);
    delete pSet;
}
void savesettest::create_2()
{
    // Fails if it exists:
    
    delete SpecTclDB::SaveSet::create(*m_pDatabase, "set");
    CPPUNIT_ASSERT_THROW(
        SpecTclDB::SaveSet::create(*m_pDatabase, "set"),
        std::invalid_argument
    );
}

void savesettest::list_1()
{
    // Empty list:
    
    auto v = SpecTclDB::SaveSet::list(*m_pDatabase);
    EQ(size_t(0), v.size());
    
}
void savesettest::list_2()
{
    // single item:
    
    makeEmptySet("set1");
    auto v = SpecTclDB::SaveSet::list(*m_pDatabase);
    EQ(size_t(1), v.size());
    EQ(1, v[0].s_id);
    EQ(std::string("set1"), v[0].s_name);
    EQ(m_savesetTime, v[0].s_stamp);
}
void savesettest::list_3()
{
    // A few items.
    
    std::vector<std::string> names;
    std::vector<time_t>      stamps;
    
    makeEmptySet("set1");
    names.push_back("set1");
    stamps.push_back(m_savesetTime);
    
    makeEmptySet("set2");
    names.push_back("set2");
    stamps.push_back(m_savesetTime);
    
    makeEmptySet("set3");
    names.push_back("set3");
    stamps.push_back(m_savesetTime);
    
    auto v = SpecTclDB::SaveSet::list(*m_pDatabase);
    EQ(size_t(3), v.size());
    for (int i =0; i < v.size(); i++) {
        int id = i+1;
        EQ(id, v[i].s_id);
        EQ(names[i], v[i].s_name);
        EQ(stamps[i], v[i].s_stamp);
    }
}

void savesettest::info_1()
{
    // Test getinfo:
    
    makeEmptySet("set1");
    SpecTclDB::SaveSet s(*m_pDatabase, "set1");
    auto i = s.getInfo();
    
    EQ(std::string("set1"), i.s_name);
    EQ(1, i.s_id);
    
}

void savesettest::db_1()
{
    // Create new save set via db:
    
    SpecTclDB::SaveSet* pSet;
    CPPUNIT_ASSERT_NO_THROW(
        pSet = m_pSpecDb->createSaveSet("myset")
    );
    ASSERT(pSet);
    EQ(std::string("myset"), pSet->m_Info.s_name);
    EQ(1, pSet->m_Info.s_id);
    // Take the timestamp on faith if we're this good.
    
    delete pSet;
}
void savesettest::db_2()
{
    // Retrieve by name.
    
    delete m_pSpecDb->createSaveSet("set1");
    delete m_pSpecDb->createSaveSet("set2");
    
    SpecTclDB::SaveSet* p2 = m_pSpecDb->getSaveSet("set2");
    SpecTclDB::SaveSet* p1 = m_pSpecDb->getSaveSet("set1");
    
    EQ(std::string("set2"), p2->m_Info.s_name);
    EQ(std::string("set1"), p1->m_Info.s_name);
    
    delete p1;
    delete p2;
}
void savesettest::db_3()
{
    // retrieve by id.
    
    delete m_pSpecDb->createSaveSet("set1");  // id = 1
    delete m_pSpecDb->createSaveSet("set2");  // id = 2
    
    SpecTclDB::SaveSet* p = m_pSpecDb->getSaveSet(2);
    EQ(std::string("set2"), p->m_Info.s_name);
    EQ(2, p->m_Info.s_id);
    
    delete p;
}
void savesettest::db_4()
{
    // Check getAll Savesets
    
    delete SpecTclDB::SaveSet::create(*m_pDatabase, "set1");
    delete SpecTclDB::SaveSet::create(*m_pDatabase, "set2");
    
    auto sets = m_pSpecDb->getAllSaveSets();
    
    EQ(size_t(2), sets.size());
    EQ(std::string("set1"), sets[0]->getInfo().s_name);
    EQ(std::string("set2"), sets[1]->getInfo().s_name);
    
    delete sets[0];
    delete sets[1];
}