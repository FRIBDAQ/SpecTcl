// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#define private public
#include "CSpectrum.h"
#undef private

#include "CAxis.h"
#include "CCondition.h"
#include "CTrueCondition.h"
#include "CFalseCondition.h"
#include "CSpectrumIncrementer.h"
#include "CSpectrumAllocator.h"
#include "CAxis.h"
#include <stdint.h>

class SpectrumTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SpectrumTests);
  CPPUNIT_TEST(construct);
  CPPUNIT_TEST(destruct);
  
  CPPUNIT_TEST(handle);
  CPPUNIT_TEST(name);
  CPPUNIT_TEST(axis);
  CPPUNIT_TEST(axisnosuch);
  CPPUNIT_TEST(condition);
  CPPUNIT_TEST(incrementer);
  CPPUNIT_TEST(allocator);

  CPPUNIT_TEST(setcondition);

  CPPUNIT_TEST(clear);
  CPPUNIT_TEST(functional);
  CPPUNIT_TEST(get);
  CPPUNIT_TEST(set);
  
  CPPUNIT_TEST(checkValidity);

  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void construct();
  void destruct();

  void handle();
  void name();
  void axis();
  void axisnosuch();
  void condition();
  void incrementer();
  void allocator();

  void setcondition();

  void clear();
  void functional();
  void get();
  void set();

  void checkValidity();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SpectrumTests);


class DummyAllocator : public CSpectrumAllocator
{
public:
  void* that;
  bool deallocated;
public:
  DummyAllocator(void* alloc) : that(alloc), deallocated(false)
				 {}
  void* allocate(CSpectrum& spec) { 
    return that;
  }
  void deallocate(CSpectrum& spec) {
    deallocated = true;
  }
};

class DummyIncrementer  : public CSpectrumIncrementer
{
public:
  bool cleared;
  bool functional;
  bool gotten;
  bool wasset;
  bool validated;
  unsigned x;
  unsigned y;
  uint32_t value;
  
public:
  DummyIncrementer() : cleared(false), functional(false),
		       gotten(false), wasset(false), 
		       validated(false), x(0), y(0),
		       value(0) {}

  void operator()(CSpectrum& spc) {
    functional = true;
  }
  uint32_t get(CSpectrum& spec, unsigned X, unsigned Y) {
    x = X;
    y = Y;
    gotten = true;
    return 1234;
  }
  void set(CSpectrum& spec, unsigned X, unsigned Y, 
		    uint32_t Value) {
    wasset = true;
    x = X;
    y = Y;
    value = Value;
  }
  void clear(CSpectrum& spec) {
    cleared = true;
  }
  void validate(CSpectrum& spec) {
    validated = true;
  }

};

/**
 * construct
 * Make sure all the bits and pieces get stuck in the right
 * attributes.
 */
void SpectrumTests::construct() {
  CTrueCondition* cond = new CTrueCondition;;
  DummyAllocator alloc(this);
  DummyIncrementer* inc = new DummyIncrementer;
  CAxis* pX = new CAxis("x", 100, 0, 99);
  CAxis* pY = new CAxis("y", 256, 0, 255);
  std::vector<CAxis*> axes;
  axes.push_back(pX);
  axes.push_back(pY);

  

  CSpectrum spec("test", &alloc, inc, cond, axes);

  EQ((void*)this, spec.m_allocationHandle); // This implies the allocator was called.
  EQ(std::string("test"), spec.m_name);
  EQ(static_cast<size_t>(2), spec.m_axes.size());
  EQ(pX,  spec.m_axes[0]);
  EQ(pY,  spec.m_axes[1]);
  EQ((CCondition*)cond, spec.m_condition);
  EQ((CSpectrumIncrementer*)inc, spec.m_incrementer);
  EQ((CSpectrumAllocator*)&alloc, spec.m_allocator);


}
/**
 * destruct
 *   The allocator's deallocate must bve called
 */
void SpectrumTests::destruct()
{

  CTrueCondition* cond = new CTrueCondition;;
  DummyAllocator alloc(this);
  DummyIncrementer* inc = new DummyIncrementer;
  CAxis* pX = new CAxis("x", 100, 0, 99);
  CAxis* pY = new CAxis("y", 256, 0, 255);
  std::vector<CAxis*> axes;
  axes.push_back(pX);
  axes.push_back(pY);
 
  {
    CSpectrum spec("test", &alloc, inc, cond, axes); // construct/destruct
  }
  ASSERT(alloc.deallocated);
    
}
/**
 * handle
 *   Should be able to use the selector to fetch the
 *   spectrum handle.
 */
