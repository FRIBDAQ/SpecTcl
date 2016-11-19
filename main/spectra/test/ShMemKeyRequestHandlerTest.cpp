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

#include "ShMemKeyRequestHandler.h"
#include "GlobalSettings.h"
#include "TestUtils.h"

#include <QNetworkReply>
#include <QString>
#include <QByteArray>

#include <string>
#include <algorithm>
#include <iostream>
#include <stdexcept>

using namespace std;

class ShMemKeyRequestHandlerTest : public CppUnit::TestFixture {
  public:

  CPPUNIT_TEST_SUITE(ShMemKeyRequestHandlerTest);
  CPPUNIT_TEST(processSuccess_0);
  CPPUNIT_TEST(processSuccess_1);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() {
  }
  void tearDown() {
  }

protected:

  void processSuccess_0 () {

      QByteArray buffer("{ \"status\" : \"OK\", \"detail\" : \"XAee\" }");

      Viewer::ShMemKeyRequestHandler handler;
      handler.processSuccess(buffer);

      EQMSG("Shared memory key should be processed by rquest handler",
            QString("XAee"), Viewer::GlobalSettings::getSharedMemoryKey());
  }

  void processSuccess_1 () {

      // missing the last curly brace... should cause a failure
      QByteArray buffer("{ \"status\" : \"OK\", \"detail\" : \"XAee\"");

      Viewer::ShMemKeyRequestHandler handler;

      CPPUNIT_ASSERT_THROW_MESSAGE( "Bad json response should cause a failure",
                                    handler.processSuccess(buffer),
                                    std::runtime_error );
  }


};

CPPUNIT_TEST_SUITE_REGISTRATION(ShMemKeyRequestHandlerTest);

