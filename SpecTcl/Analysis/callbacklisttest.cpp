#include <config.h>
#include <buffer.h>
#include <buftypes.h>


#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include <vector>

#include "CBufferCallbackList.h"
#include "CBufferCallback.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

class callbacklisttest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(callbacklisttest);
  CPPUNIT_TEST(construct);
  CPPUNIT_TEST(addone);
  CPPUNIT_TEST(addtwo);
  CPPUNIT_TEST(callbacks);
  CPPUNIT_TEST_SUITE_END();

private:
  CBufferCallbackList* m_pCallbackList;

public:
  void setUp()  
  {
    m_pCallbackList = new CBufferCallbackList(1);
  }
  void tearDown()
  {
    delete m_pCallbackList;
  }
protected:
  void construct();
  void addone();
  void addtwo();
  void callbacks();
};
CPPUNIT_TEST_SUITE_REGISTRATION(callbacklisttest);


void
callbacklisttest::construct()
{
  ASSERT(m_pCallbackList->begin() == m_pCallbackList->end());
  EQ(0, m_pCallbackList->size());
  

}

// This callback class will just be used for addone, addtwo:


class InoccuousCallback : public CBufferCallback
{
public:
  virtual void operator()(unsigned int nType, const void* pBuffer) {
  }
};

void
callbacklisttest::addone()
{
  InoccuousCallback cb;
  m_pCallbackList->addCallback(cb);

  ASSERT(m_pCallbackList->begin() != m_pCallbackList->end());
  EQ(1, m_pCallbackList->size());

  InoccuousCallback* pcb = 
    dynamic_cast<InoccuousCallback*>(*(m_pCallbackList->begin()));
  EQ(&cb, pcb);

}
void
callbacklisttest::addtwo()
{
  InoccuousCallback cb1, cb2;
  m_pCallbackList->addCallback(cb1);
  m_pCallbackList->addCallback(cb2);

  CBufferCallbackList::CallbackIterator p = m_pCallbackList->begin();

  InoccuousCallback* pcb = dynamic_cast<InoccuousCallback*>(*p);
  EQ(&cb1, pcb);

  p++;
  pcb = dynamic_cast<InoccuousCallback*>(*p);
  EQ(&cb2, pcb);

  p++;
  ASSERT(p == m_pCallbackList ->end());
}

// Stuff used for the callbacks test:


static vector<int> order;		// Keeps track of call order.

class OrderedCallback : public CBufferCallback
{
  unsigned int m_n;		// my ordinal.
public:
  OrderedCallback(int n) :
    m_n(n)
  {}

  virtual void operator()(unsigned int ntype, const void* p) {
    order.push_back(m_n);	// Indicate call order.
    EQ((unsigned int)1, ntype);		// should be type  1 buffer.
    EQ((const void*)&order, p);
  }
};

void
callbacklisttest::callbacks()
{
  // Clear the order vetor in case we're invoked several times.

  if(!order.empty()) {
    order.erase(order.begin(), order.end());
  }

  // make and register a slew of callbacks.

  vector<OrderedCallback*> callbacks;
  for(int i = 0; i < 100; i++) {
    OrderedCallback* pcb = new OrderedCallback(i);
    m_pCallbackList->addCallback(*pcb);
    callbacks.push_back(pcb);
  }
  // Invoke them:
  
  (*m_pCallbackList)(1, &order);

  // Check the call order:

  EQ((size_t)100, order.size());
  for(int i = 0; i < 100; i++) {
    EQ(i, order[i]);
  }

  // Destroy the first callback and run again (test of removeCallback):

  CBufferCallbackList::CallbackIterator i = m_pCallbackList->begin();

  m_pCallbackList->removeCallback(i);
  order.erase(order.begin(), order.end());

  (*m_pCallbackList)(1, &order);

  EQ((size_t)99, order.size());
  for(int i = 0; i < 99; i++) {
    EQ(i+1, order[i]);
  }

  // Destroy the callback objects:

  for(int i = 0; i < 100; i++) {
    delete callbacks[i];
  }
  


}
