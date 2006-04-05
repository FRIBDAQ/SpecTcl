// Template for a test suite.
// This implementation of TreeParameter is based on the ideas and original code of::
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//

#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CTreeParameter.h"
#include "CTreeParameterArray.h"
#include "ListVisitor.h"
#include <tcl.h>
#include <TCLInterpreter.h>
#include <TCLResult.h>
#include <TCLList.h>
#include <vector>
#include <string>

#include "TreeTestSupport.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


class ListTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ListTests);
  CPPUNIT_TEST(ListAll);
  CPPUNIT_TEST(ListOne);
  CPPUNIT_TEST_SUITE_END();


private:
  Tcl_Interp*       m_pRawInterp;
  CTCLInterpreter*  m_pInterp;
  CTCLResult*      m_pResult;
public:
  void setUp() {
    
    m_pRawInterp = Tcl_CreateInterp();
    m_pInterp    = new CTCLInterpreter(m_pRawInterp);
    m_pResult    = new CTCLResult(m_pInterp);
  }
  void tearDown() {
    
    delete m_pResult;
    delete m_pInterp;
    Tcl_DeleteInterp(m_pRawInterp);
    TreeTestSupport::ClearMap();
  }
protected:
  void ListAll();
  void ListOne();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ListTests);

void ListTests::ListAll() {
  CTreeParameterArray   george("george", 10, 0);
  CTreeParameter::BindParameters();

  ListVisitor listAll("*", *m_pResult);
  for_each(CTreeParameter::begin(), CTreeParameter::end(), listAll);

  CTCLList List(m_pInterp, string((*m_pResult)));
  StringArray list;
  List.Split(list);

  EQMSG("Count", (size_t)10, list.size());
}

void
ListTests::ListOne()
{
  CTreeParameterArray   george("george", 10, 0);
  CTreeParameter::BindParameters();

  ListVisitor listAll("*.00", *m_pResult);
  for_each(CTreeParameter::begin(), CTreeParameter::end(), listAll);

  CTCLList List(m_pInterp, string((*m_pResult)));
  StringArray list;
  List.Split(list);

  EQMSG("count", (size_t)1, list.size());
  
  CTCLList item(m_pInterp, list[0]);
  StringArray items;
  item.Split(items);

  EQMSG("name", string("george.00"), items[0]);
  EQMSG("bins", string("100")      , items[1]);
  EQMSG("start",string("1"),         items[2]);
  EQMSG("stop", string("100"),       items[3]);
  EQMSG("inc",  string("0.99"),      items[4]);
  EQMSG("units", string("unknown"),  items[5]);

  EQMSG("elements", (size_t)6, items.size());
}
