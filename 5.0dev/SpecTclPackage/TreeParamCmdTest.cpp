// Template for a test suite.

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include <tcl.h>
#define private public
#include "CTreeParameterCommand.h"
#include "CSpecTclInterpreter.h"
#include "CParameterDictionary.h"
#undef private
#include "CParameter.h"
#include <string>
#include <TCLObject.h>
#include <map>
#include <iostream>


class TreePCmdTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(TreePCmdTests);
  CPPUNIT_TEST(singleton);
  CPPUNIT_TEST(cmdexists);
  CPPUNIT_TEST(needsubcommand);
  CPPUNIT_TEST(validsubcommand);

  // Create subcommand.

  CPPUNIT_TEST(createcheckparamcount);
  CPPUNIT_TEST(createcheckexists);
  CPPUNIT_TEST(createchecklow);
  CPPUNIT_TEST(createcheckhigh);
  CPPUNIT_TEST(createchecklowhigh);
  CPPUNIT_TEST(createcheckbins);
  CPPUNIT_TEST(createbinspositive);
  CPPUNIT_TEST(createok);
 
  // list subcommand

  CPPUNIT_TEST(listcheckparamcount);
  CPPUNIT_TEST(listempty);
  CPPUNIT_TEST(listall);
  CPPUNIT_TEST(listpattern);

  // modify subcommand

  CPPUNIT_TEST(modifycheckparamcount);
  CPPUNIT_TEST(modifycheckexists);
  CPPUNIT_TEST(modifychecklow);
  CPPUNIT_TEST(modifycheckhigh);
  CPPUNIT_TEST(modifychecklowhigh);
  CPPUNIT_TEST(modifycheckbins);
  CPPUNIT_TEST(modifyok);

  // setbins command

  CPPUNIT_TEST(setbinscheckparamcount);
  CPPUNIT_TEST(setbinscheckexists);
  CPPUNIT_TEST(setbinscheckbins);
  CPPUNIT_TEST(setbinscheckbinspositive);
  CPPUNIT_TEST(setbinsok);

  // setlimits command


  CPPUNIT_TEST(setlimitsparamcount);
  CPPUNIT_TEST(setlimitscheckexists);
  CPPUNIT_TEST(setlimitschecklimits);
  CPPUNIT_TEST(setlimitsok);

  // setunits

  CPPUNIT_TEST(setunitsparamcount);
  CPPUNIT_TEST(setunitsexist);
  CPPUNIT_TEST(setunitsok);

  // trace
  
  CPPUNIT_TEST(traceneedmore);

  // trace add

  CPPUNIT_TEST(traceaddparamcount);
  CPPUNIT_TEST(traceaddok);
  CPPUNIT_TEST(tracecalled);
  CPPUNIT_TEST(tracecallsmultiple);
 
  // trace remove

  CPPUNIT_TEST(traceremoveparamcount);
  CPPUNIT_TEST(traceremoveok);
  CPPUNIT_TEST(traceremovenotcalled);
  CPPUNIT_TEST(traceremovenosuch);

  // trace show

  CPPUNIT_TEST(traceshowparamcount);
  CPPUNIT_TEST(traceshowempty);
  CPPUNIT_TEST(traceshowlist);

  CPPUNIT_TEST(traceinvalidsubcommand);

  CPPUNIT_TEST_SUITE_END();


private:
  CParameterDictionary* m_pDict;
  Tcl_Interp*           m_pInterp;
  
  // Assumptions:  The dictionary is a map and traversal is reproducible.

  std::map<std::string, CParameter*> m_parameters;

public:
  void setUp() {
    m_pInterp = Tcl_CreateInterp();
    CSpecTclInterpreter::setInterp(m_pInterp);
    m_pDict = CParameterDictionary::instance();
    CParameter::enableAutoRegistration();
  }
  void tearDown() {
    delete CTreeParameterCommand::m_pInstance;
    CTreeParameterCommand::m_pInstance =  0;
    delete CParameterDictionary::m_pInstance;
    CParameterDictionary::m_pInstance = 0;
    m_pDict = 0;
    delete CSpecTclInterpreter::m_pInstance;
    CSpecTclInterpreter::m_pInstance = 0;
    CSpecTclInterpreter::m_pInterp = 0;
    
    m_parameters.clear();

  }
