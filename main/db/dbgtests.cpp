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
        unlink(m_name.c_str());
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
    
    CPPUNIT_TEST(create2_1);    // Create 2d gates.
    CPPUNIT_TEST(create2_2);
    CPPUNIT_TEST(create2_3);
    
    CPPUNIT_TEST(createc_1);   // createCompoundGate.
    CPPUNIT_TEST(createc_2);
    CPPUNIT_TEST(createc_3);
    CPPUNIT_TEST(createc_4);
    CPPUNIT_TEST(createc_5);
    CPPUNIT_TEST(createc_6);
    CPPUNIT_TEST(createc_7);
    
    CPPUNIT_TEST(createm_1);   // createMaskGate
    CPPUNIT_TEST(createm_2);
    CPPUNIT_TEST(createm_3);
    CPPUNIT_TEST(createm_4);
    CPPUNIT_TEST(createm_5);
    CPPUNIT_TEST(createm_6);
    CPPUNIT_TEST(createm_7);
    
    CPPUNIT_TEST(list_1);
    CPPUNIT_TEST(list_2);
    CPPUNIT_TEST(list_3);
    
    CPPUNIT_TEST(construct_1);  // Construct from name.
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    CPPUNIT_TEST(construct_4);
    CPPUNIT_TEST(construct_5);
    
    CPPUNIT_TEST(construct_6);  // Construct from id
    CPPUNIT_TEST(construct_7);
    
    CPPUNIT_TEST(getpars_1);
    CPPUNIT_TEST(getpars_2);
    CPPUNIT_TEST(getpars_3);
    
    CPPUNIT_TEST(getgates_1);
    CPPUNIT_TEST(getgates_2);
    CPPUNIT_TEST(getgates_3);
    
    CPPUNIT_TEST(getpts_1);
    CPPUNIT_TEST(getpts_2);
    CPPUNIT_TEST(getpts_3);
    
    CPPUNIT_TEST(getmask_1);
    CPPUNIT_TEST(getmask_2);
    CPPUNIT_TEST(getmask_3);
    
    CPPUNIT_TEST(save_1);    // Save set API tests.
    CPPUNIT_TEST(save_2);
    CPPUNIT_TEST(save_3);
    CPPUNIT_TEST(save_4);
    CPPUNIT_TEST(save_5);
    CPPUNIT_TEST(save_6);
    CPPUNIT_TEST(save_7);
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
    
    void create2_1();
    void create2_2();
    void create2_3();
    
    void createc_1();
    void createc_2();
    void createc_3();
    void createc_4();
    void createc_5();
    void createc_6();
    void createc_7();
    
    void createm_1();
    void createm_2();
    void createm_3();
    void createm_4();
    void createm_5();
    void createm_6();
    void createm_7();
    
    void list_1();
    void list_2();
    void list_3();
    
    void construct_1();
    void construct_2();
    void construct_3();
    void construct_4();
    void construct_5();
    void construct_6();
    void construct_7();
    
    void getpars_1();
    void getpars_2();
    void getpars_3();
    
    void getgates_1();
    void getgates_2();
    void getgates_3();
    
    void getpts_1();
    void getpts_2();
    void getpts_3();
    
    void getmask_1();
    void getmask_2();
    void getmask_3();
    
    void save_1();
    void save_2();
    void save_3();
    void save_4();
    void save_5();
    void save_6();
    void save_7();
private:
    void makeSomeParams();
    void makeSome1dGates();
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

