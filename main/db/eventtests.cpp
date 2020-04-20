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
#include "SaveSet.h"
#include "CSqlite.h"
#include "CSqliteStatement.h"

#include <errno.h>
#include <string>
#include <string.h>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include <stdexcept>

class eventtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(eventtest);
    CPPUNIT_TEST(start_1);
    CPPUNIT_TEST_SUITE_END();
    
private:
    std::string        m_filename;
    CSqlite*           m_pConn;
    SpecTcl::CDatabase* m_pDb;
    SpecTcl::SaveSet*  m_pSaveset;
public:
    void setUp() {
        const char* fnameTemplate="eventtestdbXXXXXX";
        char fname[100];
        strcpy(fname, fnameTemplate);
        int fd = mkstemp(fname);
        if (fd < 0) {
            int e = errno;
            std::stringstream msg;
            msg << " Unable to make temp file" << fname
                << " : " << strerror(e);
            throw std::logic_error(msg.str());
        }
        close(fd);
        
        m_filename = fname;
        m_pConn = new CSqlite(fname);
        SpecTcl::CDatabase::create(fname);
        m_pDb = new SpecTcl::CDatabase(fname);
        m_pSaveset = m_pDb->createSaveSet("save");
    }
    void tearDown() {
        delete m_pSaveset;
        delete m_pDb;
        delete m_pConn;
        unlink(m_filename.c_str());
    }
protected:
    void start_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(eventtest);

void eventtest::start_1()
{
}