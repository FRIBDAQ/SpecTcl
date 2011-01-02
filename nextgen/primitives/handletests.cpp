#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <string>
#include <iostream>
#include "handleManager.h"

using namespace std;

int main(int argc, char** argv)
{

  CppUnit::TextUi::TestRunner   
               runner; // Control tests.
  CppUnit::TestFactoryRegistry& 
               registry(CppUnit::TestFactoryRegistry::getRegistry());

  runner.addTest(registry.makeTest());

  bool wasSucessful;
  try {
    wasSucessful = runner.run("",false);
  } 
  catch(string& rFailure) {
    cerr << "Caught a string exception from test suites.: \n";
    cerr << rFailure << endl;
    wasSucessful = false;
  }
  return !wasSucessful;
}


// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"



class handles : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(handles);
  CPPUNIT_TEST(empty);
  CPPUNIT_TEST(add);
  CPPUNIT_TEST(add2);
  CPPUNIT_TEST(remove);
  CPPUNIT_TEST_SUITE_END();
  CHandleManager* m_pManager;

private:

public:
  void setUp() {
    m_pManager =new CHandleManager("test");
  }
  void tearDown() {
    delete m_pManager;
  }
protected:
  void empty();
  void add();
  void add2();
  void remove();
};

CPPUNIT_TEST_SUITE_REGISTRATION(handles);

// Should not be able to find any thing in an empty guy:

void handles::empty()
{
  void*p = m_pManager->find("test_0");
  EQ(reinterpret_cast<void*>(NULL), p);
}

// Add - should be able to find it.

void handles::add() {
  string name = m_pManager->add(m_pManager);
  void*  p    = m_pManager->find(name);

  EQ(m_pManager, reinterpret_cast<CHandleManager*>(p));
}

// adding 2 elemements 
// 1. they should give different handles.
// 2. the handles should find the right pointers.
//
void handles::add2()
{
  int item1;
  int item2;
  int* p1 = &item1;
  int* p2 = &item2;

  string h1 = m_pManager->add(p1);
  string h2 = m_pManager->add(p2);

  ASSERT(h1 != h2);

  EQ(p1, reinterpret_cast<int*>(m_pManager->find(h1)));
  EQ(p2, reinterpret_cast<int*>(m_pManager->find(h2)));
}
//  Remove - once removed a handel cannot be found...but the remaining one can be:

void handles::remove()
{
  int item1;
  int item2;
  int* p1 = &item1;
  int* p2 = &item2;

  string h1 = m_pManager->add(p1);
  string h2 = m_pManager->add(p2);

  m_pManager->unregister(h1);

  EQ(reinterpret_cast<void*>(NULL), m_pManager->find(h1));
  EQ(p2, reinterpret_cast<int*>(m_pManager->find(h2)));

  m_pManager->unregister(h2);

  EQ(reinterpret_cast<void*>(NULL), m_pManager->find(h2));


}