void dbgtest::makeSome1dGates()
{
    makeSomeParams();
    SpecTcl::DBGate::NameList param = {"param.0"};
    for (int i =0; i < 10; i++) {
        std::stringstream name;
        name << "gate." << i;
        delete SpecTcl::DBGate::create1dGate(
            *m_pConn, m_pSaveset->getInfo().s_id, name.str().c_str(),
            "s", param, 10, 20
        );
    }
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
    SpecTcl::DBGate::NameList params = {"param.0","param.2", "param.5", "param.6"};
    
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
// 2d gates share most of their action code with 1d gates (createPOint Gate).
// the extensive testing of thta logic in the create1_n tests means our
// tests for 2d gates are simpler in scope:

void dbgtest::create2_1()
{
    // Good creation.
    
    makeSomeParams();
    SpecTcl::DBGate::NameList params = {"param.0","param.2", "param.5", "param.6"};
    SpecTcl::DBGate::Points pts = {
        {100.0, 100.0},
        {200.0, 100.0},
        {200.0, 200.0},
        {100.0, 200.0}
    };                            // Nice rectangle.
    
    SpecTcl::DBGate* pGate;
    CPPUNIT_ASSERT_NO_THROW(
        pGate = SpecTcl::DBGate::create2dGate(
            *m_pConn, m_pSaveset->getInfo().s_id, "test2", "gb",
            params, pts
        )
    );
    
    delete pGate;
}
void dbgtest::create2_2()
{
    // invalid gate type exception.
    
    makeSomeParams();
    SpecTcl::DBGate::NameList params = {"param.0","param.2", "param.5", "param.6"};
    SpecTcl::DBGate::Points pts = {
        {100.0, 100.0},
        {200.0, 100.0},
        {200.0, 200.0},
        {100.0, 200.0}     // Nice rectangle.
    };
    SpecTcl::DBGate* pGate;
   
    CPPUNIT_ASSERT_THROW(
        pGate = SpecTcl::DBGate::create2dGate(
            *m_pConn, m_pSaveset->getInfo().s_id, "test2", "T",
            params, pts
        ),
        std::invalid_argument
    );
}

void dbgtest::create2_3()
{
    // invalid parameter in the list throws.
}


void dbgtest::createc_1()
{
    // Good insert.
    
    makeSome1dGates();
    SpecTcl::DBGate::NameList components = {
        "gate.1", "gate.2", "gate.4"
    };
    SpecTcl::DBGate* pGate;
    CPPUNIT_ASSERT_NO_THROW(
        SpecTcl::DBGate::createCompoundGate(
            *m_pConn, m_pSaveset->getInfo().s_id,
            "orgate", "+", components
        )
    );
    
    delete pGate;
}
void dbgtest::createc_2()
{
    // check info
    
    makeSome1dGates();
    SpecTcl::DBGate::NameList components = {
        "gate.1", "gate.2", "gate.4"
    };
    SpecTcl::DBGate* pGate = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "orgate", "+", components
    );
    
    auto& info = pGate->getInfo();
    auto& base = info.s_info;
    
    EQ(11, base.s_id);             // makeSome1dGates made 10 gates.
    EQ(m_pSaveset->getInfo().s_id, base.s_saveset);
    EQ(std::string("orgate"), base.s_name);
    EQ(std::string("+"), base.s_type);
    EQ(SpecTcl::DBGate::compound, base.s_basictype);
    
    EQ(components.size(), info.s_gates.size());
    for (int i =0; i < components.size(); i++) {
        int gid = SpecTcl::DBGate::gateId(
            *m_pConn, m_pSaveset->getInfo().s_id, components[i]
        );
         EQ(gid, info.s_gates[i]);   
    }
    
    delete pGate;
}

