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

#include <stdexcept>




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

    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    CPPUNIT_TEST(construct_4);
    CPPUNIT_TEST(construct_5);  // Copy construction.

    CPPUNIT_TEST(assign_1);
    CPPUNIT_TEST(assign_2);

    CPPUNIT_TEST(eq_1);        // Comparisons ==
    CPPUNIT_TEST(eq_2);
    CPPUNIT_TEST(ne_1);        // !=
    CPPUNIT_TEST(ne_2); 

    CPPUNIT_TEST(push_1);
    
    CPPUNIT_TEST(indexing_1);
    CPPUNIT_TEST(indexing_2);

    CPPUNIT_TEST(size_1);
    CPPUNIT_TEST(alloc_1);

    CPPUNIT_TEST(reset_1);
    CPPUNIT_TEST(reset_2);
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

  void construct_1();
  void construct_2();
  void construct_3();
  void construct_4();
  void construct_5();

  void assign_1();
  void assign_2();

  void eq_1();
  void eq_2();
  void ne_1();
  void ne_2();

  void push_1();

  void indexing_1();
  void indexing_2();

  void size_1();
  void alloc_1();

  void reset_1();
  void reset_2();
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

void TreeVectorTests::getinfo_1() {
    // Creates a new info block with my
    // desired properties.

    auto p = CTreeParameterVector::getInfoBlock("testing", -1.0, 1.0, "mm");
    ASSERT(p);   //Got one.
    EQ(double(-1.0), p->s_low);
    EQ(double(1.0), p->s_high);
    EQ(std::string("mm"), p->s_units);

    // Not going to look at the vectors because we tested construction already.
    // But it should have gone in the map.

    ASSERT(CTreeParameterVector::m_baseNameMap.find(std::string("testing")) != CTreeParameterVector::m_baseNameMap.end());
}
void TreeVectorTests::getinfo_2() {
    // I can override existing block 

    auto p1 = CTreeParameterVector::getInfoBlock("testing", -1.0, 1.0, "mm");
    auto p2 = CTreeParameterVector::getInfoBlock("testing", 0.0, 360.0, "degrees"); 

    // Shoulid be the same block:

    EQ(p1, p2);

    // But we have the new values for the limits etc.:

    EQ(double(0.0), p1->s_low);
    EQ(double(360.0), p1->s_high);
    EQ(std::string("degrees"), p1->s_units);
    EQ(size_t(1), CTreeParameterVector::m_baseNameMap.size());   // only one map entry!
}
void TreeVectorTests::getinfo_3() {
    // Can create a new one with default params:

    auto p = CTreeParameterVector::getInfoBlock("testing");
    ASSERT(p);   // got one.

    EQ(double(0.0), p->s_low);
    EQ(double(100.0), p->s_high);
    EQ(std::string(""), p->s_units);

    // And an entry was made:

    ASSERT(CTreeParameterVector::m_baseNameMap.find(std::string("testing")) != CTreeParameterVector::m_baseNameMap.end());
}

void TreeVectorTests::getinfo_4() {
    // Default construction won't override existing block data:

    auto p1 = CTreeParameterVector::getInfoBlock("testing", -1.0, 1.0, "mm");
    auto p2 = CTreeParameterVector::getInfoBlock("testing");

    EQ(p1, p2);

    // The block should not have had low, high, units modified:

    EQ(double(-1.0), p2->s_low);
    EQ(double(1.0), p2->s_high);
    EQ(std::string("mm"), p2->s_units);

    // Only one block:

    EQ(size_t(1), CTreeParameterVector::m_baseNameMap.size());

}

void TreeVectorTests::construct_1() {
    // construct no such with  low, high, units set

    CTreeParameterVector v("test", -1.0, 1.0, "mm");
    ASSERT(v.m_pInfo);    // There is an info block....
    EQ(double(-1.0), v.m_pInfo->s_low);
    EQ(double(1.0), v.m_pInfo->s_high);
    EQ(std::string("mm"), v.m_pInfo->s_units);

    // Alreday tested the getInfBlock enters into the dict.
}
void TreeVectorTests::construct_2() {
    //Construct with defaults


    CTreeParameterVector v("test");
    ASSERT(v.m_pInfo);    // There is an info block....
    EQ(double(0), v.m_pInfo->s_low);
    EQ(double(100.0), v.m_pInfo->s_high);
    EQ(std::string(""), v.m_pInfo->s_units);
}
void TreeVectorTests::construct_3() {
    // Default won't override the existing:

    CTreeParameterVector v1("test", -1.0, 1.0, "mm");
    CTreeParameterVector v2("test");

    EQ(v1.m_pInfo, v2.m_pInfo);
    EQ(double(-1.0), v2.m_pInfo->s_low);
    EQ(double(1.0), v2.m_pInfo->s_high);
    EQ(std::string("mm"), v2.m_pInfo->s_units);

}
void TreeVectorTests::construct_4() {
    //  parameterized constructor can override existing def:

    CTreeParameterVector v1("test");
    CTreeParameterVector v2("test", -1.0, 1.0, "mm");
    
    EQ(v1.m_pInfo, v2.m_pInfo);
    EQ(double(-1.0), v2.m_pInfo->s_low);
    EQ(double(1.0), v2.m_pInfo->s_high);
    EQ(std::string("mm"), v2.m_pInfo->s_units);
}

