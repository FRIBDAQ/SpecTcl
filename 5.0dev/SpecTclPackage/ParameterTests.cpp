// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#define private public
#include "CParameter.h"
#include "CParameterDictionary.h"
#undef private

#include <algorithm>
#include <math.h>


class ParameterTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ParameterTests);
  CPPUNIT_TEST(defaultCons);
  CPPUNIT_TEST(limitsCons);
  CPPUNIT_TEST(unitsCons);
  CPPUNIT_TEST(binsCons);
  CPPUNIT_TEST(destruct);

  CPPUNIT_TEST(opequals);
  CPPUNIT_TEST(opne);

  CPPUNIT_TEST(identi1);

  CPPUNIT_TEST(assdbl);
  CPPUNIT_TEST(assobj);

  CPPUNIT_TEST(dup);
  CPPUNIT_TEST(getname);
  CPPUNIT_TEST(getlow);
  CPPUNIT_TEST(gethigh);
  CPPUNIT_TEST(getunits);
  CPPUNIT_TEST(getbins);

  CPPUNIT_TEST(setvalue);
  CPPUNIT_TEST(changelow);
  CPPUNIT_TEST(changelowPropagates);
  CPPUNIT_TEST(changehigh);
  CPPUNIT_TEST(changehighPropagates);

  CPPUNIT_TEST(changebins);
  CPPUNIT_TEST(changebinsPropagates);
  CPPUNIT_TEST(changeunits);
  CPPUNIT_TEST(changeunitsPropagates);
  CPPUNIT_TEST(enablelimcheck);
  CPPUNIT_TEST(disablelimcheck);
  CPPUNIT_TEST(isvalid);
  CPPUNIT_TEST(checklimits);
  CPPUNIT_TEST(invalidate);
  CPPUNIT_TEST(resetall);




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
  void defaultCons();
  void limitsCons();
  void unitsCons();
  void binsCons();
  void destruct();

  void opequals();
  void opne();
  
  void identi1();

  void assdbl();
  void assobj();

  void dup();

  void getname();
  void getlow();
  void gethigh();
  void getunits();
  void getbins();

  void setvalue();
  void changelow();
  void changelowPropagates();
  void changehigh();
  void changehighPropagates();
  void changebins();
  void changebinsPropagates();
  void changeunits();
  void changeunitsPropagates();

  void enablelimcheck();
  void disablelimcheck();

  void isvalid();
  void checklimits();
  void invalidate();
  void resetall();
private:
  void checkRegistered(CParameter* pParam)
  {
    std::string name = pParam->getName();
    CParameterDictionary::DictionaryIterator pItem = m_pDict->find(name.c_str());
    ASSERT(pItem != m_pDict->end());
    CParameterDictionary::pParameterInfo pInfo = pItem->second;
    ASSERT(std::find(pInfo->s_references.begin(), pInfo->s_references.end(), pParam)  
	   != pInfo->s_references.end());
  }
  


};

CPPUNIT_TEST_SUITE_REGISTRATION(ParameterTests);

/**
 * NOTE: all parameter creations must be dynamic to support
 * the destruction of the dictionary which will delete registered params.

/**
 * defaultCons
 *
 *  Test the default constructor.  This is partly black box...the only
 *  thing we expect the default constructor to set is the name.
 *  with auto-registration turned on all constructors should register
 *  the parameter with the dictionay.
 */
void ParameterTests::defaultCons() {
   const char* pName = "testing";
   CParameter* pParam = new CParameter(pName); 
  
  // Check the contents:

  EQ(std::string(pName), pParam->m_name);
  checkRegistered(pParam);
  
}
/**
 * limitsCons
 *    Construct supplying limits require the parameter
 *    have name and correct limits as well as having been registered in the
 *    dict.
 */
void
ParameterTests::limitsCons()
{
  const char* pName = "testing";
  double low = 0.0;
  double hi  = 4095.0;
  CParameter* pParam = new CParameter(pName, low, hi);

  EQ(std::string(pName), pParam->m_name);
  EQ(low,                pParam->m_low);
  EQ(hi,                 pParam->m_high);

  checkRegistered(pParam);

}
/**
 * unitscons
 *  Construct supplying low/high and units.
 */
