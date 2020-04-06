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
#include <set>

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
    
    CPPUNIT_TEST(valbinfo_1);
    CPPUNIT_TEST(valbinfo_2);
    CPPUNIT_TEST(valbinfo_3);
    CPPUNIT_TEST(valbinfo_4);
    
    CPPUNIT_TEST(enter_1);
    CPPUNIT_TEST(enter_2);
    CPPUNIT_TEST(enter_3);
    CPPUNIT_TEST(enter_4);
    CPPUNIT_TEST(enter_5);
    CPPUNIT_TEST(enter_6);
    
    CPPUNIT_TEST(getpar_1);
    
    CPPUNIT_TEST(construct_1);
    CPPUNIT_TEST(construct_2);
    CPPUNIT_TEST(construct_3);
    CPPUNIT_TEST(construct_4);
    CPPUNIT_TEST(construct_5);
    
    CPPUNIT_TEST(list_1);
    CPPUNIT_TEST(list_2);
    CPPUNIT_TEST(list_3);
    CPPUNIT_TEST(list_4);
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
    
    void valbinfo_1();
    void valbinfo_2();
    void valbinfo_3();
    void valbinfo_4();
    
    void enter_1();
    void enter_2();
    void enter_3();
    void enter_4();
    void enter_5();
    void enter_6();
    
    void getpar_1();
    
    void construct_1();
    void construct_2();
    void construct_3();
    void construct_4();
    void construct_5();
    
    
    void list_1();
    void list_2();
    void list_3();
    void list_4();
