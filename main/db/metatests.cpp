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

#include <sqlite3.h>
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

    CPPUNIT_TEST(seltv_1);    // No such tree variable.
    CPPUNIT_TEST(seltv_2);    // good selection.

    CPPUNIT_TEST(set_1);      // Set parameter metadata.
    CPPUNIT_TEST(set_2);      // Set spectrum metadata.
    CPPUNIT_TEST(set_3);      // set gate metadata.
    CPPUNIT_TEST(set_4);      // set tree var metadata.
    
    CPPUNIT_TEST(get_1);      // get metadata nonexistent.
    CPPUNIT_TEST(get_2);      // get metadata exists.
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

    void seltv_1();
    void seltv_2();

    void set_1();
    void set_2();
    void set_3();
    void set_4();

    void get_1();
    void get_2();
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

        delete m_saveset->createVariable("pi", 3.14159265359, "radians");

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

void metatests::seltv_1() {
    SpecTclDB::DBMetadata md(*m_connection, m_saveid);
    CPPUNIT_ASSERT_THROW(
        md.selectTreevar("nosuchvar"),
        std::exception
    );
}

void metatests::seltv_2() {
    SpecTclDB::DBMetadata md(*m_connection, m_saveid);
    CPPUNIT_ASSERT_NO_THROW(
        md.selectTreevar("pi")
    );
    EQ(std::string("treevariable"), md.m_type);

    auto v = m_saveset->lookupVariable("pi");
    int sb = v->getInfo().s_id;
    delete v;

    EQ(sb, md.m_fkId);
}

void metatests::set_1() {
    SpecTclDB::DBMetadata md(*m_connection, m_saveid);
    md.selectParameter("aparameter");

    // Setting once will make a record. of the right shape:

    md.setMetadataItem("anitem", "aaaa");
    CSqliteStatement stmt(*m_connection,
        "SELECT type, item_id, value FROM metadata WHERE name = ?"
    );
    stmt.bind(1, "anitem", -1, SQLITE_STATIC);
    ++stmt;
    ASSERT(!stmt.atEnd()); // there is a record.
    std::string type = reinterpret_cast<const char*>(stmt.getText(0));
    int item_id = stmt.getInt(1);
    std::string value = reinterpret_cast<const char*>(stmt.getText(2));

    EQ(std::string("parameter"), type);
    EQ(std::string("aaaa"), value);
    
    EQ(md.m_fkId, item_id);

    ++stmt;
    ASSERT(stmt.atEnd());     // no second one.

    // Setting twice will not make a dup record but overwrite.

    stmt.reset();
    md.setMetadataItem("anitem", "bbbb");
    ++stmt;
    ASSERT(!stmt.atEnd());
    value = reinterpret_cast<const char*>(stmt.getText(2));
    EQ(std::string("bbbb"),  value);

    ++stmt;
    ASSERT(stmt.atEnd());             // didn't make another one.
}
void metatests::set_2() {
    SpecTclDB::DBMetadata md(*m_connection, m_saveid);
    md.selectSpectrum("aspec");

    // Setting once will make a record. of the right shape:

    md.setMetadataItem("anitem", "aaaa");
    CSqliteStatement stmt(*m_connection,
        "SELECT type, item_id, value FROM metadata WHERE name = ?"
    );
    stmt.bind(1, "anitem", -1, SQLITE_STATIC);
    ++stmt;
    ASSERT(!stmt.atEnd()); // there is a record.
    std::string type = reinterpret_cast<const char*>(stmt.getText(0));
    int item_id = stmt.getInt(1);
    std::string value = reinterpret_cast<const char*>(stmt.getText(2));

    EQ(std::string("spectrum"), type);
    EQ(std::string("aaaa"), value);
    
    EQ(md.m_fkId, item_id);

    ++stmt;
    ASSERT(stmt.atEnd());     // no second one.

    // Setting twice will not make a dup record but overwrite.

    stmt.reset();
    md.setMetadataItem("anitem", "bbbb");
    ++stmt;
    ASSERT(!stmt.atEnd());
    value = reinterpret_cast<const char*>(stmt.getText(2));
    EQ(std::string("bbbb"),  value);

    ++stmt;
    ASSERT(stmt.atEnd());             // didn't make another one.
}
void metatests::set_3() {
    SpecTclDB::DBMetadata md(*m_connection, m_saveid);
    md.selectGate("agate");

    // Setting once will make a record. of the right shape:

    md.setMetadataItem("anitem", "aaaa");
    CSqliteStatement stmt(*m_connection,
        "SELECT type, item_id, value FROM metadata WHERE name = ?"
    );
    stmt.bind(1, "anitem", -1, SQLITE_STATIC);
    ++stmt;
    ASSERT(!stmt.atEnd()); // there is a record.
    std::string type = reinterpret_cast<const char*>(stmt.getText(0));
    int item_id = stmt.getInt(1);
    std::string value = reinterpret_cast<const char*>(stmt.getText(2));

    EQ(std::string("gate"), type);
    EQ(std::string("aaaa"), value);
    
    EQ(md.m_fkId, item_id);

    ++stmt;
    ASSERT(stmt.atEnd());     // no second one.

    // Setting twice will not make a dup record but overwrite.

    stmt.reset();
    md.setMetadataItem("anitem", "bbbb");
    ++stmt;
    ASSERT(!stmt.atEnd());
    value = reinterpret_cast<const char*>(stmt.getText(2));
    EQ(std::string("bbbb"),  value);

    ++stmt;
    ASSERT(stmt.atEnd());             // didn't make another one.
}
void metatests::set_4() {
    SpecTclDB::DBMetadata md(*m_connection, m_saveid);
    md.selectTreevar("pi");

    // Setting once will make a record. of the right shape:

    md.setMetadataItem("anitem", "aaaa");
    CSqliteStatement stmt(*m_connection,
        "SELECT type, item_id, value FROM metadata WHERE name = ?"
    );
    stmt.bind(1, "anitem", -1, SQLITE_STATIC);
    ++stmt;
    ASSERT(!stmt.atEnd()); // there is a record.
    std::string type = reinterpret_cast<const char*>(stmt.getText(0));
    int item_id = stmt.getInt(1);
    std::string value = reinterpret_cast<const char*>(stmt.getText(2));

    EQ(std::string("treevariable"), type);
    EQ(std::string("aaaa"), value);
    
    EQ(md.m_fkId, item_id);

    ++stmt;
    ASSERT(stmt.atEnd());     // no second one.

    // Setting twice will not make a dup record but overwrite.

    stmt.reset();
    md.setMetadataItem("anitem", "bbbb");
    ++stmt;
    ASSERT(!stmt.atEnd());
    value = reinterpret_cast<const char*>(stmt.getText(2));
    EQ(std::string("bbbb"),  value);

    ++stmt;
    ASSERT(stmt.atEnd());             // didn't make another one.
}

// For get we're going to just test with parameters assuming if that works, it's the same
// query with just a different item type and the set tests
// determine the types all get set properly.

void
metatests::get_1() {
    SpecTclDB::DBMetadata md(*m_connection, m_saveid);
    md.selectParameter("aparameter");

    CPPUNIT_ASSERT_THROW(
        md.getMetadataValue("nosuchitem"),
        std::exception
    );
}
void
metatests::get_2() {
    SpecTclDB::DBMetadata md(*m_connection, m_saveid);
    md.selectParameter("aparameter");
    md.setMetadataItem("some", "value");

    std::string value; 
    CPPUNIT_ASSERT_NO_THROW(
        value = md.getMetadataValue("some")
    );
    EQ(std::string("value"), value);
}