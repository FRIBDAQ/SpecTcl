/**

#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2013.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Author:
#            Ron Fox
#            NSCL
#            Michigan State University
#            East Lansing, MI 48824-1321

##
# @file   statementTest.cpp
# @brief  tests for CSqliteStatement class.
# @author <fox@nscl.msu.edu>
*/

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "CSqlite.h"
#include "CSqliteStatement.h"
#include "CSqliteException.h"
#include <sqlite3.h>

#include <string>
#include <string.h>
#include <stdint.h>

class StatementTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(StatementTest);
  CPPUNIT_TEST(execute);
  
  CPPUNIT_TEST(constructok);
  CPPUNIT_TEST(constructfail);
  
  // Test bind variants. Note that since Sqlite is weakly typed
  // Binding the 'wrong type' of thing to a field will be accepted by it.
  // The binds follow the same template so error checking is only done
  // for the blobs (the first test).  
  
  CPPUNIT_TEST(bindIndex);
  CPPUNIT_TEST(bindBlob);
  CPPUNIT_TEST(bindBlobBadIndex);
  CPPUNIT_TEST(bindDouble);
  CPPUNIT_TEST(bindInt);
  CPPUNIT_TEST(bindInt64);
  CPPUNIT_TEST(bindNull);
  CPPUNIT_TEST(bindText);
  CPPUNIT_TEST(bindNullBlob);
  CPPUNIT_TEST(clearBindings);

  // Tests for the various iteration supporting methods:
  
  CPPUNIT_TEST(stepTest);
  CPPUNIT_TEST(endTest);
  CPPUNIT_TEST(reset);
  
  
  // Tests below are for column getters.
  
  CPPUNIT_TEST(blob);
  CPPUNIT_TEST(getDouble);
  CPPUNIT_TEST(getInt);
  CPPUNIT_TEST(getInt64);
  CPPUNIT_TEST(getText);
  
  // Result set properties:
  
  CPPUNIT_TEST(columnType);
  CPPUNIT_TEST(colbytes);
  CPPUNIT_TEST(colbytes16);
  
  // Statement properties:
  
  CPPUNIT_TEST(getsql);
  CPPUNIT_TEST(lastid);
  
  // End of all tests.
  
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void execute();
  
  void constructok();
  void constructfail();
  
  void bindIndex();
  void bindBlob();
  void bindBlobBadIndex();
  void bindDouble();
  void bindInt();
  void bindInt64();
  void bindNull();
  void bindText();
  void bindNullBlob();
  void clearBindings();
  
  void stepTest();
  void endTest();
  void reset();
  
  void blob();
  void getDouble();
  void getInt();
  void getInt64();
  void getText();
  
  void columnType();
  void colbytes();
  void colbytes16();
  
  void getsql();
  void lastid();
};

CPPUNIT_TEST_SUITE_REGISTRATION(StatementTest);

void StatementTest::execute() {
    CSqlite db(":memory:");              // Memory databases don't need cleanup.
    
    CSqliteStatement::execute(
        db, "CREATE TABLE george (id INTEGER PRIMARY KEY NOT NULL)"
    );
    
    // See if I can drop the table the old fashioned way.
    
    sqlite3* pDb = db.connection();
    sqlite3_stmt* pStatement;
    int status = sqlite3_prepare_v2(pDb, "DROP TABLE george", -1, &pStatement, NULL);
    
    EQ(SQLITE_OK, status);
    
    status = sqlite3_step(pStatement);
    EQ(SQLITE_DONE, status);
    
    status = sqlite3_finalize(pStatement);
    EQ(SQLITE_OK, status);
}

void StatementTest::constructok()
{
    
    CSqlite db(":memory:");
    const char* pSql = "CREATE TABLE george (id INTEGER PRIMARY KEY)";
    CSqliteStatement stmt(db, pSql); // Legal on all new dbs.
    
    EQ(db.connection(), stmt.database().connection());
    
    sqlite3_stmt* s = stmt.statement();
    
    std::string sql(sqlite3_sql(s));
    EQ(std::string(pSql), sql);
    
}

void StatementTest::constructfail()
{
    CSqlite db(":memory:");
    
    CPPUNIT_ASSERT_THROW(CSqliteStatement(db, "THIS is not valid sql"), CSqliteException);
    
}

