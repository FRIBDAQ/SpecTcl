// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CDataSource.h"
#include "CDataSourceCreator.h"

// White box testing and the need to re-init the interp.

#define private public
#include "CAttachCommand.h"
#include "CSpecTclInterpreter.h"
#include "CDataSourceFactory.h"
#include "CAnalysisPipeline.h"
#undef private


#include <tcl.h>
#include <TCLInterpreter.h>
#include <string.h>


class AttachTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(AttachTests);
  CPPUNIT_TEST(registered);
  CPPUNIT_TEST(singleton);

  CPPUNIT_TEST(needparam);
  CPPUNIT_TEST(baduri);
  CPPUNIT_TEST(noscheme);

  CPPUNIT_TEST(rightscheme);
  CPPUNIT_TEST(nosuchcreator);
  CPPUNIT_TEST(attach);


  CPPUNIT_TEST(informpipeline);

  CPPUNIT_TEST_SUITE_END();


private:
  Tcl_Interp* m_pInterp;

public:
  // Setup a SpecTcl interpreter:
  void setUp() {
    CSpecTclInterpreter::setInterp(Tcl_CreateInterp());
    m_pInterp = CSpecTclInterpreter::instance()->getInterpreter();
  }
  // Reset so a new 'singleton' can be created.
  void tearDown() {
    delete CAttachCommand::m_pInstance;
    CAttachCommand::m_pInstance = 0;

    Tcl_DeleteInterp(CSpecTclInterpreter::m_pInterp);
    delete CSpecTclInterpreter::m_pInstance;

    CDataSourceFactory::clearCreators();
    delete CAnalysisPipeline::m_pInstance;

    CAnalysisPipeline::m_pInstance = 0;
    CSpecTclInterpreter::m_pInterp = 0;
    CSpecTclInterpreter::m_pInstance = 0;
  }
protected:
  void registered();
  void singleton();

  void needparam();
  void baduri();
  void noscheme();

  void rightscheme();
  void nosuchcreator();
  void attach();

  void informpipeline();
};

CPPUNIT_TEST_SUITE_REGISTRATION(AttachTests);


//
// Ensure that instantiating a CAttachCommand
// on an interpreter adds the ::spectcl::attach command to the interp.
void AttachTests::registered() 
{
  CAttachCommand* pCommand = CAttachCommand::instance();

  // Should have a ::spectcl::attach command in SpecTcl's interpreter:



  Tcl_CmdInfo info;
  EQ(1, Tcl_GetCommandInfo(m_pInterp, "::spectcl::attach", &info));


}
//  The instance() method shouild always return the same value:

void AttachTests::singleton()
{
  EQ(CAttachCommand::instance(), CAttachCommand::instance());
}

//  ::spectcl::attach without a parameter fails.

void AttachTests::needparam()
{
  CAttachCommand::instance();	// Register the command.
  int stat = Tcl_GlobalEval(m_pInterp, "::spectcl::attach");

  EQ(TCL_ERROR, stat);
  std::string error = Tcl_GetStringResult(m_pInterp);

  EQ(std::string("spectcl::attach"), error.substr(0, strlen("spectcl::attach")));

  
}
/**
 * ::spectcl::attach with a bad URI should also fail.
 */
void AttachTests::baduri()
{
  CAttachCommand::instance();

  int stat = Tcl_Eval(m_pInterp, "::spectcl::attach  y&///kdjfa!##%&*");
  EQ(TCL_ERROR, stat);

  std::string error = Tcl_GetStringResult(m_pInterp);
  EQ(std::string("spectcl::attach"), error.substr(0, strlen("spectcl::attach")));

}
/**
 * ::spectcl::attach with a schemeless URI (e.g. /1/2/3) is bad too:
 */
void
AttachTests::noscheme()
{
  CAttachCommand::instance();

  int stat = Tcl_Eval(m_pInterp, "::spectcl::attach  /a/b/c");
  EQ(TCL_ERROR, stat);

  std::string error = Tcl_GetStringResult(m_pInterp);
  EQ(std::string("spectcl::attach"), error.substr(0, strlen("spectcl::attach")));

}
/**
 * rightscheme
 *
 * Ensures that attach null:///some/stuff
 * actually attempts to construct a 'null' data source.
 */
class CNullDataSource;

CNullDataSource* pSource = 0;
CNullDataSource* pPrior  = 0;
std::string      target;

class CNullDataSource : public CDataSource
{
public:
  CNullDataSource() {target = ""; }
  ~CNullDataSource()
  {
    pPrior = this;
  }
  virtual void    onAttach(CTCLInterpreter& interp, const char* pSource)  {
    target = pSource;
  }
  virtual void    createEvent(CTCLInterpreter& interp, ChannelHandler* pHandler, void* pClientData) {}
  virtual size_t  read(void* pBuffer, size_t nBytes)  {}
  virtual void    close() {}
  virtual bool    isEof() {} 
};

class CNullDataSourceCreator : public CDataSourceCreator
{
public:
  CNullDataSourceCreator() { 
    pSource = 0;
    pPrior = 0;
  }
  virtual CDataSource* create() {
    pSource = new CNullDataSource; // so we know we have it.
    return pSource;
  }
};

/**
 * Ensure the attach can get the right scheme and create a new
 * data source for it.
 */
void AttachTests::rightscheme()
{
  CDataSourceFactory fact; 
  fact.addCreator("null", new CNullDataSourceCreator);

  CAttachCommand::instance();

  int stat = Tcl_Eval(m_pInterp, "::spectcl::attach  null:/a/b/c");
  EQ(TCL_OK, stat);

  ASSERT(pSource);

}
/**
 *  nosuchcreator 
 *
 *   It's an error if there's no creator for the scheme.
 */
void AttachTests::nosuchcreator()
{
  CAttachCommand::instance();

  int stat = Tcl_Eval(m_pInterp, "::spectcl::attach  null:///a/b/c");
  EQ(TCL_ERROR, stat);
}


/**
 * attach
 *   The new data source should get attached to the hier part of the URI:
 */
void
AttachTests::attach()
{
  CDataSourceFactory fact; 
  fact.addCreator("null", new CNullDataSourceCreator);

  CAttachCommand::instance();

  int stat = Tcl_Eval(m_pInterp, "::spectcl::attach  null:///a/b/c");
  EQ(TCL_OK, stat);

  EQ(std::string("///a/b/c"), target);
}
/**
 * informpipeline
 *
 *   After creating and attaching the data source, attach must
 *   make it known to the event processing pipeline.
 */

// Mock for the analysis pipeline:

CAnalysisPipeline* CAnalysisPipeline::m_pInstance(0);

CDataSource*  apDataSource(0);

CAnalysisPipeline::CAnalysisPipeline() {
  apDataSource = 0;
}

CAnalysisPipeline* CAnalysisPipeline::instance()
{
  if (!m_pInstance) {
    m_pInstance = new CAnalysisPipeline;
  }
  return m_pInstance;
}
void CAnalysisPipeline::setDataSource(CDataSource* pSource) {
  apDataSource = pSource;
}

CDecoder* CAnalysisPipeline::getDecoder() {
  return reinterpret_cast<CDecoder*>(this);
}


void
AttachTests::informpipeline()
{
  CDataSourceFactory fact; 
  fact.addCreator("null", new CNullDataSourceCreator);

  CAttachCommand::instance();

  int stat = Tcl_Eval(m_pInterp, "::spectcl::attach  null:///a/b/c");
  EQ(TCL_OK, stat);
  
  ASSERT(apDataSource);
}