void
ParameterTests::unitsCons()
{
  const char* pName = "testing";
  double low        = 0.0;
  double hi         = 1023.0;
  const char* pUnits= "millfurlongs";

  CParameter* pParam = new CParameter(pName, low, hi, pUnits);

  EQ(std::string(pName), pParam->m_name);
  EQ(low,                pParam->m_low);
  EQ(hi,                 pParam->m_high);
  EQ(std::string(pUnits),pParam->m_units);

  checkRegistered(pParam);
  
}
/**
 * binsCons
 *   Construct with bins supplied
 */
void
ParameterTests::binsCons()
{
  const char* pName = "testing";
  double low        = 0.0;
  double hi         = 1023.0;
  const char* pUnits= "millifurlongs";
  unsigned bins     = 1024;

  CParameter* pParam = new CParameter(pName, low, hi, bins, pUnits);

  EQ(std::string(pName), pParam->m_name);
  EQ(low,                pParam->m_low);
  EQ(hi,                 pParam->m_high);
  EQ(std::string(pUnits),pParam->m_units);
  EQ(bins,               pParam->m_bins);

  checkRegistered(pParam);
  
}
/**
 * destruct
 *  If the parameter is registered in the dictionary it should get removed..
 *  and if the only one, the parameter destroyed.
 */
void ParameterTests::destruct()
{
  CParameter* pParam = new CParameter("testing"); // we've got autoregister on.
  delete pParam;				  // should drop from dict..

  ASSERT(m_pDict->find("testing") == m_pDict->end());
}
  
/**
 * opequals
 *   CParameters are equal if their underlying valid value are equal.
 */
void 
ParameterTests::opequals()
{
  CParameter* pParam1 = new CParameter("test1");
  CParameter* pParam2 = new CParameter("test2");
  CParameter* pParam3 = new CParameter("test1");
  double piish = 3.14161;	// Using a literal both places is wonky with g++?!?

  // pParam1 and pParam2 are only equal if their underyling values
  // have the same number.
  // Need to set values as otherwise the actual get throws.

  *(pParam1->m_parameter) = piish;
  *(pParam2->m_parameter) = 0.0;

  // Test1 and test3 are always equal since they are the same 
  // valid value
  
  ASSERT(*pParam1 == *pParam3);


  ASSERT(!(*pParam1 == *pParam2));
  (*pParam2->m_parameter) = piish;
  ASSERT (*pParam1 == *pParam2);
}
/**
 * opne
 *  test the !=  operator.
 */
void
ParameterTests::opne() {
  CParameter* pParam1 = new CParameter("test1");
  CParameter* pParam2 = new CParameter("test2");
  
  double piish = 3.14161;	// Using a literal both places is wonky with g++?!?

  // pParam1 and pParam2 are only equal if their underyling values
  // have the same number.
  // Need to set values as otherwise the actual get throws.

  *(pParam1->m_parameter) = piish;
  *(pParam2->m_parameter) = 0.0;
  ASSERT(*pParam1 != *pParam2);
}
/**
 * identi1
 *
 *  Test identicality(means all properties match).
 */
void
ParameterTests::identi1()
{
  CParameter* p1 = new CParameter("t1", 0, 100, 101, "Units");
  CParameter* p2 = new CParameter("t2", 0, 100, 101, "Units");
  CParameter* p3 = new CParameter("t3", -1, 1, 100, "milliunits");

  ASSERT(p1->identical(*p2));
  ASSERT(! p1->identical(*p3));
}
/**
 * assdbl
 *   assign to an object from a double.
 */
void ParameterTests::assdbl()
{
  CParameter* p1 = new CParameter("p1");


  *p1 = 2.0;
  ASSERT(p1->m_parameter->isvalid());
  EQ(2.0, (double)(*p1->m_parameter));
}
/** assobj
 *    Assign from another CParameter object.
 */
void
ParameterTests::assobj()
{
  CParameter* p1 = new CParameter("p1");
  CParameter* p2 = new CParameter("p2");

  *p2 = 2.0;
  *p1 = *p2;			// assignment from obj.

  ASSERT(*p1 == *p2);		// Underlying values should now ==.
}
/**
 * dup
 *   Copy everything but the name and value from
 *   another obj
 */