void dbgtest::createc_3()
{
    // Check gates in table.
    
    // check info
    
    makeSome1dGates();
    SpecTcl::DBGate::NameList components = {
        "gate.1", "gate.2", "gate.4"
    };
    SpecTcl::DBGate* pGate = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "orgate", "+", components
    );
    
    CSqliteStatement fetch(
        *m_pConn,
        "SELECT child_gate FROM component_gates   \
            WHERE parent_gate = ?                 \
            ORDER BY id ASC"
    );
    fetch.bind(1, pGate->getInfo().s_info.s_id);
    for (int i = 0; i < components.size(); i++) {
        ++fetch;
        EQ(false, fetch.atEnd());
        int dbid = fetch.getInt(0);
        int gid  = SpecTcl::DBGate::gateId(
            *m_pConn, m_pSaveset->getInfo().s_id, components[i]
        );
        EQ(gid, dbid);
    }
    
    ++fetch;
    EQ(true, fetch.atEnd());
    delete pGate;
}
void dbgtest::createc_4()
{
    // Bad saveset.
    
    makeSome1dGates();
    SpecTcl::DBGate::NameList components = {
        "gate.1", "gate.2", "gate.4"
    };
    SpecTcl::DBGate* pGate;
    CPPUNIT_ASSERT_THROW(
        pGate = SpecTcl::DBGate::createCompoundGate(
            *m_pConn, m_pSaveset->getInfo().s_id + 1, 
            "orgate", "+", components
        ),
        std::invalid_argument
    );
}
void dbgtest::createc_5()
{
    // duplicate name.
    
    makeSome1dGates();
    SpecTcl::DBGate::NameList components = {
        "gate.1", "gate.2", "gate.4"
    };
    SpecTcl::DBGate* pGate = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "orgate", "+", components
    );
    
    CPPUNIT_ASSERT_THROW(
        pGate = SpecTcl::DBGate::createCompoundGate(
            *m_pConn, m_pSaveset->getInfo().s_id,
            "orgate", "+", components
        ),
        std::invalid_argument
    );
    
    delete pGate;
}
void dbgtest::createc_6()
{
    // bad type
    
    makeSome1dGates();
    SpecTcl::DBGate::NameList components = {
        "gate.1", "gate.2", "gate.4"
    };
    SpecTcl::DBGate* pGate;
    CPPUNIT_ASSERT_THROW(
        pGate = SpecTcl::DBGate::createCompoundGate(
            *m_pConn, m_pSaveset->getInfo().s_id,
            "orgate", "s", components
        ),
        std::invalid_argument
    );
}
void dbgtest::createc_7()
{
    // Bad dependent gate.
    
    makeSome1dGates();
    SpecTcl::DBGate::NameList components = {
        "gate.1", "ggate.2", "gate.4"
    };
    SpecTcl::DBGate* pGate;
    CPPUNIT_ASSERT_THROW(
        pGate = SpecTcl::DBGate::createCompoundGate(
            *m_pConn, m_pSaveset->getInfo().s_id,
            "orgate", "+", components
        ),
        std::invalid_argument
    );
}
void dbgtest::createm_1() {
    // Successful insert.
    
    makeSomeParams();
    SpecTcl::DBGate* pGate;
    CPPUNIT_ASSERT_NO_THROW(
        pGate = SpecTcl::DBGate::createMaskGate(
            *m_pConn, m_pSaveset->getInfo().s_id,
            "testgate", "em", "param.1", 0xaaaaaaaa
        )
    );
    
    delete pGate;
}
void dbgtest::createm_2() {
    // Info struct is correct.
    
    makeSomeParams();
    SpecTcl::DBGate* pGate =SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "testgate", "em", "param.1", 0xaaaaaaaa
    );
    
    auto& info = pGate->getInfo();
    auto& base = info.s_info;
    
    EQ(1, base.s_id);
    EQ(m_pSaveset->getInfo().s_id, base.s_saveset);
    EQ(std::string("testgate"), base.s_name);
    EQ(std::string("em"), base.s_type);
    EQ(SpecTcl::DBGate::mask, base.s_basictype);
    
    EQ(size_t(1), info.s_parameters.size());
    auto param = m_pSaveset->findParameter("param.1");
    EQ(param->getInfo().s_id, info.s_parameters.at(0));
    
    EQ(size_t(0), info.s_gates.size());
    EQ(size_t(0), info.s_points.size());
    EQ(int(0xaaaaaaaa), info.s_mask);
    
    delete param;
    delete pGate;
}
void dbgtest::createm_3() {
    // Root record is correct.
 
     makeSomeParams();
     SpecTcl::DBGate* pGate =SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "testgate", "em", "param.1", 0xaaaaaaaa
    );

    CSqliteStatement fet(
        *m_pConn,
        "SELECT saveset_id, type FROM gate_defs WHERE name = 'testgate'"
    );
    ++fet;
    EQ(false, fet.atEnd());
    
    EQ(m_pSaveset->getInfo().s_id, fet.getInt(0));
    EQ(
        std::string("em"),
        std::string(reinterpret_cast<const char*>(fet.getText(1)))
    );
    
    ++fet;
    EQ(true, fet.atEnd());
    
    delete pGate;
       
    
}
void dbgtest::createm_4() {
    // Parameters record(s) are correct.
    
     makeSomeParams();
     SpecTcl::DBGate* pGate =SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "testgate", "em", "param.1", 0xaaaaaaaa
    );
    auto param = m_pSaveset->findParameter("param.1");

    CSqliteStatement fet(
        *m_pConn,
        "SELECT parameter_id FROM gate_parameters WHERE parent_gate = ?"
    );
    fet.bind(1, pGate->getInfo().s_info.s_id);
    ++fet;
    EQ(false, fet.atEnd());
    
    EQ(param->getInfo().s_id, fet.getInt(0));
    
    ++fet;
    EQ(true, fet.atEnd());
    delete param;
    delete pGate;    
}
void dbgtest::createm_5() {
    // Mask record is correct.
    
    makeSomeParams();
    SpecTcl::DBGate* pGate =SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "testgate", "em", "param.1", 0xaaaaaaaa
    );
 
    CSqliteStatement fet(
        *m_pConn,
        "SELECT mask from gate_masks WHERE parent_gate = ?"
    );
    fet.bind(1, pGate->getInfo().s_info.s_id);
    ++fet;
    EQ(false, fet.atEnd());

    EQ(int(0xaaaaaaaa), fet.getInt(0));
    
    ++fet;
    EQ(true, fet.atEnd());

}
void dbgtest::createm_6() {
    // invalid type throws.
    
    makeSomeParams();
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBGate::createMaskGate(
            *m_pConn, m_pSaveset->getInfo().s_id,
            "testgate", "c", "param.1", 0xaaaaaaaa
        ),
        std::invalid_argument
    );
}
void dbgtest::createm_7() {
    // Duplicate name throws.
    makeSomeParams();
    SpecTcl::DBGate* pGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "testgate", "em", "param.1", 0xaaaaaaaa
    );
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBGate::createMaskGate(
            *m_pConn, m_pSaveset->getInfo().s_id,
            "testgate", "em", "param.1", 0xaaaaaaaa
        ),
        std::invalid_argument 
    );
    
    
    delete pGate;
}
void dbgtest::list_1()
{
    // Empty to begin with.
    
    auto result =
        SpecTcl::DBGate::listGates(*m_pConn, m_pSaveset->getInfo().s_id);
    EQ(size_t(0), result.size());
}
void dbgtest::list_2()
{
    // I can list gates and get the correct results.
    
    // Make one of each type of gate so we chan check out how
    // the info gets loaded for each
    
    SpecTcl::DBGate::NameList p1d = {"param.5"};
    makeSomeParams();
    auto ptsGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "1d", "s", p1d, 100.0, 200.
    );                                // id 1.
    auto maskGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "mask", "em", "param.3", 0x12345678
    );
    
    SpecTcl::DBGate::NameList gates = {"mask", "1d"};
    auto compound = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "compound", "+", gates
    );
    
    auto gatelist = SpecTcl::DBGate::listGates(*m_pConn, m_pSaveset->getInfo().s_id);
    
    EQ(size_t(3), gatelist.size());
    
    // The infos for our created gates should match the listed gates:
    
    // 0 - should be the points gate.
    
    auto& ptsInfo = ptsGate->getInfo();
    auto& info0   = gatelist[0]->getInfo();
    
    EQ(ptsInfo.s_info.s_id, info0.s_info.s_id);
    EQ(ptsInfo.s_info.s_saveset, info0.s_info.s_saveset);
    EQ(ptsInfo.s_info.s_name, info0.s_info.s_name);
    EQ(ptsInfo.s_info.s_type, info0.s_info.s_type);
    EQ(ptsInfo.s_info.s_basictype, info0.s_info.s_basictype);
    
    EQ(ptsInfo.s_parameters.size(), info0.s_parameters.size());
    EQ(ptsInfo.s_parameters[0], info0.s_parameters[0]);
    
    EQ(ptsInfo.s_points.size(), info0.s_points.size());
    EQ(ptsInfo.s_points[0].s_x, info0.s_points[0].s_x);
    EQ(ptsInfo.s_points[0].s_y, info0.s_points[0].s_y);
    EQ(ptsInfo.s_points[1].s_x, info0.s_points[1].s_x);
    EQ(ptsInfo.s_points[1].s_y, info0.s_points[1].s_y);
    
    // 1 should be the mask gate.
    
    auto& maskInfo = maskGate->getInfo();
    auto& info1    = gatelist[1]->getInfo();
    
    EQ(maskInfo.s_info.s_id, info1.s_info.s_id);
    EQ(maskInfo.s_info.s_saveset, info1.s_info.s_saveset);
    EQ(maskInfo.s_info.s_name, info1.s_info.s_name);
    EQ(maskInfo.s_info.s_type, info1.s_info.s_type);
    EQ(maskInfo.s_info.s_basictype, info1.s_info.s_basictype);
    
    EQ(maskInfo.s_parameters.size(), info1.s_parameters.size());
    EQ(maskInfo.s_parameters[0], info1.s_parameters[0]);
    
    EQ(maskInfo.s_mask, info1.s_mask);
    
    // 2 should be the compound gate:
    
    auto& cInfo = compound->getInfo();
    auto& info2 = gatelist[2]->getInfo();
    
    EQ(cInfo.s_info.s_id, info2.s_info.s_id);
    EQ(cInfo.s_info.s_saveset, info2.s_info.s_saveset);
    EQ(cInfo.s_info.s_name, info2.s_info.s_name);
    EQ(cInfo.s_info.s_type, info2.s_info.s_type);
    EQ(cInfo.s_info.s_basictype, info2.s_info.s_basictype);
    
    EQ(size_t(2), info2.s_gates.size());
    EQ(cInfo.s_gates[0], info2.s_gates[0]);
    EQ(cInfo.s_gates[1], info2.s_gates[1]);
    
    for (int i =0; i < gatelist.size(); i++) {
        delete gatelist[i];
    }
    delete compound;
    delete maskGate;
    delete ptsGate;
}
void dbgtest::list_3()
{
    // bad saveset throws:
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBGate::listGates(*m_pConn, m_pSaveset->getInfo().s_id+1),
        std::invalid_argument
    );
}
// Note that all the construction tests will make the
// same set of gates just so I can copy /paste.

