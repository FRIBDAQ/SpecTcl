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

#ifndef GGATETEST_H
#define GGATETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include "Asserts.h"

#include "MainWindow.h"

#include <iostream>
#include <algorithm>
#include <memory>

using namespace std;


namespace Viewer
{

class MainWindowTest : public CppUnit::TestFixture
{
  private:

  public:
    CPPUNIT_TEST_SUITE( MainWindowTest );
    CPPUNIT_TEST_SUITE_END();

  public:
    void setUp() {

    }
    void tearDown() {
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION(MainWindowTest);

#endif


} // end of namespace
