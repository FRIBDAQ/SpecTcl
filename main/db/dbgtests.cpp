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

#include "SaveSet.h"
#define private public
#include "SpecTclDatabase.h"
#include "DBGate.h"
#undef private

#include "DBParameter.h"
#include "CSqlite.h"
#include "CSqliteStatement.h"

#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <stdexcept>
#include <errno.h>

class dbgtest : public CppUnit::TestFixture {
    
private:
    std::string        m_name;
    SpecTcl::CDatabase* m_pDb;
    SpecTcl::SaveSet*  m_pSaveset;
    CSqlite*           m_pConn;
public:
    void setUp() {
        // Make a temp file, database and saveset:
        
        const char* nameTemplate="dbgtestsXXXXXX";
        char name[100];
        strcpy(name, nameTemplate);
        int fd = mkstemp(name);
        if (fd < 0) {
            int e = errno;
            std::stringstream msg;
            msg << "Unable to make temp file from " <<nameTemplate
                << " : " << strerror(e);
            throw std::logic_error(msg.str());
        }
        close(fd);
        m_name = name;
        SpecTcl::CDatabase::create(name);
        m_pDb = new SpecTcl::CDatabase(name);
        m_pSaveset =  m_pDb->createSaveSet("my-saveset");
        m_pConn      =  &m_pDb->m_connection;
    }
    void tearDown() {
        delete m_pSaveset;
        delete m_pDb;
    }
private:
    CPPUNIT_TEST_SUITE(dbgtest);
    CPPUNIT_TEST(exists_1);
    CPPUNIT_TEST(exists_2);
    CPPUNIT_TEST(exists_3);
    CPPUNIT_TEST_SUITE_END();

protected:
    void exists_1();
    void exists_2();
    void exists_3();
};

CPPUNIT_TEST_SUITE_REGISTRATION(dbgtest);

void dbgtest::exists_1()
{
    // Exists initially false:
    
    EQ(
        false,
        SpecTcl::DBGate::exists(*m_pConn, m_pSaveset->getInfo().s_id, "junk")
    );
}
void dbgtest::exists_2()
{
    // If I create a minimal gate, exists says it exists:
    
    CSqliteStatement ins(
        *m_pConn,
        "INSERT INTO gate_defs (saveset_id, name, type)     \
            VALUES (?, 'junk', 'F')"
    );
    ins.bind(1, m_pSaveset->getInfo().s_id);
    ++ins;
    
    EQ(
        true,
        SpecTcl::DBGate::exists(*m_pConn, m_pSaveset->getInfo().s_id, "junk")
    );
}
void dbgtest::exists_3()
{
    // If i use a bad save set, exists throws:
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBGate::exists(*m_pConn, m_pSaveset->getInfo().s_id + 1, "junk"),
        std::invalid_argument
    );
}