void dbgtest::construct_1()
{
    // good retrieval. point gate.
    
    SpecTcl::DBGate::NameList p1d = {"param.5"};
    makeSomeParams();
    auto ptsGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "1d", "s", p1d, 100.0, 200.
    );                                // id 1.
    auto maskGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "mask", "em", "param.3", 0x12345678
    );
    
    SpecTcl::DBGate::NameList gates = {"mask", "1d"};
    auto compound = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "compound", "+", gates
    );
    
    SpecTcl::DBGate g(*m_pConn, m_pSaveset->getInfo().s_id, "1d");
    auto& expected = ptsGate->getInfo();
    auto& actual   = g.getInfo();
    
    // Base part.
    
    EQ(expected.s_info.s_id, actual.s_info.s_id);
    EQ(expected.s_info.s_saveset, actual.s_info.s_saveset);
    EQ(expected.s_info.s_name, actual.s_info.s_name);
    EQ(expected.s_info.s_type, actual.s_info.s_type);
    EQ(expected.s_info.s_basictype, actual.s_info.s_basictype);
    
    // Parameters.
    
    EQ(expected.s_parameters.size(), actual.s_parameters.size());
    EQ(expected.s_parameters[0], actual.s_parameters[0]);
    
    
    // There are no gates.
    
    EQ(size_t(0), actual.s_gates.size());
    
    // Points.
    
    EQ(expected.s_points.size(), actual.s_points.size());
    EQ(expected.s_points[0].s_x, actual.s_points[0].s_x);
    EQ(expected.s_points[0].s_y, actual.s_points[0].s_y);
    EQ(expected.s_points[1].s_x, actual.s_points[1].s_x);
    EQ(expected.s_points[1].s_y, actual.s_points[1].s_y);
    
    // Free storage.
    
    delete ptsGate;
    delete maskGate;
    delete compound;
}
void dbgtest::construct_2()
{
    // good retrieval compound gate.
    
    SpecTcl::DBGate::NameList p1d = {"param.5"};
    makeSomeParams();
    auto ptsGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "1d", "s", p1d, 100.0, 200.
    );                                // id 1.
    auto maskGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "mask", "em", "param.3", 0x12345678
    );
    
    SpecTcl::DBGate::NameList gates = {"mask", "1d"};
    auto compound = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "compound", "+", gates
    );
    
    SpecTcl::DBGate gate(*m_pConn, m_pSaveset->getInfo().s_id, "compound");
    
    auto& expected = compound->getInfo();
    auto& actual   = gate.getInfo();
    
    // Base.
    EQ(expected.s_info.s_id, actual.s_info.s_id);
    EQ(expected.s_info.s_saveset, actual.s_info.s_saveset);
    EQ(expected.s_info.s_name, actual.s_info.s_name);
    EQ(expected.s_info.s_type, actual.s_info.s_type);
    EQ(expected.s_info.s_basictype, actual.s_info.s_basictype);
        
    // no parameters.
    
    EQ(size_t(0), actual.s_parameters.size());
    
    // Gates
    
    EQ(expected.s_gates.size(), actual.s_gates.size());
    for (int i = 0; i < expected.s_gates.size(); i++) {
        EQ(expected.s_gates[i], actual.s_gates[i]);
    }

    // No points.
    
    EQ(size_t(0), actual.s_points.size());
       
    delete ptsGate;
    delete maskGate;
    delete compound;
}
void dbgtest::construct_3()
{
    // good retrieval mask gate.
    
    SpecTcl::DBGate::NameList p1d = {"param.5"};
    makeSomeParams();
    auto ptsGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "1d", "s", p1d, 100.0, 200.
    );                                // id 1.
    auto maskGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "mask", "em", "param.3", 0x12345678
    );
    
    SpecTcl::DBGate::NameList gates = {"mask", "1d"};
    auto compound = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "compound", "+", gates
    );
    
    
    SpecTcl::DBGate gate(*m_pConn, m_pSaveset->getInfo().s_id, "mask");
    auto& expected = maskGate->getInfo();
    auto& actual   = gate.getInfo();
    
    // Base.
    EQ(expected.s_info.s_id, actual.s_info.s_id);
    EQ(expected.s_info.s_saveset, actual.s_info.s_saveset);
    EQ(expected.s_info.s_name, actual.s_info.s_name);
    EQ(expected.s_info.s_type, actual.s_info.s_type);
    EQ(expected.s_info.s_basictype, actual.s_info.s_basictype);
        
    // no parameters.
    
    EQ(expected.s_parameters.size(), actual.s_parameters.size());
    EQ(expected.s_parameters[0], actual.s_parameters[0]);
    
    // Gates
   
   EQ(size_t(0), expected.s_gates.size());

    // No points.
    
    EQ(size_t(0), actual.s_points.size());
    
    // mask
    
    EQ(expected.s_mask, actual.s_mask);
    
    delete ptsGate;
    delete maskGate;
    delete compound;
}
void dbgtest::construct_4()
{
    // no such saveset.
    
    SpecTcl::DBGate::NameList p1d = {"param.5"};
    makeSomeParams();
    auto ptsGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "1d", "s", p1d, 100.0, 200.
    );                                // id 1.
    auto maskGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "mask", "em", "param.3", 0x12345678
    );
    
    SpecTcl::DBGate::NameList gates = {"mask", "1d"};
    auto compound = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "compound", "+", gates
    );
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBGate g(
            *m_pConn, m_pSaveset->getInfo().s_id+1, "compound"
        ),
        std::invalid_argument
    );
    
    delete ptsGate;
    delete maskGate;
    delete compound;
}
void dbgtest::construct_5()
{
    // No such gate.
    
    SpecTcl::DBGate::NameList p1d = {"param.5"};
    makeSomeParams();
    auto ptsGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "1d", "s", p1d, 100.0, 200.0
    );                                // id 1.
    auto maskGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "mask", "em", "param.3", 0x12345678
    );
    
    SpecTcl::DBGate::NameList gates = {"mask", "1d"};
    auto compound = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "compound", "+", gates
    );
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBGate gate(
            *m_pConn, m_pSaveset->getInfo().s_id, "nope"
        ),
        std::invalid_argument
    );
    
    delete ptsGate;
    delete maskGate;
    delete compound;
}

