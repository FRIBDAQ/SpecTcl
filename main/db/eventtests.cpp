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

/** @file:  
 *  @brief: 
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "SpecTclDatabase.h"
#define private public
#include "SaveSet.h"
#undef private
#include "CSqlite.h"
#include "CSqliteStatement.h"

#include <errno.h>
#include <string>
#include <string.h>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include <stdexcept>
#include <time.h>
#include <stdint.h>

class eventtest : public CppUnit::TestFixture {
    
private:
    std::string        m_filename;
    CSqlite*           m_pConn;
    SpecTcl::CDatabase* m_pDb;
    SpecTcl::SaveSet*  m_pSaveSet;
public:
    void setUp() {
        const char* fnameTemplate="eventtestdbXXXXXX";
        char fname[100];
        strcpy(fname, fnameTemplate);
        int fd = mkstemp(fname);
        if (fd < 0) {
            int e = errno;
            std::stringstream msg;
            msg << " Unable to make temp file" << fname
                << " : " << strerror(e);
            throw std::logic_error(msg.str());
        }
        close(fd);
        
        m_filename = fname;
        m_pConn = new CSqlite(fname);
        SpecTcl::CDatabase::create(fname);
        m_pDb = new SpecTcl::CDatabase(fname);
        m_pSaveSet = m_pDb->createSaveSet("save");
    }
    void tearDown() {
        delete m_pSaveSet;
        delete m_pDb;
        delete m_pConn;
        unlink(m_filename.c_str());
    }
private:
    CPPUNIT_TEST_SUITE(eventtest);
    CPPUNIT_TEST(start_1);
    
    CPPUNIT_TEST(end_1);
    
    CPPUNIT_TEST(scaler_1);
    CPPUNIT_TEST(scaler_2);
    
    CPPUNIT_TEST(event_1);
    CPPUNIT_TEST(event_2);
    CPPUNIT_TEST(event_3);
    CPPUNIT_TEST_SUITE_END();

protected:
    void start_1();
    
    void end_1();
    
    void scaler_1();
    void scaler_2();
    
    void event_1();
    void event_2();
    void event_3();
};

CPPUNIT_TEST_SUITE_REGISTRATION(eventtest);

void eventtest::start_1()
{
    // create and check the insertion.
    
    time_t stamp = time(nullptr);
    int id = m_pSaveSet->startRun(12, "This is a run", stamp);
    EQ(1, id);
    CSqliteStatement fet(
        *m_pConn,
        "SELECT config_id, run_number, title, start_time, stop_time\
            FROM runs WHERE id = ?"
    );
    fet.bind(1, id);
    ++fet;
    EQ(false, fet.atEnd());
    
    EQ(m_pSaveSet->getInfo().s_id, fet.getInt(0));
    EQ(12, fet.getInt(1));
    std::string title  = reinterpret_cast<const char*>(fet.getText(2));
    EQ(std::string("This is a run"), title);
    EQ(int(stamp), fet.getInt(3));
    EQ(CSqliteStatement::null, fet.columnType(4));
    
    ++fet;
    EQ(true, fet.atEnd());
}
void eventtest::end_1()
{
    // Create a start record:
    
    time_t stamp = time(nullptr);
    int id = m_pSaveSet->startRun(12, "This is a run", stamp);
    
    // Now an end record 10 seconds later:
    
    
    m_pSaveSet->endRun(id, stamp+10);
    
    // Check:
    
    CSqliteStatement fet(
        *m_pConn,
        "SELECT stop_time FROM runs WHERE id = ?"
    );
    fet.bind(1, id);
    
    ++fet;
    EQ(false, fet.atEnd());
    
    EQ(int(stamp+10), fet.getInt(0));
    
    ++fet;
    EQ(true, fet.atEnd());
}

void eventtest::scaler_1()
{
    // Check root record.
    
    time_t stamp = time(nullptr);
    int id = m_pSaveSet->startRun(33, "This is a test run for scalers", stamp);
    
    uint32_t* empty=nullptr;
    
    m_pSaveSet->saveScalers(id, 10, 0, 10, 1, stamp+10, 0, empty);
    
    CSqliteStatement fet(
        *m_pConn,
        "SELECT source_id, start_offset, stop_offset, divisor, clock_time \
            FROM scaler_readouts WHERE run_id = ?"
    );
    fet.bind(1, id);
    ++fet;
    EQ(false, fet.atEnd());
    
    EQ(10, fet.getInt(0));
    EQ(0,  fet.getInt(1));
    EQ(10, fet.getInt(2));
    EQ(1, fet.getInt(3));
    EQ(int(stamp+10), fet.getInt(4));
    
    ++fet;
    EQ(true, fet.atEnd());
}

void eventtest::scaler_2()
{
    // check scaler records.
    
    time_t stamp = time(nullptr);
    int id = m_pSaveSet->startRun(33, "This is a test run for scalers", stamp);
    
    uint32_t scalers[32];
    for (int i =0; i < 32; i++) {
        scalers[i] = i*100;
    }
    int rdoid = m_pSaveSet->saveScalers(id, 10, 0, 10, 1, stamp+10, 32, scalers);
    
    CSqliteStatement fet(
        *m_pConn,
        "SELECT channel, value FROM scaler_channels \
            WHERE readout_id =? ORDER BY channel"
    );
    fet.bind(1, rdoid);
    for (int i =0; i < 32; i++) {
        ++fet;
        EQ(false, fet.atEnd());
        EQ(i, fet.getInt(0));
        EQ(int(scalers[i]), fet.getInt(1));
    }
    ++fet;
    EQ(true, fet.atEnd());
}
void eventtest::event_1()
{
    // plain old store an event.
    
    time_t stamp = time(nullptr);
    int id = m_pSaveSet->startRun(1, "This is run 1", stamp);
    
    int params[5] = {2, 3, 5, 7, 11}; //  The prime parameters LoL.
    double values[5] = {200, 300, 500, 700, 1100};
    m_pSaveSet->saveEvent(id, 1, 5, params, values);
    
    CSqliteStatement fet(
        *m_pConn,
        "SELECT event_number, parameter_count, event_data FROM events \
        WHERE run_id = ?"
    );
    fet.bind(1, id);
    ++fet;
    EQ(false, fet.atEnd());
    
    EQ(1, fet.getInt(0));
    EQ(5, fet.getInt(1));
    const void* blob = fet.getBlob(2);
    const SpecTcl::SaveSet::EventParameter* p =
        reinterpret_cast<const SpecTcl::SaveSet::EventParameter*>(blob);
    for (int i =0; i < 5; i++) {
        EQ(params[i], p[i].s_number);
        EQ(values[i], p[i].s_value);
    }
    
    ++fet;
    EQ(true, fet.atEnd());
}
void eventtest::event_2()
{
    // store an event within a savepoint.
    
    time_t stamp = time(nullptr);
    int id = m_pSaveSet->startRun(1, "This is run 1", stamp);
    void* sp = m_pSaveSet->startEvents(id);
    
    int params[5] = {2, 3, 5, 7, 11}; //  The prime parameters LoL.
    double values[5] = {200, 300, 500, 700, 1100};
    m_pSaveSet->saveEvent(id, 1, 5, params, values);
    
    m_pSaveSet->endEvents(sp);
    
    // There should be an event record... we trust its contents.
    
    CSqliteStatement c(
        *m_pConn,
        "SELECT COUNT(*) FROM events WHERE run_id=?"
    );
    c.bind(1, id);
    ++c;
    EQ(1, c.getInt(0));
    
}
void eventtest::event_3()
{
    // store an event but rollback savepoint.
    
    time_t stamp = time(nullptr);
    int id = m_pSaveSet->startRun(1, "This is run 1", stamp);
    void* sp = m_pSaveSet->startEvents(id);
    
    int params[5] = {2, 3, 5, 7, 11}; //  The prime parameters LoL.
    double values[5] = {200, 300, 500, 700, 1100};
    m_pSaveSet->saveEvent(id, 1, 5, params, values);
    
    m_pSaveSet->rollbackEvents(sp);
    
    // There should be an event record... we trust its contents.
    
    CSqliteStatement c(
        *m_pConn,
        "SELECT COUNT(*) FROM events WHERE run_id=?"
    );
    c.bind(1, id);
    ++c;
    EQ(0, c.getInt(0));
}