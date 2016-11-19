//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2016.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Jeromy Tompkins
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "SharedMemorySizeCommand.h"

#include "TCLInterpreter.h"
#include "TCLException.h"

#include <string>
#include <iomanip>
#include <iostream>
#include <stdexcept>

#include <unistd.h>


using namespace std;

class CSharedMemorySizeCommandTests : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CSharedMemorySizeCommandTests);
    CPPUNIT_TEST(execute_0);
    CPPUNIT_TEST(execute_1);
    CPPUNIT_TEST(tooManyArgs_0);
    CPPUNIT_TEST_SUITE_END();

private:
    std::string m_oldStr;

public:
    void setUp() {
        m_oldStr = ::getenv("XAMINE_SHMEM_SIZE");
        ::unsetenv("XAMINE_SHMEM_SIZE");
    }

    void tearDown() {
        ::setenv("XAMINE_SHMEM_SIZE", m_oldStr.c_str(), 1);
    }

    void execute_0 () {
        CTCLInterpreter interp;

        string expected("123");
        ::setenv("XAMINE_SHMEM_SIZE", expected.c_str(), 1);

        CSharedMemorySizeCommand cmd(interp);

        string result = interp.GlobalEval("shmemsize");

        EQMSG("Shared memory size command (shmemsize) should return correct size",
              expected, result);
    }


    void execute_1 () {
        CTCLInterpreter interp;

        CSharedMemorySizeCommand cmd(interp);

        string result = interp.GlobalEval("shmemsize");

        EQMSG("shmemsize should return empty string if XAMINE_SHMEM_SIZE not set",
              string(), result);
    }

    void tooManyArgs_0() {
        CTCLInterpreter interp;

        CSharedMemorySizeCommand cmd(interp);

        CPPUNIT_ASSERT_THROW_MESSAGE (
                    "An exception should occur when too many arguments provided.",
            interp.GlobalEval("shmemsize asdf"),
            CTCLException
        );
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(CSharedMemorySizeCommandTests);




