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

/** @file:  dbspectests.cpp
 *  @brief: Test the SpecTcl::DBSpectrum class.
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"

#define private public    // allows testing of private statics.
#include "DBSpectrum.h"
#undef public

#include "SpecTclDatabase.h"
#include "SaveSet.h"
#include "DBParameter.h"

#include "CSqlite.h"
#include "CSqliteStatement.h"

#include <string>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <stdexcept>
#include <errno.h>

class dbspectest : public CppUnit::TestFixture {

    
private:
    std::string m_filename;
    CSqlite*           m_pDb;
    SpecTcl::CDatabase* m_pSDb;
    SpecTcl::SaveSet*  m_pSaveSet;
public:
    void setUp() {
        // Make the tempfile.
        
        const char* temptemplate="dbspectestXXXXXX";
        char filename[200];
        strcpy(filename, temptemplate);
        int fd = mkstemp(filename);
        if (fd < 0) {
            int e = errno;
            std::stringstream msg;
            msg << "Unable to create tempfile: "
                << filename << " : " << strerror(e);
            throw std::logic_error(msg.str());
        }
        close(fd);
        m_filename = filename;
        
        // Open it for sqlite3 and create the schema:
        
        m_pDb = new CSqlite(filename);
        SpecTcl::CDatabase::create(filename);
        
        // Create a database and a save set:
        
        m_pSDb = new SpecTcl::CDatabase(filename);
        m_pSaveSet = m_pSDb->createSaveSet("test-save-set");
    }
    void tearDown() {
        delete m_pSaveSet;
        delete m_pSDb;
        delete  m_pDb;
        unlink(m_filename.c_str());
    }
    // tests:
    CPPUNIT_TEST_SUITE(dbspectest);
    CPPUNIT_TEST(exists_1);
    CPPUNIT_TEST(exists_2);
    CPPUNIT_TEST(exists_3);
    
    CPPUNIT_TEST(fetchpar_1);    // note private method.
    CPPUNIT_TEST(fetchpar_2);
    CPPUNIT_TEST(fetchpar_3);
    CPPUNIT_TEST(fetchpar_4);
    
    CPPUNIT_TEST(valdtype_1);
    CPPUNIT_TEST(valdtype_2);
    CPPUNIT_TEST(valdtype_3);
    CPPUNIT_TEST(valdtype_4);
    
    CPPUNIT_TEST(valsptype_1);
    CPPUNIT_TEST(valsptype_2);
    
    CPPUNIT_TEST(valaxiscount_1);
    CPPUNIT_TEST(valaxiscount_2);
    
    CPPUNIT_TEST_SUITE_END();
protected:
    void exists_1();
    void exists_2();
    void exists_3();
    
    void fetchpar_1();
    void fetchpar_2();
    void fetchpar_3();
    void fetchpar_4();
    
    void valdtype_1();
    void valdtype_2();
    void valdtype_3();
    void valdtype_4();
    
    void valsptype_1();
    void valsptype_2();
    
    void valaxiscount_1();
    void valaxiscount_2();

private:
    void addDummySpectrum(
        const char* name, const char* type, const char* dtype
    );
};

void
dbspectest::addDummySpectrum(
    const char* name, const char* type, const char* dtype
)
{
    CSqliteStatement s(
        *m_pDb,
        "INSERT INTO spectrum_defs (save_id, name, type, datatype) \
            VALUES (?,?,?,?)"
    );
    s.bind(1, m_pSaveSet->getInfo().s_id);
    s.bind(2, name, -1, SQLITE_STATIC);
    s.bind(3, type, -1, SQLITE_STATIC);
    s.bind(4, dtype, -1, SQLITE_STATIC);
    ++s;
}

CPPUNIT_TEST_SUITE_REGISTRATION(dbspectest);

void dbspectest::exists_1()
{
    // Nonexistent spectrum returns false for no such saveset.
    
    EQ(false, SpecTcl::DBSpectrum::exists(*m_pDb, 1, "nope"));
}
void dbspectest::exists_2()
{
    // False even if the save set exists.
    
    EQ(
        false,
        SpecTcl::DBSpectrum::exists(*m_pDb, m_pSaveSet->getInfo().s_id, "nope")
    );
}
void dbspectest::exists_3()
{
    // Add a dummy spectrum (incomplete) .. should show existence.
    
    addDummySpectrum("test", "1", "long");
    EQ(
        true,
        SpecTcl::DBSpectrum::exists(*m_pDb, m_pSaveSet->getInfo().s_id, "test")
    );
}

void dbspectest::fetchpar_1()
{
    // Fetching a parameter when there are no parameters fails:

    std::vector<const char*> pnames = {
        "p1"
    };
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBSpectrum::fetchParameters(
            *m_pDb, m_pSaveSet->getInfo().s_id, pnames
        ),
        std::invalid_argument
    );
}
void dbspectest::fetchpar_2()
{
    // One missing parameter in the group still makes a failure:
    // make some parameters:
    
    delete m_pSaveSet->createParameter("p1", 1);
    delete m_pSaveSet->createParameter("p2", 2);
    
    std::vector<const char*> pnames = {"p1", "p2", "p3"};
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBSpectrum::fetchParameters(
            *m_pDb, m_pSaveSet->getInfo().s_id, pnames
        ),
        std::invalid_argument
    );
}
void dbspectest::fetchpar_3()
{
    // all ok if all parameters exist.
    
    delete m_pSaveSet->createParameter("p1", 1);
    delete m_pSaveSet->createParameter("p2", 2);
    delete m_pSaveSet->createParameter("p3", 3);
    
    std::vector<const char*> pnames = {"p1", "p2", "p3"};
    CPPUNIT_ASSERT_NO_THROW(
        SpecTcl::DBSpectrum::fetchParameters(
            *m_pDb, m_pSaveSet->getInfo().s_id, pnames
        )
    );
}
void dbspectest::fetchpar_4()
{
    // I get the right answers back too:
    
    SpecTcl::DBParameter* p1 = m_pSaveSet->createParameter("p1", 1);
    SpecTcl::DBParameter* p2 = m_pSaveSet->createParameter("p2", 2);
    SpecTcl::DBParameter* p3 = m_pSaveSet->createParameter("p3", 3);
    
    
    std::vector<const char*> pnames = {"p1", "p2", "p3"};
    
    auto params = SpecTcl::DBSpectrum::fetchParameters(
        *m_pDb, m_pSaveSet->getInfo().s_id, pnames
    );
    EQ(size_t(3), params.size());
    EQ(p1->getInfo().s_id, params[0]);
    EQ(p2->getInfo().s_id, params[1]);
    EQ(p3->getInfo().s_id, params[2]);
    
    delete p1;
    delete p2;
    delete p3;
}
void dbspectest::valdtype_1()
{
    // "byte" is ok
    
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateDataType("byte"));
}
void dbspectest::valdtype_2()
{
    // "word" is ok
    
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateDataType("word"));
}
void dbspectest::valdtype_3()
{
    // "long is ok"
    
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateDataType("long"));
}
void dbspectest::valdtype_4()
{
    // uint32_t is not valid
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBSpectrum::validateDataType("uint32_t"),
        std::invalid_argument
    );
}
void dbspectest::valsptype_1()
{
    // try all the legal ones:
    
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateSpectrumType("1"));
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateSpectrumType("2"));
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateSpectrumType("b"));
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateSpectrumType("s"));
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateSpectrumType("g1"));
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateSpectrumType("S"));
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateSpectrumType("g2"));
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateSpectrumType("gd"));
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateSpectrumType("gs"));
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateSpectrumType("m2"));
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateSpectrumType("2dmproj"));
}
void dbspectest::valsptype_2()
{
    // Illegal spectrum type:
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBSpectrum::validateSpectrumType("2d"),
        std::invalid_argument
    );
}
void dbspectest::valaxiscount_1()
{
    // All good numbers:
    
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateAxisCount("1", 1));
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateAxisCount("2", 2));
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateAxisCount("b", 1));
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateAxisCount("s", 1));
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateAxisCount("g1", 1));
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateAxisCount("S", 1));
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateAxisCount("g2", 2));
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateAxisCount("gd", 2));
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateAxisCount("gs", 1));
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateAxisCount("m2", 2));
    CPPUNIT_ASSERT_NO_THROW(SpecTcl::DBSpectrum::validateAxisCount("2dmproj",1));
}
void dbspectest::valaxiscount_2()
{
    // Invalid:
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBSpectrum::validateAxisCount("1", 2),
            std::invalid_argument
        );
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBSpectrum::validateAxisCount("2", 1),
        std::invalid_argument
    );
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBSpectrum::validateAxisCount("b", 2),
        std::invalid_argument
    );
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBSpectrum::validateAxisCount("s", 2),
        std::invalid_argument
    );
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBSpectrum::validateAxisCount("g1", 2),
        std::invalid_argument
    );
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBSpectrum::validateAxisCount("S", 2),
        std::invalid_argument
    );
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBSpectrum::validateAxisCount("g2", 1),
        std::invalid_argument
    );
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBSpectrum::validateAxisCount("gd", 1),
        std::invalid_argument
    );
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBSpectrum::validateAxisCount("gs", 2),
        std::invalid_argument
    );
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBSpectrum::validateAxisCount("m2", 1),
        std::invalid_argument
    );
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBSpectrum::validateAxisCount("2dmproj",2),
        std::invalid_argument
    );
}
