// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#define private public
#include "CParameterDictionary.h"
#include "CParameter.h"
#undef private

#include <vector>


class ParamDictTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ParamDictTests);
  CPPUNIT_TEST(instance);

  // Dictionary management:

  CPPUNIT_TEST(emptydict);
  CPPUNIT_TEST(add1have1);
  CPPUNIT_TEST(add1canget1);
  CPPUNIT_TEST(add1iszero);
  CPPUNIT_TEST(add1setvaluecalled);

  // Multibinding

  CPPUNIT_TEST(add2samename);
  CPPUNIT_TEST(add2mbidentical);

  // Don't get confused with a new name.

  CPPUNIT_TEST(add2differentname);

  // Remove from dictionary:

  CPPUNIT_TEST(remove);
  CPPUNIT_TEST(removelast);
  CPPUNIT_TEST(removenoex);
  CPPUNIT_TEST(removefake);

  // Exports of the 'standard' container methods for the dict:

  CPPUNIT_TEST(begin);
  CPPUNIT_TEST(end);
  CPPUNIT_TEST(size);
  CPPUNIT_TEST(findok);
  CPPUNIT_TEST(findnoex);

  // Observers

  CPPUNIT_TEST(observeaddfirst);
  CPPUNIT_TEST(observeaddReference);
  CPPUNIT_TEST(observeremoveReference);
  CPPUNIT_TEST(observeremoveLast);


  // Observers getting info about the operation.

  CPPUNIT_TEST(observeisadd1st);
  CPPUNIT_TEST(observeisadd);
  CPPUNIT_TEST(observeisremove);
  CPPUNIT_TEST(observeremovelast);

  // Observers that trigger nested observation.

  CPPUNIT_TEST(observertriggers);

  CPPUNIT_TEST_SUITE_END();


private:
  CParameterDictionary* m_pDict;
public:
  void setUp() {
    m_pDict = CParameterDictionary::instance();
    CParameter::disableAutoRegistration();
  }
  void tearDown() {
    delete CParameterDictionary::m_pInstance;
    CParameterDictionary::m_pInstance = 0;
    m_pDict = 0;
  }
protected:
  void instance();
  
  void emptydict();
  void add1have1();
  void add1canget1();
  void add1iszero();
  void add1setvaluecalled();

  void add2samename();
  void add2mbidentical();
  void add2differentname();

  void remove();
  void removelast();
  void removenoex();
  void removefake();

  void begin();
  void end();
  void size();
  void findok();
  void findnoex();

  void observeaddfirst();
  void observeaddReference();
  void observeremoveReference();
  void observeremoveLast();

  void observeisadd1st();
  void observeisadd();
  void observeisremove();
  void observeremovelast();

  void observertriggers();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ParamDictTests);

/**
 * instance 
 *
 *  The instance() methd should return non null and the value
 * should match that in CParameterDictionary::m_pInstance
 */
void ParamDictTests::instance() {
  CParameterDictionary* pDict = CParameterDictionary::instance();

  ASSERT(pDict);
  EQ(CParameterDictionary::m_pInstance, pDict);
}

/**
 * emptydict
 * 
 *  We should be starting out with an empty dict:
 */
void
ParamDictTests::emptydict()
{
  ASSERT(m_pDict->m_parameters.size() == 0);
}


/**
 * add1have1 - Adding a single cparameter gives us a dict.entry.
 */
void
ParamDictTests::add1have1()
{
  CParameter* p1 = new CParameter("test param");
  m_pDict->add(p1);

  EQ(static_cast<size_t>(1), m_pDict->m_parameters.size());
}
/**
 * add1canget1 - should be able to find a parameter info as the only
 * dict item and it should have our parameter at the front of a one element
 * list.
 */
void
ParamDictTests::add1canget1()
{
  CParameter* p1 = new CParameter("a test");
  m_pDict->add(p1);

  // Should be able to find our parameter in the dict.

  CParameterDictionary::DictionaryIterator p = 
    m_pDict->m_parameters.find("a test");
  ASSERT(p != m_pDict->m_parameters.end());

  // The list shouild have one element

  CParameterDictionary::pParameterInfo pInfo = p->second;
  EQ(static_cast<size_t>(1), pInfo->s_references.size());
  EQ(p1, pInfo->s_references.back());
}
/**
 * add1iszero
 *
 *   The first item I add to the dictionary will be parameter number zero.
 */
