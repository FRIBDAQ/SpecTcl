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

#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <stdexcept>
#include <errno.h>

class dbgtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(dbgtest);
    CPPUNIT_TEST(test_1);
    CPPUNIT_TEST_SUITE_END();
    
private:
    std::string        m_name;
    SpecTcl::CDatabase* m_pDb;
    SpecTcl::SaveSet*  m_pSaveset;
    
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
    }
    void tearDown() {
        delete m_pSaveset;
        delete m_pDb;
    }
protected:
    void test_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(dbgtest);

void dbgtest::test_1()
{
}