protected:
  void singleton();
  void cmdexists();
  void needsubcommand();
  void validsubcommand();

  void createcheckparamcount();
  void createcheckexists();
  void createchecklow();
  void createcheckhigh();
  void createchecklowhigh();
  void createcheckbins();
  void createbinspositive();
  void createok();

  void listcheckparamcount();
  void listempty();
  void listall();
  void listpattern();

  void modifycheckparamcount();
  void modifycheckexists();
  void modifychecklow();
  void modifycheckhigh();
  void modifychecklowhigh();
  void modifycheckbins();
  void modifycheckbinspositive();
  void modifyok();

  void setbinscheckparamcount();
  void setbinscheckexists();
  void setbinscheckbins();
  void setbinscheckbinspositive();
  void setbinsok();

  void setlimitsparamcount();
  void setlimitscheckexists();
  void setlimitschecklimits();
  void setlimitsok();

  void setunitsparamcount();
  void setunitsexist();
  void setunitsok();

  void traceneedmore();

  void traceaddparamcount();
  void traceaddok();
  void tracecalled();
  void tracecallsmultiple();

  void traceremoveparamcount();
  void traceremoveok();
  void traceremovenotcalled();
  void traceremovenosuch();

  void traceshowparamcount();
  void traceshowempty();
  void traceshowlist();

  void traceinvalidsubcommand();
private:
  void createParameters();	// Create some tree parametersin the dict.

};

CPPUNIT_TEST_SUITE_REGISTRATION(TreePCmdTests);

static std::map<std::string, CParameter*> parameters;

/**
 * utility to stock the dictionary with some parameters.
 */
void
TreePCmdTests::createParameters()
{
  // Note that cleanup with the dict will free these.

  m_parameters["test1"]  =  new CParameter("test1", -1,1,100, "arbitrary");
  m_parameters["test2"]  = new CParameter("test2", 0,4095, 4096, "adc-counts");
  m_parameters["george"] = new CParameter("george", 0, 10, 100, "mm");

}

/**
 * singleton:
 *    CTreeParameterCommand is a singleton object:
 */
void TreePCmdTests::singleton() {
  // Initially no command object.
  EQ(reinterpret_cast<CTreeParameterCommand*>(0), CTreeParameterCommand::m_pInstance);

  // Instance should also set m_pInstancde.
  CTreeParameterCommand* p = CTreeParameterCommand::instance();
  ASSERT(p);
  EQ(p, CTreeParameterCommand::m_pInstance);
  
  // All instance calls give the same pointer.
  CTreeParameterCommand* p2 = CTreeParameterCommand::instance();
  EQ(p, p2);
  EQ(p2, CTreeParameterCommand::m_pInstance);

}
/**
 * cmdexists
 *   When the singleton is instantiated, the command
 *   ::spectcl::treeparameter should exist.
 */
void
TreePCmdTests::cmdexists()
{
  CTreeParameterCommand* pC = CTreeParameterCommand::instance();
  Tcl_CmdInfo info;
  int status = Tcl_GetCommandInfo(m_pInterp, "::spectcl::treeparameter", &info);
  ASSERT(status);
}

/**
 * needsubcommand
 *
 *  ::spectcl::treeparameter requires a subcommand.
 *  the result is a string of the form "::spectcl::treeparameter missing subcommand"
 */
void
TreePCmdTests::needsubcommand()
{
  CTreeParameterCommand::instance();
  int status = Tcl_GlobalEval(m_pInterp, "::spectcl::treeparameter");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter missing subcommand"),
     std::string(Tcl_GetStringResult(m_pInterp)));
}
/**
 * validsubcommand
 *
 *  An invalid subcommand results in an error
 */
void
TreePCmdTests::validsubcommand()
{
  CTreeParameterCommand::instance();
  int status = Tcl_GlobalEval(m_pInterp, "::spectcl::treeparameter goobledygook");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter bad subcommand"),
     std::string(Tcl_GetStringResult(m_pInterp)));
 
}
/** 
 * createcheckparamcount
 *   create requires exactly 7 total parameters.
 */
void
TreePCmdTests::createcheckparamcount()
{
  CTreeParameterCommand::instance();

  int status = Tcl_GlobalEval(m_pInterp, "::spectcl::treeparameter create");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter create - incorrect number of parameters"),
     std::string(Tcl_GetStringResult(m_pInterp)));


  status = Tcl_GlobalEval(m_pInterp, 
			  "::spectcl::treeparameter create george -1 1 100 units");
  EQ(TCL_OK, status);
}
/**
 * createcheckexists
 *    Creating an existing parameter is invalid too.
 */
