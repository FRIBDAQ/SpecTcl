// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "CSqlite.h"
#include "CSqliteStatement.h"
#include "CSqliteTransaction.h"
#include "CSqliteException.h"

static std::string message("This is the error message");

class TransactionTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TransactionTest);
  
  // Test the exception internal class:
  
  CPPUNIT_TEST(ExceptionMsg);
  CPPUNIT_TEST(ExceptionCopy);
  CPPUNIT_TEST(ExceptionAssign);
  
  // Tests for the CSqliteTransaction class
  
  CPPUNIT_TEST(ConstructBegin);
  CPPUNIT_TEST(DestructCommit);
  CPPUNIT_TEST(DestructRollback);
  CPPUNIT_TEST(ImmediateCommit);
  CPPUNIT_TEST(ImmediateRollback);
  
  // Error detection.
  
  CPPUNIT_TEST(CommitWhenCompleted);
  CPPUNIT_TEST(CommitWhenRollbackPending);
  CPPUNIT_TEST(RollbackWhenCompleted);
  CPPUNIT_TEST(ScheduleRollbackWhenCompleted);
  CPPUNIT_TEST_SUITE_END();


private:
    // these two connect to the same in-memory database:
    
    CSqlite* m_pDb1;
    CSqlite* m_pDb2;
public:
  void setUp() {
    // Sqlite docs claim this will create two connections to the same
    // in memory database:
    
    int sqliteFlags =
        SQLITE_OPEN_SHAREDCACHE | SQLITE_OPEN_URI | SQLITE_OPEN_READWRITE;
    
    m_pDb1 = new CSqlite("file::memory:?cache=shared", sqliteFlags);
    m_pDb2 = new CSqlite("file::memory:?cache=shared", sqliteFlags); 
  }
  void tearDown() {
    delete m_pDb1;
    delete m_pDb2;
  }
protected:
  void ExceptionMsg();
  void ExceptionCopy();
  void ExceptionAssign();
  
  void ConstructBegin();
  void DestructCommit();
  void DestructRollback();
  void ImmediateCommit();
  void ImmediateRollback();
  
  void CommitWhenCompleted();
  void CommitWhenRollbackPending();
  void RollbackWhenCompleted();
  void ScheduleRollbackWhenCompleted();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TransactionTest);

void TransactionTest::ExceptionMsg() {
    // The transaction exception just holds an error string:
    
    CSqliteTransaction::CException e(message);
    EQ(message, std::string(e.what()));
}
void TransactionTest::ExceptionCopy() {
    CSqliteTransaction::CException e1(message);
    CSqliteTransaction::CException e2(e1);
    EQ(message, std::string(e2.what()));
}
void TransactionTest::ExceptionAssign()
{
    CSqliteTransaction::CException e1(message);
    CSqliteTransaction::CException e2("");
    CSqliteTransaction::CException e3("");
    
    /// The test below also ensures the *this is returned:
    
    e3 = e2 = e1;
    EQ(message, std::string(e2.what()));
    EQ(message, std::string(e3.what()));
}

void TransactionTest::ConstructBegin()
{
    // If a transaction begins with the creation of a CSqliteTransaction
    // then operations in the transaction should not be visible to other connections
    // in the database while the transaction is active:
    
    CSqliteStatement::execute(*m_pDb1,
        "CREATE TABLE g (   \
            id     INTEGER PRIMARY KEY NOT NULL, \
            text   VARCHAR(128)                  \
        )"
    );                                             // Visible in both connections.
    CSqliteTransaction t(*m_pDb1);
    CSqliteStatement::execute(
        *m_pDb1, "INSERT INTO g (text) VALUES('Hello World')"
                              
     );                                            // Should only be visible in db1
    
    //  Try to retrieve in db2 connection Since the write is pending I think
    //  we should either not see the transaction or have an exception thrown due
    //  to lock issues:
    
    try {
    
        CSqliteStatement s(
            *m_pDb2, "SELECT COUNT(*) AS c FROM g"
        );
        ++s;
        EQ(0, s.getInt(0));
        return;
    }
    catch (CSqliteException e) {
        EQ(std::string("A table in the database is locked"), std::string(e.what()));
        return;
    }
    
    CPPUNIT_FAIL("Fell through where we should not");
    
}

