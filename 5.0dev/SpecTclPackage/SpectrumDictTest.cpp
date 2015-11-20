// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#define private public
#include "CSpectrumDictionary.h"
#undef private

#include "CAxis.h"
#include "CSpectrum.h"
#include "CSpectrumAllocator.h"
#include "CSpectrumIncrementer.h"
#include "CTrueCondition.h"
#include "CObservable.h"

#include <stdint.h>

#include <vector>


class SpectrumDictTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SpectrumDictTest);
  CPPUNIT_TEST(instance);

  CPPUNIT_TEST(add);
  CPPUNIT_TEST(adddup);
  
  CPPUNIT_TEST(findnox);
  CPPUNIT_TEST(find);

  CPPUNIT_TEST(removenox);
  CPPUNIT_TEST(removeok);


  CPPUNIT_TEST(begin);
  CPPUNIT_TEST(beginnonempty);

  CPPUNIT_TEST(end);


  CPPUNIT_TEST(size);

  // Observer tests:

  CPPUNIT_TEST(observeradd);
  CPPUNIT_TEST(observerremove);
  CPPUNIT_TEST(getreasonempty);

  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
    delete CSpectrumDictionary::m_pInstance;
  }
protected:
  void instance();
  void add();
  void adddup();

  void findnox();
  void find();

  void removenox();
  void removeok();

  void begin();
  void beginnonempty();

  void end();
  
  void size();

  void observeradd();
  void observerremove();
  void getreasonempty();
};



/**
 * Make a test spectrum.
 */

class CDummyAllocator : public CSpectrumAllocator
{
public:
  void* allocate(CSpectrum& spec) {return 0;}
  void  deallocate(CSpectrum& spec) {}
};
class CDummyIncrementer : public CSpectrumIncrementer
{
public:
  void validate(CSpectrum& spec) {}
  void operator()(CSpectrum& spec) {}
  uint32_t get(CSpectrum& spec, unsigned x, unsigned y) {return 0;}
  void set(CSpectrum& spec, unsigned x, unsigned y, uint32_t value) {}
  void clear(CSpectrum& spec) {}
};

static CSpectrum* makeSpectrum()
{
  CAxis *pX = new CAxis("X-axis", 0.0, 127.0, 128);
  std::vector<CAxis*> axes;
  axes.push_back(pX);
  CDummyAllocator* pAlloc = new CDummyAllocator;
  CDummyIncrementer* pInc = new CDummyIncrementer;
  CTrueCondition*     pTrue = new CTrueCondition;

  return new CSpectrum("test spec", pAlloc, pInc, pTrue, axes);
  
}

CPPUNIT_TEST_SUITE_REGISTRATION(SpectrumDictTest);

/**
 * instance
 * Singleton test:
 */
void SpectrumDictTest::instance() {
  CSpectrumDictionary* p = CSpectrumDictionary::instance();
  ASSERT(p);
  EQ(p, CSpectrumDictionary::instance());
}
/**
 * add
 *   Add a new spectrum to the dictionary.
 */
void
SpectrumDictTest::add()
{
  CSpectrum* pSpec = makeSpectrum();
  CSpectrumDictionary* pDict = CSpectrumDictionary::instance();

  pDict->add(*pSpec);
  ASSERT(pDict->m_Dictionary.find(pSpec->getName()) != pDict->m_Dictionary.end());
  EQ(pSpec->getName(), pDict->m_Dictionary.find(pSpec->getName())->first);
  
}
/**
 * adddup
 *
 *  Adding a duplicate spectrum name throws a spectrum_dictionary_exception;
 */
void
SpectrumDictTest::adddup()
{
  CSpectrum* pSpec = makeSpectrum();
  CSpectrumDictionary* pDict = CSpectrumDictionary::instance();

  pDict->add(*pSpec);

  CSpectrum* pSpec2 = makeSpectrum();


  bool thrown = false;
  bool rthrow = false;
  std::string msg = "";
  try {
    pDict->add(*pSpec2);
  }
  catch(spectrum_dictionary_exception& e) {
    thrown = true;
    rthrow = true;
    msg = e.what();
  }
  catch(...) {
    thrown = true;
  }
  ASSERT(thrown);
  ASSERT(rthrow);
  EQ(std::string("CSpectrumDictionary::add - cannot insert duplicate spectrum named: 'test spec'"),
     msg);


  delete pSpec2;
}
/**
 * findnox
 *
 *  Finding a non-existing spectrum returns a null pointer.
 */
void
SpectrumDictTest::findnox()
{
  CSpectrumDictionary* pDict = CSpectrumDictionary::instance();
  CSpectrum* pSpec = pDict->find("testing");
  
  EQ(reinterpret_cast<CSpectrum*>(0), pSpec);
}
/**
 * find
 *   Finding an existing spectrum gives its pointer.
 */
void
SpectrumDictTest::find()
{
  CSpectrum* pSpec = makeSpectrum();
  CSpectrumDictionary* pDict = CSpectrumDictionary::instance();

  pDict->add(*pSpec);

  CSpectrum* pFound = pDict->find(pSpec->getName().c_str());


  EQ(pSpec, pFound);
}
/**
 * removing a nonexistent spectrum throws an exception.
 */