void
TreePCmdTests::createcheckexists()
{
  CTreeParameterCommand::instance();
  CParameter param("george");	// now one exists.

  int status = Tcl_GlobalEval(m_pInterp, 
			      "::spectcl::treeparameter create george -1 1 100 units");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter create - parameter already exists"),
     std::string(Tcl_GetStringResult(m_pInterp)));
}
/**
 * createchecklow
 *
 *   If the low limit on a parameter does not have a valid double
 *   representation; that's an error too.
 */
void
TreePCmdTests::createchecklow()
{
  CTreeParameterCommand::instance();

  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter create george harry 1.0 100 units");
  EQ(TCL_ERROR, status);
  EQ(std::string("expected floating-point number but got \"harry\""),
     std::string(Tcl_GetStringResult(m_pInterp)));

}
/** createcheckhigh
 *   If the high limit on a parameter does not have a valid double rep that's
 *   an error.
 */
void
TreePCmdTests::createcheckhigh()
{
  CTreeParameterCommand::instance();

  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter create george 1.0 lou 100 units");
  EQ(TCL_ERROR, status);
  EQ(std::string("expected floating-point number but got \"lou\""),
     std::string(Tcl_GetStringResult(m_pInterp)));
}
/**
 * createchecklowhigh
 **   It's an error for low >= high
 */
void 
TreePCmdTests::createchecklowhigh()
{
  CTreeParameterCommand::instance();

  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter create george 1.0 -1.0 100 units");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter - low must be < high"),
     std::string(Tcl_GetStringResult(m_pInterp)));
}
/**
 * createcheckbins
 *   Bins must parse as an integer.
 */
void
TreePCmdTests::createcheckbins()
{
  CTreeParameterCommand::instance();

  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter create george -1 1 crap  units");
  EQ(TCL_ERROR, status);
  EQ(std::string("expected integer but got \"crap\""),
     std::string(Tcl_GetStringResult(m_pInterp)));

  status = Tcl_GlobalEval(m_pInterp,
			  "::spectcl::treeparameter create george -1 1 3.4  units");
  EQ(TCL_ERROR, status);
  EQ(std::string("expected integer but got \"3.4\""),
     std::string(Tcl_GetStringResult(m_pInterp)));

}
/**
 *  createbinspositive
 * 
 *  The bins paramter must be positive too.
 */
void
TreePCmdTests::createbinspositive()
{
  CTreeParameterCommand::instance();

  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter create george -1 1 -12  units");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter - Bins parameter must be greater than zero"),
     std::string(Tcl_GetStringResult(m_pInterp)));
}
/**
 * createok
 *
 *   Create that should succeed:
 *   - parameter is findable in the dictionary.
 *   - parameter shouild have the proper metadata.
 */
void
TreePCmdTests::createok()
{
  CTreeParameterCommand::instance();

  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter create george -1 1 100 furlongs");
  EQ(TCL_OK, status);
  CParameterDictionary::DictionaryIterator p = m_pDict->find("george");
  ASSERT(p != m_pDict->end());
  CParameterDictionary::pParameterInfo pInfo = p->second;
  CParameter* pParam = pInfo->s_references.front();
  EQ(-1.0, pParam->getLow());
  EQ(1.0, pParam->getHigh());
  EQ(100U, pParam->getBins());
  EQ(std::string("furlongs"), pParam->getUnits());
  EQ(std::string("george"), pParam->getName());
  EQ(std::string("george"), std::string(Tcl_GetStringResult(m_pInterp)));
}
/**
 * listcheckparamcount
 *
 *  The list subcommand has 2 or three parameters
 *  (optional pattern to match).
 */
void
TreePCmdTests::listcheckparamcount()
{
  CTreeParameterCommand::instance();

  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter list * george"); // too many.
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter list - incorrect number of parameters"),
     std::string(Tcl_GetStringResult(m_pInterp)));
  
}
/**
 * listempty
 *
 * An Empty parameter dictionary produces an empty tcl list.
 */
void
TreePCmdTests::listempty()
{
   CTreeParameterCommand::instance();

  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter list"); 
  EQ(TCL_OK, status);
  CTCLObject result;
  result = Tcl_GetStringResult(m_pInterp);
  result.Bind(CSpecTclInterpreter::instance());
  EQ(0, result.llength());
}
/**
 * listall
 *    list without any pattern returns all elements.
 */