private:
    void addDummySpectrum(
        const char* name, const char* type, const char* dtype
    );
    void makeStandardParams();
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
void dbspectest::makeStandardParams()
{
    for (int i =0; i < 10; i++) {
        std::stringstream p;
        p << "param." << i;
        delete m_pSaveSet->createParameter(p.str().c_str(), i);
    }
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

void dbspectest::valbinfo_1()
{
    // The name must not yet exist:
    
    addDummySpectrum("test" , "1", "long");
    SpecTcl::DBSpectrum::BaseInfo info;
    info.s_saveset=m_pSaveSet->getInfo().s_id;
    info.s_name = "test";
    info.s_type = "1";
    info.s_dataType = "long";
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBSpectrum::validateBaseInfo(*m_pDb, info),
        std::invalid_argument
    );
    
}
void dbspectest::valbinfo_2()
{
    SpecTcl::DBSpectrum::BaseInfo info;
    info.s_saveset=m_pSaveSet->getInfo().s_id;
    info.s_name = "test";
    info.s_type = "1d";
    info.s_dataType = "long";
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBSpectrum::validateBaseInfo(*m_pDb, info),
        std::invalid_argument
    );
}
void dbspectest::valbinfo_3()
{
    SpecTcl::DBSpectrum::BaseInfo info;
    info.s_saveset=m_pSaveSet->getInfo().s_id;
    info.s_name = "test";
    info.s_type = "1";
    info.s_dataType = "short";
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBSpectrum::validateBaseInfo(*m_pDb, info),
        std::invalid_argument
    );
}
void dbspectest::valbinfo_4()
{
    SpecTcl::DBSpectrum::BaseInfo info;
    info.s_saveset=m_pSaveSet->getInfo().s_id;
    info.s_name = "test";
    info.s_type = "1";
    info.s_dataType = "word";
    
    CPPUNIT_ASSERT_NO_THROW(
        SpecTcl::DBSpectrum::validateBaseInfo(*m_pDb, info)
    );
}
void dbspectest::enter_1()
{
    // The theory is that all the error checking has been tested.
    // except the save set.
    
    
    makeStandardParams();
    std::vector<const char*> pnames={"param.0"};
    SpecTcl::DBSpectrum::Axes axes = {{-1, -10.0, 10.0, 100}};
    
    CPPUNIT_ASSERT_NO_THROW(
        delete SpecTcl::DBSpectrum::create(
            *m_pDb, m_pSaveSet->getInfo().s_id, "test-spectrum", "1",
            pnames, axes            // default type is long
        )
    );
}
void dbspectest::enter_2()
{
    // make in bad saveset.
    
    makeStandardParams();
    std::vector<const char*> pnames={"param.0"};
    SpecTcl::DBSpectrum::Axes axes = {{-1, -10.0, 10.0, 100}};
    
    CPPUNIT_ASSERT_THROW(
        delete SpecTcl::DBSpectrum::create(
            *m_pDb, m_pSaveSet->getInfo().s_id+1, "test-spectrum", "1",
            pnames, axes            // default type is long
        ),
        std::invalid_argument
    );
}
void dbspectest::enter_3()
{
    // Check the spectrum is made with correct info:
    
    makeStandardParams();
    std::vector<const char*> pnames={"param.0"};
    SpecTcl::DBSpectrum::Axes axes = {{-1, -10.0, 10.0, 100}};
    
    auto pSpec = SpecTcl::DBSpectrum::create(
            *m_pDb, m_pSaveSet->getInfo().s_id, "test-spectrum", "1",
            pnames, axes            // default type is long
    );
    
    
    // Don't check the ids but check everything else:
    
    auto info = pSpec->getInfo();
    SpecTcl::DBParameter param(*m_pDb, m_pSaveSet->getInfo().s_id, "param.0");
    
    // base:
    
    EQ(m_pSaveSet->getInfo().s_id, info.s_base.s_saveset);
    EQ(std::string("test-spectrum"), info.s_base.s_name);
    EQ(std::string("1"), info.s_base.s_type);
    EQ(std::string("long"), info.s_base.s_dataType);
    
    // Parameters
    
    EQ(size_t(1), info.s_parameters.size());
    EQ(param.getInfo().s_id, info.s_parameters[0]);
    
    // axes:
    
    EQ(size_t(1), info.s_axes.size());
    ASSERT(info.s_axes[0].s_id != -1);     // Should have been overwritten!
    EQ(-10.0, info.s_axes[0].s_low);
    EQ(10.0, info.s_axes[0].s_high);
    EQ(100, info.s_axes[0].s_bins);
    
    delete pSpec;
}
void dbspectest::enter_4()
{
    // ensure that:
    // 1.  The root table entry in spectrum_defs is made.
    // 2.  Its id gets propagated into info.s_base.s_id
    
    makeStandardParams();
    std::vector<const char*> pnames={"param.0"};
    SpecTcl::DBSpectrum::Axes axes = {{-1, -10.0, 10.0, 100}};
    
    auto pSpec = SpecTcl::DBSpectrum::create(
            *m_pDb, m_pSaveSet->getInfo().s_id, "test-spectrum", "1",
            pnames, axes            // default type is long
    );
    
    
    // Don't check the ids but check everything else:
    
    auto& info = pSpec->getInfo();
    CSqliteStatement f(
        *m_pDb,
        "SELECT * FROM spectrum_defs WHERE name='test-spectrum' AND save_id=?"
    );
    f.bind(1, m_pSaveSet->getInfo().s_id);
    ++f;
    
    EQ(false, f.atEnd());
    EQ(info.s_base.s_id, f.getInt(0));
    EQ(m_pSaveSet->getInfo().s_id, f.getInt(1));
    EQ(
        std::string("test-spectrum"),
        std::string(reinterpret_cast<const char*>(f.getText(2)))
    );
    EQ(
        std::string("1"),
        std::string(reinterpret_cast<const char*>(f.getText(3)))
    );
    EQ(
        std::string("long"),
        std::string(reinterpret_cast<const char*>(f.getText(4)))
    );
    
    delete pSpec;
    
    ++f;
    EQ(true, f.atEnd());    // There can be only one.
}
void dbspectest::enter_5()
{
    // Check that the correct parameter database entry was made:
    
    makeStandardParams();
    std::vector<const char*> pnames={"param.2"};
    SpecTcl::DBSpectrum::Axes axes = {{-1, -10.0, 10.0, 100}};
    
    auto pSpec = SpecTcl::DBSpectrum::create(
            *m_pDb, m_pSaveSet->getInfo().s_id, "test-spectrum", "1",
            pnames, axes            // default type is long
    );
    auto pParam = m_pSaveSet->findParameter("param.2");
    auto& sinfo   = pSpec->getInfo();
    
    CSqliteStatement f(
        *m_pDb,
        "SELECT pd.id, pd.name FROM spectrum_params AS sp \
        INNER JOIN parameter_defs AS pd ON pd.id = sp.parameter_id \
        WHERE sp.spectrum_id =?"
    );
    f.bind(1, sinfo.s_base.s_id);
    ++f;
    EQ(false, f.atEnd());
    
    EQ(pParam->getInfo().s_id, f.getInt(0));
    EQ(
        std::string("param.2"),
        std::string(reinterpret_cast<const char*>(f.getText(1)))
    );
    
    ++f;
    EQ(true, f.atEnd());
    delete pSpec;
    delete pParam;
}
void dbspectest::enter_6()
{
    // Check the axis is right:
    
    makeStandardParams();
    std::vector<const char*> pnames={"param.2"};
    SpecTcl::DBSpectrum::Axes axes = {{-1, -10.0, 10.0, 100}};
    
    auto pSpec = SpecTcl::DBSpectrum::create(
            *m_pDb, m_pSaveSet->getInfo().s_id, "test-spectrum", "1",
            pnames, axes            // default type is long
    );
    auto& sinfo   = pSpec->getInfo();
    
    CSqliteStatement f(
        *m_pDb,
        "SELECT id, low, high, bins FROM axis_defs WHERE spectrum_id = ?"
    );
    f.bind(1, sinfo.s_base.s_id);
    ++f;
    EQ(false, f.atEnd());
    
    EQ(sinfo.s_axes[0].s_id, f.getInt(0));
    EQ(sinfo.s_axes[0].s_low, f.getDouble(1));
    EQ(sinfo.s_axes[0].s_high, f.getDouble(2));
    EQ(sinfo.s_axes[0].s_bins, f.getInt(3));
    
    ++f;
    EQ(true, f.atEnd());
    delete pSpec;
}
void dbspectest::getpar_1()
{
    // get parameters from a spectrum.
    
        // Check the axis is right:
    
    makeStandardParams();
    std::vector<const char*> pnames={"param.2", "param.1", "param.0"};
    SpecTcl::DBSpectrum::Axes axes = {{-1, -10.0, 10.0, 100}};
    
    auto pSpec = SpecTcl::DBSpectrum::create(
            *m_pDb, m_pSaveSet->getInfo().s_id, "test-spectrum", "s",
            pnames, axes            // default type is long
    );
    
    auto params = pSpec->getParameterNames();
    EQ(pnames.size(), params.size());
    for (int i = 0; i < pnames.size(); i++) {
        EQ(std::string(pnames[i]), params[i]);
    }
    
    delete pSpec;
}