void dbgtest::construct_6()
{
    // good construct from id. NOte that we trust that
    // if we get one info loaded the other types will load
    // because it's common code.
    
    SpecTcl::DBGate::NameList p1d = {"param.5"};
    makeSomeParams();
    auto ptsGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "1d", "s", p1d, 100.0, 200.
    );                                // id 1.
    auto maskGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "mask", "em", "param.3", 0x12345678
    );
    
    SpecTcl::DBGate::NameList gates = {"mask", "1d"};
    auto compound = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "compound", "+", gates
    );
    
    SpecTcl::DBGate g(*m_pConn, compound->getInfo().s_info.s_id);
    
    auto& expected = compound->getInfo();
    auto& actual   = g.getInfo();
    
    // Base.
    EQ(expected.s_info.s_id, actual.s_info.s_id);
    EQ(expected.s_info.s_saveset, actual.s_info.s_saveset);
    EQ(expected.s_info.s_name, actual.s_info.s_name);
    EQ(expected.s_info.s_type, actual.s_info.s_type);
    EQ(expected.s_info.s_basictype, actual.s_info.s_basictype);
        
    // no parameters.
    
    EQ(size_t(0), actual.s_parameters.size());
    
    // Gates
    
    EQ(expected.s_gates.size(), actual.s_gates.size());
    for (int i = 0; i < expected.s_gates.size(); i++) {
        EQ(expected.s_gates[i], actual.s_gates[i]);
    }

    // No points.
    
    EQ(size_t(0), actual.s_points.size());
       
    delete ptsGate;
    delete maskGate;
    delete compound;
    
    
}
void dbgtest::construct_7()
{
    // Bad gate id.
    
    // good construct from id. NOte that we trust that
    // if we get one info loaded the other types will load
    // because it's common code.
    
    SpecTcl::DBGate::NameList p1d = {"param.5"};
    makeSomeParams();
    auto ptsGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "1d", "s", p1d, 100.0, 200.
    );                                // id 1.
    auto maskGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "mask", "em", "param.3", 0x12345678
    );
    
    SpecTcl::DBGate::NameList gates = {"mask", "1d"};
    auto compound = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "compound", "+", gates
    );
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBGate g(*m_pConn, 123456),
        std::invalid_argument
    );
    
    delete ptsGate;
    delete maskGate;
    delete compound;
    
    
}
void dbgtest::getpars_1()
{
    // Get pars from point gate works fine:
    
    SpecTcl::DBGate::NameList p1d = {"param.5"};
    makeSomeParams();
    auto ptsGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "1d", "s", p1d, 100.0, 200.
    );                                // id 1.
    auto maskGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "mask", "em", "param.3", 0x12345678
    );
    
    SpecTcl::DBGate::NameList gates = {"mask", "1d"};
    auto compound = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "compound", "+", gates
    );
    
    SpecTcl::DBGate g(*m_pConn, ptsGate->getInfo().s_info.s_id);
    auto params = g.getParameters();
    EQ(size_t(1), params.size());
    EQ(std::string("param.5"), params[0]);
    
    delete ptsGate;
    delete maskGate;
    delete compound;
    
}
void dbgtest::getpars_2()
{
    // get pars from mask gate works 

    SpecTcl::DBGate::NameList p1d = {"param.5"};
    makeSomeParams();
    auto ptsGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "1d", "s", p1d, 100.0, 200.
    );                                // id 1.
    auto maskGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "mask", "em", "param.3", 0x12345678
    );
    
    SpecTcl::DBGate::NameList gates = {"mask", "1d"};
    auto compound = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "compound", "+", gates
    );

    SpecTcl::DBGate g(*m_pConn, maskGate->getInfo().s_info.s_id);
    auto params = g.getParameters();
    EQ(size_t(1), params.size());
    EQ(std::string("param.3"), params[0]);
    
    delete ptsGate;
    delete maskGate;
    delete compound;
    
}
void dbgtest::getpars_3()
{
    // getpars from compound throws.
    
    
    SpecTcl::DBGate::NameList p1d = {"param.5"};
    makeSomeParams();
    auto ptsGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "1d", "s", p1d, 100.0, 200.
    );                                // id 1.
    auto maskGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "mask", "em", "param.3", 0x12345678
    );
    
    SpecTcl::DBGate::NameList gates = {"mask", "1d"};
    auto compound = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "compound", "+", gates
    );
 
    SpecTcl::DBGate g(*m_pConn, compound->getInfo().s_info.s_id);
    CPPUNIT_ASSERT_THROW(
        g.getParameters(),
        std::invalid_argument
    );
    
    delete ptsGate;
    delete maskGate;
    delete compound;
    
}

