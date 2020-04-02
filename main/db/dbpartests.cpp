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
    CPPUNIT_TEST_SUITE_END();
    
private:
    std::string         m_dbfile;
    SpecTcl::CDatabase* m_pDb;
    SpecTcl::SaveSet*   m_pSet;
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
    
    }
    void tearDown() {
        delete m_pSet;
        delete m_pDb;
        unlink(m_dbfile.c_str());
    }
protected:
    void exists_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(dbpartest);

void dbpartest::exists_1()
{
}