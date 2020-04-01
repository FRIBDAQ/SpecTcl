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
#include "CSqlite.h"
#include "CSqliteStatement.h"
#include "CSqliteException.h"

#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <stdexcept>
#include <errno.h>
#include <sstream>


class specdbtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(specdbtest);
    CPPUNIT_TEST(create_1);
    CPPUNIT_TEST(create_2);
    CPPUNIT_TEST(create_3);
    CPPUNIT_TEST_SUITE_END();
    
private:
    std::string m_dbfile;
public:
    void setUp() {
        const char* fileTemplate="specdbtestXXXXXX";
        char dbfile[200];
        strcpy(dbfile, fileTemplate);
        int fd = mkstemp(dbfile);
        if (fd < 0) {
            int e = errno;
            std::stringstream s;
            s <<  "Unable to make temp file " << fileTemplate
                << " : " << strerror(e);
            throw std::invalid_argument(s.str());
        }
        close(fd);
        unlink(dbfile);
        m_dbfile = dbfile;
    }
    void tearDown() {
        unlink(m_dbfile.c_str());
    }
protected:
    void create_1();
    void create_2();
    void create_3();
};

CPPUNIT_TEST_SUITE_REGISTRATION(specdbtest);

void specdbtest::create_1()
{
    // Creating an impossible one should throw
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::CDatabase::create("/should/not/be/able/to/create"),
        CSqliteException
    );
}
void specdbtest::create_2()
{
    // Creating in good file doesn't throw.
    CPPUNIT_ASSERT_NO_THROW(
        SpecTcl::CDatabase::create(m_dbfile.c_str())
    );
}
void specdbtest::create_3()
{
    // create in existing db file doesn't throw.
    
    CPPUNIT_ASSERT_NO_THROW(
        SpecTcl::CDatabase::create(m_dbfile.c_str())
    );
    CPPUNIT_ASSERT_NO_THROW(
        SpecTcl::CDatabase::create(m_dbfile.c_str())
    );
}