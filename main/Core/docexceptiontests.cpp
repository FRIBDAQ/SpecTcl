// Template for a test suite.
#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "DocumentationException.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif



class docexception : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(docexception);
  CPPUNIT_TEST(nosuchvar);
  CPPUNIT_TEST(nosuchelement);
  CPPUNIT_TEST(nosuchhandler);
  CPPUNIT_TEST(badreason);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
  }
protected:
  void nosuchvar();
  void nosuchelement();
  void nosuchhandler();
  void badreason();
};

CPPUNIT_TEST_SUITE_REGISTRATION(docexception);

void
docexception::nosuchvar()   
{
  CDocumentationException e(CDocumentationException::NoSuchVariable,
			    "testing");
  int reasoncode = e.ReasonCode();
  CDocumentationException::ExceptionReason r = 
    CDocumentationException::CodeToReason(reasoncode);
  EQ(CDocumentationException::NoSuchVariable, r);
  EQ(string("No such variable"), CDocumentationException::ReasonToText(reasoncode));
  EQ(string("Exception in documentation buffer subsystem: No such variable while: testing"),
     string(e.ReasonText()));
	    
}

void
docexception::nosuchelement()
{
  CDocumentationException e(CDocumentationException::NoSuchElement,
			    "testing");
  int reasoncode = e.ReasonCode();
  CDocumentationException::ExceptionReason r = 
    CDocumentationException::CodeToReason(reasoncode);
  EQ(CDocumentationException::NoSuchElement, r);
  EQ(string("No such array element"), 
     CDocumentationException::ReasonToText(reasoncode));
  EQ(string("Exception in documentation buffer subsystem: No such array element while: testing"),
     string(e.ReasonText()));
}

void
docexception::nosuchhandler()
{
  CDocumentationException e(CDocumentationException::NoSuchHandler,
			    "testing");
  int reasoncode = e.ReasonCode();
  CDocumentationException::ExceptionReason r = 
    CDocumentationException::CodeToReason(reasoncode);
  EQ(CDocumentationException::NoSuchHandler, r);
  EQ(string("No such change handler"), 
     CDocumentationException::ReasonToText(reasoncode));
  EQ(string("Exception in documentation buffer subsystem: No such change handler while: testing"),
     string(e.ReasonText()));

}

void
docexception::badreason()   
{
  string answer(CDocumentationException::ReasonToText(-1));
  EQ(string("BUGBUG : reason code is invalid in ReasonToText"), answer);
}
