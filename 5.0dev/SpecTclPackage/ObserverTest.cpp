// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#define private public
#define protected public
#include "CObservable.h"
#undef private
#undef protected

#include <vector>


class ObserverTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ObserverTest);
  CPPUNIT_TEST(empty);
  CPPUNIT_TEST(add1);
  CPPUNIT_TEST(add3iterate);
  CPPUNIT_TEST(add3removemid);
  CPPUNIT_TEST(observe);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void empty();
  void add1();
  void add3iterate();
  void add3removemid();
  void observe();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ObserverTest);

class TestObservable;
class TestObservable : public CObservable<TestObservable> 
{
};

std::vector<int> observations;
class TestObserver : public CObserver<TestObservable> 
{
  int m_id;
public:
  TestObserver(int id = 0) : m_id(id) {}
  virtual void operator()(CObservable<TestObservable>* pObservable) {
    observations.push_back(m_id);
  }
};

/*
 * An observable has an empty list of observers initially:
 */

void ObserverTest::empty() {
  TestObservable o;

  ASSERT(o.m_observers.empty());
}
/**
 * add1
 *  Adding an observer should make it show up in the list.
 */
void 
ObserverTest::add1()
{
  CObserver<TestObservable>* pO = new TestObserver;
  TestObservable o;
  o.addObserver(pO);

  ASSERT(!(o.m_observers.empty()));
  EQ(static_cast<size_t>(1), o.observerCount());

  TestObservable::ObserverIterator p = o.beginObservers();
  EQ(pO, *p);

  delete pO;

}
/**
 * add3iterate
 *    Add 3 observers, make sure that iteration produces them
 *    in the right order and ends.
 */
void
ObserverTest::add3iterate()
{
  CObserver<TestObservable>* p0 = new TestObserver;
  CObserver<TestObservable>* p1 = new TestObserver;
  CObserver<TestObservable>* p2 = new TestObserver;

  TestObservable o;
  o.addObserver(p0);
  o.addObserver(p1);
  o.addObserver(p2);

  EQ(static_cast<size_t>(3), o.observerCount());

  TestObservable::ObserverIterator p = o.beginObservers();
  EQ(p0, *p);
  p++;
  EQ(p1, *p);
  p++;
  EQ(p2, *p);
  p++;				// should be at end.

  ASSERT(p == o.endObservers());

  delete p0;
  delete p1;
  delete p2;
}

/**
 * add3removemid
 *
 *  Add 3 observers and eremove the middle one.. make sure
 *  the two remaining are in the right order.
 */
void
ObserverTest::add3removemid()
{
  CObserver<TestObservable>* p0 = new TestObserver;
  CObserver<TestObservable>* p1 = new TestObserver;
  CObserver<TestObservable>* p2 = new TestObserver;

  TestObservable o;
  o.addObserver(p0);
  o.addObserver(p1);
  o.addObserver(p2);

  o.removeObserver(p1);		// Remove the middle.

  EQ(static_cast<size_t>(2), o.observerCount());
  TestObservable::ObserverIterator p = o.beginObservers();
  EQ(p0, *p);
  p++;
  EQ(p2,*p);
  p++;
  ASSERT(p == o.endObservers());

  delete p0;
  delete p1;
  delete p2;
}
/**
 * observe
 *   Trigger the observation and ensure it happens in the right order.
 */
void
ObserverTest::observe()
{
  CObserver<TestObservable>* p0 = new TestObserver(1);
  CObserver<TestObservable>* p1 = new TestObserver(2);
  CObserver<TestObservable>* p2 = new TestObserver(3);

  TestObservable o;
  o.addObserver(p0);
  o.addObserver(p1);
  o.addObserver(p2);

  observations.clear();
  o.observe();

  EQ(static_cast<size_t>(3), observations.size());
  EQ(1, observations[0]);
  EQ(2, observations[1]);
  EQ(3, observations[2]);
  
  delete p0;
  delete p1;
  delete p2;
}
