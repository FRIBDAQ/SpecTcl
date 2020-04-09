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

#include "DBParameter.h"
#include "DBApplications.h"
#include "DBGate.h"
#include "DBSpectrum.h"
#include "SaveSet.h"
#define private public
#include "SpecTclDatabase.h"
#undef private
#include "CSqlite.h"
#include "CSqliteStatement.h"

#include <string>
#include <sstream>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdexcept>
#include <string.h>

class gapptest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(gapptest);
    CPPUNIT_TEST(apply_1);
    CPPUNIT_TEST(apply_2);
    CPPUNIT_TEST(apply_3);
    CPPUNIT_TEST(apply_4);
    CPPUNIT_TEST(apply_5);
    CPPUNIT_TEST(apply_6);
    CPPUNIT_TEST_SUITE_END();
    
private:
    std::string                m_filename;
    SpecTcl::CDatabase*        m_pDb;
    SpecTcl::SaveSet*          m_pSaveset;
    
    CSqlite*                   m_pConn;
public:
    void setUp() {
        static const char* filenameTemplate("gapptestfileXXXXXX");
        char filename[200];
        strcpy(filename, filenameTemplate);
        int fd = mkstemp(filename);
        if (fd < 0) {
            int e = errno;
            std::stringstream msg;
            msg << " Unable to create temp file "  << filename
                << " : " << strerror(e);
            throw std::logic_error(msg.str());
        }
        
        close(fd);
        m_filename = filename;
        SpecTcl::CDatabase::create(filename);
        m_pDb = new SpecTcl::CDatabase(filename);
        
        m_pSaveset = m_pDb->createSaveSet("MySaveset");
        m_pConn    = &m_pDb->m_connection;
        
    }
    void tearDown() {
        delete m_pSaveset;
        delete m_pDb;
        unlink(m_filename.c_str());
    }
protected:
    void apply_1();
    void apply_2();
    void apply_3();
    void apply_4();
    void apply_5();
    void apply_6();
};

CPPUNIT_TEST_SUITE_REGISTRATION(gapptest);

void gapptest::apply_1()
{
    // Apply  a gate to a spectrum successfully.
    
    delete m_pSaveset->createParameter("param1", 1);
    std::vector<SpecTcl::SaveSet::SpectrumAxis> axes = {{0.0, 100.0, 200}};
    std::vector<const char*> pname={"param1"};
    delete m_pSaveset->createSpectrum("spec1", "1", pname, axes);
    std::vector<const char*> gates;
    delete m_pSaveset->createCompoundGate("g1", "F", gates);
    
    SpecTcl::DBApplication* app;
    CPPUNIT_ASSERT_NO_THROW(
         app = SpecTcl::DBApplication::applyGate(
            *m_pConn, m_pSaveset->getInfo().s_id, "g1", "spec1"
        )
    );
    delete app;
}

void gapptest::apply_2()
{
    // Applying a gate fills in info correctly:
    
    delete m_pSaveset->createParameter("param1", 1);
    std::vector<SpecTcl::SaveSet::SpectrumAxis> axes = {{0.0, 100.0, 200}};
    std::vector<const char*> pname={"param1"};
    auto pSpec =  m_pSaveset->createSpectrum("spec1", "1", pname, axes);
    std::vector<const char*> gates;
    auto pGate = m_pSaveset->createCompoundGate("g1", "F", gates);
    
    auto pApp = SpecTcl::DBApplication::applyGate(
        *m_pConn, m_pSaveset->getInfo().s_id, "g1", "spec1"
    );
    
    auto info = pApp->getInfo();
    EQ(1, info.s_id);
    EQ(pGate->getInfo().s_info.s_id, info.s_gateid);
    EQ(pSpec->getInfo().s_base.s_id, info.s_spectrumid);
    
    delete pSpec;
    delete pGate;
    delete pApp;
}
void gapptest::apply_3()
{
    // Database entry is correct.
    
    delete m_pSaveset->createParameter("param1", 1);
    std::vector<SpecTcl::SaveSet::SpectrumAxis> axes = {{0.0, 100.0, 200}};
    std::vector<const char*> pname={"param1"};
    auto pSpec =  m_pSaveset->createSpectrum("spec1", "1", pname, axes);
    std::vector<const char*> gates;
    auto pGate = m_pSaveset->createCompoundGate("g1", "F", gates);
    
    auto pApp = SpecTcl::DBApplication::applyGate(
        *m_pConn, m_pSaveset->getInfo().s_id, "g1", "spec1"
    );
    
    // We think we already  know from apply_2 that info is right.
    
    auto info = pApp->getInfo();
    
    CSqliteStatement fetch(
        *m_pConn,
        "SELECT spectrum_id, gate_id FROM gate_applications WHERE id=?"
    );
    fetch.bind(1, info.s_id);
    ++fetch;
    EQ(false, fetch.atEnd());
    
      
    EQ(info.s_spectrumid, fetch.getInt(0));
    EQ(info.s_gateid, fetch.getInt(1));
    
    ++fetch;
    EQ(true, fetch.atEnd());
  
    
    delete pSpec;
    delete pGate;
    delete pApp;
}

void gapptest::apply_4()
{
    // Bad saveset id throws.
    
    delete m_pSaveset->createParameter("param1", 1);
    std::vector<SpecTcl::SaveSet::SpectrumAxis> axes = {{0.0, 100.0, 200}};
    std::vector<const char*> pname={"param1"};
    delete m_pSaveset->createSpectrum("spec1", "1", pname, axes);
    std::vector<const char*> gates;
    delete m_pSaveset->createCompoundGate("g1", "F", gates);
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBApplication::applyGate(
            *m_pConn,  m_pSaveset->getInfo().s_id+1, "g1", "spec1"
        ),
        std::invalid_argument
    );
    
}
void gapptest::apply_5()
{
    // bad gate name throws
    
    delete m_pSaveset->createParameter("param1", 1);
    std::vector<SpecTcl::SaveSet::SpectrumAxis> axes = {{0.0, 100.0, 200}};
    std::vector<const char*> pname={"param1"};
    delete m_pSaveset->createSpectrum("spec1", "1", pname, axes);
    std::vector<const char*> gates;
    delete m_pSaveset->createCompoundGate("g1", "F", gates);
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBApplication::applyGate(
            *m_pConn,  m_pSaveset->getInfo().s_id, "g11", "spec1"
        ),
        std::invalid_argument
    );
    
}
void gapptest::apply_6()
{
    // bad spectrum name throws.
    
    delete m_pSaveset->createParameter("param1", 1);
    std::vector<SpecTcl::SaveSet::SpectrumAxis> axes = {{0.0, 100.0, 200}};
    std::vector<const char*> pname={"param1"};
    delete m_pSaveset->createSpectrum("spec1", "1", pname, axes);
    std::vector<const char*> gates;
    delete m_pSaveset->createCompoundGate("g1", "F", gates);
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBApplication::applyGate(
            *m_pConn,  m_pSaveset->getInfo().s_id, "g1", "spec11"
        ),
        std::invalid_argument
    );
    
}