// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#define private public
#include "CParameterArray.h"
#include "CParameterDictionary.h"
#undef private
#include "CParameter.h"

#include <stdio.h>
#include <stdexcept>



class PArrayTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(PArrayTests);
  // Tests for construction in the first constructor defined.

  CPPUNIT_TEST(construct1);	// simplest case
  CPPUNIT_TEST(destruct);    // Destructor destroys elements.
  CPPUNIT_TEST(construct2);  // index offset.
  CPPUNIT_TEST(construct3);  // Second constructor.
  CPPUNIT_TEST(construct4);  // 3'd constructor
  CPPUNIT_TEST(construct5);  // 4'th constructor.

  CPPUNIT_TEST(rangecheck1);  // Turn on/off range checking.
  
  CPPUNIT_TEST(index1);
  CPPUNIT_TEST(index2);        // Biased indexing.
  CPPUNIT_TEST(rangecheck2);  // Range checking works when on. std::range_error.

  CPPUNIT_TEST(changelow);
  CPPUNIT_TEST(changehigh);
  CPPUNIT_TEST(changebins);
  CPPUNIT_TEST(changeunits);
  CPPUNIT_TEST(enablelimcheck);
  CPPUNIT_TEST(disablelimcheck);



  CPPUNIT_TEST_SUITE_END();


private:
  CParameterDictionary* m_pDict;

public:
  void setUp() {
    m_pDict = CParameterDictionary::instance();
    CParameter::enableAutoRegistration();
  }
  void tearDown() {
   delete CParameterDictionary::m_pInstance;
    CParameterDictionary::m_pInstance = 0;
    m_pDict = 0;
  }
protected:
  void construct1();
  void destruct();
  void construct2();
  void construct3();
  void construct4();
  void construct5();

  void rangecheck1();
  void index1();
  void index2();
  void rangecheck2();

  void changelow();
  void changehigh();
  void changebins();
  void changeunits();

  void enablelimcheck();
  void disablelimcheck();
};

CPPUNIT_TEST_SUITE_REGISTRATION(PArrayTests);

/**
 * construct1
 *   without a base offset, constructing should produce a 
 *   number of parameters in the dictionary e.g. 16 of them 
 *   produces basename.00 .. basename.15
 */
void PArrayTests::construct1() {
  CParameterArray* pArray = new CParameterArray("test", 16);

  for (int i =0; i < 16; i++) {
    char name[100];
    sprintf(name, "%s.%02d", "test", i);
    
    // Should be able to find name in the dictionary:

    
    CParameterDictionary::DictionaryIterator p = m_pDict->find(name);
    ASSERT(p != m_pDict->end());
  }
}
/**
 * destruct
 *    Destroying a parameter array should also destry the parameter.l
 */
void PArrayTests::destruct()
{
  CParameterArray* pArray = new CParameterArray("test", 16);
  delete pArray;
  for (int i =0; i < 16; i++) {
    char name[100];
    sprintf(name, "%s.%02d", "test", i);
    
    // Should be able to find name in the dictionary:

    
    CParameterDictionary::DictionaryIterator p = m_pDict->find(name);
    ASSERT(p == m_pDict->end());
  }

}
/**
 * construct2 - a non zero indexLow should bias the index/name
 * of the first element.
 */
void
PArrayTests::construct2()
{
  CParameterArray* pArray = new CParameterArray("test", 16, 100);
  
  EQ(100U, pArray->m_indexLow);
  EQ(std::string("test.100"), pArray->m_array[0]->getName());
}
/**
 * Construction that specifies the low/and high limit of the parameter.
 */
void 
PArrayTests::construct3()
{
  CParameterArray* pArray = new CParameterArray("test", -1.0, 1.0, 16);
  for (int i =0; i < 16; i++) {
    char name[1000];
    sprintf(name, "%s.%02d", "test", i);
    ASSERT(m_pDict->find(name) != m_pDict->end());

    CParameter* p = pArray->m_array[i];
    EQ(-1.0, p->getLow());
    EQ(1.0, p->getHigh());
  }
}
/**
 * construct4 - constructor for low, high ,units.
 */
void PArrayTests::construct4()
{
  CParameterArray* pArray = new CParameterArray("test", -1.0, 1.0,"furlongs", 16);
  for (int i =0; i < 16; i++) {
    char name[1000];
    sprintf(name, "%s.%02d", "test", i);
    ASSERT(m_pDict->find(name) != m_pDict->end());

    CParameter* p = pArray->m_array[i];
    EQ(-1.0, p->getLow());
    EQ(1.0, p->getHigh());
    EQ(std::string("furlongs"), p->getUnits());
  }
}
/**
 * construct5 - constructor with low, high , binnning and units.
 */
