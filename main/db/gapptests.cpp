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

#include "DBApplications.h"
#include "DBGate.h"
#include "DBSpectrum.h"
#include "SaveSet.h"
#define private public
#include "SpecTclDatabase.h"
#undef private


#include <string>
#include <sstream>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdexcept>
#include <string.h>

class gapptest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(gapptest);
    CPPUNIT_TEST(test_1);
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
    void test_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(gapptest);

void gapptest::test_1()
{
}