void
ParameterTests::dup()
{
  CParameter* p1 = new CParameter("abc", 0, 100, 101, "units");
  CParameter* p2 = new CParameter("cde");
  p2->dup(*p1);

  EQ(p1->m_low, p2->m_low);
  EQ(p1->m_high, p2->m_high);
  EQ(p1->m_bins, p2->m_bins);
  EQ(p1->m_units, p2->m_units);
}
/**
 * getname
 *   getName returns the name of the parameter.
 */
void
ParameterTests::getname()
{
  std::string name("a name");
  CParameter* p1 = new CParameter(name.c_str());
  EQ(name, p1->getName());
}
/**
 * getlow
 ** getLow returns the low limit.
 */
void
ParameterTests::getlow()
{
  CParameter* p1 = new CParameter("a", -1, 1);
  EQ(-1.0, p1->getLow());
}
/**
 * gethigh
 *   Return the upper limit on a parameter
 */
void
ParameterTests::gethigh()
{
  CParameter* p1 = new CParameter("a", -1, 1);
  EQ(1.0, p1->getHigh());

}
/**
 * getunits
 *   Return a parameter's units of measure.
 */
void
ParameterTests::getunits()
{
  std::string units = "centiliters/fortnight"; // units of flow.

  CParameter* p1 = new CParameter("a", -1, 1, 100, units.c_str());
  EQ(units, p1->getUnits());

}
/**
 * getbins
 *   return a parameter's requested binnin.
 */
void
ParameterTests::getbins()
{
  std::string units = "centiliters/fortnight"; // units of flow.

  CParameter* p1 = new CParameter("a", -1, 1, 100, units.c_str());
  EQ(100U, p1->getBins());

}
/*
 * setvalue
 *
 *   Sets the underyling value object. This should really only
 *   be used by CParameterDictionary when adding a parameter
 */
void
ParameterTests::setvalue()
{
  CValidValue<double> aValue;
  CParameter* p  = new CParameter("p");
  p->setValue(&aValue);
  EQ(&aValue, p->m_parameter);
}
/**
 * changelow - change the low limit.
 */
void
ParameterTests::changelow()
{
  CParameter* p1 = new CParameter("a", -1, 1, 100, "milli-man-hours");
  p1->changeLow(-100);
  
  EQ(-100.0, p1->m_low);
}
/**
 * changelowPropagates
 *   changeLow should sync the low values on all parameters that use
 *   the same underlying param.
 */
void
ParameterTests::changelowPropagates()
{
  CParameter* p1 = new CParameter("a", -1, 1, 100, "milli-man-hours");
  CParameter* p2 = new CParameter("a", -1, 1, 100, "milli-man-hours");

  p1->changeLow(-100);

  EQ(p1->getLow(), p2->getLow());
}
/**
 * changehigh
 *
 *   Change the m_high value of a parameter.
 */
void
ParameterTests::changehigh()
{
  CParameter* p1 = new CParameter("a", -1, 1, 100, "millibarns/square-degree");
  p1->changeHigh(100);
  EQ(100.0, p1->getHigh());
}
void
ParameterTests::changehighPropagates()
{
  CParameter* p1 = new CParameter("a", -1, 1, 100, "milli-man-hours");
  CParameter* p2 = new CParameter("a", -1, 1, 100, "milli-man-hours");

  p1->changeHigh(100);
  EQ(p1->getHigh(), p2->getHigh());

}
/**
 * changebins
 *
 *   Changes the suggested binnning for a parameter.
 */
void
ParameterTests::changebins()
{
  CParameter* p1 = new CParameter("a", -1, 1, 100, "millibarns/man-hour");
  p1->changeBins(200);
  EQ(200U, p1->getBins());
}
/**
 * changebinsPropagates
 *
 *  Changes to the binning should propagate to all referring
 *  parameters.
 */
void
ParameterTests::changebinsPropagates()
{
  CParameter* p1 = new CParameter("a", -1, 1, 100, "millibarns/man-hour");
  CParameter* p2 = new CParameter("a", -1, 1, 100, "millibarns/man-hour");
  p1->changeBins(200);

  EQ(200U, p2->getBins());
}
/**
 * changeunits
 *   Change the units of measure for a parameter.
 */