void
TreePCmdTests::listall()
{
  CTreeParameterCommand::instance();
  CTCLInterpreter* pInterp = CSpecTclInterpreter::instance();
  createParameters();

  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter list");
  EQ(TCL_OK, status);

  CTCLObject result;
  result = Tcl_GetStringResult(m_pInterp);
  result.Bind(pInterp);

  EQ(m_parameters.size(), static_cast<size_t>(result.llength()));
  
  // Assume the traversal of the maps are reproducible:

  std::map<std::string,CParameter*>::iterator pParams = m_parameters.begin();
  for (int i = 0; i < result.llength(); i++) {
    CParameter* pParam    = pParams->second;
    CTCLObject description = result.lindex(i);
    description.Bind(pInterp);

    EQ(5, description.llength());
    CTCLObject element;
    element.Bind(pInterp);

    //Name:

    element = description.lindex(0);
    EQ(pParam->getName(), (std::string)(element)); 

    // Low

    element = description.lindex(1);
    EQ(pParam->getLow(), (double)element);

    // High

    element = description.lindex(2);
    EQ(pParam->getHigh(), (double)element);

    //Bins

    element = description.lindex(3);
    EQ(pParam->getBins(), static_cast<unsigned>((int)element));
    
    // units:
    
    element = description.lindex(4);
    EQ(pParam->getUnits(), (std::string)element);

    pParams++;
  }
}
/**
 * listpattern
 *   The final parameter of the list operation should be a glob
 *   filter.
 */
void
TreePCmdTests::listpattern()
{
  CTreeParameterCommand::instance();
  CTCLInterpreter* pInterp =  CSpecTclInterpreter::instance();
  createParameters();

  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter list g*");
  EQ(TCL_OK, status);
  
  // Should only get george back out:

  CTCLObject result;
  result.Bind(pInterp);
  result = Tcl_GetStringResult(m_pInterp);
  EQ(1, result.llength());
  CTCLObject description = result.lindex(0);
  description.Bind(pInterp);

  // Just verify it's the name we expect to see.

  EQ(std::string("george"), (std::string)(description.lindex(0)));
}
/**
 * modifycheckparamcount
 *
 *   The modify subcommand looks just like the create command
 *   -- needs 7 parameters.
 */
void
TreePCmdTests::modifycheckparamcount()
{
  CTreeParameterCommand::instance();
  createParameters();
  
  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter modify");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter modify - Incorrect number of parameters"),
     std::string(Tcl_GetStringResult(m_pInterp)));
}
/**
 * modifycheckexists
 *
 *   The modify command requires that the name parameter refer to an
 *   existing parameter.
 */
void
TreePCmdTests::modifycheckexists()
{
  CTreeParameterCommand::instance();
  createParameters();
  
  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter modify harry -1 1 1000 mm/sr");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter modify - parameter does not exist."),
     std::string(Tcl_GetStringResult(m_pInterp)));
}
/**
 * modifychecklow
 *    low limit must be a double else well defined error.
 */
void
TreePCmdTests::modifychecklow()
{
  CTreeParameterCommand::instance();
  createParameters();
  
  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter modify george farad 1 1000 mm/sr");
  EQ(TCL_ERROR, status);
  EQ(std::string("expected floating-point number but got \"farad\""),
     std::string(Tcl_GetStringResult(m_pInterp)));

}
/**
 * modifycheckhigh
 */
void
TreePCmdTests::modifycheckhigh()
{
  CTreeParameterCommand::instance();
  createParameters();
  
  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter modify george -1 farad 1000 mm/sr");
  EQ(TCL_ERROR, status);
  EQ(std::string("expected floating-point number but got \"farad\""),
     std::string(Tcl_GetStringResult(m_pInterp)));

}
/**
 * modifychecklowhigh
 */
void
TreePCmdTests::modifychecklowhigh()
{
  CTreeParameterCommand::instance();
  createParameters();
  
  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter modify george 2 1 1000 mm/sr");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter - low must be < high"),
     std::string(Tcl_GetStringResult(m_pInterp)));

}

/**
 * modifycheckbins
 */
void
TreePCmdTests::modifycheckbins()
{
  CTreeParameterCommand::instance();
  createParameters();
  
  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter modify george -1 1 farad mm/sr");
  EQ(TCL_ERROR, status);
  EQ(std::string("expected integer but got \"farad\""),
     std::string(Tcl_GetStringResult(m_pInterp)));

}
/**
 * modifycheckbinspositive
 */
