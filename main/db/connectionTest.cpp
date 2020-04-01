// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CSqlite.h"
#include "CSqliteException.h"
#include <sqlite3.h>

#include <string>


class ConnectionTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ConnectionTest);
  
  CPPUNIT_TEST(simpleConstruct);
  CPPUNIT_TEST(simpleConstructFail);
  CPPUNIT_TEST(constructReadonly);
  
  CPPUNIT_TEST(versionString);
  CPPUNIT_TEST(versionNumber);

  CPPUNIT_TEST(tblExistsNo);
  CPPUNIT_TEST(tblExistsYes);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void simpleConstruct();
  void simpleConstructFail();
  void constructReadonly();
  
  void versionString();
  void versionNumber();

  void tblExistsNo();
  void tblExistsYes();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ConnectionTest);

// Tests of the simple constructor.

void ConnectionTest::simpleConstruct() {
    CSqlite db(":memory:");      // So we don't need special files.

    // Get the connection and do something harmless and simple:
    
    sqlite3* pDb = db.connection();
    
    sqlite3_stmt* pStatement;
    int status = sqlite3_prepare(pDb, "SELECT * from sqlite_master", -1, &pStatement, NULL);
    EQ(SQLITE_OK, status);
}

void ConnectionTest::simpleConstructFail()
{
    CPPUNIT_ASSERT_THROW(CSqlite("/this/file/does/not/exist"), CSqliteException);
}
// Test of the constructor with flags (try to make a readonly database)


void ConnectionTest::constructReadonly()
{
    CSqlite db(":memory:", CSqlite::readonly);
    sqlite3* pDb = db.connection();
    
    sqlite3_stmt* pStatement;
    int status = sqlite3_prepare_v2(
        pDb, "CREATE TABLE test (id INTEGER PRIMARY KEY NOT NULL)",
        -1, &pStatement, NULL
    );
    EQ(SQLITE_OK, status);             // preparing should not fail,.
    
    status = sqlite3_step(pStatement);
    EQ(SQLITE_READONLY, status);
    
}


void ConnectionTest::versionString()
{

    EQ(std::string(SQLITE_VERSION), std::string(CSqlite::version()));
}

void ConnectionTest::versionNumber()
{
    EQ(SQLITE_VERSION_NUMBER, CSqlite::versionNumber());
}
// tableExists when it does not:

void  ConnectionTest::tblExistsNo()
{
    CSqlite db(":memory:");
    
    ASSERT(!db.tableExists("NoSuchtable"));
}
// Table exists when it does:

void ConnectionTest::tblExistsYes()
{
    CSqlite db(":memory:");
    sqlite3* pDb = db.connection();
    
    sqlite3_stmt* pStatement;
    int status = sqlite3_prepare_v2(
        pDb, "CREATE TABLE test (id INTEGER PRIMARY KEY NOT NULL)",
        -1, &pStatement, NULL
    );
    EQ(SQLITE_OK, status);             // preparing should not fail,.
    
    status = sqlite3_step(pStatement);
    EQ(SQLITE_DONE, status);
    
    ASSERT(db.tableExists("test"));
}