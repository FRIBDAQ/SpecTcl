// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include <Asserts.h>

#include <CSqlite.h>
#include <CSqliteTransaction.h>
#include <CSqliteException.h>
#include <CSqliteTransaction.h>
#include <CSqliteStatement.h>


class SaveptTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SaveptTest);
  CPPUNIT_TEST(constructStarts);
  CPPUNIT_TEST(destructCommits);
  CPPUNIT_TEST(rollbackOnDestruction);
  CPPUNIT_TEST(immediateCommit);
  CPPUNIT_TEST(immediateRollback);
  
  // Tests on nested savepoints:
  
  CPPUNIT_TEST(innerCommit);
  CPPUNIT_TEST(innerCommitOuterRollback);
  CPPUNIT_TEST(innerRollbackOuterCommit);
  CPPUNIT_TEST(outerFirst);
  CPPUNIT_TEST_SUITE_END();


private:
    // these two connect to the same in-memory database:
    
    CSqlite* m_pDb1;
    CSqlite* m_pDb2;
public:
  void setUp() {
  // Sqlite docs claim this will create two connections to the same
  // in memory database.  Without the ?cache=shared and opening via URI
  //  we'd have two seaparate in-memory databases.
    
    int sqliteFlags =
        SQLITE_OPEN_SHAREDCACHE | SQLITE_OPEN_URI | SQLITE_OPEN_READWRITE;
    
    m_pDb1 = new CSqlite("file::memory:?cache=shared", sqliteFlags);
    m_pDb2 = new CSqlite("file::memory:?cache=shared", sqliteFlags);
    CSqliteStatement::execute(
    *m_pDb1,
    "CREATE TABLE IF NOT EXISTS g (                                            \
      id     INTEGER PRIMARY KEY NOT NULL,                       \
      text   VARCHAR(128)                                        \
    )"
  );                                  // Test table:
    CSqliteStatement::execute(
      *m_pDb1, "DELETE FROM g WHERE 1=1"
    );                                // IN case the :memory table persisted?!?
  }
  void tearDown() {
    delete m_pDb1;
    delete m_pDb2;
  }
protected:
  void constructStarts();
  void destructCommits();
  void rollbackOnDestruction();
  void immediateCommit();
  void immediateRollback();
  void innerCommit();
  void innerCommitOuterRollback();
  void innerRollbackOuterCommit();
  void outerFirst();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SaveptTest);


// Constructing a save point starts the save points changes in one
// handle should not be visible in the other (since they've not yet
// been comitted)

void SaveptTest::constructStarts() {
  
  
  CSqliteSavePoint savept(*m_pDb1, "Save1");
  CSqliteStatement::execute(
    *m_pDb1,
    "INSERT INTO g (text) VALUES('Hello World')"
  );
  /// We will either not see the transaction or get a table is locked exception:
  
  try {
    CSqliteStatement s(
      *m_pDb2, "SELECT COUNT(*) FROM g"
    );
    ++s;
    EQ(0, s.getInt(0));
  }
  catch (CSqliteException& e) {
    EQ(std::string("A table in the database is locked"), std::string(e.what()));
  }
  
  // Commit only happens here... if that works
}
// Destroying the savepoint commits the transaction:

void SaveptTest::destructCommits()
{
  {
    CSqliteSavePoint s(*m_pDb1, "save1");
    CSqliteStatement::execute(
      *m_pDb1,
      "INSERT INTO g (text) VALUES('Hello World')"
    );
  }                                          // Commit.
  CSqliteStatement s(
      *m_pDb2, "SELECT COUNT(*) FROM g"
  );
  ++s;
  EQ(1, s.getInt(0));
}

// Marking a savepoint for rollback means on destruction the inserts are gone:

void SaveptTest::rollbackOnDestruction()
{
  {
    CSqliteSavePoint s(*m_pDb1, "save1");
    s.scheduleRollback();
    CSqliteStatement::execute(
      *m_pDb1,
      "INSERT INTO g (text) VALUES('Hello World')"
    );                                          // Will get rolled back.


  }                                             // rollback.
   CSqliteStatement s(
      *m_pDb2, "SELECT COUNT(*) FROM g"
  );
  ++s;
  EQ(0, s.getInt(0));
  
}
// Committing in the block finishes the transaction:

void SaveptTest::immediateCommit()
{
  CSqliteSavePoint s(*m_pDb1, "save1");
  CSqliteStatement::execute(
    *m_pDb1,
    "INSERT INTO g (text) VALUES('Hello World')"
  );                                          // Will get rolled back.
  s.commit();                                 // commits now:
  CSqliteStatement q(
      *m_pDb2, "SELECT COUNT(*) FROM g"
  );
  ++q;
  EQ(1, q.getInt(0));
  
}
// rollback can be done immediately:

void SaveptTest::immediateRollback()
{
  CSqliteSavePoint s(*m_pDb1, "save1");
  CSqliteStatement::execute(
    *m_pDb1,
    "INSERT INTO g (text) VALUES('Hello World')"
  );                                          // Will get rolled back.
  s.rollback();                                 // commits now:
  CSqliteStatement q(
      *m_pDb2, "SELECT COUNT(*) FROM g"
  );
  ++q;
  EQ(0, q.getInt(0));
}
// Inner commit does not actually unlock the table:

void SaveptTest::innerCommit()
{
  CSqliteSavePoint s1(*m_pDb1, "Outer");
  CSqliteStatement::execute(
    *m_pDb1,
    "INSERT INTO g (text) VALUES('row1')"
  );
  
  CSqliteSavePoint s2(*m_pDb1, "Inner");
  CSqliteStatement::execute(
    *m_pDb1,
    "INSERT INTO g (text) VALUES ('row2')"
  );
  
  // Committing the inner transaction retains the lock on the table:
  
  s2.commit();
  
  CPPUNIT_ASSERT_THROW(
    CSqliteStatement::execute(*m_pDb2, "SELECT COUNT(*) FROM g"),
    CSqliteException
  );
  // Committing the outer transaction is what really does the work:
  
  s1.commit();
  CSqliteStatement q(*m_pDb2, "SELECT COUNT(*) FROM g");
  ++q;
  EQ(2, q.getInt(0));
}
// Committing an innter savepoint does nothing to save the data if the outer
// savepoint is rolled back?

void
SaveptTest::innerCommitOuterRollback()
{
  CSqliteSavePoint s1(*m_pDb1, "Outer");
  CSqliteStatement::execute(
    *m_pDb1,
    "INSERT INTO g (text) VALUES('row1')"
  );
  
  CSqliteSavePoint s2(*m_pDb1, "Inner");
  CSqliteStatement::execute(
    *m_pDb1,
    "INSERT INTO g (text) VALUES ('row2')"
  );
  
  // Committing the inner transaction retains the lock on the table:
  
  s2.commit();
  
  CPPUNIT_ASSERT_THROW(
    CSqliteStatement::execute(*m_pDb2, "SELECT COUNT(*) FROM g"),
    CSqliteException
  );
  // Committing the outer transaction is what really does the work:
  
  s1.rollback();
  CSqliteStatement q(*m_pDb2, "SELECT COUNT(*) FROM g");
  ++q;
  EQ(0, q.getInt(0));  
}

// Inner inner rollback/outercommit -- the stuff done in the outer transaction
// is retained:


void
SaveptTest::innerRollbackOuterCommit()
{
  CSqliteSavePoint s1(*m_pDb1, "Outer");
  CSqliteStatement::execute(
    *m_pDb1,
    "INSERT INTO g (text) VALUES('row1')"
  );
  
  CSqliteSavePoint s2(*m_pDb1, "Inner");
  CSqliteStatement::execute(
    *m_pDb1,
    "INSERT INTO g (text) VALUES ('row2')"
  );
  
  // Committing the inner transaction retains the lock on the table:
  
  s2.rollback();
  
  CPPUNIT_ASSERT_THROW(
    CSqliteStatement::execute(*m_pDb2, "SELECT COUNT(*) FROM g"),
    CSqliteException
  );
  // Committing the outer transaction is what really does the work:
  
  s1.commit();
  CSqliteStatement q(*m_pDb2, "SELECT COUNT(*) FROM g");
  ++q;
  EQ(1, q.getInt(0));    
}
// If outer first what happens to inner?  Should become a no-op.

void
SaveptTest::outerFirst()
{
    CSqliteSavePoint s1(*m_pDb1, "outer");
    CSqliteStatement::execute(
      *m_pDb1,
      "INSERT INTO g (text) VALUES('row1')"
    );
    CSqliteSavePoint s2(*m_pDb1, "inner");
    
    CSqliteStatement::execute(
    *m_pDb1,
    "INSERT INTO g (text) VALUES ('row2')"
  );
    
    s1.commit();
    
    CPPUNIT_ASSERT_THROW(
      s2.rollback(),
      CSqliteException
    );
    
  
    CSqliteStatement q(*m_pDb2, "SELECT COUNT(*) FROM g");
    ++q;
    EQ(2, q.getInt(0));    
}