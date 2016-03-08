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

