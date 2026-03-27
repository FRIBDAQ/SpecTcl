//
// Test the tree parameter vector class:

#include <config.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#include "CTreeParameter.h"
#include "CTreeException.h"
#define private public
#include "CTreeParameterVector.h"
#undef private
#include <Event.h>
#include <Parameter.h>

#include "TreeTestSupport.h"


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


class TreeVectorTests : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TreeVectorTests);
    CPPUNIT_TEST(infoConstruct_1);
    
    CPPUNIT_TEST(exceptConstruct_1);
    CPPUNIT_TEST(exceptConstruct_2);
    CPPUNIT_TEST(exceptConstruct_3);
    CPPUNIT_TEST(exceptAssign_1);
    CPPUNIT_TEST(exceptAssign_2);
    CPPUNIT_TEST(exceptWhat_1);

    CPPUNIT_TEST(getinfo_1);
    CPPUNIT_TEST(getinfo_2);
    CPPUNIT_TEST(getinfo_3);
    CPPUNIT_TEST(getinfo_4);
    CPPUNIT_TEST_SUITE_END();

protected:
  void infoConstruct_1();

  void exceptConstruct_1();  // The exception
  void exceptConstruct_2();
  void exceptConstruct_3();
  void exceptAssign_1();
  void exceptAssign_2();
  void exceptWhat_1();

  void getinfo_1();       // Get/create info blocks.
  void getinfo_2();
  void getinfo_3();
  void getinfo_4();
public:
  void setUp() {
  }
  void tearDown() {
    CTreeParameterVector::ClearMap();
    TreeTestSupport::ClearMap();
  }


};

CPPUNIT_TEST_SUITE_REGISTRATION(TreeVectorTests);


void TreeVectorTests::infoConstruct_1() {
    // Construction of an info block is correct:
    // We assume we know the default values for low, high and units:

    CTreeParameterVector::TreeVectorInfo info;
    EQ(double(0.0), info.s_low);
    EQ(double(100.0), info.s_high);
    EQ(std::string(""), info.s_units);
    ASSERT(info.s_createdParameters.empty());
    ASSERT(info.s_event.empty());


}

// Note, exceptConstruct_1 is fragile in that if the message format changes it will break.

void TreeVectorTests::exceptConstruct_1() {
    // from a const char*

    
    CTreeParameterVector::NoSuchVectorException e("name");
    EQ(std::string("There is no vector parameter with the base name name"), e.m_message);
}
void TreeVectorTests::exceptConstruct_2() {
    // From an std::string reference:

    const char* n="name";
    CTreeParameterVector::NoSuchVectorException reference(n);
    std::string name(n);
    CTreeParameterVector::NoSuchVectorException e(name);

    // the two messages are identical:

    EQ(reference.m_message, e.m_message);
}

void TreeVectorTests::exceptConstruct_3() {
    // Copy construction:

    CTreeParameterVector::NoSuchVectorException reference("test");
    CTreeParameterVector::NoSuchVectorException e(reference);

    EQ(reference.m_message, e.m_message);
}

void TreeVectorTests::exceptAssign_1() {
    // Self assignment works.

    CTreeParameterVector::NoSuchVectorException r("test");
    std::string expected = r.m_message;
    CTreeParameterVector::NoSuchVectorException& ref(r = r);
    EQ(expected, r.m_message);
    EQ(&r, &ref);            // Proper reference returned.
}
void TreeVectorTests::exceptAssign_2() {
    // assign to defferent object:

    CTreeParameterVector::NoSuchVectorException rhs("test");
    CTreeParameterVector::NoSuchVectorException lhs("junk");

    CTreeParameterVector::NoSuchVectorException& ref(lhs = rhs);

    EQ(rhs.m_message, lhs.m_message);
    EQ(&ref, &lhs);
}

void TreeVectorTests::exceptWhat_1() {
    // I get m_message from what:

    CTreeParameterVector::NoSuchVectorException e("Testing");
    std::string msg(e.what());

    EQ(e.m_message, msg);
}