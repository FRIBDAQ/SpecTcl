/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  MdirTests.cpp
 *  @brief: Test mirror directory
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "MirrorDirectory.h"

class MdirTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(MdirTest);
    CPPUNIT_TEST(empty);
    CPPUNIT_TEST(put_1);
    CPPUNIT_TEST(put_2);
    CPPUNIT_TEST(get_1);
    CPPUNIT_TEST(get_2);
    CPPUNIT_TEST(list_1);
    CPPUNIT_TEST_SUITE_END();
    
private:
    MirrorDirectory* m_pInstance;
public:
    void setUp() {
        m_pInstance = new MirrorDirectory;
    }
    void tearDown() {
        delete m_pInstance;
    }
protected:
    void empty();
    void put_1();
    void put_2();
    void get_1();
    void get_2();
    void list_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION(MdirTest);

void MdirTest::empty()
{
    auto listing = m_pInstance->list();
    EQ(size_t(0), listing.size());
}
// Put a single element:
void MdirTest::put_1()
{
    m_pInstance->put("somehost", "ABCD");
    auto listing = m_pInstance->list();
    EQ(size_t(1), listing.size());
    
    EQ(std::string("somehost"), listing[0].first);
    EQ(std::string("ABCD"), listing[0].second);
}

// Put duplicate throws logic_error:
void MdirTest::put_2()
{
    m_pInstance->put("somehost", "ABCD");
    CPPUNIT_ASSERT_THROW(
        m_pInstance->put("somehost", "EFGH"),
        std::logic_error
    );
}
// put a few and get one OK
void MdirTest::get_1()
{
    m_pInstance->put("host1", "abcd");
    m_pInstance->put("host2", "efgh");
    m_pInstance->put("host3", "ijkl");
    
    EQ(std::string("efgh"), m_pInstance->get("host2"));
}
// failed get

void MdirTest::get_2()
{
    m_pInstance->put("host1", "abcd");
    m_pInstance->put("host2", "efgh");
    m_pInstance->put("host3", "ijkl");
    
    CPPUNIT_ASSERT_THROW(
        m_pInstance->get("host4"), std::logic_error
    );
}
// list gives all in alpha order.

void MdirTest::list_1()
{
    // Reverse alpha order:
    
    m_pInstance->put("host3", "ijkl");
    m_pInstance->put("host2", "efgh");
    m_pInstance->put("host1", "abcd");
    
    auto  listing = m_pInstance->list();
    EQ(size_t(3), listing.size());    // They're all there.
    
    EQ(std::string("host1"), listing[0].first);
    EQ(std::string("abcd"), listing[0].second);
    
    EQ(std::string("host2"), listing[1].first);
    EQ(std::string("efgh"), listing[1].second);
    
    EQ(std::string("host3"), listing[2].first);
    EQ(std::string("ijkl"), listing[2].second);
}