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

#include <config.h>
#include "SharedMemoryKeyCommand.h"
#include "SpecTclDisplayManager.h"
#include "SpecTcl.h"

#include "TCLInterpreter.h"
#include "TCLException.h"

#include <sstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

#include <unistd.h>


extern CDisplayInterface* gpDisplayInterface;

using namespace std;

class CSharedMemorySizeCommandTests : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CSharedMemorySizeCommandTests);
    CPPUNIT_TEST(execute_0);
    CPPUNIT_TEST(tooManyArgs_0);
    CPPUNIT_TEST(noDisplay_0);
    CPPUNIT_TEST_SUITE_END();

private:

public:
    void setUp() {
    }

    void execute_0 () {
        CTCLInterpreter interp;

        SpecTcl& api = *SpecTcl::getInstance();

        CSpecTclDisplayInterface interface;
        api.SetDisplayInterface(interface);

        interface.createDisplay("spectra", "spectra");
        interface.setCurrentDisplay("spectra");

        CSharedMemoryKeyCommand cmd(interp, api);

        string result = interp.GlobalEval("shmemkey");

        ostringstream expected;
        expected << "XA" << hex << setfill('0') << setw(2) << (0xff&getpid());

        EQMSG("Shared memory key command (shmemkey) should return correct key",
              expected.str(), result);
    }

    void tooManyArgs_0() {
        CTCLInterpreter interp;

        SpecTcl& api = *SpecTcl::getInstance();

        CSpecTclDisplayInterface interface;
        api.SetDisplayInterface(interface);

        interface.createDisplay("spectra", "spectra");
        interface.setCurrentDisplay("spectra");

        CSharedMemoryKeyCommand cmd(interp, api);

        CPPUNIT_ASSERT_THROW_MESSAGE (
                    "An exception should occur when too many arguments provided.",
            interp.GlobalEval("shmemkey asdf"),
            CTCLException
        );
    }

    void noDisplay_0() {
        CTCLInterpreter interp;

        SpecTcl& api = *SpecTcl::getInstance();

        // Force the display to clear
        gpDisplayInterface = nullptr;


        CSharedMemoryKeyCommand cmd(interp, api);

        CPPUNIT_ASSERT_THROW_MESSAGE (
                    "Exception should be thrown when no display is provided.",
            interp.GlobalEval("shmemkey"),
            CTCLException
        );
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(CSharedMemorySizeCommandTests);



