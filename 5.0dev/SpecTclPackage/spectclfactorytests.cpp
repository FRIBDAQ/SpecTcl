// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"


#include "CSpecTclHistogrammerFactory.h"
#include "CSpectrumIncrementer.h"

// Need acces to private methods of these classes for testing:

#define private public
#include "CParameterDictionary.h"
#undef private

#include "CParameter.h"		//  probably this order is required.

// This file's going to contain a test suite per 
// method of the factory class since testing can get quite involved.
//



class Alloc1d : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(Alloc1d);
  CPPUNIT_TEST(requireX);
  CPPUNIT_TEST(requireNoY);
  CPPUNIT_TEST(paramMustExist);
  CPPUNIT_TEST(ok);		// Should give me an allocator.
  CPPUNIT_TEST_SUITE_END();


private:
  CSpecTclHistogrammerFactory* m_pFact;
public:
  void setUp() {
    m_pFact = new CSpecTclHistogrammerFactory;
  }
  void tearDown() {
    delete m_pFact;
    CParameterDictionary::instance()->clearDict(); // Get rid of parameters.
    delete CParameterDictionary::m_pInstance;
    CParameterDictionary::m_pInstance = 0;
    CParameterDictionary::m_nNextParamNo = 0;
    
  }
protected:
  void requireX();
  void requireNoY();
  void paramMustExist();
  void ok();
};

CPPUNIT_TEST_SUITE_REGISTRATION(Alloc1d);


// If not exactly 1 x parameter, should throw

