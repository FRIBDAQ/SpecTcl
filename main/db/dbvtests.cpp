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

/** @file:  dbtvtests.cpp
 *  @brief: Test DBTreeVariable
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "SpecTclDatabase.h"
#include "DBTreeVariable.h"
#include "SaveSet.h"
#include "CSqlite.h"
#include "CSqliteStatement.h"

#include <string>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdexcept>
#include <sstream>

class dbvtest : public CppUnit::TestFixture {
    
    
private:
    std::string       m_filename;
    CSqlite*          m_pConn;
    SpecTcl::CDatabase* m_pdb;
    SpecTcl::SaveSet* m_pSaveset;
public:
    void setUp() {
        // Make db file as a temp:
        
        const char* fnameTemplate="dbvtestXXXXXX";
        char fname[200];
        strcpy(fname, fnameTemplate);
        int fd = mkstemp(fname);
        if  (fd < 0) {
            int e = errno;
            std::stringstream msg;
            msg << "Unable to make temp file: " << fname
                << " : " << strerror(e);
            throw std::logic_error(msg.str());
        }
        close(fd);
        m_filename = fname;
        
        SpecTcl::CDatabase::create(fname);
        m_pConn = new CSqlite(fname);
        m_pdb   = new SpecTcl::CDatabase(fname);
        m_pSaveset = m_pdb->createSaveSet("save-set");
    }
    void tearDown() {
        delete m_pSaveset;
        delete m_pdb;
        delete m_pConn;
        unlink(m_filename.c_str());
    }
private:
    CPPUNIT_TEST_SUITE(dbvtest);
    CPPUNIT_TEST(exists_1);
    CPPUNIT_TEST(exists_2);
    CPPUNIT_TEST(exists_3);
    
    CPPUNIT_TEST(create_1);
    CPPUNIT_TEST(create_2);
    CPPUNIT_TEST(create_3);
    CPPUNIT_TEST(create_4);
    CPPUNIT_TEST(create_5);
    
    CPPUNIT_TEST(list_1);
    CPPUNIT_TEST(list_2);
    CPPUNIT_TEST(list_3);
    
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    CPPUNIT_TEST(construct_4);

    CPPUNIT_TEST_SUITE_END();
protected:
    void exists_1();
    void exists_2();
    void exists_3();
    
    void create_1();
    void create_2();
    void create_3();
    void create_4();
    void create_5();
    
    void list_1();
    void list_2();
    void list_3();
    
    void construct_1();
    void construct_2();
    void construct_3();
    void construct_4();
};

CPPUNIT_TEST_SUITE_REGISTRATION(dbvtest);

void dbvtest::exists_1()
{
    // No such tree variable returns false.
    
    EQ(
        false,
        SpecTcl::DBTreeVariable::exists(
            *m_pConn, m_pSaveset->getInfo().s_id, "Junk"
        )
    );
}
void dbvtest::exists_2()
{
    // NO such saveset throws std::invalid_argument:
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBTreeVariable::exists(
            *m_pConn, m_pSaveset->getInfo().s_id+1, "Junk"
        ),
        std::invalid_argument
    );
}
void dbvtest::exists_3()
{
    // Parameter exists -> true
    
    CSqliteStatement put(
        *m_pConn,
        "INSERT INTO treevariables (save_id, name, value, units)    \
            VALUES (?,'pi', 3.1416, 'none')"
    );
    put.bind(1, m_pSaveset->getInfo().s_id);
    ++put;
    
    EQ(
        true,
        SpecTcl::DBTreeVariable::exists(
            *m_pConn, m_pSaveset->getInfo().s_id, "pi"
        )
    );
}

void dbvtest::create_1()
{
    // Good insert does not throw.
    
    SpecTcl::DBTreeVariable* p;
    CPPUNIT_ASSERT_NO_THROW(
        p = SpecTcl::DBTreeVariable::create(
            *m_pConn, m_pSaveset->getInfo().s_id,
            "pi", 3.14159, "unitless"
        )
    );
    delete p;
}
void dbvtest::create_2()
{
    // Good insert gives proper info values.
    
    SpecTcl::DBTreeVariable* p;
    CPPUNIT_ASSERT_NO_THROW(
        p = SpecTcl::DBTreeVariable::create(
            *m_pConn, m_pSaveset->getInfo().s_id,
            "pi", 3.14159, "unitless"
        )
    );
    auto info = p->getInfo();
    EQ(1, info.s_id );
    EQ(m_pSaveset->getInfo().s_id, info.s_saveset);
    EQ(std::string("pi"), info.s_name);
    EQ(3.14159, info.s_value);
    EQ(std::string("unitless"), info.s_units);
    
    delete p;
}
void dbvtest::create_3()
{
    // Good insert properly inserts into database.
    
    SpecTcl::DBTreeVariable* p;
    CPPUNIT_ASSERT_NO_THROW(
        p = SpecTcl::DBTreeVariable::create(
            *m_pConn, m_pSaveset->getInfo().s_id,
            "pi", 3.14159, "unitless"
        )
    );
    
    CSqliteStatement r(
        *m_pConn,
        "SELECT id, value, units FROM treevariables WHERE save_id=? AND name = ?"
    );
    r.bind(1, m_pSaveset->getInfo().s_id);
    r.bind(2, "pi", -1, SQLITE_STATIC);
    ++r;
    EQ(false, r.atEnd());
    
    EQ(1, r.getInt(0));
    EQ(3.14159, r.getDouble(1));
    std::string units = reinterpret_cast<const char*>(r.getText(2));
    EQ(std::string("unitless"), units);
    
    ++r;
    EQ(true, r.atEnd());
    
    delete p;
}
void dbvtest::create_4()
{
    // Insert into no such saveset throws.
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBTreeVariable::create(
            *m_pConn, m_pSaveset->getInfo().s_id+1, "pi", 3.1416
        ), std::invalid_argument
    );
}
void dbvtest::create_5()
{
    // Duplicate name throws.
    
    delete SpecTcl::DBTreeVariable::create(
        *m_pConn, m_pSaveset->getInfo().s_id, "pi", 3.14159, "unitless"
    );
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBTreeVariable::create(
            *m_pConn, m_pSaveset->getInfo().s_id, "pi", 3.14159, "unitless"
        ), std::invalid_argument
    );
}

void dbvtest::list_1()
{
    // Initially empty.
    
    EQ(
        size_t(0),
        SpecTcl::DBTreeVariable::list(*m_pConn, m_pSaveset->getInfo().s_id).size()
    );
}
void dbvtest::list_2()
{
    // Return existing variables.
    
    // Enter some:
    
    for (int i =0; i < 10; i++) {
        std::stringstream name;
        std::stringstream units;
        name << "Variable." << i;
        units << "Units." << i;
        
        std::string n = name.str();
        std::string u = units.str();
        
        delete SpecTcl::DBTreeVariable::create(
            *m_pConn, m_pSaveset->getInfo().s_id, n.c_str(),
            double(i), u.c_str()
        );
    }
    auto listing = SpecTcl::DBTreeVariable::list(
        *m_pConn, m_pSaveset->getInfo().s_id
    );
    EQ(size_t(10), listing.size());
    
    for (int i =0; i < 10; i++) {
        std::stringstream name;
        std::stringstream units;
        name << "Variable." << i;
        units << "Units." << i;
        
        std::string n = name.str();
        std::string u = units.str();
        
        auto& info = listing[i]->getInfo();
        
        EQ(i+1, info.s_id);
        EQ(m_pSaveset->getInfo().s_id, info.s_saveset);
        EQ(n, info.s_name);
        EQ(double(i), info.s_value);
        EQ(u, info.s_units);
    }
    
}
void dbvtest::list_3()
{
    // bad saveset throws.
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBTreeVariable::list(*m_pConn, m_pSaveset->getInfo().s_id+1),
        std::invalid_argument
    );
}

void dbvtest::construct_1()
{
    // Good lookup.
    
    delete SpecTcl::DBTreeVariable::create(
        *m_pConn, m_pSaveset->getInfo().s_id, "test", 1.234, "inches"
    );
    
    CPPUNIT_ASSERT_NO_THROW(
        SpecTcl::DBTreeVariable v(*m_pConn, m_pSaveset->getInfo().s_id, "test")
    );
}
void dbvtest::construct_2()
{
    // Good info from the lookup.
    delete SpecTcl::DBTreeVariable::create(
        *m_pConn, m_pSaveset->getInfo().s_id, "test", 1.234, "inches"
    );
    
    
    SpecTcl::DBTreeVariable v(*m_pConn, m_pSaveset->getInfo().s_id, "test");
    
    auto& info = v.getInfo();
    EQ(1, info.s_id);
    EQ(m_pSaveset->getInfo().s_id, info.s_saveset);
    EQ(std::string("test"), info.s_name);
    EQ(1.234, info.s_value);
    EQ(std::string("inches"), info.s_units);
}
void dbvtest::construct_3()
{
    // bad saveset
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBTreeVariable v(
            *m_pConn, m_pSaveset->getInfo().s_id + 1, "test"
        ), std::invalid_argument
    );
}
void dbvtest::construct_4()
{
    // No such variable.
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBTreeVariable v(
            *m_pConn, m_pSaveset->getInfo().s_id , "test"
        ), std::invalid_argument
    );
}