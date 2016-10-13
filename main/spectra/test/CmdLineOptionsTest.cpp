
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

#include "CmdLineOptions.h"
#include "TestUtils.h"

#include <QString>

#include <iostream>
#include <array>
#include <string>
#include <cstring>

using namespace std;

namespace Viewer
{

class CmdLineOptionsTest : public CppUnit::TestFixture
{
  public:
    CPPUNIT_TEST_SUITE( CmdLineOptionsTest );
    CPPUNIT_TEST( getPort_0 );
    CPPUNIT_TEST( getHost_0 );
    CPPUNIT_TEST_SUITE_END();

    char* m_opts[5];

  public:
    void setUp() {
        const char* prog = "program";
        const char* opt1 = "--port";
        const char* arg1 = "1234";
        const char* opt2 = "--host";
        const char* arg2 = "asdfasdf";
        m_opts[0] = new char[strlen(prog)];
        strcpy(m_opts[0], prog);

        m_opts[1] = new char[strlen(opt1)];
        strcpy(m_opts[1], opt1);

        m_opts[2] = new char[strlen(arg1)];
        strcpy(m_opts[2], arg1);

        m_opts[3] = new char[strlen(opt2)];
        strcpy(m_opts[3], opt2);

        m_opts[4] = new char[strlen(arg2)];
        strcpy(m_opts[4], arg2);


    }
    void tearDown() {
        for (int i=0; i<5; ++i) {
            delete [] m_opts[i];
        }

    }

  protected:
    void getPort_0()
    {
        CmdLineOptions args;
        args.parse(5, m_opts);

        EQMSG("Port can be extracted properly",
              1234, args.getPort());
    }

    void getHost_0()
    {
        CmdLineOptions args;
        args.parse(5, m_opts);

        EQMSG("host can be extracted properly",
              QString("asdfasdf"), args.getHost());
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(CmdLineOptionsTest);


} // end of namespace