void dbspectest::construct_1()
{
    // NO such spectrum leads to an std::invalid_argument
    
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBSpectrum s(
            *m_pDb, m_pSaveSet->getInfo().s_id, "test_spectrum"
        ),
        std::invalid_argument
    );
}

void dbspectest::construct_2()
{
    // No throw on valid spectrum:
    
    makeStandardParams();
    std::vector<const char*> pnames={"param.2"};
    SpecTcl::DBSpectrum::Axes axes = {{-1, -10.0, 10.0, 100}};
    
    delete  SpecTcl::DBSpectrum::create(
            *m_pDb, m_pSaveSet->getInfo().s_id, "test-spectrum", "1",
            pnames, axes            // default type is long
    );
    CPPUNIT_ASSERT_NO_THROW(
        SpecTcl::DBSpectrum s(
            *m_pDb, m_pSaveSet->getInfo().s_id, "test-spectrum"
        )
    );
}
void dbspectest::construct_3()
{
    // Base part filled in properly.
    
    makeStandardParams();
    std::vector<const char*> pnames={"param.2"};
    SpecTcl::DBSpectrum::Axes axes = {{-1, -10.0, 10.0, 100}};
    
    delete  SpecTcl::DBSpectrum::create(
            *m_pDb, m_pSaveSet->getInfo().s_id, "test-spectrum", "1",
            pnames, axes            // default type is long
    );
    
    SpecTcl::DBSpectrum spec(*m_pDb, m_pSaveSet->getInfo().s_id, "test-spectrum");
    auto& base(spec.getInfo().s_base);
    EQ(1, base.s_id);
    EQ(m_pSaveSet->getInfo().s_id, base.s_saveset);
    EQ(std::string("test-spectrum"), base.s_name);
    EQ(std::string("1"), base.s_type);
    EQ(std::string("long"), base.s_dataType);
    
}
void dbspectest::construct_4()
{
    // parameters filled properly:
    
    makeStandardParams();
    std::vector<const char*> pnames={"param.2"};
    SpecTcl::DBSpectrum::Axes axes = {{-1, -10.0, 10.0, 100}};
    
    delete  SpecTcl::DBSpectrum::create(
            *m_pDb, m_pSaveSet->getInfo().s_id, "test-spectrum", "1",
            pnames, axes            // default type is long
    );
    
    SpecTcl::DBSpectrum spec(*m_pDb, m_pSaveSet->getInfo().s_id, "test-spectrum");
    auto& params(spec.getInfo().s_parameters);
    EQ(size_t(1), params.size());
    auto param = SpecTcl::DBParameter::get(
        *m_pDb, m_pSaveSet->getInfo().s_id, params[0]
    );
    
    std::string name = param->getInfo().s_name;
    EQ(std::string("param.2"), name);

    delete param;
}