void TreeVectorTests::construct_5() {
    // Copy construction:

    CTreeParameterVector v1("test", -1.0, 1.0, "mm");
    CTreeParameterVector v2(v1);

    EQ(v1.m_pInfo, v2.m_pInfo);
    EQ(double(-1.0), v2.m_pInfo->s_low);
    EQ(double(1.0), v2.m_pInfo->s_high);
    EQ(std::string("mm"), v2.m_pInfo->s_units);
    EQ(v1.m_baseName, v2.m_baseName);
}

void TreeVectorTests::assign_1() {
    // Assignment from other creates a duplicate but shares the info.
    CTreeParameterVector v1("test");
    CTreeParameterVector v2("rhs", 0.0, 360.0, "degrees");


    CTreeParameterVector& r(v1 = v2);   // NOte the info block and created params are still there..

    EQ(v1.m_pInfo, v2.m_pInfo);
    EQ(&v1, &r);                        // COrrect reference.

    EQ(size_t(2), CTreeParameterVector::m_baseNameMap.size());   // both are still there ...
}
void TreeVectorTests::assign_2() {
    // Self assign is ok.

    CTreeParameterVector v("test", -1.0, 1.0, "mm");
    auto expected = v.m_pInfo;

    CTreeParameterVector& r(v = v);

    EQ(expected, v.m_pInfo);
    EQ(&v, &r);
}

void TreeVectorTests::eq_1() {
    // Test for equality when they are:

    CTreeParameterVector v1("test", 0.0, 360.0, "degrees");
    CTreeParameterVector v2("test");

    ASSERT(v1 == v2);
}
void TreeVectorTests::eq_2() {
    // test for equality when they're not:

    CTreeParameterVector v1("test1");
    CTreeParameterVector v2("test2");

    ASSERT(!(v1 == v2));
}

void TreeVectorTests::ne_1() {
    // inequality when they are equal:

    CTreeParameterVector v1("test", 0.0, 360.0, "degrees");
    CTreeParameterVector v2("test");

    ASSERT(!(v1 != v2));
}
void TreeVectorTests::ne_2() {
    // inequality when they're not equal:

    CTreeParameterVector v1("test1");
    CTreeParameterVector v2("test2");

    ASSERT(v1 != v2);
}
// pushing values works:

void TreeVectorTests::push_1() {
    CEvent event;
    CTreeParameter::setEvent(event);

    CTreeParameterVector v("test");
    CTreeParameter& r(v.push_back(3.1416));

    // Ok so we made "test(0)" both in the created events active
    // parameters nd test(0) has the value 3.1416.
    // r References that tree parameter too.

    EQ(size_t(1), v.m_pInfo->s_createdParameters.size());
    EQ(size_t(1), v.m_pInfo->s_event.size());

    CTreeParameter* p(v.m_pInfo->s_event[0]);
    EQ(p, &r);
    EQ(std::string("test(0)"), r.getName());
    EQ(double(3.1416), double(r));

}

void TreeVectorTests::indexing_1() {
    // INdexing in range:

    CEvent event;
    CTreeParameter::setEvent(event);

    CTreeParameterVector v("test");
    for (int i =0; i < 10; i++) {
        v.push_back(double(i));
    }

    CTreeParameter* p;
    CPPUNIT_ASSERT_NO_THROW(
        p = &(v[5])
    );
    EQ(double(5.0), double(*p));
}
void TreeVectorTests::indexing_2() {
    // indexing out of range...

    CEvent event;
    CTreeParameter::setEvent(event);

    CTreeParameterVector v("test");
    for (int i =0; i < 10; i++) {
        v.push_back(double(i));
    }


    CPPUNIT_ASSERT_THROW(
        v[20],
        std::out_of_range
    );
}

void TreeVectorTests::size_1() {
    // size returns number in the event:

    CEvent event;
    CTreeParameter::setEvent(event);

    CTreeParameterVector v("test");
    for (int i =0; i < 10; i++) {
        v.push_back(double(i));
    }
    EQ(size_t(10), v.size());
}

void TreeVectorTests::alloc_1() {
    // allocation keeps up with size if event is not reset.

    CEvent event;
    CTreeParameter::setEvent(event);

    CTreeParameterVector v("test");
    for (int i =0; i < 10; i++) {
        v.push_back(double(i));
    }
    EQ(size_t(10), v.allocation());
}


void TreeVectorTests::reset_1() {
    // Reset resets the event but keeps the allocation:

    CEvent event;
    CTreeParameter::setEvent(event);

    CTreeParameterVector v("test");
    for (int i =0; i < 10; i++) {
        v.push_back(double(i));
    }

    v.reset();

    EQ(size_t(0), v.size());
    EQ(size_t(10),v.allocation());
}

void TreeVectorTests::reset_2() {
    // Allocatd parameters get re-used in order.

    CEvent event;
    CTreeParameter::setEvent(event);

    CTreeParameterVector v("test");
    for (int i =0; i < 10; i++) {
        v.push_back(double(i));
    }

    v.reset();

    v.push_back(3.1416);

    EQ(size_t(1), v.size());
    EQ(size_t(10), v.allocation());   // no new one allocated.

    EQ(std::string("test(0)"), v[0].getName());
}