void
ParameterTests::changeunits()
{
  CParameter* p1 = new CParameter("a", -1, 1, 100, "millibarns/man-hour");
  p1->changeUnits("furlongs/fortnight");
  EQ(std::string("furlongs/fortnight"), p1->getUnits());
  
}
/**
 * changeunitsPropagates --etc.
 */
void 
ParameterTests::changeunitsPropagates()
{
  CParameter* p1 = new CParameter("a", -1, 1, 100, "millibarns/man-hour");
  CParameter* p2 = new CParameter("a", -1, 1, 100, "millibarns/man-hour");
  p1->changeUnits("furlongs/fortnight");

  EQ(std::string("furlongs/fortnight"), p2->getUnits());

}
/**
 * enablelimcheck - note limit checking is not propogated but
 *                  applies only to the binding set on.
 */
void ParameterTests::enablelimcheck()
{
  CParameter* p1 = new CParameter("a", -1, 1, 100, "millibarns/man-hour");
  ASSERT(!p1->m_checkLimits);
  p1->enableLimitCheck();
  ASSERT(p1->m_checkLimits);
}
/**
 * disablelimcheck
 *
 *  Turns off limit checking
 */
void ParameterTests::disablelimcheck()
{
  CParameter* p1 = new CParameter("a", -1, 1, 100, "millibarns/man-hour");
  p1->enableLimitCheck();
  p1->disableLimitCheck();
  ASSERT(!p1->m_checkLimits);
}
/**
 * isvalid 
 *   Validitity should match that of the underlying value.
 */
void ParameterTests::isvalid()
{
  CParameter* p1 = new CParameter("a", -1, 1, 100, "millibarns/man-hour");
  ASSERT(! p1->isvalid());

  *p1 = 1.234;
  ASSERT(p1->isvalid());
  
}
/**
 * checklimits
 *
 *  enable limit checking and see if out of limit throws
 *  an exception.
 *  disable should not throw.
 */
void
ParameterTests::checklimits()
{
  CParameter* p1 = new CParameter("a", -1, 1, 100, "millibarns/man-hour");
  p1->enableLimitCheck();

  bool threw = false;
  bool rightthrow = false;
  std::string msg;
  try {
    *p1 = -2;			// Out of low limit.
  }
  catch (parameter_limit e) {
    threw = true;
    rightthrow  = true;
    msg = e.what();
  } 
  catch (...) {
    threw = true;
  }
  ASSERT(threw);
  ASSERT(rightthrow);
  EQ(std::string("Value for parameter fails limit check"), msg);

  threw = false;
  rightthrow = false;
  msg = "";

  try {
    *p1 = 1.5;			// Too big.
  }
  catch (parameter_limit e) {
    threw = true;
    rightthrow  = true;
    msg = e.what();
  } 
  catch (...) {
    threw = true;
  }
  ASSERT(threw);
  ASSERT(rightthrow);
  EQ(std::string("Value for parameter fails limit check"), msg);

   
  threw = false;
  rightthrow = false;
  msg = "";
  try {
    *p1 = 0.0;			// just right.
  }
 catch (parameter_limit e) {
    threw = true;
    rightthrow  = true;
    msg = e.what();
  } 
  catch (...) {
    threw = true;
  }
  ASSERT(! threw);
  
}
/**
 * invalidate
 *
 *  Calling reset shouild reset the validity flag.
 */
void ParameterTests::invalidate()
{
  CParameter *p1= new CParameter("a", -1, 1, 100, "millibarns/man-hour");
  *p1 = 1.0;			// valid.
  p1->reset();
  ASSERT(!p1->isvalid());
}
/**
 * resetall
 *   invalidateall invalidates all registered parameters.
 *   This is a static method.
 */
void ParameterTests::resetall()
{
  CParameter *p1= new CParameter("a", -1, 1, 100, "millibarns/man-hour");
  CParameter *p2 = new CParameter("b" -100, 100, 201, "Units");

  // Make bot parameters valid

  *p1 = 2;
  *p2 = -6;

  CParameter::invalidateAll();	// all should now be invalid:

  ASSERT(!p1->isvalid());
  ASSERT(!p2->isvalid());

}