void dbspectest::construct_5()
{
    // Axes filled in correctly
    
    makeStandardParams();
    std::vector<const char*> pnames={"param.2"};
    SpecTcl::DBSpectrum::Axes axes = {{-1, -10.0, 10.0, 100}};
    
    delete  SpecTcl::DBSpectrum::create(
            *m_pDb, m_pSaveSet->getInfo().s_id, "test-spectrum", "1",
            pnames, axes            // default type is long
    );
    
    SpecTcl::DBSpectrum spec(*m_pDb, m_pSaveSet->getInfo().s_id, "test-spectrum");
    
    auto& a(spec.getInfo().s_axes);
    EQ(size_t(1), a.size());
    EQ(-10.0, a[0].s_low);
    EQ(10.0, a[0].s_high);
    EQ(100, a[0].s_bins);
}

void dbspectest::list_1()
{
    // Bad save set throws:
    
    CPPUNIT_ASSERT_THROW(
        SpecTcl::DBSpectrum::list(*m_pDb, m_pSaveSet->getInfo().s_id+1),
        std::invalid_argument
    );
}
void dbspectest::list_2()
{
    // initially empty:
    
    EQ(size_t(0), SpecTcl::DBSpectrum::list(*m_pDb, m_pSaveSet->getInfo().s_id).size());
}
void dbspectest::list_3()
{
    // single spectrum.
     // get parameters from a spectrum.
    
        // Check the axis is right:
    
    makeStandardParams();
    std::vector<const char*> pnames={"param.2", "param.1", "param.0"};
    SpecTcl::DBSpectrum::Axes axes = {{-1, -10.0, 10.0, 100}};
    
    auto pSpec = SpecTcl::DBSpectrum::create(
            *m_pDb, m_pSaveSet->getInfo().s_id, "test-spectrum", "s",
            pnames, axes            // default type is long
    );
    delete pSpec;
    
    auto specs = SpecTcl::DBSpectrum::list(*m_pDb, m_pSaveSet->getInfo().s_id);
    EQ(size_t(1), specs.size());
    EQ(std::string("test-spectrum"), specs[0]->getInfo().s_base.s_name);
    
    delete specs[0];
    
}
void dbspectest::list_4()
{
    // Multiple spectra:
    
    makeStandardParams();
    std::vector<const char*> pnames={"param.2", "param.1", "param.0"};
    SpecTcl::DBSpectrum::Axes axes = {{-1, -10.0, 10.0, 100}};
    std::set<std::string> names;  // List has no gaurantees about order.
    
    for (int i =0; i < 10; i++) {
        std::stringstream sname;
        sname << "spectrum-" << i;
        delete SpecTcl::DBSpectrum::create(
            *m_pDb, m_pSaveSet->getInfo().s_id, sname.str().c_str(), "s",
            pnames, axes            // default type is long
        );
        names.insert(sname.str());    
    }
    
    auto spectra = SpecTcl::DBSpectrum::list(*m_pDb, m_pSaveSet->getInfo().s_id);
    EQ(names.size(), spectra.size());
    for (int i =0; i < spectra.size(); i++) {
        EQ(size_t(1), names.count(spectra[i]->getInfo().s_base.s_name));
        delete spectra[i];
    }
}