// Template for a test suite.

#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include <tcl.h>
#include "TCLInterpreter.h"
#include "TCLResult.h"



using namespace std;



class resultTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(resultTest);
  CPPUNIT_TEST(commit);
  CPPUNIT_TEST(consistent);
  CPPUNIT_TEST(append);
  CPPUNIT_TEST(lappend);
  CPPUNIT_TEST(getString);
  CPPUNIT_TEST_SUITE_END();


private:
  Tcl_Interp*      m_pRawInterp;
  CTCLInterpreter* m_pInterp;
  CTCLResult*      m_pResult;
public:
  void setUp() {
    m_pRawInterp = Tcl_CreateInterp();
    m_pInterp    = new CTCLInterpreter(m_pRawInterp);
    m_pResult    = new CTCLResult(m_pInterp); // This one is empty.
  }
  void tearDown() {
    delete m_pResult;
    delete m_pInterp;

  }
protected:
  void commit();
  void consistent();
  void append();
  void lappend();
  void getString();
};

CPPUNIT_TEST_SUITE_REGISTRATION(resultTest);


// Test commit to interp result.

void resultTest::commit() {
  (*m_pResult) = "A result";
  m_pResult->commit();
  EQ(string("A result"), string(Tcl_GetStringResult(m_pRawInterp)));
}

// Results constructed after a commit should reflect the
// committed value if not reset.
void resultTest::consistent()
{
  (*m_pResult) = "Test Result";
  m_pResult->commit();
  
  CTCLResult copy(m_pInterp, false);
  EQ(string("Test Result"), string(copy));
}

// Test adaption of += for strings /const char* s to simple append.

void resultTest::append()
{
  (*m_pResult) += "Testing now";
  EQ(string("Testing now"), (string(*m_pResult)));

  (*m_pResult) += string(" more tests");
  EQ(string("Testing now more tests"), string(*m_pResult));

  // Commit and double check this.

  m_pResult->commit();

  EQ(string("Testing now more tests"), string(Tcl_GetStringResult(m_pRawInterp)));
}
// Test list append functions.

void resultTest::lappend()
{
  m_pResult->AppendElement("first element");
  
  EQMSG("lappend const char*", string("{first element}"), string(*m_pResult));

  m_pResult->AppendElement(string("second element"));
  EQMSG("lappend string", string("{first element} {second element}"),
	string(*m_pResult));
  m_pResult->commit();

  EQMSG("Committed list",string("{first element} {second element}"),
	string(Tcl_GetStringResult(m_pRawInterp)));
}
// Test get string...

void resultTest::getString()
{
  string value("This is a test");
  (*m_pResult) = value;


  EQMSG("From object", value, m_pResult->getString());
  
  // Now the string should be committed.

  EQMSG("from tcl result", value, string(Tcl_GetStringResult(m_pRawInterp)));
}
