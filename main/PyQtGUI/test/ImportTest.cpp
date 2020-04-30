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

#include "ImportTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ImportTest);

int ImportTest::result(const char* module) {
    // Initialize the Python Interpreter
    Py_Initialize();

    int rv = -1;
    PyObject* importlib;
    Py_Initialize();
    PyRun_SimpleString("import sys; sys.path.append('.')");
      
    if (PyImport_ImportModule(module))
      rv=1;
    else
      rv=0;

    Py_Finalize();
    return rv;
}
  
void ImportTest::module_0() {
  int rv = result("matplotlib");
  CPPUNIT_ASSERT(1 == rv);
}

void ImportTest::module_1() {
  int rv = result("json");
  CPPUNIT_ASSERT(1 == rv);
}

void ImportTest::module_2() {
  int rv = result("httplib2");
  CPPUNIT_ASSERT(1 == rv);
}

void ImportTest::module_3() {
  int rv = result("threading");
  CPPUNIT_ASSERT(1 == rv);
}

void ImportTest::module_4() {
  int rv = result("itertools");
  CPPUNIT_ASSERT(1 == rv);
}

void ImportTest::module_5() {
  int rv = result("time");
  CPPUNIT_ASSERT(1 == rv);
}

void ImportTest::module_6() {
  int rv = result("multiprocessing");
  CPPUNIT_ASSERT(1 == rv);
}

void ImportTest::module_7() {
  int rv = result("PyQt5");
  CPPUNIT_ASSERT(1 == rv);
}

void ImportTest::module_8() {
  int rv = result("mpl_toolkits");
  CPPUNIT_ASSERT(1 == rv);
}

