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
 *  @brief: Test SpecTcl::DBParameter
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

class dbpartest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(dbpartest);
    CPPUNIT_TEST(exists_1);
    CPPUNIT_TEST(exists_2);
    
    CPPUNIT_TEST(create_1);
    CPPUNIT_TEST(create_2);
    CPPUNIT_TEST(create_3);
    CPPUNIT_TEST(create_4);
    CPPUNIT_TEST(create_5);
    CPPUNIT_TEST_SUITE_END();
    
private:
    std::string         m_dbfile;
    SpecTcl::CDatabase* m_pDb;
    SpecTcl::SaveSet*   m_pSet;
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
        SpecTcl::CDatabase::create(fname);
        m_pDb = new SpecTcl::CDatabase(fname);
        m_pSet = m_pDb->createSaveSet("set1");
        m_pConn = new CSqlite(m_dbfile.c_str());
    }
    void tearDown() {
        delete m_pSet;
        delete m_pDb;
        delete m_pConn;
        unlink(m_dbfile.c_str());
    }
protected:
    void exists_1();
    void exists_2();
    
    void create_1();
    void create_2();
    void create_3();
    void create_4();
    void create_5();
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
    
    ASSERT(!SpecTcl::DBParameter::exists(*m_pConn, 1, "atest"));
}
void dbpartest::exists_2()
{
    // Can find the one and only parameter:
    
    makeMinimalParameter(1, "atest", 1);
    ASSERT(SpecTcl::DBParameter::exists(*m_pConn, 1, "atest"));
}
void dbpartest::create_1()
{
    // Can create a simple, in an existing save set.
    
    SpecTcl::DBParameter* p(nullptr);
    CPPUNIT_ASSERT_NO_THROW(
        p = SpecTcl::DBParameter::create(*m_pConn, 1, "atest", 1)
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
        SpecTcl::DBParameter::create(*m_pConn, 2, "name", 1),
        std::invalid_argument
    );
   
}
void dbpartest::create_3()
{
    // Can't make duplicate in valid saveset:
    
    delete SpecTcl::DBParameter::create(*m_pConn, 1, "name", 1);
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBParameter::create(*m_pConn, 1, "name", 1),
        std::invalid_argument
    );
}
void dbpartest::create_4()
{
    // Can create non duplicate parameter:
    
    delete SpecTcl::DBParameter::create(*m_pConn, 1, "name", 1);
    CPPUNIT_ASSERT_NO_THROW(
        delete SpecTcl::DBParameter::create(*m_pConn, 1, "new", 2)
    );
}
void dbpartest::create_5()
{
    // can't make duplicate parameter number.
    
    delete SpecTcl::DBParameter::create(*m_pConn, 1, "name", 1);
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBParameter::create(*m_pConn, 1, "New", 1),
        std::invalid_argument
    );
    
}