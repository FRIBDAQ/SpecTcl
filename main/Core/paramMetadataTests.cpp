#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "Parameter.h"
#include "CNoSuchObjectException.h"
#include "TCLException.h"
#include <vector>
#include <SpecTcl.h>
#include <TCLInterpreter.h>
#include "TreeTestSupport.h"
#include "ParameterCommand.h"

class ParamMetadataTests : public CppUnit::TestFixture {
private:
    CPPUNIT_TEST_SUITE(ParamMetadataTests);
    CPPUNIT_TEST(get_1);
    CPPUNIT_TEST(set_1);
    CPPUNIT_TEST(getall_1);
    CPPUNIT_TEST(cmdset_1);        // No such parameter.
    CPPUNIT_TEST(cmdset_2);        // Incorrect param count.
    CPPUNIT_TEST(cmdset_3);        // good -setmetaata
    CPPUNIT_TEST(cmdget_1);        // No such parameter
    CPPUNIT_TEST(cmdget_2);        // incorrect param count.
    CPPUNIT_TEST(cmdget_3);        // No such metadata.
    CPPUNIT_TEST(cmdget_4);        // good get.
    CPPUNIT_TEST(cmddump_1);       // empty dump
    CPPUNIT_TEST(cmddump_2);       // dump with stuff.
    CPPUNIT_TEST_SUITE_END();

protected:
    void get_1();
    void set_1();
    void getall_1();

    void cmdset_1();
    void cmdset_2();
    void cmdset_3();
    void cmdget_1();
    void cmdget_2();
    void cmdget_3();
    void cmdget_4();
    void cmddump_1();
    void cmddump_2();

private:
    CParameter* m_pTestp;
    CTCLInterpreter* m_pInterp;
    CParameterCommand* m_pCommand;

public:
    void setUp() {
        TreeTestSupport::InitTestInterpreter();
        m_pInterp = TreeTestSupport::getInterpreter();
        m_pCommand = new CParameterCommand(m_pInterp);
        SpecTcl::getInstance()->AddParameter("test", 1, "");
        m_pTestp = SpecTcl::getInstance()->FindParameter("test");
    }
    void tearDown() {
        TreeTestSupport::ClearMap();
        delete m_pCommand;
        TreeTestSupport::TeardownTestInterpreter;
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
// Tests of the  parameter metadata sub-commands:

void
ParamMetadataTests::cmdset_1() {
    // No such parameter

    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("::spectcl::serial::parameter -setmetadata junk a b"),
        CTCLException
    );
}

void
ParamMetadataTests::cmdset_2() {
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("::spectcl::serial::parameter -setmetadata junk"),
        CTCLException
    );

}
void 
ParamMetadataTests::cmdset_3() {
    CPPUNIT_ASSERT_NO_THROW(
        m_pInterp->GlobalEval("::spectcl::serial::parameter -setmetadata test a b")
    );
    std::string value;
    CPPUNIT_ASSERT_NO_THROW(
        value = m_pTestp->getMetadata("a")
    );
    EQ(std::string("b"), value);

}

void 
ParamMetadataTests::cmdget_1() {
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("::spectcl::serial::parameter -getmetadata junk a"),
        CTCLException
    );
}
void
ParamMetadataTests::cmdget_2() {
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("::spectcl::serial::parameter -getmetadata junk a b"),
        CTCLException
    );
}
void
ParamMetadataTests::cmdget_3() {
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("::spectcl::serial::parameter -getmetadata test a"),
        CTCLException
    );
}
void
ParamMetadataTests::cmdget_4() {
    m_pTestp->setMetadata("a", "b");   // now there's metdata to get...
    std::string value;
    CPPUNIT_ASSERT_NO_THROW(
        value = m_pInterp->GlobalEval("::spectcl::serial::parameter -getmetadata test a")
    );
    EQ(std::string("b"), value);
}
void
ParamMetadataTests::cmddump_1() {
    std::string value;
    CPPUNIT_ASSERT_NO_THROW(
        value = m_pInterp->GlobalEval("::spectcl::serial::parameter -dumpmetadata test")
    );
    EQ(std::string(""), value);
}

void
ParamMetadataTests::cmddump_2() {
    m_pTestp->setMetadata("a", "b");
    m_pTestp->setMetadata("c", "d");

    std::string value = m_pInterp->GlobalEval("::spectcl::serial::parameter -dumpmetadata test");
    EQ(std::string("a b c d"), value);
}