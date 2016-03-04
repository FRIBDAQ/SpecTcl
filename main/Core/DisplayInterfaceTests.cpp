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

#include <Asserts.h>

#include <DisplayInterface.h>
#include <NullDisplay.h>

class DisplayInterfaceTests : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(DisplayInterfaceTests);
    CPPUNIT_TEST(createDisplay_0);
    CPPUNIT_TEST(createDisplay_1);
    CPPUNIT_TEST(createDisplay_2);
    CPPUNIT_TEST(getDisplay_0);
    CPPUNIT_TEST(getDisplay_1);
    CPPUNIT_TEST(getCurrentDisplay_0);
    CPPUNIT_TEST(getCurrentDisplay_1);
    CPPUNIT_TEST(setCurrentDisplay_0);
    CPPUNIT_TEST_SUITE_END();

private:
    CDisplayInterface* pInterface;

    typedef CDisplayInterface DI;

public:
    void setUp() {
        pInterface = new CDisplayInterface;
    }

    void tearDown() {
        delete pInterface;
    }

    void createDisplay_0 () {
        DI::Result result = pInterface->createDisplay("test", "i don't exist");
        EQMSG("Cannot create display if not creator exists",
              DI::NO_CREATOR, result);
    }

    void createDisplay_1 () {
        pInterface->getFactory().addCreator("null", *(new CNullDisplayCreator));

        DI::Result result = pInterface->createDisplay("test", "null");
        EQMSG("we can create displays if creator exists",
              DI::SUCCESS, result);
    }

    void createDisplay_2 () {
        pInterface->getFactory().addCreator("null", *(new CNullDisplayCreator));

        DI::Result result = pInterface->createDisplay("test", "null");

        result = pInterface->createDisplay("test", "somehting");
        EQMSG("we cannot create multiple displays with same name",
              DI::DISPLAY_EXISTS, result);
    }

    void getDisplay_0 () {
        pInterface->getFactory().addCreator("null", *(new CNullDisplayCreator));

        DI::Result result = pInterface->createDisplay("test", "null");
        ASSERTMSG("An already created display should be gettable",
                  pInterface->getDisplay("test") != NULL);
    }


    void getDisplay_1 () {
        ASSERTMSG("Getting an uncreated display should return NULL pointer",
                  pInterface->getDisplay("test") == NULL);
    }

    void getCurrentDisplay_0 () {
        ASSERTMSG("Current display should be NULL at creation",
                  pInterface->getCurrentDisplay() == NULL);
    }


    void getCurrentDisplay_1 () {
        pInterface->getFactory().addCreator("null", *(new CNullDisplayCreator));

        DI::Result result = pInterface->createDisplay("test", "null");

        ASSERTMSG("Creating first display should cause it to become current",
                  pInterface->getCurrentDisplay() != NULL);
    }

    void setCurrentDisplay_0 () {
        pInterface->getFactory().addCreator("null", *(new CNullDisplayCreator));

        pInterface->createDisplay("test1", "null");
        pInterface->createDisplay("test2", "null");


        pInterface->setCurrentDisplay("test1");
        CDisplay* pFirst = pInterface->getCurrentDisplay();
        pInterface->setCurrentDisplay("test2");
        CDisplay* pSecond = pInterface->getCurrentDisplay();

        ASSERTMSG("Setting current display works as it should",
                  pFirst != pSecond);
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(DisplayInterfaceTests);