void
SpectrumDictTest::removenox()
{
  CSpectrumDictionary* pDict = CSpectrumDictionary::instance();

  bool thrown(false);
  bool rthrow(false);
  std::string msg = "";
  try {
    pDict->remove("no such spectrum");
  }
  catch(spectrum_dictionary_exception& e) {
    thrown = true;
    rthrow = true;
    msg = e.what();
  }
  catch (...) {
    thrown = true;
  }
  ASSERT(thrown);
  ASSERT(rthrow);
  EQ(std::string("CSpectrumDictionary::remove - cannot remove spectrum 'no such spectrum' does not exist"),
     msg);

}
/**
 * removeok
 *
 */
void
SpectrumDictTest::removeok()
{
  CSpectrum* pSpec = makeSpectrum();
  CSpectrumDictionary* pDict = CSpectrumDictionary::instance();

  pDict->add(*pSpec);
  
  pDict->remove(pSpec->getName().c_str());
  ASSERT(!pDict->find(pSpec->getName().c_str()));

  delete pSpec;
}
/**
 * begin
 *   Test begin method is begi iterator:
 */
void
SpectrumDictTest::begin()
{
  CSpectrumDictionary* pDict = CSpectrumDictionary::instance();
  ASSERT(pDict->m_Dictionary.begin() == pDict->begin());
}
/**
 * beginnonempty
 *   If there's a spectrum in the dict:
 *   * begin 'points' to its pair.
 *   * the iterator from begin incremented gets us to end.
 */
void
SpectrumDictTest::beginnonempty()
{
  CSpectrum* pSpec = makeSpectrum();
  CSpectrumDictionary* pDict = CSpectrumDictionary::instance();

  pDict->add(*pSpec);

  CSpectrumDictionary::DictionaryIterator p = pDict->begin();
  EQ(pSpec->getName(), p->first);
  EQ(pSpec, p->second);

  p++;
  ASSERT(p == pDict->m_Dictionary.end());
}
/**
 * end
 */
void
SpectrumDictTest::end()
{
  CSpectrumDictionary* pDict = CSpectrumDictionary::instance();
  ASSERT(pDict->m_Dictionary.end() == pDict->end());
}
/**
 * size
 *
 *  Shows how many spectra are in the dictionary.
 */
void
SpectrumDictTest::size()
{
  CSpectrumDictionary* pDict = CSpectrumDictionary::instance();
  EQ(static_cast<size_t>(0), pDict->size());

  pDict->add(*(makeSpectrum()));

  EQ(static_cast<size_t>(1), pDict->size());

  CAxis *pX = new CAxis("X-axis", 0.0, 127.0, 128);
  std::vector<CAxis*> axes;
  axes.push_back(pX);
  CDummyAllocator* pAlloc = new CDummyAllocator;
  CDummyIncrementer* pInc = new CDummyIncrementer;
  CTrueCondition*     pTrue = new CTrueCondition;

  pDict->add(*new CSpectrum("test2", pAlloc, pInc, pTrue, axes));
  EQ(static_cast<size_t>(2), pDict->size());
   
}
/**
 * observeradd
 *
 *  Add observer must be called when a spectrum is added
 *  and the operation at the time must be 'Add'.
 */
class WhatObserved : public CObserver<CSpectrumDictionary> 
{
public:
  bool called;
  CSpectrumDictionary::Op op;

  WhatObserved() : called(false) {}

  void operator()(CObservable<CSpectrumDictionary>* s)
  {
    CSpectrumDictionary* pDict 
      = reinterpret_cast<CSpectrumDictionary*>(s);

    called = true;
    op = pDict->getReason();
  }
};

void
SpectrumDictTest::observeradd()
{
  CSpectrum* pSpec = makeSpectrum();
  CSpectrumDictionary* pDict = CSpectrumDictionary::instance();
  WhatObserved observer;
  pDict->addObserver(&observer);

  pDict->add(*pSpec);

  ASSERT(observer.called);
  EQ(CSpectrumDictionary::Add, observer.op);
  
}
/**
 * observerremove
 *
 * Test that an observer gets called on remove.
 */
void
SpectrumDictTest::observerremove()
{
  CSpectrum* pSpec = makeSpectrum();
  CSpectrumDictionary* pDict = CSpectrumDictionary::instance();
  WhatObserved observer;

  pDict->add(*pSpec);


  pDict->addObserver(&observer);
  pDict->remove(pSpec->getName().c_str());

  ASSERT(observer.called);
  EQ(CSpectrumDictionary::Remove, observer.op);
  
}
/**
 * getreasonempty
 *
 *  If I get the reason when not processing
 *  observations, a spectrum_dictionary_exception
 *  gets thrown.
 */
void
SpectrumDictTest::getreasonempty()
{
  CSpectrumDictionary* pDict = CSpectrumDictionary::instance();

  bool thrown = false;
  bool rthrow = false;
  std::string msg;

  try {
    pDict->getReason();
  }
  catch(spectrum_dictionary_exception& e) {
    thrown = true;
    rthrow = true;
    msg = e.what();
  } 
  catch (...) {
    thrown = true;
  }
  ASSERT(thrown);
  ASSERT(rthrow);
  EQ(std::string("CSpectrumDictionary::getReason - not observing so there's no reason")
,
     msg);
}
