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
#include "CSqliteTransaction.h"

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
    
    CPPUNIT_TEST(create1_1);    // Create 1d gatex.
    CPPUNIT_TEST(create1_2);
    CPPUNIT_TEST(create1_3);
    CPPUNIT_TEST(create1_4);
    CPPUNIT_TEST(create1_5);
    CPPUNIT_TEST(create1_6);
    CPPUNIT_TEST(create1_7);
    CPPUNIT_TEST(create1_8);
    CPPUNIT_TEST(create1_9);
    CPPUNIT_TEST_SUITE_END();

protected:
    void exists_1();
    void exists_2();
    void exists_3();
    
    void create1_1();
    void create1_2();
    void create1_3();
    void create1_4();
    void create1_5();
    void create1_6();
    void create1_7();
    void create1_8();
    void create1_9();
private:
    void makeSomeParams();
};

void dbgtest::makeSomeParams()
{
    CSqliteTransaction t(*m_pConn);      // faster in transactionl.
    for (int i =0; i < 100; i++) {
        std::stringstream name;
        name << "param." << i;
        delete SpecTcl::DBParameter::create(
            *m_pConn, m_pSaveset->getInfo().s_id, name.str().c_str(), i+1
        );
    }
    // commit when going out of scope.
}

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
void dbgtest::create1_1()
{
    // Successful create
    
    makeSomeParams();
    SpecTcl::DBGate::NameList params = {"param.0"};
    
    SpecTcl::DBGate* pGate;
    CPPUNIT_ASSERT_NO_THROW(
        pGate = SpecTcl::DBGate::create1dGate(
            *m_pConn, m_pSaveset->getInfo().s_id, "test", "s",
            params, 100.0, 200.0
        )
    );
    delete pGate;
}
void dbgtest::create1_2()
{
    // Created gate has correct info.
    
    // Successful create
    
    makeSomeParams();
    SpecTcl::DBGate::NameList params = {"param.0"};
    
    SpecTcl::DBGate* pGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id, "test", "s",
        params, 100.0, 200.0
    );
    auto& info = pGate->getInfo();
    EQ(1, info.s_info.s_id);
    EQ(m_pSaveset->getInfo().s_id, info.s_info.s_saveset);
    EQ(std::string("test"), info.s_info.s_name);
    EQ(std::string("s"), info.s_info.s_type);
    EQ(SpecTcl::DBGate::point, info.s_info.s_basictype);
    
    auto pParam = m_pSaveset->findParameter("param.0");
    EQ(size_t(1), info.s_parameters.size());
    EQ(pParam->getInfo().s_id, info.s_parameters[0]);
    
    EQ(size_t(0), info.s_gates.size());
    
    EQ(size_t(2), info.s_points.size());
    EQ(100.0, info.s_points[0].s_x);
    EQ(200.0, info.s_points[1].s_x);
    
    delete pParam;
    delete pGate;
}
void dbgtest::create1_3()
{
    // Created gate has correct root record.
    
    makeSomeParams();
    SpecTcl::DBGate::NameList params = {"param.0"};
    
    SpecTcl::DBGate* pGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id, "test", "s",
        params, 100.0, 200.0
    );
    
    CSqliteStatement fetch(
        *m_pConn,
        "SELECT id, type FROM gate_defs WHERE saveset_id = ? AND name = ?"
    );
    fetch.bind(1, m_pSaveset->getInfo().s_id);
    fetch.bind(2, "test", -1, SQLITE_STATIC);
    ++fetch;
    EQ(false, fetch.atEnd());
    
    EQ(1, fetch.getInt(0));
    EQ(
       std::string("s"),
       std::string(reinterpret_cast<const char*>(fetch.getText(1)))
    );
    
    ++fetch;
    EQ(true, fetch.atEnd());
    delete pGate;
}
void dbgtest::create1_4()
{
    // Created gate has correct parameter record for s
    
    makeSomeParams();
    SpecTcl::DBGate::NameList params = {"param.0"};
    
    SpecTcl::DBGate* pGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id, "test", "s",
        params, 100.0, 200.0
    );
    
    auto pParam = m_pSaveset->findParameter("param.0");
    
    CSqliteStatement fetch(
        *m_pConn,
        "SELECT parameter_id FROM gate_parameters WHERE parent_gate=?"
    );
    fetch.bind(1, pGate->getInfo().s_info.s_id);
    ++fetch;
    
    EQ(false, fetch.atEnd());
    
    EQ(pParam->getInfo().s_id, fetch.getInt(0));
    
    ++fetch;
    EQ(true, fetch.atEnd());
    
    delete pParam;
    delete pGate;
    
}
void dbgtest::create1_5()
{
     // created gate gate has correct parameter record for gs...
     // they can have an arbitrary number of parameters:
     
    makeSomeParams();
    SpecTcl::DBGate::NameList params = {"param.0","param.2", "param.5", "param.6"};;
    
    SpecTcl::DBGate* pGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id, "test", "gs",
        params, 100.0, 200.0
    );

    CSqliteStatement fetch(
        *m_pConn,
        "SELECT parameter_id FROM gate_parameters WHERE parent_gate=? \
            ORDER BY id ASC"
    );
    fetch.bind(1, pGate->getInfo().s_info.s_id);
    for (int i = 0; i < params.size(); i++) {
        auto p = m_pSaveset->findParameter(params[i]);
        ++fetch;
        EQ(false, fetch.atEnd());
        EQ(p->getInfo().s_id, fetch.getInt(0));
        delete p;
    }
    ++fetch;
    EQ(true, fetch.atEnd());
    
    delete pGate;

}
void dbgtest::create1_6()
{
    // created gate has correct limits in gate_points.
    
    makeSomeParams();
    SpecTcl::DBGate::NameList params = {"param.0","param.2", "param.5", "param.6"};;
    
    SpecTcl::DBGate* pGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id, "test", "gs",
        params, 100.0, 200.0
    );
    
    CSqliteStatement fetch(
        *m_pConn,
        "SELECT x FROM gate_points WHERE gate_id = ? ORDER BY id ASC"
    );
    fetch.bind(1, pGate->getInfo().s_info.s_id);
    
    ++fetch;
    EQ(false, fetch.atEnd());
    EQ(100.0, fetch.getDouble(0));
    
    ++fetch;
    EQ(false, fetch.atEnd());
    EQ(200.0, fetch.getDouble(0));
    
    ++fetch;
    EQ(true, fetch.atEnd());
    
    delete pGate;
}
void dbgtest::create1_7()
{
    // invalid saveset.
    
    makeSomeParams();
    SpecTcl::DBGate::NameList params = {"param.0","param.2", "param.5", "param.6"};;
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBGate::create1dGate(
            *m_pConn, m_pSaveset->getInfo().s_id+1, "test", "gs",
            params, 100.0, 200.0),
        std::invalid_argument
    );
}
void dbgtest::create1_8()
{
    // duplicate name
    
    makeSomeParams();
    SpecTcl::DBGate::NameList params = {"param.0","param.2", "param.5", "param.6"};;
    
    SpecTcl::DBGate* pGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id, "test", "gs",
        params, 100.0, 200.0
    );
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBGate::create1dGate(
            *m_pConn, m_pSaveset->getInfo().s_id, "test", "gs",
            params, 100.0, 200.0
        ),
        std::invalid_argument
    );
    
    
    delete pGate;
}
void dbgtest::create1_9()
{
    // bad type
    
    makeSomeParams();
    SpecTcl::DBGate::NameList params = {"param.0","param.2", "param.5", "param.6"};;
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBGate::create1dGate(
            *m_pConn, m_pSaveset->getInfo().s_id, "test", "gb",
            params, 100.0, 200.0),
        std::invalid_argument
    );
}