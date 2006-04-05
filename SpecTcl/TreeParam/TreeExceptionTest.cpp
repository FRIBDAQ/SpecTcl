// This implementation of TreeParameter is based on the ideas and original code of::
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//
// Template for a test suite.

#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CTreeException.h"
#include <string>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif




class TreeExceptionTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TreeExceptionTest);
  CPPUNIT_TEST(Relops);		// Test relational operations.
  CPPUNIT_TEST(Assigns);        // Test assignments and copy construction.
  CPPUNIT_TEST(ReasonText);	// Test reason text generation.
  CPPUNIT_TEST(ReasonCodes);	// Test unit code handling.
  CPPUNIT_TEST_SUITE_END();


private:
  // We'll have one of each type of exception:

  CTreeException* m_pNotBound;
  CTreeException* m_pBound;
  CTreeException* m_pNoParam;
  CTreeException* m_pHaveParam;
  CTreeException* m_pInvalidIndex;
  CTreeException* m_pUnitsMismatch;
  CTreeException* m_pInvalid;
  CTreeException* m_pBadCode;
public:
  void setUp() {
    m_pNotBound = new CTreeException(CTreeException::NotBound,
				     "Not bound test");
    m_pBound    = new CTreeException(CTreeException::Bound,
				     "Bound test");
    m_pNoParam  = new CTreeException(CTreeException::NoParam,
				     "No parameter test");
    m_pHaveParam= new CTreeException(CTreeException::HaveParam,
				     "Have parameter test");
    m_pInvalidIndex = new CTreeException(CTreeException::InvalidIndex,
					 "bad index test");
    m_pUnitsMismatch = new CTreeException(CTreeException::UnitsMismatch,
					  "Unitsmismatch test");
    m_pInvalid  = new CTreeException(CTreeException::Invalid,
					  "Validly invalid test");
    m_pBadCode  = new CTreeException((CTreeException::TreeErrorReason)123,
					  "Really bad code test");
  }
  void tearDown() {
    delete m_pNotBound;
    delete m_pBound;
    delete m_pNoParam;
    delete m_pHaveParam;
    delete m_pInvalidIndex;
    delete m_pUnitsMismatch;
    delete m_pInvalid;
    delete m_pBadCode;
  }
protected:
  // Tests here.

  void Relops();
  void Assigns();
  void ReasonText();
  void ReasonCodes();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TreeExceptionTest);


void
TreeExceptionTest::Relops()
{
  // Equality Requires identical reason and text:

  CTreeException NotBound(CTreeException::NotBound,
			  "Not bound test");

  ASSERT(NotBound == *m_pNotBound);
  ASSERT(NotBound != *m_pHaveParam);

}

void
TreeExceptionTest::Assigns()
{
  CTreeException NotBound(*m_pNotBound);

  ASSERT(NotBound == *m_pNotBound); // Copy results in equality.
  
  NotBound = *m_pBound;
  ASSERT(NotBound != *m_pNotBound); // NO longer equal to this.
  ASSERT(NotBound == *m_pBound);    // Assign results in equality...

}

void 
TreeExceptionTest::ReasonText()
{
  string text(m_pNotBound->ReasonText());

  EQMSG("Notbound", 
	string("TreeException: Tree Parameter is not bound to a parameter in an event array and should be. while: Not bound test"), 
	text);

  text = m_pBound->ReasonText();
  EQMSG("Bound",
	string("TreeException: Tree Parameter is bound to a parameter in an event array and should not be. while: Bound test"),
	text);

  text = m_pNoParam->ReasonText();
  EQMSG("NoParam",
	string("TreeException: Tree Parameter needs an underlying parameter and does not have one. while: No parameter test"),
	text);

  text = m_pHaveParam->ReasonText();
  EQMSG("HaveParam", 
	string("TreeException: Tree Parameter had an underlying parameter and should not have one. while: Have parameter test"),
	text);

  text = m_pInvalidIndex->ReasonText();
  EQMSG("InvalidIndex",
	string("TreeException: Tree Parameter index is outside the valid range of indices. while: bad index test"),
	text);

  text = m_pUnitsMismatch->ReasonText();
  EQMSG("BadUnits",
	string("TreeException: Units mismatch in Tree Parameter assignment while: Unitsmismatch test"),
	text);

  text = m_pInvalid->ReasonText();
  EQMSG("Validinvalid", string("TreeException: Unrecognized exception reason code while: Validly invalid test"),
	text);

  text = m_pBadCode->ReasonText();
  EQMSG("InvalidInvalid", string("TreeException: Unrecognized exception reason code while: Really bad code test"),
	text);

}

void
TreeExceptionTest::ReasonCodes()
{
  CTreeException::TreeErrorReason reasoncode;
  Int_t                           reasonvalue;

  reasonvalue = m_pNotBound->ReasonCode();
  reasoncode  = CTreeException::ReasonToCode(reasonvalue);
  EQMSG("NotBound", CTreeException::NotBound, reasoncode);

  reasonvalue = m_pBound->ReasonCode();
  reasoncode  = CTreeException::ReasonToCode(reasonvalue);
  EQMSG("Bound", CTreeException::Bound, reasoncode);

  reasonvalue = m_pNoParam->ReasonCode();
  reasoncode  = CTreeException::ReasonToCode(reasonvalue);
  EQMSG("NoParam", CTreeException::NoParam, reasoncode); 

  reasonvalue = m_pHaveParam->ReasonCode();
  reasoncode  = CTreeException::ReasonToCode(reasonvalue);
  EQMSG("HaveParam", CTreeException::HaveParam, reasoncode);

  reasonvalue = m_pInvalidIndex->ReasonCode();
  reasoncode  = CTreeException::ReasonToCode(reasonvalue);
  EQMSG("INvalidIndex", CTreeException::InvalidIndex, reasoncode);

  reasonvalue = m_pUnitsMismatch->ReasonCode();
  reasoncode  = CTreeException::ReasonToCode(reasonvalue);
  EQMSG("Unitsmismatch", CTreeException::UnitsMismatch, reasoncode);

  reasonvalue = m_pInvalid->ReasonCode();
  reasoncode  = CTreeException::ReasonToCode(reasonvalue);
  EQMSG("Invalid", CTreeException::Invalid, reasoncode);

  reasonvalue = m_pBadCode->ReasonCode();
  reasoncode  = CTreeException::ReasonToCode(reasonvalue);

  reasonvalue = m_pBound->ReasonCode();
  reasoncode  = CTreeException::ReasonToCode(reasonvalue);
  ASSERT(CTreeException::Invalid != reasoncode);

}
