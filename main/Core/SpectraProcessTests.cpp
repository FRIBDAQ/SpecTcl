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

#include <config.h>

#include "SpectraProcess.h"
#include "SpecTcl.h"

#include <Asserts.h>

#include <unistd.h>

#include <memory>


using namespace std;

class SpectraProcessTests : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(SpectraProcessTests);
    CPPUNIT_TEST(generatePath_0);
    CPPUNIT_TEST_SUITE_END();


public:

    void generatePath_0() {
        Spectra::CSpectraProcess process;

        setenv("SPECTRA_EXECUTABLE_PATH", "/test/path", 1);
        EQMSG("Correct path is generated",
              string("/test/path"), process.generatePath() );
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(SpectraProcessTests);