void
SpectrumTests::handle()
{
  CTrueCondition* cond = new CTrueCondition;;
  DummyAllocator alloc(this);
  DummyIncrementer* inc = new DummyIncrementer;
  CAxis* pX = new CAxis("x", 100, 0, 99);
  CAxis* pY = new CAxis("y", 256, 0, 255);
  std::vector<CAxis*> axes;
  axes.push_back(pX);
  axes.push_back(pY);
  CSpectrum spec("test", &alloc, inc, cond, axes); // construct/destruct

  EQ(reinterpret_cast<void*>(this), spec.getHandle());

 }
/**
 * name
 *   Test name selector
 */
void 
SpectrumTests::name()
{
  CTrueCondition* cond = new CTrueCondition;;
  DummyAllocator alloc(this);
  DummyIncrementer* inc = new DummyIncrementer;
  CAxis* pX = new CAxis("x", 100, 0, 99);
  CAxis* pY = new CAxis("y", 256, 0, 255);
  std::vector<CAxis*> axes;
  axes.push_back(pX);
  axes.push_back(pY);
  CSpectrum spec("test", &alloc, inc, cond, axes); // construct/destruct

  EQ(std::string("test"), spec.getName());
}
/**
 * axis
 *   Ensure we can get the spectrum axes back via the selector.
 */
void
SpectrumTests::axis()
{
  CTrueCondition* cond = new CTrueCondition;;
  DummyAllocator alloc(this);
  DummyIncrementer* inc = new DummyIncrementer;
  CAxis* pX = new CAxis("x", 100, 0, 99);
  CAxis* pY = new CAxis("y", 256, 0, 255);
  std::vector<CAxis*> axes;
  axes.push_back(pX);
  axes.push_back(pY);
  CSpectrum spec("test", &alloc, inc, cond, axes); // construct/destruct
  EQ(pX, spec.getAxis(0));
  EQ(pY, spec.getAxis(1));


}
/**
 * axisnosuch
 *
 *  If asking for an axis that does not exist, should get
 *  NULL.
 */
void
SpectrumTests::axisnosuch()
{
  CTrueCondition* cond = new CTrueCondition;;
  DummyAllocator alloc(this);
  DummyIncrementer* inc = new DummyIncrementer;
  CAxis* pX = new CAxis("x", 100, 0, 99);
  CAxis* pY = new CAxis("y", 256, 0, 255);
  std::vector<CAxis*> axes;
  axes.push_back(pX);
  axes.push_back(pY);
  CSpectrum spec("test", &alloc, inc, cond, axes); // construct/destruct

  ASSERT(! spec.getAxis(3));

}
/**
 * condition
 *  Test the getCondition selector.
 */
void
SpectrumTests::condition()
{
  CTrueCondition* cond = new CTrueCondition;;
  DummyAllocator alloc(this);
  DummyIncrementer* inc = new DummyIncrementer;
  CAxis* pX = new CAxis("x", 100, 0, 99);
  CAxis* pY = new CAxis("y", 256, 0, 255);
  std::vector<CAxis*> axes;
  axes.push_back(pX);
  axes.push_back(pY);
  CSpectrum spec("test", &alloc, inc, cond, axes); // construct/destruct

  EQ(reinterpret_cast<CCondition*>(cond), spec.getCondition());
}
/**
 * incrementer
 *
 *   Test the get incrementer selector
 */
void
SpectrumTests::incrementer()
{
  CTrueCondition* cond = new CTrueCondition;;
  DummyAllocator alloc(this);
  DummyIncrementer* inc = new DummyIncrementer;
  CAxis* pX = new CAxis("x", 100, 0, 99);
  CAxis* pY = new CAxis("y", 256, 0, 255);
  std::vector<CAxis*> axes;
  axes.push_back(pX);
  axes.push_back(pY);
  CSpectrum spec("test", &alloc, inc, cond, axes); // construct/destruct

  EQ(reinterpret_cast<CSpectrumIncrementer*>(inc), spec.getIncrementer());

}
/**
 * allocator
 *   Test the getAllocator selector.
 */
void
SpectrumTests::allocator()
{
  CTrueCondition* cond = new CTrueCondition;;
  DummyAllocator alloc(this);
  DummyIncrementer* inc = new DummyIncrementer;
  CAxis* pX = new CAxis("x", 100, 0, 99);
  CAxis* pY = new CAxis("y", 256, 0, 255);
  std::vector<CAxis*> axes;
  axes.push_back(pX);
  axes.push_back(pY);
  CSpectrum spec("test", &alloc, inc, cond, axes); // construct/destruct

  EQ(reinterpret_cast<CSpectrumAllocator*>(&alloc), spec.getAllocator());
}
/**
 * setcondition
 *    test mutator that modifies the current condition.
 */
