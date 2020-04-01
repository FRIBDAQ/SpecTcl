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
#define private public
#include "SaveSet.h"
#undef private

#include <string>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdexcept>
#include <errno.h>
#include <sstream>

class savesettest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(savesettest);
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST_SUITE_END();
    
private:
    CSqlite*    m_pDatabase;
    std::string m_file;
public:
    void setUp() {
        makeTempFile();
        makeDatabase();
        m_pDatabase = new CSqlite(m_file.c_str());
    }
    void tearDown() {
        delete m_pDatabase;
        unlink(m_file.c_str());
    }
protected:
    void construct_1();
private:
    void makeTempFile();
    void makeDatabase();
};
/**
 * use mkstemp to make temporary file.
 */
void
savesettest::makeTempFile()
{
    const char* dbtemplate="savesettestXXXXXX";
    char filename[200];
    strcpy(filename, dbtemplate);
    int fd = mkstemp(filename);
    if (fd < 0) {
        int e = errno;
        std::stringstream msg;
        msg << "Unable to make temp file: " << dbtemplate
            << " : " << strerror(e);
        throw std::logic_error(msg.str());
    }
    close(fd);
    m_file = filename;
}
/**
 * makeDatabase
 *    Create the database in the file m_file.
 */
void
savesettest::makeDatabase()
{
    SpecTcl::CDatabase::create(m_file.c_str());
}

CPPUNIT_TEST_SUITE_REGISTRATION(savesettest);

void savesettest::construct_1()
{
}