void StatementTest::bindIndex()
{
    CSqlite db(":memory:");
    
    // Make a table we can put stuff in
    
    CSqliteStatement::execute(
        db, "CREATE TABLE g (                  \
            id      INTEGER PRIMARY KEY,       \
            last    VARCHAR(255) NOT NULL,     \
            first   VARCHAR(255) NOT NULL      \
        )");
    
    CSqliteStatement stmt(
        db, "INSERT INTO g (last, first) VALUES(:last, :first)"
    );
    
    EQ(1, stmt.bindIndex(":last"));
    EQ(2, stmt.bindIndex(":first"));
    EQ(0, stmt.bindIndex(":nosuch"));
    
    
}
void StatementTest::bindBlob()
{
    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  BLOB                    \
        )"
    );
    
    // Now insert a blob using bindings.. the blob will be a bunch of integers:
    
    int blob[100];
    for (int i =0; i  < 100; i++) blob[i] = i;
    CSqliteStatement s(
        db, "INSERT INTO g (id, data) VALUES (1, :data)"    
    );
    s.bind(1, &blob, static_cast<int>(sizeof(blob)), SQLITE_TRANSIENT);
    
    // Execute the statement:
    
    int status = sqlite3_step(s.statement());
    EQ(SQLITE_DONE, status);
    
    // Retrieve the data:
    
    CSqliteStatement s2(
        db, "SELECT data FROM g WHERE id=1"
    );
    sqlite3_stmt* pS = s2.statement();
    EQ(SQLITE_ROW, sqlite3_step(pS));  // Can now get the data
    
    const int* gotblob = reinterpret_cast<const int*>(sqlite3_column_blob(pS, 0));
    ASSERT(gotblob != NULL);
    
    for (int i =0; i < 100; i++) {
        EQ(blob[i], gotblob[i]);
    }
    
    EQ(SQLITE_DONE, sqlite3_step(pS));
    
}

void StatementTest::bindBlobBadIndex()
{
    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  BLOB                    \
        )"
    );
        // Now insert a blob using bindings.. the blob will be a bunch of integers:
    
    int blob[100];
    for (int i =0; i  < 100; i++) blob[i] = i;
    CSqliteStatement s(
        db, "INSERT INTO g (id, data) VALUES (1, :data)"    
    );
    
    CPPUNIT_ASSERT_THROW(
        s.bind(2, &blob, static_cast<int>(sizeof(blob)), SQLITE_TRANSIENT),
        CSqliteException);
    
}
void StatementTest::bindDouble()
{
    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  REAL                    \
        )"
    );
    
    // now do an insertion with a bound float:
    
    CSqliteStatement s(
        db, "INSERT INTO g (id, data) VALUES (1, :data)"
    );
    double pi=3.14159;
    s.bind(1, pi);
 
    int status = sqlite3_step(s.statement());
    EQ(SQLITE_DONE, status);
    
    // Retrieve the data:
    
    CSqliteStatement s2(
        db, "SELECT data FROM g WHERE id=1"
    );
    sqlite3_stmt* pS = s2.statement();
    EQ(SQLITE_ROW, sqlite3_step(pS));  // Can now get the data   
    
    double sbpi = sqlite3_column_double(pS, 0);
    
    EQ(pi, sbpi);
    
    
}

void StatementTest::bindInt()
{
    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  INTEGER                   \
        )"
    );
    // Do an insertion with a bound integer:

    CSqliteStatement s(
        db, "INSERT INTO g (id, data) VALUES (1, :data)"
    );
    int answer=42;
    s.bind(1, answer);
 
    int status = sqlite3_step(s.statement());
    EQ(SQLITE_DONE, status);
    
    // Try to retreive it backout:
    
    CSqliteStatement s2(
        db, "SELECT data FROM g WHERE id=1"
    );
    sqlite3_stmt* pS = s2.statement();
    EQ(SQLITE_ROW, sqlite3_step(pS));  // Can now get the data   
    
    int sbanswer = sqlite3_column_int(pS, 0);
    
    EQ(answer, sbanswer);   
    
}