void
TreePCmdTests::modifycheckbinspositive()
{
  CTreeParameterCommand::instance();
  createParameters();
  
  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter modify george -1 1 0 mm/sr");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter - Bins parameter must be greater than zero"),
     std::string(Tcl_GetStringResult(m_pInterp)));

}
/**
 * modifyok
 *   Correct modification.
 */
void
TreePCmdTests::modifyok()
{
  CTreeParameterCommand::instance();
  createParameters();
  
  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter modify george -1 1 50  mm/sr");
  EQ(TCL_OK, status);

  // Now did it change correctly:

  CParameterDictionary* pDict = CParameterDictionary::instance();
  CParameterDictionary::DictionaryIterator pItem = pDict->find("george");
  ASSERT(pItem != pDict->end());

  CParameter* pParam = pItem->second->s_references.front();
  EQ(-1.0, pParam->getLow());
  EQ(1.0, pParam->getHigh());
  EQ(50U, pParam->getBins());
  EQ(std::string("mm/sr"), pParam->getUnits());

  EQ(std::string("george"), std::string(Tcl_GetStringResult(m_pInterp)));
}
/**
 * setbinsparamcount
 *
 *   setbins must have 4 parameters treeparameter setbins name new-value.
 */
void
TreePCmdTests::setbinscheckparamcount()
{
  CTreeParameterCommand::instance();
  createParameters();

  int status = Tcl_GlobalEval(m_pInterp, 
			      "::spectcl::treeparameter setbins");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter setbins - Incorrect parameter count."),
     std::string(Tcl_GetStringResult(m_pInterp)));

}
/**
 *  setbinscheckexists
 */
void
TreePCmdTests::setbinscheckexists()
{
  CTreeParameterCommand::instance();
  createParameters();

  int status = Tcl_GlobalEval(m_pInterp, 
			      "::spectcl::treeparameter setbins test69 4000");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter setbins - parameter does not exist."),
     std::string(Tcl_GetStringResult(m_pInterp)));
}
/** 
 * setbinscheckbins
 *
 *  setbins bins must be an integer.
 */
void
TreePCmdTests::setbinscheckbins()
{
  CTreeParameterCommand::instance();
  createParameters();

  int status = Tcl_GlobalEval(m_pInterp, 
			      "::spectcl::treeparameter setbins test1 help");
  EQ(TCL_ERROR, status);
  EQ(std::string("expected integer but got \"help\""), 
     std::string(Tcl_GetStringResult(m_pInterp)));
  
}
/**
 * setbinscheckbinspositive
 *   bins must be positive too.
 */
void
TreePCmdTests::setbinscheckbinspositive()
{
  CTreeParameterCommand::instance();
  createParameters();

  int status = Tcl_GlobalEval(m_pInterp, 
			      "::spectcl::treeparameter setbins test1 0");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter setbins - bin count must be > 0"), 
     std::string(Tcl_GetStringResult(m_pInterp)));
}
/**
 * setbinsok
 *   Good set.
 */
void
TreePCmdTests::setbinsok()
{
  CTreeParameterCommand::instance();
  createParameters();

  int status = Tcl_GlobalEval(m_pInterp, 
			      "::spectcl::treeparameter setbins test1 52");
  EQ(TCL_OK, status);
  CParameterDictionary* pDict = CParameterDictionary::instance();
  CParameterDictionary::DictionaryIterator p  = pDict->find("test1");
  EQ(52U, p->second->s_references.front()->getBins());
  EQ(std::string("test1"), std::string(Tcl_GetStringResult(m_pInterp)));
}
/**
 * setlimitsparamcount()
 *    The setlimits subcommand needs 5 command parameters.
 */
void
TreePCmdTests::setlimitsparamcount()
{
  CTreeParameterCommand::instance();
  createParameters();

  int status = Tcl_GlobalEval(m_pInterp, 
			      "::spectcl::treeparameter setlimits");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter setlimits - incorrect number of parameters"),
     std::string(Tcl_GetStringResult(m_pInterp)));
}
/**
 * setlimitscheckexists
 *
 *   Parameter name must exist.
 */