void TransactionTest::DestructCommit()
{
   
    CSqliteStatement::execute(*m_pDb1,
        "CREATE TABLE g (   \
            id     INTEGER PRIMARY KEY NOT NULL, \
            text   VARCHAR(128)                  \
        )"
    );                                             // Visible in both connections.
    {                                              // This is the scope of the
        CSqliteTransaction t(*m_pDb1);             // transaction
        CSqliteStatement::execute(
            *m_pDb1, "INSERT INTO g (text) VALUES('Hello World')"
                                  
         );
    }                                             // Auto commit.
   
    CSqliteStatement s(
        *m_pDb2, "SELECT COUNT(*) AS c FROM g"
    );
    ++s;
    EQ(1, s.getInt(0));    
    
}
void TransactionTest::DestructRollback()
{

    CSqliteStatement::execute(*m_pDb1,
        "CREATE TABLE g (   \
            id     INTEGER PRIMARY KEY NOT NULL, \
            text   VARCHAR(128)                  \
        )"
    );                                             // Visible in both connections.
    {                                              // This is the scope of the
        CSqliteTransaction t(*m_pDb1);             // transaction
        t.scheduleRollback(); 
        CSqliteStatement::execute(
            *m_pDb1, "INSERT INTO g (text) VALUES('Hello World')"
                                  
         );
    }                                             // Rollback here.
   
    CSqliteStatement s1(
        *m_pDb2, "SELECT COUNT(*) AS c FROM g"
    );
    ++s1;
    EQ(0, s1.getInt(0));    
    

}
void TransactionTest::ImmediateCommit()
{
    CSqliteStatement::execute(*m_pDb1,
        "CREATE TABLE g (   \
            id     INTEGER PRIMARY KEY NOT NULL, \
            text   VARCHAR(128)                  \
        )"
    );                                             // Visible in both connections.
    {                                              // This is the scope of the
        CSqliteTransaction t(*m_pDb1);             // transaction
        CSqliteStatement::execute(
            *m_pDb1, "INSERT INTO g (text) VALUES('Hello World')"                                  
         );
        t.commit();                                 // Commit here.
        CSqliteStatement s1(
            *m_pDb2, "SELECT COUNT(*) AS c FROM g"
        );
        ++s1;
        EQ(1, s1.getInt(0));    

    }                                               // No-op here.
    
}
void TransactionTest::ImmediateRollback()
{
    CSqliteStatement::execute(*m_pDb1,
        "CREATE TABLE g (   \
            id     INTEGER PRIMARY KEY NOT NULL, \
            text   VARCHAR(128)                  \
        )"
    );                                             // Visible in both connections.
    {                                              // This is the scope of the
        CSqliteTransaction t(*m_pDb1);             // transaction
        CSqliteStatement::execute(
            *m_pDb1, "INSERT INTO g (text) VALUES('Hello World')"                                  
         );
        t.rollback();                                 // Rollback here.
        CSqliteStatement s1(
            *m_pDb2, "SELECT COUNT(*) AS c FROM g"
        );
        ++s1;
        EQ(0, s1.getInt(0));    

    }                                               // No-op here.
    
}

void TransactionTest::CommitWhenCompleted()
{
    // committing when the transaction is in the completed state is an error
    // that throws CSqliteTransaction::CException.
    
    {                                              
        CSqliteTransaction t(*m_pDb1);             
        t.rollback();                // Completed.
        CPPUNIT_ASSERT_THROW(t.commit(), CSqliteTransaction::CException);
    }
    
}
void TransactionTest::CommitWhenRollbackPending()
{
    {                                              
        CSqliteTransaction t(*m_pDb1);             
        t.scheduleRollback();                // Completed.
        CPPUNIT_ASSERT_THROW(t.commit(), CSqliteTransaction::CException);
    }
    
}

void TransactionTest::RollbackWhenCompleted()
{
    CSqliteTransaction t(*m_pDb1);
    t.commit();                             // Commit transaction.
    CPPUNIT_ASSERT_THROW(t.rollback(), CSqliteTransaction::CException);
}

void TransactionTest::ScheduleRollbackWhenCompleted()
{
    CSqliteTransaction t(*m_pDb1);
    t.commit();
    CPPUNIT_ASSERT_THROW(t.scheduleRollback(), CSqliteTransaction::CException);
}