// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <CPacketCreator.h>
#include <CModuleCommand.h>
#include <CSegmentUnpacker.h>
#include <tcl.h>
#include <TCLInterpreter.h>
#include <CModuleDictionary.h>

#include <string>



class CreatorTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(CreatorTest);
  CPPUNIT_TEST(ConstructTest);
  CPPUNIT_TEST(TypeInfoTest);
  CPPUNIT_TEST(CreateTest);
  CPPUNIT_TEST_SUITE_END();


private:
  Tcl_Interp*        m_pRawI;
  CTCLInterpreter*   m_pInterp;
  CModuleDictionary* m_pDict;
  CModuleCommand*    m_pModule;

public:
  void setUp() {
    m_pRawI   = Tcl_CreateInterp();
    m_pInterp = new CTCLInterpreter(m_pRawI); 
    m_pDict   = new CModuleDictionary;
    m_pModule = new CModuleCommand(*m_pInterp, "module", *m_pDict);
  }
  void tearDown() {
    delete m_pModule;
    delete m_pDict;
    delete m_pInterp;
    Tcl_DeleteInterp(m_pRawI);
  }
protected:
  void ConstructTest();
  void TypeInfoTest();
  void CreateTest();
};

CPPUNIT_TEST_SUITE_REGISTRATION(CreatorTest);

//
// On creation, the creator should be able to locate the correct
// module dictionary.
//
void 
CreatorTest::ConstructTest() {
  CPacketCreator creator("packet", m_pModule);
  EQ(m_pDict, creator.m_pModules);

}
//
/// Should be able to get the appropriate type info too.
//
void 
CreatorTest::TypeInfoTest()
{
  CPacketCreator creator("packet", m_pModule);
  EQ(string("Unpacks both tagged and untagged packets"), creator.TypeInfo());
}
//
// Create a module.. should identify itself as a packet.
//
void
CreatorTest::CreateTest() 
{
  CPacketCreator creator("packet", m_pModule);

  CSegmentUnpacker* pUnpacker = creator.Create(*m_pInterp, "testing");
  EQ(string("module-container"), pUnpacker->getType());
}
