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

/** @file:  dbpartests.cpp
 *  @brief: Test SpecTclDB::DBParameter
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "CSqlite.h"
#include "CSqliteStatement.h"
#include "SpecTclDatabase.h"
#include "SaveSet.h"
#include "DBParameter.h"

#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include <string>
#include <set>
#include <errno.h>
#include <stdexcept>
#include <map>

class dbpartest : public CppUnit::TestFixture {
    
private:
    std::string         m_dbfile;
    SpecTclDB::CDatabase* m_pDb;
    SpecTclDB::SaveSet*   m_pSet;
    CSqlite*            m_pConn;
public:
    void setUp() {
        const char* fileTemplate="dbpartestsXXXXXX";
        char fname[200];
        strcpy(fname, fileTemplate);
        int fd = mkstemp(fname);
        if (fd < 0) {
            int e = errno;
            std::stringstream msg;
            msg << "Unable to create tempfile: " << fileTemplate
                << " : " << strerror(e);
            throw std::logic_error(msg.str());
        }
        close(fd);
        
        m_dbfile = fname;
        SpecTclDB::CDatabase::create(fname);
        m_pDb = new SpecTclDB::CDatabase(fname);
        m_pSet = m_pDb->createSaveSet("set1");
        m_pConn = new CSqlite(m_dbfile.c_str());
    }
    void tearDown() {
        delete m_pSet;
        delete m_pDb;
        delete m_pConn;
        unlink(m_dbfile.c_str());
    }
private:
    CPPUNIT_TEST_SUITE(dbpartest);
    CPPUNIT_TEST(exists_1);
    CPPUNIT_TEST(exists_2);
    
    CPPUNIT_TEST(create_1);
    CPPUNIT_TEST(create_2);
    CPPUNIT_TEST(create_3);
    CPPUNIT_TEST(create_4);
    CPPUNIT_TEST(create_5);
    CPPUNIT_TEST(create_6);
    
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    CPPUNIT_TEST(construct_4);
    CPPUNIT_TEST(construct_5);
    CPPUNIT_TEST(construct_6);
    
    CPPUNIT_TEST(list_1);
    CPPUNIT_TEST(list_2);
    CPPUNIT_TEST(list_3);
    
    CPPUNIT_TEST(get_1);
    CPPUNIT_TEST(get_2);
    CPPUNIT_TEST(get_3);
    
    CPPUNIT_TEST(save_1);      // Test save set wrappings.
    CPPUNIT_TEST(save_2);
    CPPUNIT_TEST(save_3);
    CPPUNIT_TEST(save_4);
    CPPUNIT_TEST(save_5);
    CPPUNIT_TEST(save_6);
    
    CPPUNIT_TEST_SUITE_END();

protected:
    void exists_1();
    void exists_2();
    
    void create_1();
    void create_2();
    void create_3();
    void create_4();
    void create_5();
    void create_6();
    
    void construct_1();
    void construct_2();
    void construct_3();
    void construct_4();
    void construct_5();
    void construct_6();
    
    void list_1();
    void list_2();
    void list_3();
    
    void get_1();
    void get_2();
    void get_3();
    
    void save_1();
    void save_2();
    void save_3();
    void save_4();
    void save_5();
    void save_6();
private:
    void makeMinimalParameter(int set, const char* name, int num);
};

void
dbpartest::makeMinimalParameter(int set, const char* name, int num)
{
    CSqliteStatement s(
        *m_pConn,
        "INSERT INTO parameter_defs (save_id, name, number) VALUES (?,?,?)"
    );
    s.bind(1, set);
    s.bind(2, name, -1, SQLITE_STATIC);
    s.bind(3, num);
    ++s;
}

CPPUNIT_TEST_SUITE_REGISTRATION(dbpartest);

void dbpartest::exists_1()
{
    // Initially a save set has no parameters:
    
    ASSERT(!SpecTclDB::DBParameter::exists(*m_pConn, 1, "atest"));
}
void dbpartest::exists_2()
{
    // Can find the one and only parameter:
    
    makeMinimalParameter(1, "atest", 1);
    ASSERT(SpecTclDB::DBParameter::exists(*m_pConn, 1, "atest"));
}
void dbpartest::create_1()
{
    // Can create a simple, in an existing save set.
    
    SpecTclDB::DBParameter* p(nullptr);
    CPPUNIT_ASSERT_NO_THROW(
        p = SpecTclDB::DBParameter::create(*m_pConn, 1, "atest", 1)
    );
    ASSERT(p);                    // No longer null.
    
    // Info correct:
    
    auto i = p->getInfo();
    EQ(1, i.s_id);
    EQ(1, i.s_savesetId);
    EQ(std::string("atest"), i.s_name);
    EQ(1, i.s_number);
    EQ(false, i.s_haveMetadata);
    delete p;
    
    // It's in the database:
    
    CSqliteStatement s(
        *m_pConn,
        "SELECT id, number, low FROM parameter_defs \
         WHERE save_id = ? AND name = ?"
    );
    s.bind(1, 1);
    s.bind(2, "atest", -1, SQLITE_STATIC);
    ++s;
    ASSERT(!s.atEnd());
    EQ(1, s.getInt(0));
    EQ(1, s.getInt(1));
    EQ(CSqliteStatement::null, s.columnType(2));   // low is null.
}
void dbpartest::create_2()
{
    // Can't make in bad save set:
    
    CPPUNIT_ASSERT_THROW(
        SpecTclDB::DBParameter::create(*m_pConn, 2, "name", 1),
        std::invalid_argument
    );
   
}
void dbpartest::create_3()
{
    // Can't make duplicate in valid saveset:
    
    delete SpecTclDB::DBParameter::create(*m_pConn, 1, "name", 1);
    CPPUNIT_ASSERT_THROW(
        SpecTclDB::DBParameter::create(*m_pConn, 1, "name", 1),
        std::invalid_argument
    );
}
void dbpartest::create_4()
{
    // Can create non duplicate parameter:
    
    delete SpecTclDB::DBParameter::create(*m_pConn, 1, "name", 1);
    CPPUNIT_ASSERT_NO_THROW(
        delete SpecTclDB::DBParameter::create(*m_pConn, 1, "new", 2)
    );
}
void dbpartest::create_5()
{
    // can't make duplicate parameter number.
    
    delete SpecTclDB::DBParameter::create(*m_pConn, 1, "name", 1);
    CPPUNIT_ASSERT_THROW(
        SpecTclDB::DBParameter::create(*m_pConn, 1, "New", 1),
        std::invalid_argument
    );
    
}
void dbpartest::create_6()
{
    // Create with full metadata. note that we believe the edge cases
    // will work because we factored them into createCheckOk.
    
    SpecTclDB::DBParameter* p;
    CPPUNIT_ASSERT_NO_THROW(
        p = SpecTclDB::DBParameter::create(
            *m_pConn, 1, "Name", 2, -10.0, 10.0, 100, "inches"
        )
    );
    // Check the info struct:
    
    auto i = p->getInfo();
    EQ(1, i.s_id);
    EQ(1, i.s_savesetId);
    EQ(std::string("Name"), i.s_name);
    EQ(2, i.s_number);
    EQ(true, i.s_haveMetadata);
    EQ(-10.0, i.s_low);
    EQ(10.0, i.s_high);
    EQ(100, i.s_bins);
    EQ(std::string("inches"), i.s_units);
    
    delete p;
    
    // Check the database:
    //
    CSqliteStatement s(
        *m_pConn,
        "SELECT id,number,low,high,bins,units FROM parameter_defs \
            WHERE save_id = 1 AND name = 'Name'"

    );
    ++s;
    EQ(1, s.getInt(0));
    EQ(2, s.getInt(1));
    EQ(-10.0, s.getDouble(2));
    EQ(10.0, s.getDouble(3));
    EQ(100, s.getInt(4));
    EQ(
        std::string("inches"),
        std::string(reinterpret_cast<const char*>(s.getText(5)))
    );

}

void dbpartest::construct_1()
{
    // good retrieval.
    
    delete SpecTclDB::DBParameter::create(*m_pConn, 1, "test", 2);
    SpecTclDB::DBParameter* p;
    CPPUNIT_ASSERT_NO_THROW(
        p = new SpecTclDB::DBParameter(*m_pConn, 1, "test")
    );
    // Check the info.
    
    auto& i = p->getInfo();
    EQ(1, i.s_id);
    EQ(1, i.s_savesetId);
    EQ(std::string("test"), i.s_name);
    EQ(2, i.s_number);
    EQ(false, i.s_haveMetadata);
    
    delete p;
}
void dbpartest::construct_2()
{
    // No such parameter exception.
    
    delete SpecTclDB::DBParameter::create(*m_pConn, 1, "test", 2);
    CPPUNIT_ASSERT_THROW(
        new SpecTclDB::DBParameter(*m_pConn, 1, "testing"),
        std::invalid_argument
    );
}
void dbpartest::construct_3()
{
    // Parameter is in another save set.
    
    delete SpecTclDB::DBParameter::create(*m_pConn, 1, "test", 2);
    CPPUNIT_ASSERT_THROW(
        new SpecTclDB::DBParameter(*m_pConn, 2, "test"),
        std::invalid_argument
    );
    
}
void dbpartest::construct_4()
{
    // fetch parameter with metadata.
    
    delete SpecTclDB::DBParameter::create(
        *m_pConn, 1, "test", 2,
        -100.0, 100.0, 400, "cm"
    );
    SpecTclDB::DBParameter p(*m_pConn, 1, "test");
    auto& i = p.getInfo();
    
    ASSERT(i.s_haveMetadata);
    EQ(-100.0, i.s_low);
    EQ(100.0, i.s_high);
    EQ(400, i.s_bins);
    EQ(std::string("cm"), i.s_units);
}
void dbpartest::construct_5()
{
    // successful construct by parameter number.
    
    delete SpecTclDB::DBParameter::create(
        *m_pConn, 1, "test", 2,
        -100.0, 100.0, 400, "cm"
    );
    SpecTclDB::DBParameter* p;
    CPPUNIT_ASSERT_NO_THROW(
        p = new SpecTclDB::DBParameter(*m_pConn, 1, 2)
    );
    // If we get the name correct in the info block I believe all because
    // that block is filled with common code.
    
    auto& i = p->getInfo();
    EQ(std::string("test"), i.s_name);
    
    delete p;
}
void dbpartest::construct_6()
{
    delete SpecTclDB::DBParameter::create(
        *m_pConn, 1, "test", 2,
        -100.0, 100.0, 400, "cm"
    );
    CPPUNIT_ASSERT_THROW(
        new SpecTclDB::DBParameter(*m_pConn, 1, 1),
        std::invalid_argument
    );
}
void dbpartest::list_1()
{
    // Emtpy list at first:
    
    auto pars = SpecTclDB::DBParameter::list(*m_pConn, 1);
    EQ(size_t(0), pars.size());
}
void dbpartest::list_2()
{
    // Make some.. .should see them in the list.
    
    const char* names[] = {
        "parameter1",
        "another1",
        "the.last.one",
        nullptr
    };
    std::set<std::string> expectedNames;
    int i =0;
    const char** p = names;
    while (*p) {
        delete SpecTclDB::DBParameter::create(*m_pConn, 1, *p, i);
        expectedNames.insert(*p);
        p++;
        i++;
    }
    // Get the list and see that all our names are there.
    
    auto v = SpecTclDB::DBParameter::list(*m_pConn, 1);
    EQ(expectedNames.size(), v.size());
    for (int i =0; i < v.size(); i++) {
        EQ(size_t(1), expectedNames.count(v[i]->getInfo().s_name));
        delete v[i];              // Done with it.
    }    
}
void dbpartest::list_3()
{
    // It's an error to list an nonexisting save set:
    
    CPPUNIT_ASSERT_THROW(
        SpecTclDB::DBParameter::list(*m_pConn, 2),
        std::invalid_argument
    );
}


void dbpartest::get_1() {
    // illegal save set throws.
    // Make some..
    
    const char* names[] = {
        "parameter1",
        "another1",
        "the.last.one",
        nullptr
    };
   
    int i =0;
    const char** p = names;
    while (*p) {
        delete SpecTclDB::DBParameter::create(*m_pConn, 1, *p, i);
        p++;
        i++;
    }    
    CPPUNIT_ASSERT_THROW(
        SpecTclDB::DBParameter::get(*m_pConn, 2, 1),
        std::invalid_argument
    );
    
}
void dbpartest::get_2() {
    // No such parameter throws.
    // illegal save set throws.
    // Make some..
    
    const char* names[] = {
        "parameter1",
        "another1",
        "the.last.one",
        nullptr
    };
   
    int i =0;
    const char** p = names;
    while (*p) {
        delete SpecTclDB::DBParameter::create(*m_pConn, 1, *p, i);
        p++;
        i++;
    }    
    CPPUNIT_ASSERT_THROW(
        SpecTclDB::DBParameter::get(*m_pConn, 1, 16),
        std::invalid_argument
    );

}
void dbpartest::get_3() {
    // good get.

    // illegal save set throws.
    // Make some..
    
    const char* names[] = {
        "parameter1",
        "another1",
        "the.last.one",
        nullptr
    };
   
    int i =0;
    const char** p = names;
    while (*p) {
        delete SpecTclDB::DBParameter::create(*m_pConn, 1, *p, i);
        p++;
        i++;
    }
    SpecTclDB::DBParameter* param;
    CPPUNIT_ASSERT_NO_THROW(param = SpecTclDB::DBParameter::get(*m_pConn, 1, 1));
    
    EQ(std::string("parameter1"), param->getInfo().s_name);
    
    delete param;
}

void dbpartest::save_1()
{
    // Save set wrapping of DBParameter::list.
    
    delete SpecTclDB::DBParameter::create(*m_pConn, 1, "test", 234);
    auto v = m_pSet->listParameters();
    EQ(size_t(1), v.size());
    EQ(std::string("test"), v[0]->getInfo().s_name);
    delete v[0];
}
void dbpartest::save_2()
{
    // make simple parameter
    
    delete m_pSet->createParameter("pname", 12);
    auto v = m_pSet->listParameters();
    EQ(size_t(1), v.size());
    EQ(std::string("pname"), v[0]->getInfo().s_name);
    EQ(12, v[0]->getInfo().s_number);
    delete v[0];
}

void dbpartest::save_3()
{
    // make param with metadata:
    
    delete m_pSet->createParameter(
        "pname", 34, -10.0, 10.0, 125, "cm"
    );
    auto v = m_pSet->listParameters();
    EQ(size_t(1), v.size());
    EQ(std::string("pname"), v[0]->getInfo().s_name);
    EQ(true, v[0]->getInfo().s_haveMetadata);
    delete v[0];
}
void dbpartest::save_4()
{
    // retrieve parameter given name.
    
    delete m_pSet->createParameter(
        "pname", 34, -10.0, 10.0, 125, "cm"
    );
    auto p = m_pSet->findParameter("pname");
    EQ(std::string("pname"), p->getInfo().s_name);
    delete p;
}
void dbpartest::save_5()
{
    // retrieve parameter given number:
    
    delete m_pSet->createParameter(
        "pname", 34, -10.0, 10.0, 125, "cm"
    );
    auto p = m_pSet->findParameter(34);
    EQ(std::string("pname"), p->getInfo().s_name);
    delete p;
}
void dbpartest::save_6()
{
    // retrieve parameter given id.
    
    delete m_pSet->createParameter(
        "pname", 34, -10.0, 10.0, 125, "cm"
    );
    auto p = m_pSet->getParameter(1);
    EQ(std::string("pname"), p->getInfo().s_name);
    delete p;
}