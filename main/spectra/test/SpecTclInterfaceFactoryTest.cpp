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

    char* m_pOldKey;
    char* m_pOldSize;

  public:
    void setUp() {
        // make sure that we don't have preexisting state
        m_pOldKey = ::getenv("XAMINE_SHMEM");
        if (m_pOldKey) {
            unsetenv("XAMINE_SHMEM");
        }

        m_pOldSize = ::getenv("XAMINE_SHMEM_SIZE");
        if (m_pOldSize) {
            unsetenv("XAMINE_SHMEM_SIZE");
        }
    }
    void tearDown() {
        if (m_pOldKey) {
            setenv("XAMINE_SHMEM", m_pOldKey, 1);
        }

        if (m_pOldSize) {
            setenv("XAMINE_SHMEM_SIZE", m_pOldSize, 1);
        }
    }

    void setUpShMemEnv_0() {

        const char* theKey = "thekey";
        ::setenv("XAMINE_SHMEM", theKey, 1);
        GlobalSettings::setSharedMemoryKey("");


        SpecTclInterfaceFactory factory;
        factory.setUpShMemKeyEnv();

        QString key = GlobalSettings::getSharedMemoryKey();
        ASSERTMSG("XAMINE_SHMEM that we define should be retrievable with GlobalSettings",
                  key == theKey);
    }

    void setUpShMemEnv_1() {

        ::setenv("XAMINE_SHMEM_SIZE", "1234", 1);

        GlobalSettings::setSharedMemorySize(0);

        SpecTclInterfaceFactory factory;
        factory.setUpShMemSizeEnv();

        QString size = GlobalSettings::getSharedMemorySize();

        ASSERTMSG("XAMINE_SHMEM_SIZE that we define should be retrievable in GlobalSettings",
              size == "1234");
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(SpecTclInterfaceFactoryTest);

#endif
} // end of namespcae