void StatementTest::bindInt64()
{
    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  BIGINT                   \
        )"
    );
    
    // insert inot the table with a 64 bit value:
    
    CSqliteStatement s(
        db, "INSERT INTO g (id, data) VALUES (1, :data)"
    );
    int64_t data = 0x123456787654321;           // a nice 64 bit value.
    s.bind(1, data);
    int status = sqlite3_step(s.statement());
    EQ(SQLITE_DONE, status);
    
    // Try to retreive it backout:
    
    CSqliteStatement s2(
        db, "SELECT data FROM g WHERE id=1"
    );
    sqlite3_stmt* pS = s2.statement();
    EQ(SQLITE_ROW, sqlite3_step(pS));  // Can now get the data   
    int64_t sbdata = sqlite3_column_int64(pS, 0);
    EQ(data, sbdata);
    
}
void StatementTest::bindNull()
{
    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  BIGINT                   \
        )"
    );
    
    // insert inot the table with a null value:
    
    CSqliteStatement s(
        db, "INSERT INTO g (id, data) VALUES (1, :data)"
    );
    
    s.bind(1);               // Explicitly bind to null.
    int status = sqlite3_step(s.statement());
    EQ(SQLITE_DONE, status);
    
    // Fetch out the record -- data shouild be null.
    
       // Try to retreive it backout:
    
    CSqliteStatement s2(
        db, "SELECT data FROM g WHERE id=1"
    );
    sqlite3_stmt* pS = s2.statement();
    EQ(SQLITE_ROW, sqlite3_step(pS));  // Can now get the data
    
    EQ(SQLITE_NULL, sqlite3_column_type(pS, 0));
    EQ(
        reinterpret_cast<const unsigned char*>(NULL),
        sqlite3_column_text(pS, 0)
    );
}

void StatementTest::bindText()
{
    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  VARCHAR(128)            \
        )"
    );
    // Insert a record with some string in the data:
    
    CSqliteStatement s(
        db, "INSERT INTO g (id, data) VALUES (1, :data)"
    );   
    std::string data="Some String";
    s.bind(1, data.c_str(), -1, SQLITE_TRANSIENT);
    int status = sqlite3_step(s.statement());
    EQ(SQLITE_DONE, status);
    
    // Fetch out the record -- data shouild be null.
    
       // Try to retreive it backout:
    
    CSqliteStatement s2(
        db, "SELECT data FROM g WHERE id=1"
    );
    sqlite3_stmt* pS = s2.statement();
    EQ(SQLITE_ROW, sqlite3_step(pS));  // Can now get the data
    std::string sbdata = reinterpret_cast<const char*>(sqlite3_column_text(pS, 0));
    EQ(data, sbdata);
     
}

void StatementTest::bindNullBlob()
{
    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  BLOB            \
        )"
    );
    // Insert a record with some string in the data:
    
    CSqliteStatement s(
        db, "INSERT INTO g (id, data) VALUES (1, :data)"
    );
    s.bind(1, static_cast<uint64_t>(1234));            // 1234 bytes of 0 in the blob.
    EQ(SQLITE_DONE, sqlite3_step(s.statement()));
    
    // Retrieve the row:
    
    CSqliteStatement s2(
        db, "SELECT data FROM g WHERE id=1"
    );
    sqlite3_stmt* pS = s2.statement();
    EQ(SQLITE_ROW, sqlite3_step(pS));
    EQ(SQLITE_BLOB, sqlite3_column_type(pS, 0));  // Col must be a blob.
    EQ(1234, sqlite3_column_bytes(pS, 0));
    
    // no point in actually getting the BLOB for now.
    
}

