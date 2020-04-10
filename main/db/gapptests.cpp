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
#include "CSqliteTransaction.h"

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
    
    CPPUNIT_TEST(list_1);
    CPPUNIT_TEST(list_2);
    CPPUNIT_TEST(list_3);
    CPPUNIT_TEST(list_4);
    
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    CPPUNIT_TEST(construct_4);
    CPPUNIT_TEST(construct_5);
    
    CPPUNIT_TEST(gatename_1);
    CPPUNIT_TEST(specname_1);
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
    
    void list_1();
    void list_2();
    void list_3();
    void list_4();

    void construct_1();
    void construct_2();
    void construct_3();
    void construct_4();
    void construct_5();
    
    void gatename_1();
    void specname_1();
private:
    void makeSomePars();
    void makeSomeSpectra();
    void makeSome1dGates();
    
};

void gapptest::makeSomePars()
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
void gapptest::makeSome1dGates()
{

                                            // Gate entry uses transactions
    for (int i =0; i < 10; i++) {           // tranactions don't nest.
        std::stringstream pname;
        pname << "param." << i;
        std::string paramName = pname.str();
        SpecTcl::DBGate::NameList param;
        param.push_back(paramName.c_str());
        std::stringstream name;
        name << "gate." << i;
        std::string gname = name.str();
        
        delete SpecTcl::DBGate::create1dGate(
            *m_pConn, m_pSaveset->getInfo().s_id, gname.c_str(),
            "s", param, 10, 20
        );
    }    
}
void gapptest::makeSomeSpectra()
{
    SpecTcl::SaveSet::SpectrumAxis a = {-10.0, 10.0, 20};
    std::vector<SpecTcl::SaveSet::SpectrumAxis> axes;
    axes.push_back(a);
                                              // spectrum entry uses transactions
                                              // and those don't nest.
    for (int i=0; i < 10; i++) {
        std::stringstream pname;
        pname << "param." << i;
        std::string parname = pname.str();
        std::vector<const char*> pnames;
        pnames.push_back(parname.c_str());
        
        std::stringstream sname;
        sname << "spectrum." << i;
        std::string specname = sname.str();
        delete m_pSaveset->createSpectrum(
            specname.c_str(), "1", pnames, axes
        );
    }
}

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


void gapptest::list_1()
{
    // Intially empty.
    
    auto l = SpecTcl::DBApplication::listApplications(
        *m_pConn, m_pSaveset->getInfo().s_id
    );
    EQ(size_t(0), l.size());
}
void gapptest::list_2()
{
delete m_pSaveset->createParameter("param1", 1);
    std::vector<SpecTcl::SaveSet::SpectrumAxis> axes = {{0.0, 100.0, 200}};
    std::vector<const char*> pname={"param1"};
    auto s =  m_pSaveset->createSpectrum("spec1", "1", pname, axes);
    std::vector<const char*> gates;
    auto g =  m_pSaveset->createCompoundGate("g1", "F", gates);
    
    SpecTcl::DBApplication* app;
    CPPUNIT_ASSERT_NO_THROW(
         app = SpecTcl::DBApplication::applyGate(
            *m_pConn, m_pSaveset->getInfo().s_id, "g1", "spec1"
        )
    );
    delete app;
    
    auto l = SpecTcl::DBApplication::listApplications(
        *m_pConn, m_pSaveset->getInfo().s_id
    );
    
    EQ(size_t(1), l.size());
    
    auto& info = l[0]->getInfo();
    EQ(s->getInfo().s_base.s_id, info.s_spectrumid);
    EQ(g->getInfo().s_info.s_id, info.s_gateid);
    
    delete l[0];
    delete s;
    delete g;
    
}
void gapptest::list_3()
{
    // Retrieves multiple  in insert order.
    
    makeSomePars();
    makeSome1dGates();
    makeSomeSpectra();
    
    // apply gate.n to spectrum.n
    
    for (int i =0; i < 10; i++) {
        std::stringstream gname;
        gname << "gate." << i;
        std::stringstream spname;
        spname << "spectrum." << i;
        std::string gate = gname.str();
        std::string spec = spname.str();
        delete SpecTcl::DBApplication::applyGate(
            *m_pConn, m_pSaveset->getInfo().s_id,
            gate.c_str(), spec.c_str()
        );
    }
    auto l = SpecTcl::DBApplication::listApplications(
        *m_pConn, m_pSaveset->getInfo().s_id
    );
    
    EQ(size_t(10), l.size());
    
    for (int i = 0; i < 10; i++) {
        std::stringstream gname;
        gname << "gate." << i;
        std::stringstream spname;
        spname << "spectrum." << i;
        
        auto s = m_pSaveset->lookupSpectrum(spname.str().c_str());
        auto g = m_pSaveset->lookupGate(gname.str().c_str());
        
        auto info = l[i]->getInfo();
        EQ(g->getInfo().s_info.s_id, info.s_gateid);
        EQ(s->getInfo().s_base.s_id, info.s_spectrumid);
        
        delete l[i];
        delete g;
        delete s;
    }
}

void gapptest::list_4()
{
    // bad save set.
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBApplication::listApplications(
            *m_pConn, m_pSaveset->getInfo().s_id+1
        ),
        std::invalid_argument
    );
}