void
TreePCmdTests::setlimitscheckexists()
{
  CTreeParameterCommand::instance();
  createParameters();

  int status = Tcl_GlobalEval(m_pInterp, 
			      "::spectcl::treeparameter setlimits ubuntu -1 1");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter setlimits - parameter does not exist."),
     std::string(Tcl_GetStringResult(m_pInterp)));
  
}
/**
 * setlimitschecklimits
 *   - Low limit must be double.
 *   - high limit must be double
 *  -  Low must be < high.
 */
void
TreePCmdTests::setlimitschecklimits()
{
  CTreeParameterCommand::instance();
  createParameters();

  int status = Tcl_GlobalEval(m_pInterp, 
			      "::spectcl::treeparameter setlimits test2 test1 5");
  EQ(TCL_ERROR, status);
  EQ(std::string("expected floating-point number but got \"test1\""),
     std::string(Tcl_GetStringResult(m_pInterp)));

  status = Tcl_GlobalEval(m_pInterp, 
			  "::spectcl::treeparameter setlimits test2 -1 geroge");
  EQ(TCL_ERROR, status);
  EQ(std::string("expected floating-point number but got \"geroge\""),
     std::string(Tcl_GetStringResult(m_pInterp)));

  status = Tcl_GlobalEval(m_pInterp, 
			  "::spectcl::treeparameter setlimits test2 -1 -2");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter - low must be < high"),
     std::string(Tcl_GetStringResult(m_pInterp)));

  
}
/**
 * setlimitsok
 *   Successful setting of parameter limits.
 */
void
TreePCmdTests::setlimitsok()
{
  CTreeParameterCommand::instance();
  createParameters();

  int status = Tcl_GlobalEval(m_pInterp, 
			      "::spectcl::treeparameter setlimits test2 -400 400");
  EQ(TCL_OK, status);
  CParameterDictionary* pDict = CParameterDictionary::instance();
  CParameterDictionary::DictionaryIterator pItem = pDict->find("test2");

  EQ(-400.0, pItem->second->s_references.front()->getLow());
  EQ(400.0, pItem->second->s_references.front()->getHigh());
  EQ(std::string("test2"), std::string(Tcl_GetStringResult(m_pInterp)));
  
}
/**
 * setunitsparamcount
 *
 *  setunits needs 4 parameters on the command line.
 */
void
TreePCmdTests::setunitsparamcount()
{
  CTreeParameterCommand::instance();
  createParameters();

  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter setunits"); // too few
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter setunits - incorrect number of parameters"),
     std::string(Tcl_GetStringResult(m_pInterp)));

  status  = Tcl_GlobalEval(m_pInterp, // too many
			   "::spectcl::treeparameter setunits george milli farads");

  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter setunits - incorrect number of parameters"),
     std::string(Tcl_GetStringResult(m_pInterp)));

}
/**
 * setunitsexist
 *    The parameter must exist.
 */
void
TreePCmdTests::setunitsexist()
{
 CTreeParameterCommand::instance();
 createParameters();

  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter setunits no-such fortnights");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter setunits - parameter does not exist."),
     std::string(Tcl_GetStringResult(m_pInterp)));
}
/**
 * setunitsok
 *
 *  Successful setting of units:
 */
void
TreePCmdTests::setunitsok()
{
 CTreeParameterCommand::instance();
 createParameters();

  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter setunits test1 fortnights");
  EQ(TCL_OK, status);
  EQ(std::string("test1"), std::string(Tcl_GetStringResult(m_pInterp)));

  CParameterDictionary* pDict = CParameterDictionary::instance();
  CParameterDictionary::DictionaryIterator pItem = pDict->find("test1");

  EQ(std::string("fortnights"), pItem->second->s_references.front()->getUnits());

}
/**
 * traceneedmore
 *   Trace is a sub-ensemble.  This tests the requirement of a keyword
 *   below that as well.
 */
void
TreePCmdTests::traceneedmore()
{
  CTreeParameterCommand::instance();

  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter trace");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter trace - needs an add, remove or show subcommand."),
     std::string(Tcl_GetStringResult(m_pInterp)));

}
/**
 * traceaddparamcount
 *
 *   treeparameter trace add requires a script argument.
 */
void
TreePCmdTests::traceaddparamcount()
{
  CTreeParameterCommand:: instance();
  
  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter trace add");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter trace add - incorrect parameter count."),
     std::string(Tcl_GetStringResult(m_pInterp)));
}
/**
 * traceaddok
 *
 * Trace should add with this
 */
  /*
     The script just copies parameters to global variables:
  */