void
PArrayTests::construct5()
{
  CParameterArray* pArray = new CParameterArray("test", -1.0, 1.0, 100, 
						"furlongs", 16);
  for (int i =0; i < 16; i++) {
    char name[1000];
    sprintf(name, "%s.%02d", "test", i);
    ASSERT(m_pDict->find(name) != m_pDict->end());

    CParameter* p = pArray->m_array[i];
    EQ(-1.0, p->getLow());
    EQ(1.0, p->getHigh());
    EQ(100U, p->getBins());
    EQ(std::string("furlongs"), p->getUnits());
  }
}

/**
 * rangecheck1 - should be able to turn on-off range checking flag.
 */
void 
PArrayTests::rangecheck1()
{
  CParameterArray* pArray = new CParameterArray("test", 16);
  ASSERT(! pArray->m_rangeCheck);
  pArray->enableRangeCheck();
  ASSERT(pArray->m_rangeCheck);
  pArray->disableRangeCheck();
  ASSERT(!pArray->m_rangeCheck);
}
/**
 * index1
 *   The index operator should work too
 */
void
PArrayTests::index1()
{
  CParameterArray array("test", 16);
  for (int i =0; i < 16; i++) {
    char name[1000];
    sprintf(name, "%s.%02d", "test", i);
    EQ(std::string(name), array[i].getName());
  }
}
/**
 * index2
 *  Ensure indexing works when we use offsets as well:
 */
void
PArrayTests::index2()
{
  CParameterArray array("test", 16, 5);
  for (int i = 5; i < 5+16; i++) {
    char name[1000];
    sprintf(name, "%s.%02d", "test", i);
    EQ(std::string(name), array[i].getName());    
  }
}
/**
 * rangecheck2
 *   When range checks are enabled indexing out of bounds should
 *   throw std:range_error.
 */
void
PArrayTests::rangecheck2()
{
  CParameterArray array("test", 16, 5);
  array.enableRangeCheck();

  bool threw = false;
  bool rthrew= false;
  std::string msg;
  std::string correctms("CParameterArray index is out of bounds");
  
  try {
    array[0].getName();
  }
  catch(std::range_error e) {
    threw = true;
    rthrew= true;
    msg = e.what();
  }
  catch (...) {
    threw = true;
  }
  ASSERT(threw);
  ASSERT(rthrew);
  EQ(correctms, msg);

  threw = false;
  rthrew = false;
  msg = "";
  try {
    array[100].getName();
  }
  catch(std::range_error e) {
    threw = true;
    rthrew= true;
    msg = e.what();
  }
  catch (...) {
    threw = true;
  }
  ASSERT(threw);
  ASSERT(rthrew);
  EQ(correctms, msg);
}
/**
 * changelow
 *   changeLow - should change the low limit of all the members of the
 *   array.
 */
void
PArrayTests::changelow()
{
  CParameterArray array("test", 0, 1, 16);
  array.changeLow(-1);
  for(int i =0; i < 15; i++) {
    EQ(-1.0, array[i].getLow());
  }
}
/**
 * changehigh
 *   should change the high limit of all members of the array.
 */
void
PArrayTests::changehigh()
{
  CParameterArray array("test", -1.0, 0.0, 16);
  array.changeHigh(1);
  for (int i =0; i < 15; i++) {
    EQ(1.0, array[i].getHigh());
  }
}
/**
 * changebins
 *   changeBins should do all elements of the array.
 */
void
PArrayTests::changebins()
{
  CParameterArray array("test", -1.0, 1.0, 100, "units", 16);
  array.changeBins(200);
  for (int i =0;i < 15; i++) {
    EQ(200U, array[i].getBins());
  }
}
/**
 * changeunits
 * changeUnits on the array should do all elements.
 */
void
PArrayTests::changeunits()
{
  CParameterArray array("test", -1.0, 1.0, 100, "units", 16);
  array.changeUnits("furlongs");
  for (int i = 0; i < 15; i++) {
    EQ(std::string("furlongs"), array[i].getUnits());
  }
}
/**
 *  enablelimcheck
 *
 *     enableLimitCheck should do that for all array elements.
 */
void
PArrayTests::enablelimcheck()
{
  CParameterArray array("test", -1.0, 1.0, 100, "units", 16);
  array.enableLimitCheck();
  for (int i =0; i < 15; i++) {
    ASSERT(array[i].checkLimits());
  }
}
/**
 * disablelimcheck
 *   disableLimitcheck should do that for all elements.
 */
void
PArrayTests::disablelimcheck()
{
  CParameterArray array("test", -1.0, 1.0, 100, "units", 16);
  array.enableLimitCheck();
  array.disableLimitCheck();
  for (int i =0; i < 15; i++) {
    ASSERT(!array[i].checkLimits());
  }
}
