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
#include "CMatchingVisitor.h"

#include "TreeTestSupport.h"

#include <algorithm>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif



class MatchTests : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(MatchTests);
  CPPUNIT_TEST(MatchAll);
  CPPUNIT_TEST(MatchGeorge);
  CPPUNIT_TEST(MatchYEndings);
  CPPUNIT_TEST_SUITE_END();


private:
  CTreeParameter*  m_pParam1;
  CTreeParameter*  m_pParam2;
  CTreeParameterArray* m_pGeorgeArray;
public:
  void setUp() {
    m_pParam1 = new CTreeParameter("Larry", "cm");
    m_pParam2 = new CTreeParameter("Curly", "mm");
    m_pGeorgeArray = new CTreeParameterArray("george", 10, 1);
  }
  void tearDown() {
    delete m_pParam1;
    delete m_pParam2;
    delete m_pGeorgeArray;
    TreeTestSupport::ClearMap();

  }
protected:
  void MatchAll();
  void MatchGeorge();
  void MatchYEndings();
};


class CountMatches : public CMatchingVisitor 
{
public:
 static int m_nMatches;		// Since the visitor gets instantiated each time.
public:
  CountMatches()
  {}
  CountMatches(string pattern) : CMatchingVisitor(pattern)
  {}

  virtual void OnMatch(CTreeParameter* pParam) {
    m_nMatches++;
  }
  int getMatchCount() { return m_nMatches; }

};
int CountMatches::m_nMatches;

CPPUNIT_TEST_SUITE_REGISTRATION(MatchTests);

void MatchTests::MatchAll() 
{
  // Count matches for default construction:

  CountMatches visitor;
  CountMatches::m_nMatches = 0;
  
  for_each(CTreeParameter::begin(), CTreeParameter::end(),
	   visitor);
  EQ(12, visitor.getMatchCount());


}
//
void MatchTests::MatchGeorge()
{
  CountMatches visitor("g*");
  CountMatches::m_nMatches = 0;
  for_each(CTreeParameter::begin(), CTreeParameter::end(),
	   visitor);
  EQ(10, visitor.getMatchCount());
}
//
void MatchTests::MatchYEndings()
{
  CountMatches visitor("*y");
  CountMatches::m_nMatches = 0;

  for_each(CTreeParameter::begin(), CTreeParameter::end(),
	   visitor);
  EQ(2, visitor.getMatchCount());

}
