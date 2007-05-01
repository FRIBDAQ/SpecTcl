// Template for a test suite.


#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include <string>


#include "Dictionary.h"

using namespace std;

static int counter = 0;

static int addItem(-1);
static int removeItem(-1);

static string addName;
static string removeName;

class MyObserver : public DictionaryObserver<int> 
{
  virtual void onAdd(const STD(string)& name,
		     const int&         item) {
    counter++;
    addItem = item;
    addName = name;
  }
  virtual void onRemove(const STD(string)& name,
			const int&         item) {
    counter--;
    removeItem = item;
    removeName = name;
  }
};



class TestDictObserver : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TestDictObserver);
  //
  // Tests of the observer manager:
  //
  CPPUNIT_TEST(testEmptyManager);
  CPPUNIT_TEST(testManagerWith1);
  CPPUNIT_TEST(testRemove);
  CPPUNIT_TEST(testABunch);
  CPPUNIT_TEST(testParams);
  //
  // Full tests of the dictionary.
  //
  CPPUNIT_TEST(testDictionaryNoObs);
  CPPUNIT_TEST(testDictionaryWithObs);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
    counter    = 0;
    addItem    = -1;
    removeItem = -1;
    addName    = "";
    removeName = "";

  }
  void tearDown() {
  }
protected:
  void testEmptyManager();
  void testManagerWith1();
  void testRemove();
  void testABunch();
  void testParams();

  void testDictionaryNoObs();
  void testDictionaryWithObs();
};




CPPUNIT_TEST_SUITE_REGISTRATION(TestDictObserver);

void TestDictObserver::testEmptyManager() {
  CDictionaryObserverManager<int> manager;
  string name("a");
  int    i(0);
  manager.invokeAdd(name, i);
  EQMSG("Invoke add on empty manager", 0, counter);

  manager.invokeRemove(name, i);
  EQMSG("Invoke remove on empty manager", 0, counter);
}

void TestDictObserver::testManagerWith1()
{
  CDictionaryObserverManager<int> manager;
  MyObserver                      observer;
  string name("a");
  int    i(0);

  manager.addObserver(&observer);

  manager.invokeAdd(name, i);
  EQMSG("Add with one observer", 1, counter);

  manager.invokeRemove(name, i);
  EQMSG("Remove with one observer", 0, counter);

}

void TestDictObserver::testRemove()
{
  CDictionaryObserverManager<int> manager;
  MyObserver                      observer;
  string name("a");
  int    i(0);

  manager.addObserver(&observer);

  manager.invokeAdd(name, i);
  EQMSG("Add with one observer (before remove)", 1, counter);

  manager.removeObserver(&observer);
  manager.invokeRemove(name, i);
  EQMSG("Remove after observers removed", 1, counter);
  manager.invokeAdd(name, i);
  EQMSG("Add after observers removed", 1, counter);

}

void TestDictObserver::testABunch()
{
  CDictionaryObserverManager<int> manager;
  MyObserver                      obs1, obs2, obs3;
  string name("a");
  int    i(0);

  manager.addObserver(&obs1);
  manager.addObserver(&obs2);
  manager.addObserver(&obs3);

  manager.invokeAdd(name, i);
  EQMSG("Add with 3 observers: ", 3, counter);

  manager.removeObserver(&obs2);
  manager.invokeRemove(name, i);
  EQMSG("Remove with 2 observers: ", 1, counter);


}

void TestDictObserver::testParams()
{
  CDictionaryObserverManager<int> manager;
  string name("a");
  int    i(123);
  MyObserver obs;

  manager.addObserver(&obs);
  manager.invokeAdd(name, i);
  manager.invokeRemove(name, i);

  EQMSG("addItem", i, addItem);
  EQMSG("addName", name, addName);

  EQMSG("removeItem", i , removeItem);
  EQMSG("removeName", name, removeName);
  

}

void TestDictObserver::testDictionaryNoObs()
{
  CDictionary<int> dict;
  string key("a");
  int    item(0);
  dict.Enter(key, item);

  EQMSG("add no observers", 0, counter);

  dict.Remove(key);
  EQMSG("remove no observers", 0, counter);
  
}
void TestDictObserver::testDictionaryWithObs()
{
  CDictionary<int> dict;
  MyObserver       obs;
  string key("a");
  int    item(1234);

  dict.addObserver(&obs);

  dict.Enter(key, item);
  EQMSG("add with obs", 1, counter);
  EQMSG("AddItem",      item, addItem);
  EQMSG("addName",      key, addName);

  dict.Remove(key);
  EQMSG("remove with obs", 0, counter);
  EQMSG("removeItem",  item, removeItem);
  EQMSG("removeName",  key,  removeName);
}
