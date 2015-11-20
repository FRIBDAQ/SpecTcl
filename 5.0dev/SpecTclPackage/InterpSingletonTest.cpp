// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#define private public		// enables white box testing.
#include "CSpecTclInterpreter.h"
#undef private

#include <tcl.h>


class SpecTclInterpTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SpecTclInterpTest);
  CPPUNIT_TEST(constructNoInit);
  CPPUNIT_TEST(constructInit);

  CPPUNIT_TEST(isSingleton);

  CPPUNIT_TEST(doubleInterpSet);
  CPPUNIT_TEST_SUITE_END();


private:

public:
  void setUp() {
  }
  void tearDown() {
    // Poke into the innards to support idempotency in testing singletons:

    delete CSpecTclInterpreter::m_pInstance;
    CSpecTclInterpreter::m_pInstance = 0;
    if (CSpecTclInterpreter::m_pInterp) {
      if(!Tcl_InterpDeleted(CSpecTclInterpreter::m_pInterp)) {
	Tcl_DeleteInterp(CSpecTclInterpreter::m_pInterp);
      }
    }
    CSpecTclInterpreter::m_pInterp = 0;

  }
protected:
  void constructNoInit();
  void constructInit();
  
  void isSingleton();
  
  void doubleInterpSet();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SpecTclInterpTest);



/**
 * constructNoInit
 *
 *  Before the first call to instance, the application interpreter must be set 
 *  else a spectcl_interp_exception is thrown.
 */
void SpecTclInterpTest::constructNoInit() {
  bool thrown     = false;
  bool rightThrow = false;

  try {
    CSpecTclInterpreter* pInterp = CSpecTclInterpreter::instance();
  }
  catch(spectcl_interp_exception& e) {
    thrown  = true;
    rightThrow = true;
  }
  catch(...) {
    thrown = true;
  }

  ASSERT(thrown);
  ASSERT(rightThrow);
}

/**
 * constructInit
 *
 * If we have set an interpreter, getInstance should return a CTCLInterpreter for us.
 */
void
SpecTclInterpTest::constructInit()
{
  CSpecTclInterpreter::setInterp(Tcl_CreateInterp());

  ASSERT(CSpecTclInterpreter::instance());
}

/**
 * isSingleton.
 *
 *  Calling instance several times should give the same pointer:
 */
void
SpecTclInterpTest::isSingleton()
{
  CSpecTclInterpreter::setInterp(Tcl_CreateInterp());

  EQ(CSpecTclInterpreter::instance(), CSpecTclInterpreter::instance());
}
/**
 * doubleInterpSet
 *
 *  Setting the interpreter twice should throw too:
 */
void
SpecTclInterpTest::doubleInterpSet()
{
  Tcl_Interp* pInterp = Tcl_CreateInterp();

  CSpecTclInterpreter::setInterp(pInterp); // ok.

  bool thrown = false;
  bool rightThrow = false;

  try {
    CSpecTclInterpreter::setInterp(pInterp); // should throw.
  }
  catch(spectcl_interp_exception& e) {
    thrown = true;
    rightThrow = true;
  }
  catch (...) {
    thrown = true;
  }
  
  ASSERT(thrown);
  ASSERT(rightThrow);
}
