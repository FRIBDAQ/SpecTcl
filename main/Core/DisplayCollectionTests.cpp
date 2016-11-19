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

#include "NullDisplay.h"
#include "DisplayCollection.h"

#include <exception>

class DisplayCollectionTests : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(DisplayCollectionTests);
    CPPUNIT_TEST(constructEmpty_0);
    CPPUNIT_TEST(addDisplay_0);
    CPPUNIT_TEST(addDisplay_1);
    CPPUNIT_TEST(removeDisplay_0);
    CPPUNIT_TEST(removeDisplay_1);
    CPPUNIT_TEST(displayExists_0);
    CPPUNIT_TEST(displayExists_1);
    CPPUNIT_TEST(getCurrentDisplay_0);
    CPPUNIT_TEST(getCurrentDisplay_1);
    CPPUNIT_TEST(getCurrentDisplay_2);
    CPPUNIT_TEST(setCurrentDisplay_0);
    CPPUNIT_TEST(setCurrentDisplay_1);
    CPPUNIT_TEST(setCurrentDisplay_2);
    CPPUNIT_TEST_SUITE_END();


private:
    CDisplayCollection* m_pCol;
    CDisplay*           m_pDisp;

public:
    void setUp() {
        m_pCol = new CDisplayCollection;
        m_pDisp = new CNullDisplay;
    }

    void tearDown() {
        delete m_pCol;
        delete m_pDisp;
    }

    void constructEmpty_0 () {
        EQMSG("Construction creates empty collection",
              size_t(0), m_pCol->size());
    }

    void addDisplay_0 () {
        m_pCol->addDisplay(m_pDisp);
        EQMSG("After I add a display, the display exists",
                  size_t(1), m_pCol->size());
    }

    void addDisplay_1 () {
        m_pCol->addDisplay(m_pDisp);
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Redundant adds is not a failure",
                 m_pCol->addDisplay(m_pDisp));
    }

    void removeDisplay_0 () {
        // add a display
        m_pCol->addDisplay(m_pDisp);

        //  remove it
        m_pCol->removeDisplay(m_pDisp);
        EQMSG("Removing a display that exists succeeds",
              size_t(0), m_pCol->size());
    }

    void removeDisplay_1 () {
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Removal nonexistent display is not a failure",
                m_pCol->removeDisplay(m_pDisp));
    }

    void displayExists_0 () {
        m_pCol->addDisplay(m_pDisp);
        EQMSG("displayExists should return true if the display exists",
              true, m_pCol->displayExists(m_pDisp));
    }

    void displayExists_1 () {
        EQMSG("displayExists should return false if the display doesn't exists",
              false, m_pCol->displayExists(m_pDisp));
    }

    void getCurrentDisplay_0 () {
        EQMSG("Current display should be null for empty coll",
              (CDisplay*)(0), m_pCol->getCurrentDisplay());
    }

    void getCurrentDisplay_1 () {
        m_pCol->addDisplay(m_pDisp);
        EQMSG("Current display doesn't change with add",
              (CDisplay*)(0), m_pCol->getCurrentDisplay());
    }

    void getCurrentDisplay_2 () {
        m_pCol->addDisplay(m_pDisp);
        m_pCol->setCurrentDisplay(m_pDisp);

        EQMSG("Current display does the right thing",
              m_pDisp, m_pCol->getCurrentDisplay());
    }

    void setCurrentDisplay_0 () {
        m_pCol->setCurrentDisplay(m_pDisp);

        EQMSG("Setting nonexistent display to current adds it",
              size_t(1), m_pCol->size());
    }

    void setCurrentDisplay_1 () {
        m_pCol->setCurrentDisplay(m_pDisp);

        EQMSG("Setting nonexistent display succeeds if the disp is not contained",
              m_pDisp, m_pCol->getCurrentDisplay());
    }

    void setCurrentDisplay_2 () {
        CPPUNIT_ASSERT_THROW_MESSAGE("Setting current display to null is not allowed",
                                     m_pCol->setCurrentDisplay(0),
                                     std::exception);
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(DisplayCollectionTests);
