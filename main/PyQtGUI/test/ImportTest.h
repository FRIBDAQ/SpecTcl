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
//    Giordano Cerizza
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321


#ifndef IMPORTTEST_H
#define IMPORTTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include <Python.h>

using namespace std;

class ImportTest : public CppUnit::TestFixture
{
public:
  CPPUNIT_TEST_SUITE( ImportTest );
  CPPUNIT_TEST( module_0 );
  CPPUNIT_TEST( module_1 );
  CPPUNIT_TEST( module_2 );
  CPPUNIT_TEST( module_3 );
  CPPUNIT_TEST( module_4 );
  CPPUNIT_TEST( module_5 );
  CPPUNIT_TEST( module_6 );
  CPPUNIT_TEST( module_7 );
  CPPUNIT_TEST( module_8 );
  CPPUNIT_TEST_SUITE_END();
  
public:
  void setUp() {
  }
  void tearDown() {
  }
  int result(const char* module); 
  
protected:
  void module_0();
  void module_1();
  void module_2();
  void module_3();
  void module_4();
  void module_5();
  void module_6();
  void module_7();
  void module_8();

};

#endif