static const char* script = 
"proc traceProc {name op} {  \n\
   set ::name $name          \n\
   set ::op   $op            \n\
}";
void
TreePCmdTests::traceaddok()
{
  CTreeParameterCommand::instance();

  // Define the trace proc:

  int status = Tcl_GlobalEval(m_pInterp, script);
  EQ(TCL_OK, status);		// in case there's a parse error.

  status = Tcl_GlobalEval(m_pInterp, 
			  "::spectcl::treeparameter trace add traceProc");
  EQ(TCL_OK, status);
}
/**
 * tracecalled
 *
 *   Once a trace is registered it should be called on changes.
 */
void
TreePCmdTests::tracecalled()
{
  CTreeParameterCommand::instance();
  Tcl_GlobalEval(m_pInterp, script); // Establish the script.
  Tcl_GlobalEval(m_pInterp,
		 "::spectcl::treeparameter trace add traceProc");

  // Should be an addFirst:

  CParameter* p1 = new CParameter("george");
  const char* name = Tcl_GetVar(m_pInterp, "name", TCL_GLOBAL_ONLY);
  const char* op   = Tcl_GetVar(m_pInterp, "op",  TCL_GLOBAL_ONLY);

  ASSERT(name);
  ASSERT(op);
  EQ(std::string("george"), std::string(name));
  EQ(std::string("addFirst"), std::string(op));

  // Should be an addReference

  CParameter* p2 = new CParameter("george");
  name = Tcl_GetVar(m_pInterp, "name", TCL_GLOBAL_ONLY);
  op   = Tcl_GetVar(m_pInterp, "op",  TCL_GLOBAL_ONLY);
  ASSERT(name);
  ASSERT(op);
  EQ(std::string("george"), std::string(name));
  EQ(std::string("addReference"), std::string(op));

  // Should be a removeReference:

  delete p1;
  name = Tcl_GetVar(m_pInterp, "name", TCL_GLOBAL_ONLY);
  op   = Tcl_GetVar(m_pInterp, "op",  TCL_GLOBAL_ONLY);
  ASSERT(name);
  ASSERT(op);
  EQ(std::string("george"), std::string(name));
  EQ(std::string("removeReference"), std::string(op));

  // Should be a removeLast:

  delete p2;
  name = Tcl_GetVar(m_pInterp, "name", TCL_GLOBAL_ONLY);
  op   = Tcl_GetVar(m_pInterp, "op",  TCL_GLOBAL_ONLY);
  ASSERT(name);
  ASSERT(op);
  EQ(std::string("george"), std::string(name));
  EQ(std::string("removeLast"), std::string(op));

}
/**
 * tracecallsmultiple
 *
 *  Ensure that more than one trace can be called.
 */

static const  char* traceScript2 = {
  "proc trace2 {args} {incr ::traced}"
};

void 
TreePCmdTests::tracecallsmultiple()
{
  CTreeParameterCommand::instance();
  Tcl_GlobalEval(m_pInterp, script); // Establish the script.
  Tcl_GlobalEval(m_pInterp, traceScript2);

  Tcl_GlobalEval(m_pInterp,
		 "::spectcl::treeparameter trace add traceProc");
  Tcl_GlobalEval(m_pInterp,
		 "::spectcl::treeparameter trace add trace2");
 // Should be an addFirst:

  CParameter* p1 = new CParameter("george");
  const char* name = Tcl_GetVar(m_pInterp, "name", TCL_GLOBAL_ONLY);
  const char* op   = Tcl_GetVar(m_pInterp, "op",  TCL_GLOBAL_ONLY);

  ASSERT(name);
  ASSERT(op);
  EQ(std::string("george"), std::string(name));
  EQ(std::string("addFirst"), std::string(op));

  const char* traced = Tcl_GetVar(m_pInterp, "traced", TCL_GLOBAL_ONLY);
  ASSERT(traced);
  EQ(std::string("1"),std::string(traced));
}
/**
 *  traceremoveparamcount
 *   trace remove must have a script to remove.
 */
void
TreePCmdTests::traceremoveparamcount()
{
  CTreeParameterCommand::instance();

  int status = Tcl_GlobalEval(m_pInterp, "::spectcl::treeparameter trace remove");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter trace remove - incorrect parameter count."),
     std::string(Tcl_GetStringResult(m_pInterp)));

}
/**
 * traceremoveok
 *
 *   traceremove with a valid script should run ok.
 */