void
ParamDictTests::add1iszero()
{
  CParameter* p1 = new CParameter("a test");
  m_pDict->add(p1);

  // Should be able to find our parameter in the dict.

  CParameterDictionary::DictionaryIterator p = 
    m_pDict->m_parameters.find("a test");
  ASSERT(p != m_pDict->m_parameters.end());
  
  // The s_number item should be zero:

  EQ(0U,  p->second->s_number);
 
}
/**
 * add1setvaluecalled
 *
 *  Binding a parameter should make the dictionary call setParameter
 *  so that we have our value.
 */
void
ParamDictTests::add1setvaluecalled()
{
  CParameter* p1 = new CParameter("a test");
  m_pDict->add(p1);

  ASSERT(p1->m_parameter);
}
/**
 * add2samename
 *
 *  Adding 2 with the same name just appends to the list.
 */
void
ParamDictTests::add2samename()
{
  CParameter* p1 = new CParameter("a test");
  m_pDict->add(p1);

  CParameter* p2 = new CParameter("a test");
  m_pDict->add(p2);

  // Should be able to find our parameter in the dict.

  CParameterDictionary::DictionaryIterator p = 
    m_pDict->m_parameters.find("a test");

  CParameterDictionary::pParameterInfo pI = p->second;
  EQ(static_cast<size_t>(2), pI->s_references.size());

  std::list<CParameter*>::iterator pL = pI->s_references.begin();
  EQ(p1, *pL);
  pL++;
  EQ(p2, *pL);
  pL++;
  ASSERT(pI->s_references.end() == pL);
}
/**
 * add2mbidentical
 *
 *   If 2 parameters with the same name are added their
 *   properties must be identical.
 *   Properties mean low, high, bins and units.
 */
void
ParamDictTests::add2mbidentical()
{
  CParameter* p1 = new CParameter("a test");
  CParameter* p2 = new CParameter("a test");
  p2->m_units   = "have units";	// no longer identical.
  m_pDict->add(p1);

  bool threw = false;
  bool rthrew=false;
  std::string msg;

  try {
    m_pDict->add(p2);
  }
  catch (parameter_dictionary_exception& e) {
    threw = true;
    rthrew= true;
    msg = e.what();
  }
  catch (...) {
    threw = true;
  }
  ASSERT(threw);
  ASSERT(rthrew);
  EQ(std::string("parameter mappings to the same value must be identical"), msg);

}
/**
 * add2differentname
 *
 *   Adding a second parameter with a different name should build a different value
 *   and parameter number 1.
 */
void
ParamDictTests::add2differentname()
{
  CParameter* p1 = new CParameter("a test");
  CParameter* p2 = new CParameter("another test");

  m_pDict->add(p1);
  m_pDict->add(p2);

  CParameterDictionary::DictionaryIterator p = m_pDict->m_parameters.find("another test");
  CParameterDictionary::pParameterInfo pInfo = p->second;
  EQ(1U, pInfo->s_number);
  EQ(static_cast<size_t>(1), pInfo->s_references.size());
}
/**
 * remove
 *   Should be able to remove a parameter definition from the dict and the right
 *   one will vanish.
 */
void
ParamDictTests::remove()
{
  CParameter* p1 = new CParameter("a test");
  CParameter* p2 = new CParameter("a test");

  m_pDict->add(p1);
  m_pDict->add(p2);

  m_pDict->remove(p2);

  CParameterDictionary::DictionaryIterator p = m_pDict->m_parameters.find("a test");
  CParameterDictionary::pParameterInfo pInfo = p->second;
  EQ(static_cast<size_t>(1), pInfo->s_references.size());
  EQ(p1, pInfo->s_references.front());
  
}
/**
 * removelast
 *
 *  Removing the last item destroys the parameter totally:
 */
void
ParamDictTests::removelast()
{
  CParameter* p1 = new CParameter("a test");
  CParameter* p2 = new CParameter("a test");

  m_pDict->add(p1);
  m_pDict->add(p2);

  m_pDict->remove(p2);
  m_pDict->remove(p1);

  ASSERT(m_pDict->m_parameters.find("a test") == m_pDict->m_parameters.end());
}
/**
 * removenoex
 *
 *  Removing a nonexistent element throws an exception.
 */