void dbgtest::getgates_1()
{
    // Point gate fails.
    
    SpecTcl::DBGate::NameList p1d = {"param.5"};
    makeSomeParams();
    auto ptsGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "1d", "s", p1d, 100.0, 200.
    );                                // id 1.
    auto maskGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "mask", "em", "param.3", 0x12345678
    );
    
    SpecTcl::DBGate::NameList gates = {"mask", "1d"};
    auto compound = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "compound", "+", gates
    );
    SpecTcl::DBGate g(*m_pConn, m_pSaveset->getInfo().s_id, "1d");
    CPPUNIT_ASSERT_THROW(g.getGates(), std::invalid_argument);
    
    delete ptsGate;
    delete maskGate;
    delete compound;
    
}
void dbgtest::getgates_2()
{
    // mask gate fails.

    SpecTcl::DBGate::NameList p1d = {"param.5"};
    makeSomeParams();
    auto ptsGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "1d", "s", p1d, 100.0, 200.
    );                                // id 1.
    auto maskGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "mask", "em", "param.3", 0x12345678
    );
    
    SpecTcl::DBGate::NameList gates = {"mask", "1d"};
    auto compound = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "compound", "+", gates
    );
    
    SpecTcl::DBGate g(*m_pConn, m_pSaveset->getInfo().s_id, "mask");
    CPPUNIT_ASSERT_THROW(g.getGates(), std::invalid_argument);

    delete ptsGate;
    delete maskGate;
    delete compound;
    
}
void dbgtest::getgates_3()
{
    // Compound gate works.
    
    SpecTcl::DBGate::NameList p1d = {"param.5"};
    makeSomeParams();
    auto ptsGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "1d", "s", p1d, 100.0, 200.
    );                                // id 1.
    auto maskGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "mask", "em", "param.3", 0x12345678
    );
    
    SpecTcl::DBGate::NameList gates = {"mask", "1d"};
    auto compound = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "compound", "+", gates
    );
    
    SpecTcl::DBGate g(*m_pConn, m_pSaveset->getInfo().s_id, "compound");
    auto dgates = g.getGates();
    
    EQ(size_t(2), dgates.size());
    EQ(std::string("mask"), dgates[0]);
    EQ(std::string("1d"), dgates[1]);
    
    delete ptsGate;
    delete maskGate;
    delete compound;
    
}