void
SpectrumTests::setcondition()
{
  CTrueCondition* cond = new CTrueCondition;;
  DummyAllocator alloc(this);
  DummyIncrementer* inc = new DummyIncrementer;
  CAxis* pX = new CAxis("x", 100, 0, 99);
  CAxis* pY = new CAxis("y", 256, 0, 255);
  std::vector<CAxis*> axes;
  axes.push_back(pX);
  axes.push_back(pY);
  CSpectrum spec("test", &alloc, inc, cond, axes); // construct/destruct

  CFalseCondition* f = new CFalseCondition;
  spec.setCondition(f);

  EQ(reinterpret_cast<CCondition*>(f), spec.getCondition());

}
/**
 * clear
 *
 *  Clear should call the incrementer's clear method.
 */
void
SpectrumTests::clear()
{
  CTrueCondition* cond = new CTrueCondition;;
  DummyAllocator alloc(this);
  DummyIncrementer* inc = new DummyIncrementer;
  CAxis* pX = new CAxis("x", 100, 0, 99);
  CAxis* pY = new CAxis("y", 256, 0, 255);
  std::vector<CAxis*> axes;
  axes.push_back(pX);
  axes.push_back(pY);
  CSpectrum spec("test", &alloc, inc, cond, axes); // construct/destruct

  spec.clear();

  ASSERT(inc->cleared);
}
/**
 * functional
 *
 *  operator() should call incrementer's operator()
 */
void
SpectrumTests::functional()
{
  CTrueCondition* cond = new CTrueCondition;;
  DummyAllocator alloc(this);
  DummyIncrementer* inc = new DummyIncrementer;
  CAxis* pX = new CAxis("x", 100, 0, 99);
  CAxis* pY = new CAxis("y", 256, 0, 255);
  std::vector<CAxis*> axes;
  axes.push_back(pX);
  axes.push_back(pY);
  CSpectrum spec("test", &alloc, inc, cond, axes); // construct/destruct

  spec();

  ASSERT(inc->functional);
}
/**
 * get
 * Get method relays to incrementer.
 */
void
SpectrumTests::get()
{
  CTrueCondition* cond = new CTrueCondition;;
  DummyAllocator alloc(this);
  DummyIncrementer* inc = new DummyIncrementer;
  CAxis* pX = new CAxis("x", 100, 0, 99);
  CAxis* pY = new CAxis("y", 256, 0, 255);
  std::vector<CAxis*> axes;
  axes.push_back(pX);
  axes.push_back(pY);
  CSpectrum spec("test", &alloc, inc, cond, axes); // construct/destruct
  
  uint32_t value = spec.get(10, 20);
  ASSERT(inc->gotten);
  EQ(10U, inc->x);
  EQ(20U, inc->y);
  EQ(1234U, value);
    
}
/**
 * set
 *   set method relays to incrementer.
 */
void
SpectrumTests::set()
{
  CTrueCondition* cond = new CTrueCondition;;
  DummyAllocator alloc(this);
  DummyIncrementer* inc = new DummyIncrementer;
  CAxis* pX = new CAxis("x", 100, 0, 99);
  CAxis* pY = new CAxis("y", 256, 0, 255);
  std::vector<CAxis*> axes;
  axes.push_back(pX);
  axes.push_back(pY);
  CSpectrum spec("test", &alloc, inc, cond, axes); // construct/destruct
  
  spec.set(100, 200, 1234);

  ASSERT(inc->wasset);
  EQ(100U, inc->x);
  EQ(200U, inc->y);
  EQ(1234U, inc->value);
}
/**
 * checkValidity
 *   This spec function should validate both the incrementer and
 *   the condition.
 *   For this we need a custom condition.
 */
class CCalledCondition : public CCondition 
{
public:
  bool validated;
  CCalledCondition() :  validated(false) {}
  void validate(CSpectrum& spec) {validated = true;}
  bool operator()(CSpectrum& spec) {}

};
void
SpectrumTests::checkValidity()
{
  CCalledCondition* cond = new CCalledCondition;
  DummyAllocator alloc(this);
  DummyIncrementer* inc = new DummyIncrementer;
  CAxis* pX = new CAxis("x", 100, 0, 99);
  CAxis* pY = new CAxis("y", 256, 0, 255);
  std::vector<CAxis*> axes;
  axes.push_back(pX);
  axes.push_back(pY);
  CSpectrum spec("test", &alloc, inc, cond, axes); // construct/destruct

  spec.checkValidity();

  ASSERT(cond->validated);
  ASSERT(inc->validated);

  
}