void Alloc1d::requireX() {
  bool threw      = false;
  bool threwRight = false;
  std::string msg;

  try {
    std::vector<std::string> x;
    CSpectrumIncrementer* p = m_pFact->create1dIncrementer(x,x);
  }
  catch (histogrammer_factory_exception& e) {
    threw      = true;
    threwRight = true;
    msg        = e.what();
  }
  catch(...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(threwRight);
  EQ(std::string("CSpecTclHistogrammerFactory::create1dIncrementer needs exactly 1 X parameter"), 
		 msg);

  // Try 2:

  threw = false;
  threwRight = false;
  msg = "";
  try {
    std::vector<std::string> x;
    std::vector<std::string> y;
    x.push_back("param1");
    x.push_back("param2");
    
    CSpectrumIncrementer* p = m_pFact->create1dIncrementer(x,y);
  }
  catch (histogrammer_factory_exception& e) {
    threw      = true;
    threwRight = true;
    msg        = e.what();
  }
  catch(...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(threwRight);
  EQ(std::string("CSpecTclHistogrammerFactory::create1dIncrementer needs exactly 1 X parameter"), 
		 msg);


}
// not allowed any y parameters.

void Alloc1d::requireNoY()
{
  bool threw      = false;
  bool threwRight = false;
  std::string msg;

  try {
    std::vector<std::string> x;
    std::vector<std::string> y;
    x.push_back("param1");
    y.push_back("bad");

    CSpectrumIncrementer* p = m_pFact->create1dIncrementer(x,y);
  }
  catch (histogrammer_factory_exception& e) {
    threw = true;
    threwRight = true;
    msg = e.what();
  }
  catch (...) {
    threw = true;
    threwRight = true;
  }

  ASSERT(threw);
  ASSERT(threwRight);
  EQ(std::string("CSpecTclHistogrammerFactory::create1dIncrementer needs exactly 0 Y parameters"), 
		 msg);
  
}
// When the spectrum is initially created the x parameter must exist.

void Alloc1d::paramMustExist()
{
  bool threw      = false;
  bool threwRight = false;
  std::string msg;

  try {
    std::vector<std::string> x;
    std::vector<std::string> y;
    x.push_back("nonexistent");
    
    CSpectrumIncrementer* p = m_pFact->create1dIncrementer(x,y);

  }
  catch (histogrammer_factory_exception& e) {
    threw = true;
    threwRight = true;
    msg = e.what();
  }
  catch (...) {
    threw  = true;
  }

  ASSERT(threw);
  ASSERT(threwRight);
  EQ(std::string("CSpecTclHistogrammerFactory::create1dIncrementer X parameter does not exist"),
     msg);

}
// When all the stars align I should get an incrementer back:

void Alloc1d::ok()
{
  CParameter* pX = new CParameter("X-param");
  std::vector<std::string> x;
  std::vector<std::string> y;

  x.push_back("X-param");

  CSpectrumIncrementer* pInc = m_pFact->create1dIncrementer(x, y);
  ASSERT(pInc);
  delete pInc;
}


class Alloc2d : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(Alloc2d);
  CPPUNIT_TEST(needX);
  CPPUNIT_TEST(needY);
  CPPUNIT_TEST(xMustExist);
  CPPUNIT_TEST(yMustExist);
  CPPUNIT_TEST(ok);
  CPPUNIT_TEST_SUITE_END();


private:
  CSpecTclHistogrammerFactory* m_pFact;
public:
  void setUp() {
    m_pFact = new CSpecTclHistogrammerFactory;
  }
  void tearDown() {
    delete m_pFact;
    CParameterDictionary::instance()->clearDict(); // Get rid of parameters.
    delete CParameterDictionary::m_pInstance;
    CParameterDictionary::m_pInstance = 0;
    CParameterDictionary::m_nNextParamNo = 0;
    
  }
protected:
  void needX();
  void needY();
  void xMustExist();
  void yMustExist();
  void ok();
};

CPPUNIT_TEST_SUITE_REGISTRATION(Alloc2d);

// Need exactly one x parameter.

void
Alloc2d::needX()
{
  bool threw      = false;
  bool threwRight = false;
  std::string msg;

  try {
    std::vector<std::string> x;
    CSpectrumIncrementer* p = m_pFact->create2dIncrementer(x,x);
  }
  catch (histogrammer_factory_exception& e) {
    threw      = true;
    threwRight = true;
    msg        = e.what();
  }
  catch(...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(threwRight);
  EQ(std::string("CSpecTclHistogrammerFactory::create2dIncrementer needs exactly 1 X parameter"), 
		 msg);

  // Try 2:

  threw = false;
  threwRight = false;
  msg = "";
  try {
    std::vector<std::string> x;
    std::vector<std::string> y;
    x.push_back("param1");
    x.push_back("param2");
    
    CSpectrumIncrementer* p = m_pFact->create2dIncrementer(x,y);
  }
  catch (histogrammer_factory_exception& e) {
    threw      = true;
    threwRight = true;
    msg        = e.what();
  }
  catch(...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(threwRight);
  EQ(std::string("CSpecTclHistogrammerFactory::create2dIncrementer needs exactly 1 X parameter"), 
		 msg);

}
// Need exactly 1 y parameter.

void
Alloc2d::needY()
{
  bool threw      = false;
  bool threwRight = false;
  std::string msg;

  try {
    std::vector<std::string> x;
    std::vector<std::string> y;

    x.push_back("xparam");
    CSpectrumIncrementer* p = m_pFact->create2dIncrementer(x,y);
  }
  catch (histogrammer_factory_exception& e) {
    threw      = true;
    threwRight = true;
    msg        = e.what();
  }
  catch(...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(threwRight);
  EQ(std::string("CSpecTclHistogrammerFactory::create2dIncrementer needs exactly 1 Y parameter"), 
		 msg);

  // Try 2:

  threw = false;
  threwRight = false;
  msg = "";
  try {
    std::vector<std::string> x;
    std::vector<std::string> y;
    x.push_back("xparam");
    y.push_back("param1");
    y.push_back("param2");
    
    CSpectrumIncrementer* p = m_pFact->create2dIncrementer(x,y);
  }
  catch (histogrammer_factory_exception& e) {
    threw      = true;
    threwRight = true;
    msg        = e.what();
  }
  catch(...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(threwRight);
  EQ(std::string("CSpecTclHistogrammerFactory::create2dIncrementer needs exactly 1 Y parameter"), 
		 msg);
}
//  The X parameter must exist:

void
Alloc2d::xMustExist()
{
  bool threw      = false;
  bool threwRight = false;
  std::string      msg;

  try {
    std::vector<std::string> x;
    std::vector<std::string> y;
    x.push_back("nosuchx");
    y.push_back("nosuchy");

    CSpectrumIncrementer* pInc = m_pFact->create2dIncrementer(x,y);
  }
  catch (histogrammer_factory_exception& e) {
    threw = true;
    threwRight = true;
    msg = e.what();
  }
  catch (...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(threwRight);
  EQ(std::string("CSpecTclHistogrammerFactory::create2dIncrementer X parameter does not exist"), msg);
}

// The Y parameter must exist:

void
Alloc2d::yMustExist() 
{
  bool threw      = false;
  bool threwRight = false;
  std::string      msg;
  CParameter*      pX = new CParameter("xExists");

  try {
    std::vector<std::string> x;
    std::vector<std::string> y;
    x.push_back("xExists");
    y.push_back("nosuchy");

    CSpectrumIncrementer* pInc = m_pFact->create2dIncrementer(x,y);
  }
  catch (histogrammer_factory_exception& e) {
    threw = true;
    threwRight = true;
    msg = e.what();
  }
  catch (...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(threwRight);
  EQ(std::string("CSpecTclHistogrammerFactory::create2dIncrementer Y parameter does not exist"), msg);
}
// If everything is done right we should get an incrementer out of this:

void Alloc2d::ok()
{
  CParameter* pX = new CParameter("x-parameter");
  CParameter* pY = new CParameter("y-parameter");
  std::vector<std::string> X;
  std::vector<std::string> Y;
  X.push_back("x-parameter");
  Y.push_back("y-parameter");

  CSpectrumIncrementer* pInc = m_pFact->create2dIncrementer(X,Y);

  ASSERT(pInc);

  delete pInc;

}

// Tests for createSum1dIncrementer:

class AllocSummary1 : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(AllocSummary1);
  CPPUNIT_TEST(needXparams);
  CPPUNIT_TEST(need0Yparams);
  CPPUNIT_TEST(xParamsExist);
  CPPUNIT_TEST(ok);
  CPPUNIT_TEST_SUITE_END();


private:
  CSpecTclHistogrammerFactory* m_pFact;
public:
  void setUp() {
    m_pFact = new CSpecTclHistogrammerFactory;
  }
  void tearDown() {
    delete m_pFact;
    CParameterDictionary::instance()->clearDict(); // Get rid of parameters.
    delete CParameterDictionary::m_pInstance;
    CParameterDictionary::m_pInstance = 0;
    CParameterDictionary::m_nNextParamNo = 0;
    
  }
protected:
  void needXparams();
  void need0Yparams();
  void xParamsExist();
  void ok();
};

CPPUNIT_TEST_SUITE_REGISTRATION(AllocSummary1);

//  There needs to be at least 1 x parameter.

void
AllocSummary1::needXparams()
{
  bool threw = false;
  bool threwRight = false;
  std::string msg;

  std::vector<std::string> params;

  try {
    CSpectrumIncrementer* pIncm = m_pFact->createSum1dIncrementer(params, params);
  }
  catch (histogrammer_factory_exception& e) {
    threw = true;
    threwRight = true;
    msg = e.what();
  }
  catch(...) {
    threw = true;
  }

  ASSERT(threw);
  ASSERT(threwRight);

  EQ(std::string("CSpecTclHistogrammerFactory::createSum1dIncrementer needs at least 1 X parameter"), msg);
}

//  There needs to be 0 y parameters.

void
AllocSummary1::need0Yparams()
{
  bool threw = false;
  bool threwRight = false;
  std::string msg;
  std::vector<std::string> xparams;
  std::vector<std::string> yparams;

  xparams.push_back("anx-parameter");
  yparams.push_back("aY-parameter");
  try {
    CSpectrumIncrementer* pInc = m_pFact->createSum1dIncrementer(xparams, yparams);
  }
  catch (histogrammer_factory_exception& e) {
    threw = true;
    threwRight = true;
    msg = e.what();
  }
  catch (...) {
    threw = true;
  }
  ASSERT(threw);
  ASSERT(threwRight);
  EQ(std::string("CSpecTclHistogrammerFactory::createSum1dIncrementer needs exactly 0 Y parameters"), msg);
}

//  All x parameters must be defined.

void
AllocSummary1::xParamsExist()
{
  bool threw = false;
  bool threwRight = false;
  std::string msg;

  std::vector<std::string> xparams;
  std::vector<std::string> yparams;

  CParameter* x1 = new CParameter("x1");
  CParameter* x2 = new CParameter("x2");
  CParameter* x3 = new CParameter("x3");
  CParameter* x5 = new CParameter("x5"); // x4 intentionally missing so it will throw.

  xparams.push_back("x1");
  xparams.push_back("x2");
  xparams.push_back("x3");
  xparams.push_back("x4");
  xparams.push_back("x5");

  try {
    CSpectrumIncrementer* pInc = m_pFact->createSum1dIncrementer(xparams, yparams);
  }
  catch (histogrammer_factory_exception& e) {
    threw = true;
    threwRight = true;
    msg = e.what();
  }
  catch (...) {
    threw = true;
  }
  ASSERT(threw);
  ASSERT(threwRight);
  EQ(std::string("CSpecTclHistogrammerFactory::createSum1dIncrementer x4 does not exist"), msg);

  
}

//  If all preconditions are met we should get an allocator back.

void 
AllocSummary1::ok()
{
  bool threw = false;
  bool threwRight = false;
  std::string msg;

  std::vector<std::string> xparams;
  std::vector<std::string> yparams;

  CParameter* x1 = new CParameter("x1");
  CParameter* x2 = new CParameter("x2");
  CParameter* x3 = new CParameter("x3");
  CParameter* x4 = new CParameter("x4");
  CParameter* x5 = new CParameter("x5"); 

  xparams.push_back("x1");
  xparams.push_back("x2");
  xparams.push_back("x3");
  xparams.push_back("x4");
  xparams.push_back("x5");

  CSpectrumIncrementer* pInc = m_pFact->createSum1dIncrementer(xparams, yparams);

  
  ASSERT(pInc);
  delete pInc;
}

// Tests for create2dComboIncrementer

class AllocCombo : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(AllocCombo);
  CPPUNIT_TEST(atleast1X);
  CPPUNIT_TEST(atleast1Y);
  CPPUNIT_TEST_SUITE_END();


private:
  CSpecTclHistogrammerFactory* m_pFact;
public:
  void setUp() {
    m_pFact = new CSpecTclHistogrammerFactory;
  }
  void tearDown() {
    delete m_pFact;
    CParameterDictionary::instance()->clearDict(); // Get rid of parameters.
    delete CParameterDictionary::m_pInstance;
    CParameterDictionary::m_pInstance = 0;
    CParameterDictionary::m_nNextParamNo = 0;
    
  }
protected:
  void atleast1X();
  void atleast1Y();
};

CPPUNIT_TEST_SUITE_REGISTRATION(AllocCombo);

// There must be at least 1 x parameter.

void
AllocCombo::atleast1X()
{
  std::vector<std::string> params;
  bool threw = false;
  bool threwRight = false;
  std::string msg;

  try {
    CSpectrumIncrementer* pInc = m_pFact->create2dComboIncrementer(params, params);
  }
  catch (histogrammer_factory_exception& e) {
    threw = true;
    threwRight = true;
    msg = e.what();
  }
  catch (...) {
    threw = true;
  }
  ASSERT(threw);
  ASSERT(threwRight);
  EQ(std::string("CSpecTclHistogrammerFactory::create2dComboIncrementer needs at least 1 X parameter"), msg);
}


// There must be at least 1 y parameter.

void 
AllocCombo::atleast1Y()
{
  std::vector<std::string> xparams;
  std::vector<std::string> yparams;
  bool threw = false;
  bool threwRight = false;
  std::string msg;

  xparams.push_back("anX-param");
  try {
    CSpectrumIncrementer* pInc = m_pFact->create2dComboIncrementer(xparams, yparams);
  }
  catch (histogrammer_factory_exception& e) {
    threw = true;
    threwRight = true;
    msg = e.what();
  }
  catch (...) {
    threw =true;
  }
  ASSERT(threw);
  ASSERT(threwRight);
  EQ(std::string("CSpecTclHistogrammerFactory::create2dComboIncrementer needs at least 1 Y parameter"), msg);
}

// All X parameters must be defined.

// All Y parameters must be defined.

// If everything above was satisfied, we'll get an incrementer

