/** Unit test module for the metadata class (DBMetadata.{h,cpp} */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "SpecTclDatabase.h"
#include "CSqlite.h"
#include "CSqliteStatement.h"
#include "CSqliteException.h"

#define private public
#include "DBMetadata.h"
#undef private

// We need to create things to hang metadata on:

#include "SaveSet.h"
#include "DBParameter.h"
#include "DBGate.h"
#include "DBSpectrum.h"
#include "DBTreeVariable.h"

#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdexcept>
#include <sstream>
class metatests : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(metatests);
    CPPUNIT_TEST(construct_1);   // ok
    CPPUNIT_TEST(construct_2);   // no such saveset.

    CPPUNIT_TEST(selpar_1);     // no such parameter.
    CPPUNIT_TEST(selpar_2);     // Good parameter.

    CPPUNIT_TEST(selspec_1);   // No such spectrum.
    CPPUNIT_TEST(selspec_2);    // Good selection.

    CPPUNIT_TEST(selgate_1);   // NO such gate.
    CPPUNIT_TEST(selgate_2);
    CPPUNIT_TEST_SUITE_END();
protected:
    void construct_1();
    void construct_2();

    void selpar_1();
    void selpar_2();

    void selspec_1();
    void selspec_2();

    void selgate_1();
    void selgate_2();
private:
    std::string m_filename;
    SpecTclDB::CDatabase* m_db;
    SpecTclDB::SaveSet*   m_saveset;
    int                   m_saveid;
    CSqlite*              m_connection;
public:
    void setUp() {
        const char* fileTemplate= "dbmetatestXXXXXXX";
        char fname[200];
        strcpy(fname, fileTemplate);
        int fd = mkstemp(fname);
        if (fd < 0) {
            int e = errno;
            std::stringstream msg;
            msg << "Unable to create tempfile: " << fileTemplate
                << " : " << strerror(e);
            throw std::logic_error(msg.str());
        }
        close (fd);
        m_filename = fname;
        SpecTclDB::CDatabase::create(fname);
        m_db = new SpecTclDB::CDatabase(fname);
        m_saveset = m_db->createSaveSet("set1");
        m_connection = new CSqlite(fname);
        m_saveid = m_saveset->getInfo().s_id;

        // Make a parameter in the database.

        delete m_saveset->createParameter("aparameter", 1);
        std::vector<const char*> parname = {"aparameter"};
        std::vector<SpecTclDB::SaveSet::SpectrumAxis> axes = {{0, 1024, 1024}};
        delete m_saveset->createSpectrum("aspec", "1", parname, axes);

        // Make a gate simplest to make a TRUE gate.:

        std::vector<const char*> dependent;
        delete m_saveset->createCompoundGate("agate", "T", dependent);



    }
public:
    void tearDown() {
        delete m_saveset;
        delete m_db;
        delete m_connection;
        unlink(m_filename.c_str());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(metatests);

void
metatests::construct_1() {
    SpecTclDB::DBMetadata md(*m_connection, m_saveid);

    // Initializes the connection and the save set id:

    EQ(m_saveid, md.m_saveId);
    ASSERT(m_connection  == &md.m_connection);
}

void metatests::construct_2() {
    CPPUNIT_ASSERT_THROW(
        SpecTclDB::DBMetadata md(*m_connection, m_saveid+1),
        std::exception
    );
}
void metatests::selpar_1() {
    SpecTclDB::DBMetadata md(*m_connection, m_saveid);
    CPPUNIT_ASSERT_THROW(
        md.selectParameter("nosuchparam"),
        std::exception
    );
}
void metatests::selpar_2() {
    SpecTclDB::DBMetadata md(*m_connection, m_saveid);
    CPPUNIT_ASSERT_NO_THROW(
        md.selectParameter("aparameter")
    );
    EQ(std::string("parameter"), md.m_type);
    auto param = m_saveset->findParameter("aparameter");   
    int expected_id = param->getInfo().s_id;
    delete param;
    EQ(expected_id, md.m_fkId);
}

void metatests::selspec_1() {
    SpecTclDB::DBMetadata md(*m_connection, m_saveid);
    CPPUNIT_ASSERT_THROW(
        md.selectSpectrum("nosuchspectrum"),
        std::exception
    );

}
void metatests::selspec_2() {
    SpecTclDB::DBMetadata md(*m_connection, m_saveid);
    CPPUNIT_ASSERT_NO_THROW(
        md.selectSpectrum("aspec")
    );
    EQ(std::string("spectrum"), md.m_type);
    auto spec = m_saveset->lookupSpectrum("aspec");
    int id = spec->getInfo().s_base.s_id;
    delete spec;

    EQ(id, md.m_fkId);
}

void metatests::selgate_1() {
    SpecTclDB::DBMetadata md(*m_connection, m_saveid);
    CPPUNIT_ASSERT_THROW(
        md.selectGate("nosuchgate"),
        std::exception
    );
}
void metatests::selgate_2() {
    SpecTclDB::DBMetadata md(*m_connection, m_saveid);
    CPPUNIT_ASSERT_NO_THROW(
        md.selectGate("agate")
    );
    EQ(std::string("gate"), md.m_type);

    auto gate = m_saveset->lookupGate("agate");
    int id = gate->getInfo().s_info.s_id;
    delete gate;

    EQ(id, md.m_fkId);
}