void dbgtest::getpts_1()
{
    // Can get points from a point gate.
    
    SpecTcl::DBGate::NameList p1d = {"param.5"};
    makeSomeParams();
    auto ptsGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "1d", "s", p1d, 100.0, 200.
    );                                // id 1.
    auto maskGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "mask", "em", "param.3", 0x12345678
    );
    
    SpecTcl::DBGate::NameList gates = {"mask", "1d"};
    auto compound = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "compound", "+", gates
    );

    
    SpecTcl::DBGate g(*m_pConn, ptsGate->getInfo().s_info.s_id);
    auto pts = g.getPoints();
    
    EQ(ptsGate->m_Info.s_points.size(), pts.size());
    EQ(ptsGate->m_Info.s_points[0].s_x, pts[0].s_x);
    EQ(ptsGate->m_Info.s_points[0].s_y, pts[0].s_y);
    EQ(ptsGate->m_Info.s_points[1].s_x, pts[1].s_x);
    EQ(ptsGate->m_Info.s_points[1].s_y, pts[1].s_y);
    
    
    
    
    delete ptsGate;
    delete maskGate;
    delete compound;
    
}
void dbgtest::getpts_2()
{
    // Exception getting points from a mask gate.
    
    SpecTcl::DBGate::NameList p1d = {"param.5"};
    makeSomeParams();
    auto ptsGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "1d", "s", p1d, 100.0, 200.
    );                                // id 1.
    auto maskGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "mask", "em", "param.3", 0x12345678
    );
    
    SpecTcl::DBGate::NameList gates = {"mask", "1d"};
    auto compound = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "compound", "+", gates
    );
    
    SpecTcl::DBGate g(*m_pConn, maskGate->getInfo().s_info.s_id);
    CPPUNIT_ASSERT_THROW(g.getPoints(), std::invalid_argument);
    
    delete ptsGate;
    delete maskGate;
    delete compound;
    
    
}
void dbgtest::getpts_3()
{
    // Exception getting points from a compound gate.
    
    SpecTcl::DBGate::NameList p1d = {"param.5"};
    makeSomeParams();
    auto ptsGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "1d", "s", p1d, 100.0, 200.
    );                                // id 1.
    auto maskGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "mask", "em", "param.3", 0x12345678
    );
    
    SpecTcl::DBGate::NameList gates = {"mask", "1d"};
    auto compound = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "compound", "+", gates
    );
    
    SpecTcl::DBGate g(*m_pConn, compound->getInfo().s_info.s_id);
    CPPUNIT_ASSERT_THROW(g.getPoints(), std::invalid_argument);
    
    
    
    delete ptsGate;
    delete maskGate;
    delete compound;
    
}