void
TreePCmdTests::traceremoveok()
{
  CTreeParameterCommand::instance();
  Tcl_GlobalEval(m_pInterp, script); // Establish the script.

 Tcl_GlobalEval(m_pInterp,
		 "::spectcl::treeparameter trace add traceProc");
 int status = Tcl_GlobalEval(m_pInterp,
			     "::spectcl::treeparameter trace remove traceProc");
 EQ(TCL_OK, status);
}
/**
 * traceremovenotcalled
 *
 *  A removed trace should not be called:
 */
void
TreePCmdTests::traceremovenotcalled()
{
  CTreeParameterCommand::instance();
  Tcl_GlobalEval(m_pInterp, script); // Establish the script.
  
  Tcl_GlobalEval(m_pInterp,
		 "::spectcl::treeparameter trace add traceProc");
  Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter trace remove traceProc");
  
  CParameter* p1 = new CParameter("george"); // fire trace appatus.
  const char* name = Tcl_GetVar(m_pInterp, "name", TCL_GLOBAL_ONLY);
  const char* op   = Tcl_GetVar(m_pInterp, "op",  TCL_GLOBAL_ONLY);
  ASSERT(!name);
  ASSERT(!op);


}
/**
 * traceremovenosuch
 *
 *  removing a nonexistent trace should fail.
 */
void
TreePCmdTests::traceremovenosuch()
{
  CTreeParameterCommand::instance();
  int status =  Tcl_GlobalEval(m_pInterp,
			       "::spectcl::treeparameter trace remove traceProc");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter trace remove - no such trace procedure"),
     std::string(Tcl_GetStringResult(m_pInterp)));

}
/**
 * traceshowparamcount
 *
 *  The parameter count for trace show is exactly 3.
 */
void
TreePCmdTests::traceshowparamcount()
{
  CTreeParameterCommand::instance();
  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter trace show george");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter trace show - incorrect number of parameters."),
     std::string(Tcl_GetStringResult(m_pInterp)));
}
/**
 * traceshowempty
 *
 *  trace show with nothing registered gets an empty list.
 */
void
TreePCmdTests::traceshowempty()
{
  CTreeParameterCommand::instance();
  int status = Tcl_GlobalEval(m_pInterp,
			      "::spectcl::treeparameter trace show");
  EQ(TCL_OK, status);
  EQ(std::string(""), std::string(Tcl_GetStringResult(m_pInterp)));

}
/**
 * traceshowlist
 *
 *   Registering a set of traces, trace show should show them in registration
 *   order as list elements.
 */
void
TreePCmdTests::traceshowlist()
{
  CTreeParameterCommand::instance();
  Tcl_GlobalEval(m_pInterp, script); // Establish the script.
  Tcl_GlobalEval(m_pInterp, traceScript2);

  Tcl_GlobalEval(m_pInterp,
		 "::spectcl::treeparameter trace add traceProc");
  Tcl_GlobalEval(m_pInterp,
		 "::spectcl::treeparameter trace add trace2");

  int status = Tcl_GlobalEval(m_pInterp, "::spectcl::treeparameter trace show");
  EQ(TCL_OK, status);
  Tcl_Obj* result = Tcl_GetObjResult(m_pInterp);
  CTCLObject actual;
  actual.Bind(CSpecTclInterpreter::instance());
  actual = result;

  CTCLObject should;
  should.Bind(CSpecTclInterpreter::instance());
  should += "traceProc";
  should += "trace2";

  //  The corresponding list elements must be the same:

  CTCLObject should1 = should.lindex(0);
  CTCLObject is1     = actual.lindex(0);
  should1.Bind(CSpecTclInterpreter::instance());
  is1.Bind(CSpecTclInterpreter::instance());
  ASSERT(should1 == is1);


  CTCLObject should2 = should.lindex(1);
  CTCLObject is2     = actual.lindex(1);
  should2.Bind(CSpecTclInterpreter::instance());
  is2.Bind(CSpecTclInterpreter::instance());
  ASSERT(should2 == is2);
    
	     
 }
/**
 * traceinvalidsubcommand
 */
void
TreePCmdTests::traceinvalidsubcommand()
{
  CTreeParameterCommand::instance();
  int status =  Tcl_GlobalEval(m_pInterp, "::spectcl::treeparameter trace junk");
  EQ(TCL_ERROR, status);
  EQ(std::string("::spectcl::treeparameter trace - invalid subcommand."),
     std::string(Tcl_GetStringResult(m_pInterp)));
}