void
ParamDictTests::removenoex()
{
  CParameter p1("a test");


  bool threw  = false;
  bool rthrew = false;
  std::string msg;
  try {
    m_pDict->remove(&p1);
  }
  catch(parameter_dictionary_exception& e) {
    threw = true;
    rthrew = true;
    msg = e.what();
  }
  catch (...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(rthrew);
  EQ(std::string("no such parameter"), msg);

}
/**
 * removefake
 *   Throw an exception if the paramter exists but the pointer
 *   is not to a valid reference.
 */
void
ParamDictTests::removefake()
{
  CParameter* p1 = new CParameter("a test");
  CParameter*  p2 = new CParameter("a test");

  m_pDict->add(p1);		// make "a test"

  bool threw   = false;
  bool rthrew  = false;
  std::string msg;
  try {
    m_pDict->remove(p2);	// should throw.
  }
  catch (parameter_dictionary_exception& e) {
    threw = true;
    rthrew= true;
    msg = e.what();
  }
  catch (...) {
    threw = true;
  }
  ASSERT(threw);
  ASSERT(rthrew);
  EQ(std::string("not a valid reference"), msg);
}
/**
 * begin
 *   should return the begin iterator fromt he dict.
 */
void
ParamDictTests::begin()
{
  CParameter* p1 = new CParameter("a test");
  m_pDict->add(p1);

  ASSERT(m_pDict->begin() == m_pDict->m_parameters.begin());
}
/**
 * end
 *  Similarly.
 */
void
ParamDictTests::end()
{
  CParameter* p1 = new CParameter("a test");
  m_pDict->add(p1);

  ASSERT(m_pDict->end() == m_pDict->m_parameters.end());
}

/**
 * size
 *   The number of items in the dict.  This is not necessarily the
 *   number of CParameter pointers added.
 */
void
ParamDictTests::size()
{
  EQ(static_cast<size_t>(0), m_pDict->size());
  CParameter *p1 =  new CParameter("param1");
  m_pDict->add(p1);

  EQ(static_cast<size_t>(1), m_pDict->size());
 
  CParameter*p2 = new CParameter("param2");
  m_pDict->add(p2);
  EQ(static_cast<size_t>(2), m_pDict->size());

  // The tricky one is that adding an identical parameter does _not_
  // increase the count:

  m_pDict->add(new CParameter("param1"));
  EQ(static_cast<size_t>(2), m_pDict->size());


}
/**
 * findok
 *   Finds a parameter we inserted.
 */
void
ParamDictTests::findok()
{
  CParameter* p1 = new CParameter("p1");
  CParameter* p2 = new CParameter("p2");
  
  m_pDict->add(p1);
  m_pDict->add(p2);

  CParameterDictionary::DictionaryIterator p = m_pDict->find("p1");
  EQ(std::string("p1"), p->first);
  EQ(std::string("p1"), p->second->s_references.front()->getName());

  p = m_pDict->find("p2");
  EQ(std::string("p2"), p->first);
  EQ(std::string("p2"), p->second->s_references.front()->getName());
}

/**
 * findnoex
 *  Looking for a nonexistent parameter gives end.
 */
void
ParamDictTests::findnoex()
{
  CParameter* p1 = new CParameter("p1");
  CParameter* p2 = new CParameter("p2");
  
  m_pDict->add(p1);
  m_pDict->add(p2);

  ASSERT(m_pDict->find("p3") == m_pDict->end());
}
/**
 * observefirst
 *
 *   Adding the first CParameter on a name makes a unique
 *   observable event.
 */
class FlagObserver : public CObserver<CParameterDictionary>
{
public:
  int callcount;
  FlagObserver() : callcount(0) {}
  virtual void operator()(CObservable<CParameterDictionary>* pDict) {
    callcount++;
  }
};
void
ParamDictTests::observeaddfirst()
{
  FlagObserver observer;
  m_pDict->addObserver(&observer);

  CParameter* p1 = new CParameter("p1");
  m_pDict->add(p1);		// should observe.

  EQ(1, observer.callcount);

  
}
/**
 * observeaddReference
 *
 *  Adding a reference to an existing name is also
 *  an observable event.
 */
void
ParamDictTests::observeaddReference()
{
  FlagObserver observer;
  m_pDict->add(new CParameter("p1")); // establish the name.

  m_pDict->addObserver(&observer);
  m_pDict->add(new CParameter("p1")); // Should observe.

  EQ(1, observer.callcount);
}
/**
 * observeremoveReference
 *
 *  Removing a reference that is not last is an observable event.
 */
void
ParamDictTests::observeremoveReference()
{  
  FlagObserver observer;
  m_pDict->add(new CParameter("p1")); // establish the name.
  CParameter*  p2 = new CParameter("p1");

  m_pDict->add(p2);
  m_pDict->addObserver(&observer);

  m_pDict->remove(p2);
  EQ(1, observer.callcount);
  
}
/**
 * observeremoveLast
 *
 *   Removing the last item is also an observable event
 */
void 
ParamDictTests::observeremoveLast()
{
  FlagObserver obs;
  CParameter  p("p1");
  CParameter  p1("p1");
  
  m_pDict->add(&p);
  m_pDict->add(&p1);

  m_pDict->remove(&p);
  m_pDict->addObserver(&obs);
  m_pDict->remove(&p1);

  EQ(1, obs.callcount);
}
/**
 * observeisadd1st
 *    Observing add1st
 *   - getOperation should not be null.
 *   - getOperation should return the right stuff.
 */
class InfoObserver : public CObserver<CParameterDictionary>
{
public:
  CParameterDictionary::OperationInfo info;
  virtual void operator()(CObservable<CParameterDictionary>* p) {
    CParameterDictionary* pDict = reinterpret_cast<CParameterDictionary*>(p);
    CParameterDictionary::pOperationInfo pOp = pDict->getOperation();
    ASSERT(pOp);		// Must be non zero.
    info = *pOp;
  }
  
};

void
ParamDictTests::observeisadd1st()
{
 
  InfoObserver o;
  CParameter* p;
  m_pDict->addObserver(&o);
  m_pDict->add(p = new CParameter("p1"));
  EQ(CParameterDictionary::addFirst, o.info.s_op);
  EQ(std::string("p1"), o.info.s_Name);
  EQ(p, o.info.s_pParam);
}
/**
 * observeisadd
 *
 *  Ensure that an additonal add gets the right value of the info
 */
void
ParamDictTests::observeisadd()
{
  InfoObserver o;
  CParameter* p;
  m_pDict->add(new CParameter("p1")); // addfirst.
  m_pDict->addObserver(&o);
  m_pDict->add(p = new CParameter("p1"));
  EQ(CParameterDictionary::addReference, o.info.s_op);
  EQ(std::string("p1"), o.info.s_Name);
  EQ(p, o.info.s_pParam);

  // By the way getOperation now should be null:

  ASSERT(! m_pDict->getOperation());
}
/**
 * observeisremove
 *
 *  Remove should indicate if not the last.
 */
void
ParamDictTests::observeisremove()
{
  InfoObserver o;
  CParameter* p;
  m_pDict->add(new CParameter("p1")); // addfirst.
  m_pDict->add(p = new CParameter("p1"));
  m_pDict->addObserver(&o);
  m_pDict->remove(p);

  EQ(CParameterDictionary::removeReference, o.info.s_op);
  EQ(std::string("p1"), o.info.s_Name);
  EQ(p, o.info.s_pParam);


}
void
ParamDictTests::observeremovelast()
{
  InfoObserver o;
  CParameter* p;
  CParameter* p1;
  m_pDict->add(p = new CParameter("p1")); // addfirst.
  m_pDict->add(p1 = new CParameter("p1"));
  m_pDict->remove(p);
  m_pDict->addObserver(&o);
  m_pDict->remove(p1);

  EQ(CParameterDictionary::removeLast, o.info.s_op);
  EQ(std::string("p1"), o.info.s_Name);
  EQ(p1, o.info.s_pParam);
}

/**
 * observetriggers
 *  Tests the ability to handle stacked observations.
 *  - Put an item in the dictionary
 *  - Add an observer that is sensitive to removeLast
 *    and inserts the item removed back in.
 *  - Add another observer that is sensitive to all items and logs
 *    what it does.
 *  - Remove the item from the dict.
 *  - the second observer should see the add and remove in that order.
 */

class NoDeleteObserver : public CObserver<CParameterDictionary>
{
public:
  // Don't allow a last removal:

  virtual void operator()(CObservable<CParameterDictionary>* p) {
    CParameterDictionary* pDict = reinterpret_cast<CParameterDictionary*>(p);
    if (pDict->getOperation()->s_op == CParameterDictionary::removeLast) {
      pDict->add(pDict->getOperation()->s_pParam);
    }
  }
};

class OpRecordObserver : public CObserver<CParameterDictionary>
{
public:
  std::vector<CParameterDictionary::Operation> ops;
  virtual void operator()(CObservable<CParameterDictionary>* p) {
    CParameterDictionary* pDict = reinterpret_cast<CParameterDictionary*>(p);
    ops.push_back(pDict->getOperation()->s_op);
  }
};


void 
ParamDictTests::observertriggers()
{
  // Stock..
  CParameter* p;
  m_pDict->add(p = new CParameter("p1"));

  // add the observers:

  NoDeleteObserver nodel;
  OpRecordObserver op;
  m_pDict->addObserver(&nodel);
  m_pDict->addObserver(&op);

  m_pDict->remove(p);

  // Should be two calls.

  EQ(static_cast<size_t>(2), op.ops.size());

  // Ops should be addFirst and removeLast in that order I think.

  EQ(CParameterDictionary::addFirst, op.ops[0]);
  EQ(CParameterDictionary::removeLast, op.ops[1]);


}