void StatementTest::clearBindings()
{
    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  INTEGER                 \
        )"
    );
    
    // Insert a record. First bind an int, then clear the
    // bindings then do the insert.  Sqlite3 says the resulting binding is a NULL
 
    CSqliteStatement s(
        db, "INSERT INTO g (id, data) VALUES (1, :data)"
    );
    s.bind(1, 1234);            // 1234 bytes of 0 in the blob.
    s.clearBindings();
    
    EQ(SQLITE_DONE, sqlite3_step(s.statement()));
    
    // See that we inserted a null:
    
   CSqliteStatement s2(
        db, "SELECT data FROM g WHERE id=1"
    );
    sqlite3_stmt* pS = s2.statement();
    EQ(SQLITE_ROW, sqlite3_step(pS));
    EQ(SQLITE_NULL, sqlite3_column_type(pS, 0));  // Col must be a NULL
     
    
}
// Step is operator++()
void StatementTest::stepTest()
{
    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  INTEGER                 \
        )"
    );
    
    // Insert two records (iteration is too easy if there's only one)
    // This also lets us test operator++ in statements other than select.
    CSqliteStatement s(
        db, "INSERT INTO g (data) VALUES (:data)"
    );
    sqlite3_stmt* pS0 = s.statement();
    s.bind(1, 1234);
    ++s;                                     // step.
    s.reset();
    s.bind(1, 5678);                         // Do I need to reset??
    ++s;                                     // Step.
    
    // Now get the data -- should be able to get two records...but we've not
    // yet got atEnd() working/tested so we have to take it on faity:
    
    CSqliteStatement s1(db,
        "SELECT data FROM g ORDER BY id ASC"
    );
    sqlite3_stmt* pS = s1.statement();
    ++s1;                             // First record should be fetchable:
    
    EQ(1234, sqlite3_column_int(pS, 0));
    
    ++s1;                             // Should be able to get the second record:
    EQ(5678, sqlite3_column_int(pS, 0));
    
}

void StatementTest::endTest()
{
    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  INTEGER                 \
        )"
    );
    
    // Insert a record:
    
    CSqliteStatement s(
        db, "INSERT INTO g (data) VALUES (:data)"
    );
    sqlite3_stmt* pS0 = s.statement();
    s.bind(1, 1234);
    ++s;
    
    // When getting the record, s++.end() should return false.
    //  After getting the record s++.end() should return true
    
    CSqliteStatement s1(db, "SELECT data FROM g"
    );
    ASSERT(!((++s1).atEnd()));
    sqlite3_stmt* pS = s1.statement();
    EQ(1234, sqlite3_column_int(pS, 0));
    
    ASSERT((++s1).atEnd());
}

void StatementTest::reset()
{
    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  INTEGER                 \
        )"
    );
    
    // Insert a record:
    
    CSqliteStatement s(
        db, "INSERT INTO g (data) VALUES (:data)"
    );
    sqlite3_stmt* pS0 = s.statement();
    s.bind(1, 1234);
    ++s;
    
    // When getting the record, s++.end() should return false.
    //  After getting the record s++.end() should return true
    
    CSqliteStatement s1(db, "SELECT data FROM g"
    );
    ASSERT(!((++s1).atEnd()));
    sqlite3_stmt* pS = s1.statement();
    EQ(1234, sqlite3_column_int(pS, 0));
    
    // Resetting should allow me to fetch the same data:
    
    s1.reset();
    ASSERT(!((++s1).atEnd()));
    EQ(1234, sqlite3_column_int(pS, 0));
}


void StatementTest::colbytes()
{
    const char* pData = "Here's the data string";

    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  INTEGER                 \
        )"
    );
    
    // Insert a record:
    
    CSqliteStatement s(
        db, "INSERT INTO g (id, data) VALUES (1, :data)"
    );
    s.bind(1, pData, -1, SQLITE_TRANSIENT);
    ++s;
    
    // Find it and ensure that the column size is
    // the length of the string:
    
    CSqliteStatement s1(
        db, "SELECT data FROM g WHERE id = 1"
    );
    ++s1;
    EQ(strlen(pData), static_cast<size_t>(s1.bytes(0)));
}

void StatementTest::colbytes16()
{
    const char* pData = "Here's the data string";

    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  INTEGER                 \
        )"
    );
    
    // Insert a record:
    
    CSqliteStatement s(
        db, "INSERT INTO g (id, data) VALUES (1, :data)"
    );
    s.bind(1, pData, -1, SQLITE_TRANSIENT);
    ++s;
    
    // Find it and ensure that the column size is
    // the length of the string:
    
    CSqliteStatement s1(
        db, "SELECT data FROM g WHERE id = 1"
    );
    ++s1;
    
    // UTF-16 has 2 bytes for each ASCII character
    
    EQ(strlen(pData)*2, static_cast<size_t>(s1.bytes16(0)));
    
}

