
#ifndef GGATETEST_H
#define GGATETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include "Asserts.h"

#include "mainwindow.h"

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
