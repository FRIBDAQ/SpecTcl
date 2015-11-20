// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#define private public		// Hack to get at internals for white box tests.
#include "CDataSourceFactory.h"
#undef private

#include "CDataSource.h"	// For my own test data sources
#include "CDataSourceCreator.h" // ""          ""   data source creators.

class DSFactory : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(DSFactory);
  CPPUNIT_TEST(construct);
  CPPUNIT_TEST(idempotency);

  CPPUNIT_TEST(addCreator);
  CPPUNIT_TEST(duplicateCreator);

  CPPUNIT_TEST(removenexist);
  CPPUNIT_TEST(remove);
  CPPUNIT_TEST(removecorrect);

  CPPUNIT_TEST(existsno);
  CPPUNIT_TEST(existsyes);

  CPPUNIT_TEST(createnxprotocol);
  CPPUNIT_TEST(createfoundprotocol);

  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
    CDataSourceFactory::clearCreators(); // Ensure next test has no creators.
  }
protected:			
  void construct();
  void idempotency();

  void addCreator();
  void duplicateCreator();

  void removenexist();
  void remove();
  void removecorrect();

  void existsno();
  void existsyes();

  void createnxprotocol();
  void createfoundprotocol();
};

CPPUNIT_TEST_SUITE_REGISTRATION(DSFactory);



// Construction in _this_ test suite should give an empty set of creators.

void DSFactory::construct() {
  CDataSourceFactory fact;
  ASSERT(fact.m_creators.empty());
}

/**
 * idempotency
 *    For the tests to be idempotent, the clearCreators method must
 *    remove all items from m_creators.  
 *    Since we've not tested addCreator, we're going to insert one by hand with a null creator.
 */
void DSFactory::idempotency() 
{
  CDataSourceFactory fact;

  fact.m_creators["test"]  = 0;
  EQ(static_cast<size_t>(1), fact.m_creators.size());

  CDataSourceFactory::clearCreators();
  ASSERT(fact.m_creators.empty());
}


/**
 * addCreator
 *   Add a null creator for the null protocol
 *  * map size should be 1.
 *  * Should be able to find the creator in the map.
 *  * Pointer should point properly.
 */
class CNullCreator : public CDataSourceCreator
{
public:
  virtual CDataSource* create() {return 0;}
};

void DSFactory::addCreator()
{
  CDataSourceFactory fact;
  CDataSourceCreator* pC = new CNullCreator;

  fact.addCreator("null", pC);

  EQ(static_cast<size_t>(1), fact.m_creators.size());

  CDataSourceFactory::CreatorIterator p = fact.m_creators.find("null");
  ASSERT(p != fact.m_creators.end());
  EQ(pC, p->second);
}

/**
 * duplicateCreator
 *
 *  Adding a duplicate creator should throw data_source_factory_exception.
 */
void
DSFactory::duplicateCreator()
{

  CDataSourceFactory fact;
  CDataSourceCreator* pC = new CNullCreator;

  fact.addCreator("null", pC);	//  This is ok.

  bool threw= false;
  bool rightException = false;

  try {
    fact.addCreator("null", pC); // This should fail.
  }
  catch(data_source_factory_exception& e) {
    threw = true;
    rightException = true;
  }
  catch(...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(rightException);

}
/**
 *  removenexist
 *
 *   Removing a nonexistent creator shouild throw a data_source_factory_exception.
 *
 */
void
DSFactory::removenexist()
{
  CDataSourceFactory fact;
  

  bool threw= false;
  bool rightException = false;

  try {
    fact.removeCreator("null");
  }
  catch(data_source_factory_exception& e) {
    threw = true;
    rightException = true;
  }
  catch(...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(rightException);

}
/**
 * remove
 *   Removing the only creator should result in an empty set of creators
 */
void
DSFactory::remove()
{
  CDataSourceFactory fact;
  CDataSourceCreator* pC = new CNullCreator;

  fact.addCreator("null", pC);

  fact.removeCreator("null");

  EQ(static_cast<size_t>(0), fact.m_creators.size());

}
/**
 * removecorrect
 *
 *   Ensure that if there are several creators remove gets the right one.
 */
void
DSFactory::removecorrect()
{
  CDataSourceFactory fact;
  CDataSourceCreator* pC1 = new CNullCreator;
  CDataSourceCreator* pC2 = new CNullCreator;

  fact.addCreator("null1", pC1);
  fact.addCreator("null2", pC2);

  fact.removeCreator("null1");

  // Still one left:

  EQ(static_cast<size_t>(1), fact.m_creators.size());

  // Can find the null2 one:

  CDataSourceFactory::CreatorIterator p = fact.m_creators.find("null2");
  ASSERT(p != fact.m_creators.end());
  EQ(pC2, p->second);

}

/**
 * existsno
 *
 *  Asking creatorExists about one that's not there should return false
 */
void
DSFactory::existsno()
{
  CDataSourceFactory fact;
  ASSERT(!fact.creatorExists("null"));
}
/**
 * existsyes
 *
 *  asking creatorExists on one that does returns true:
 */
void
DSFactory::existsyes()
{
  CDataSourceFactory fact;
  CDataSourceCreator* pC = new CNullCreator;
  fact.addCreator("null", pC);

  ASSERT(fact.creatorExists("null"));
}

/**
 * createnxprotocol
 *
 *   Create for a nonexistent protocol should fail.
 */
void
DSFactory::createnxprotocol()
{
  CDataSourceFactory fact;

  EQ(reinterpret_cast<CDataSource*>(0), fact.create("null"));


}
/**
 * createfoundprotocol
 *
 * In this case we're going to add a creator that makes a simple
 * data source, register it with the null protocol and
 * ask the factory for a null
 *
 *  At this point we just the ds is not null.
 */

class CTestDataSource : public CDataSource
{
  virtual void    onAttach(CTCLInterpreter& interp, const char* pSource) {}
  virtual void    createEvent(CTCLInterpreter& interp, ChannelHandler* pHandler, void* p) {}
  virtual size_t  read(void* pBuffer, size_t nBytes) {return 0;}
  virtual void    close() {}
  virtual bool    isEof() {return true;}
};

class CTestSourceCreator : public CDataSourceCreator
{
public:
  CDataSource* create() {
    return new CTestDataSource;
  }
};

void
DSFactory::createfoundprotocol()
{
  CDataSourceFactory fact;
  CTestSourceCreator* pC = new CTestSourceCreator;

  fact.addCreator("null", pC);
  
  CDataSource* pDs = fact.create("null");

  ASSERT(pDs);

  delete pDs;
}
