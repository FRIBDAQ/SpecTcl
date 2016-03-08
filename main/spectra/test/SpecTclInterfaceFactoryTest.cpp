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


#ifndef GATEEDITREQUESTTEST_H
#define GATEEDITREQUESTTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "SpecTclInterfaceFactory.h"
#include "GlobalSettings.h"

#include <QString>

#include <algorithm>
#include <iostream>

#include <unistd.h>

using namespace std;

namespace Viewer
{

class SpecTclInterfaceFactoryTest : public CppUnit::TestFixture
{
  public:
    CPPUNIT_TEST_SUITE( SpecTclInterfaceFactoryTest );
    CPPUNIT_TEST( setUpShMemEnv_0 );
    CPPUNIT_TEST( setUpShMemEnv_1 );
    CPPUNIT_TEST_SUITE_END();

    char* m_pOld;

  public:
    void setUp() {
        // make sure that we don't have preexisting state
        m_pOld = ::getenv("XAMINE_SHMEM");
        if (m_pOld) {
            unsetenv("XAMINE_SHMEM");
        }
    }
    void tearDown() {
        if (m_pOld) {
            setenv("XAMINE_SHMEM", m_pOld, 1);
        }
    }

    void setUpShMemEnv_0() {

        const char* theKey = "thekey";
        GlobalSettings::setSharedMemoryKey(theKey);

        SpecTclInterfaceFactory factory;
        factory.setUpShMemEnv();

        char* pEnv = ::getenv("XAMINE_SHMEM");
        ASSERTMSG("XAMINE_SHMEM that we define should be retrievable with getenv",
                  equal(pEnv, pEnv + strlen(pEnv), theKey));
    }

    void setUpShMemEnv_1() {

        const char* defEnv = "whatever";
        setenv("XAMINE_SHMEM", defEnv, 1);

        SpecTclInterfaceFactory factory;
        factory.setUpShMemEnv();

        char* pEnv = ::getenv("XAMINE_SHMEM");
        ASSERTMSG("Previously defined XAMINE_SHMEM should be retrievable with getenv",
                  equal(pEnv, pEnv + min(strlen(pEnv), strlen(defEnv)), defEnv));
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(SpecTclInterfaceFactoryTest);

#endif
} // end of namespcae
