/**

#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2013.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Author:
#            Ron Fox
#            NSCL
#            Michigan State University
#            East Lansing, MI 48824-1321

##
# @file   exceptionTest.cpp
# @brief  Tests for CSqliteException class.
# @author <fox@nscl.msu.edu>
*/

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include <sqlite3.h>
#include "CSqliteException.h"
#include <string>


class ExceptionTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ExceptionTest);
  
  CPPUNIT_TEST(msg);
  CPPUNIT_TEST(badmsg);

  CPPUNIT_TEST(copyconst);
  
  CPPUNIT_TEST(assign);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void msg();
  void badmsg();
  
  void copyconst();
  
  void assign();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ExceptionTest);

// Tests for correct function of what() implicitily tests construction

void ExceptionTest::msg() {
    CSqliteException e(SQLITE_ABORT);
    EQ(std::string("Callback routine requested an abort"), std::string(e.what()));
}

void ExceptionTest::badmsg()
{
    // This is an invalid error code (it's a result code)
    
    CSqliteException e(SQLITE_DONE);
    EQ(std::string("Invalid error code"), std::string(e.what()));
}

// Copy Construction test

void ExceptionTest::copyconst()
{
    CSqliteException e(SQLITE_ABORT);
    CSqliteException ecopy(e);
    
    EQ(std::string("Callback routine requested an abort"), std::string(ecopy.what()));
}

// Assignment test:

void ExceptionTest::assign()
{
    CSqliteException e(SQLITE_ABORT);
    CSqliteException ecopy(SQLITE_OK);
    ecopy = e;
    
    EQ(std::string("Callback routine requested an abort"), std::string(ecopy.what()));
}