void dbgtest::getmask_1()
{
    // Get mask from point gate fails
    
    SpecTcl::DBGate::NameList p1d = {"param.5"};
    makeSomeParams();
    auto ptsGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "1d", "s", p1d, 100.0, 200.
    );                                // id 1.
    auto maskGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "mask", "em", "param.3", 0x12345678
    );
    
    SpecTcl::DBGate::NameList gates = {"mask", "1d"};
    auto compound = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "compound", "+", gates
    );
    
    SpecTcl::DBGate g(*m_pConn, ptsGate->getInfo().s_info.s_id);
    CPPUNIT_ASSERT_THROW(g.getMask(), std::invalid_argument);
    
    
    delete ptsGate;
    delete maskGate;
    delete compound;
}
void dbgtest::getmask_2()
{
    // get mask from mask gate returns correct mask.
    
    SpecTcl::DBGate::NameList p1d = {"param.5"};
    makeSomeParams();
    auto ptsGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "1d", "s", p1d, 100.0, 200.
    );                                // id 1.
    auto maskGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "mask", "em", "param.3", 0x12345678
    );
    
    SpecTcl::DBGate::NameList gates = {"mask", "1d"};
    auto compound = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "compound", "+", gates
    );
    SpecTcl::DBGate g(*m_pConn, maskGate->getInfo().s_info.s_id);
    EQ(maskGate->getInfo().s_mask, g.getMask());
    
    delete ptsGate;
    delete maskGate;
    delete compound;
}
void dbgtest::getmask_3()
{
    // get mask from compound gate fails.
    
    SpecTcl::DBGate::NameList p1d = {"param.5"};
    makeSomeParams();
    auto ptsGate = SpecTcl::DBGate::create1dGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "1d", "s", p1d, 100.0, 200.
    );                                // id 1.
    auto maskGate = SpecTcl::DBGate::createMaskGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "mask", "em", "param.3", 0x12345678
    );
    
    SpecTcl::DBGate::NameList gates = {"mask", "1d"};
    auto compound = SpecTcl::DBGate::createCompoundGate(
        *m_pConn, m_pSaveset->getInfo().s_id,
        "compound", "+", gates
    );
    SpecTcl::DBGate g(*m_pConn, compound->getInfo().s_info.s_id);
    CPPUNIT_ASSERT_THROW(g.getMask(), std::invalid_argument);
    
    delete ptsGate;
    delete maskGate;
    delete compound;
}

void dbgtest::save_1()
{
    // SaveSet::exists.
    
    makeSome1dGates();
    EQ(false, m_pSaveset->gateExists("nosuchgate"));
    EQ(true, m_pSaveset->gateExists("gate.1"));
}

void dbgtest::save_2()
{
    // SaveSet::create1dGate
    makeSomeParams();
    std::vector<const char*> pars = {"param.1"};
    auto p = m_pSaveset->create1dGate("test", "s", pars, 100.0, 200.0);
    ASSERT(p);
    delete p;
}
void dbgtest::save_3()
{
    // create compound gate via saveset.
    
    makeSome1dGates();
    SpecTcl::DBGate::NameList gates = {
        "gate.1", "gate.2", "gate.3", "gate.4"
    };
    
    auto p = m_pSaveset->createCompoundGate(
        "testing", "*", gates
    );
    ASSERT(p);
    
    delete p;
}
void dbgtest::save_4()
{
    // Create a mask gate via saveset:
    
    makeSomeParams();
    
    auto p = m_pSaveset->createMaskGate("testgate", "em", "param.1", 0x55555555);
    ASSERT(p);
    delete p;
}
void dbgtest::save_5()
{
    // lookup a gate by name:
    
    makeSome1dGates();
    auto p = m_pSaveset->lookupGate("gate.2");
    ASSERT(p);
    delete p;
}
void dbgtest::save_6()
{
    // lookup a gate by id.
    
    makeSome1dGates();
    auto p = m_pSaveset->lookupGate(2);
    ASSERT(p);
    delete p;
}
void dbgtest::save_7()
{
    // List all the gates.
    
    makeSome1dGates();
    
    std::set<std::string> gateNames;
    for (int i= 0; i < 10; i++) {
        std::stringstream s;
        s << "gate." << i;
        gateNames.insert(s.str());
    }
    
    auto gates = m_pSaveset->listGates();
    EQ(gateNames.size(), gates.size());
    for (int i =0; i < gates.size(); i++) {
        auto info = gates[i]->getInfo();
        EQ(size_t(1), gateNames.count(info.s_info.s_name));
        delete gates[i];
    }
}