void StatementTest::blob()
{
   

    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  BLOB                 \
        )"
    );
    
    // Insert a record:
    
    CSqliteStatement s(
        db, "INSERT INTO g (id, data) VALUES (1, :data)"
    );
    int blob[100];
    for (int i = 0; i < 100; i++) blob[i] = i;
    s.bind(1, blob, static_cast<int>(sizeof(blob)), SQLITE_TRANSIENT);
    ++s;
    
    // Find it and ensure that the column size is
    // the length of the string:
    
    CSqliteStatement s1(
        db, "SELECT data FROM g WHERE id = 1"
    );
    ++s1;
    
    const int* outBlob;
    int  blobSize;
    
    blobSize = s1.bytes(0);
    EQ(sizeof(blob), static_cast<size_t>(blobSize));
    outBlob = reinterpret_cast<const int*>(s1.getBlob(0));
    for (int i = 0; i < 100; i++) {
        EQ(blob[i], outBlob[i]);
    }
    
}

void StatementTest::getDouble()
{
    

    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  DOUBLE PRECISION        \
        )"
    );
    
    // Insert a record:
    
    CSqliteStatement s(
        db, "INSERT INTO g (id, data) VALUES (1, :data)"
    );
    double f = 3.14159265359;
    s.bind(1, f);
    ++s;
    
    CSqliteStatement s1(
        db, "SELECT data FROM g WHERE id = 1"
    );
    ++s1;
    EQ(f, s1.getDouble(0));    
    
    
}
void StatementTest::getInt()
{
    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  INTEGER        \
        )"
    );
    
    // Insert a record:
    
    CSqliteStatement s(
        db, "INSERT INTO g (id, data) VALUES (1, :data)"
    );
    int value = 12345;
    s.bind(1, value);
    ++s;
    
    // Get the data back:
    
    CSqliteStatement s1(
        db, "SELECT data FROM g WHERE id = 1"
    );
    ++s1;
    EQ(value, s1.getInt(0));
}

void StatementTest::getInt64()
{
    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  BIGINT        \
        )"
    );
    
    // Insert a record:
    
    CSqliteStatement s(
        db, "INSERT INTO g (id, data) VALUES (1, :data)"
    );
    int64_t data = 0x123456787654321L;              // Needs 64 bits to fit.
    s.bind(1, data);
    ++s;
    
    // Get the data back:
    
    CSqliteStatement s1(
        db, "SELECT data FROM g WHERE id = 1"
    );
    ++s1;
    EQ(data, s1.getInt64(0));
}
void StatementTest::getText()
{
    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  VARCHAR(128)       \
        )"
    );
    
    // Insert a record:
    
    CSqliteStatement s(
        db, "INSERT INTO g (id, data) VALUES (1, :data)"
    );
    const char* pData = "This is a test record";
    s.bind(1, pData, -1, SQLITE_TRANSIENT);
    ++s;
    
    
    // Get the data back:
    
    CSqliteStatement s1(
        db, "SELECT data FROM g WHERE id = 1"
    );
    ++s1;
    EQ(
        std::string(pData),
        std::string(reinterpret_cast<const char*>(s1.getText(0)))
    );
}    
void StatementTest::columnType()
{
    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  VARCHAR(128)       \
        )"
    );
    
    // Insert a record:
    
    CSqliteStatement s(
        db, "INSERT INTO g (id, data) VALUES (1, :data)"
    );
    const char* pData = "This is a test record";
    s.bind(1, pData, -1, SQLITE_TRANSIENT);
    ++s;    

    // Get the data back:
    
    CSqliteStatement s1(
        db, "SELECT data FROM g WHERE id = 1"
    );
    ++s1;
    EQ(SQLITE_TEXT, s1.columnType(0));
}

void StatementTest::getsql()
{
    const char* statement =  "CREATE TABLE g (                 \
        id    INTEGER PRIMARY KEY,    \
        data  VARCHAR(128)       \
    )";
    CSqlite db(":memory:");
    CSqliteStatement s(db, statement);
    
    EQ(std::string(statement), std::string(s.sql()));
    
}
void StatementTest::lastid()
{
    CSqlite db(":memory:");
    CSqliteStatement::execute(
        db,
        "CREATE TABLE g (                 \
            id    INTEGER PRIMARY KEY,    \
            data  VARCHAR(128)       \
        )"
    );
    
    // Insert a record:
    
    CSqliteStatement s(
        db, "INSERT INTO g (id, data) VALUES (1, :data)"
    );
    const char* pData = "This is a test record";
    s.bind(1, pData, -1, SQLITE_TRANSIENT);
    ++s;
    
    EQ(1, s.lastInsertId());
}