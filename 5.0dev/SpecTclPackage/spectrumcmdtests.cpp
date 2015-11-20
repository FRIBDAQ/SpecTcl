// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

//  Expose the guts of classes we need access to to make testing work:
#define private public
#include "CSpectrumCommand.h"
#include "CSpecTclInterpreter.h"
#undef private

#include <tcl.h>
#include <TCLInterpreter.h>


class SpectrumCommandTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SpectrumCommandTests);
  CPPUNIT_TEST(registered);
  CPPUNIT_TEST_SUITE_END();


private:
  Tcl_Interp* m_pInterp;
public:

  void setUp() {
    // All our tests will need a fresh interpreter.
    CSpecTclInterpreter::setInterp(Tcl_CreateInterp());
    m_pInterp  = CSpecTclInterpreter::instance()->getInterpreter();
  }
  void tearDown() {
    // All our tests will want to tear down the interpreter:

    Tcl_DeleteInterp(CSpecTclInterpreter::m_pInterp);
    delete CSpecTclInterpreter::m_pInstance;
    CSpecTclInterpreter::m_pInstance = 0;

  }
protected:
  void registered();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SpectrumCommandTests);

// If we can get an instance of the CSpectrum command the 
// '::spectcl::spectrum' command should exist:

void SpectrumCommandTests::registered() {
  CSpectrumCommand* pC = CSpectrumCommand::instance();
  
  Tcl_CmdInfo info;
  EQ(1, Tcl_GetCommandInfo(m_pInterp, "::spectcl::spectrum", &info));

}
