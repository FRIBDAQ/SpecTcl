#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "Parameter.h"
#include "CNoSuchObjectException.h"
#include <vector>

class ParamMetadataTests : public CppUnit::TestFixture {
private:
    CPPUNIT_TEST_SUITE(ParamMetadataTests);
    CPPUNIT_TEST(get_1);
    CPPUNIT_TEST(set_1);
    CPPUNIT_TEST(getall_1);
    CPPUNIT_TEST_SUITE_END();

protected:
    void get_1();
    void set_1();
    void getall_1();

private:
    CParameter* m_pTestp;
public:
    void setUp() {
        m_pTestp = new CParameter;
    }
    void tearDown() {
        delete m_pTestp;
        m_pTestp = nullptr;
    }

};
CPPUNIT_TEST_SUITE_REGISTRATION(ParamMetadataTests);


void
ParamMetadataTests::get_1() {
    // fail to get non-existent metadata:

    CPPUNIT_ASSERT_THROW(
        m_pTestp->getMetadata("something"),
        CNoSuchObjectException
    );
}

void
ParamMetadataTests::set_1() {
    // can set/get:

    CPPUNIT_ASSERT_NO_THROW(
        m_pTestp->setMetadata("a", "b")
    );
    std::string value;
    CPPUNIT_ASSERT_NO_THROW(
        value = m_pTestp->getMetadata("a")
    );
    EQ(std::string("b"), value);
}
void
ParamMetadataTests::getall_1() {
    // can get the whole dict:

    m_pTestp->setMetadata("c", "d");
    m_pTestp->setMetadata("a", "b");
    
    // dicts iterate in alpha order:

    std::vector<std::pair<std::string, std::string>> expect = {
        {"a", "b"}, {"c", "d"}
    };

    int i = 0;
    auto& md = m_pTestp->getAllMetadata();
    EQ(size_t(2), md.size());

    for (auto& p : md) {
        EQ(expect[i].first, p.first);
        EQ(expect[i].second, p.second);
        i++;
    }

}