void gapptest::construct_1()
{
    //good construction.
    
    makeSomePars();
    makeSome1dGates();
    makeSomeSpectra();
    
    // apply gate.n to spectrum.n
    
    for (int i =0; i < 10; i++) {
        std::stringstream gname;
        gname << "gate." << i;
        std::stringstream spname;
        spname << "spectrum." << i;
        
        delete SpecTcl::DBApplication::applyGate(
            *m_pConn, m_pSaveset->getInfo().s_id,
            gname.str().c_str(), spname.str().c_str()
        );
    }
    
    SpecTcl::DBApplication* pApp(nullptr);
    CPPUNIT_ASSERT_NO_THROW(
        pApp = new SpecTcl::DBApplication(
            *m_pConn, m_pSaveset->getInfo().s_id,
            "gate.1", "spectrum.1"
        )
    );
    
    auto pSpec = m_pSaveset->lookupSpectrum("spectrum.1");
    auto pGate = m_pSaveset->lookupGate("gate.1");
    
    EQ(pSpec->getInfo().s_base.s_id, pApp->getInfo().s_spectrumid);
    EQ(pGate->getInfo().s_info.s_id, pApp->getInfo().s_gateid);
    
    delete pSpec;
    delete pGate;
    delete pApp;
    
}
void gapptest::construct_2()
{
    // bad saveset.
    
    makeSomePars();
    makeSome1dGates();
    makeSomeSpectra();
    
    // apply gate.n to spectrum.n
    
    for (int i =0; i < 10; i++) {
        std::stringstream gname;
        gname << "gate." << i;
        std::stringstream spname;
        spname << "spectrum." << i;
        
        delete SpecTcl::DBApplication::applyGate(
            *m_pConn, m_pSaveset->getInfo().s_id,
            gname.str().c_str(), spname.str().c_str()
        );
    }
    CPPUNIT_ASSERT_THROW(
        new SpecTcl::DBApplication(
            *m_pConn, m_pSaveset->getInfo().s_id+1, "gate.2", "spectrum.2"
        ),
        std::invalid_argument
    );
}
void gapptest::construct_3()
{
    // bad gate.
    
    makeSomePars();
    makeSome1dGates();
    makeSomeSpectra();
    
    // apply gate.n to spectrum.n
    
    for (int i =0; i < 10; i++) {
        std::stringstream gname;
        gname << "gate." << i;
        std::stringstream spname;
        spname << "spectrum." << i;
        
        delete SpecTcl::DBApplication::applyGate(
            *m_pConn, m_pSaveset->getInfo().s_id,
            gname.str().c_str(), spname.str().c_str()
        );
    }
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBApplication app(
            *m_pConn, m_pSaveset->getInfo().s_id, "gate.22", "spectrum.2"
        ),
        std::invalid_argument
    );
}
void gapptest::construct_4()
{
    // bad spectrum.
    
    makeSomePars();
    makeSome1dGates();
    makeSomeSpectra();
    
    // apply gate.n to spectrum.n
    
    for (int i =0; i < 10; i++) {
        std::stringstream gname;
        gname << "gate." << i;
        std::stringstream spname;
        spname << "spectrum." << i;
        
        delete SpecTcl::DBApplication::applyGate(
            *m_pConn, m_pSaveset->getInfo().s_id,
            gname.str().c_str(), spname.str().c_str()
        );
    }
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBApplication app(
            *m_pConn, m_pSaveset->getInfo().s_id, "gate.3", "spectrum.32"
        ),
        std::invalid_argument
    );
}
void gapptest::construct_5()
{
    // no match
    
    makeSomePars();
    makeSome1dGates();
    makeSomeSpectra();
    
    // apply gate.n to spectrum.n
    
    for (int i =0; i < 10; i++) {
        std::stringstream gname;
        gname << "gate." << i;
        std::stringstream spname;
        spname << "spectrum." << i;
        
        delete SpecTcl::DBApplication::applyGate(
            *m_pConn, m_pSaveset->getInfo().s_id,
            gname.str().c_str(), spname.str().c_str()
        );
    }
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBApplication app(
            *m_pConn, m_pSaveset->getInfo().s_id, "gate.3", "spectrum.4"
        ),
        std::invalid_argument
    );
    
}
void gapptest::gatename_1()
{
    // Get gate name from application.
    makeSomePars();
    makeSome1dGates();
    makeSomeSpectra();
    
    auto papp =  SpecTcl::DBApplication::applyGate(
            *m_pConn, m_pSaveset->getInfo().s_id,
            "gate.1", "spectrum.5"
    );
    
    EQ(std::string("gate.1"), papp->getGateName());

    delete papp;
}

void gapptest::specname_1()
{
    // get spectrum name from application.
    
    makeSomePars();
    makeSome1dGates();
    makeSomeSpectra();
    
    auto papp =  SpecTcl::DBApplication::applyGate(
            *m_pConn, m_pSaveset->getInfo().s_id,
            "gate.1", "spectrum.5"
    );
    
    EQ(std::string("spectrum.5"), papp->getSpectrumName());
    delete papp;
}