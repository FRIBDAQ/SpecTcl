/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txta
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
#include "TCLInterpreter.h"
#include "TCLObject.h"
#include "TCLException.h"
#include "CMirrorCommand.h"
#include "MirrorDirectory.h"
#include <tcl.h>
#include <string>


class mcmdtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(mcmdtest);
    CPPUNIT_TEST(needsub);
    CPPUNIT_TEST(badsub);
    CPPUNIT_TEST(listall_1);
    CPPUNIT_TEST(listall_2);
    CPPUNIT_TEST(listall_3);
    CPPUNIT_TEST(partial_1);
    CPPUNIT_TEST_SUITE_END();
    
private:
    CTCLInterpreter* m_pInterp;
    CMirrorCommand*  m_pCommand;
public:
    void setUp() {
        m_pInterp = new CTCLInterpreter;
        m_pCommand = new CMirrorCommand(*m_pInterp);
    }
    void tearDown() {
        delete m_pCommand;
        delete m_pInterp;
        
        // Empty the mirror directory singleton:
        
        MirrorDirectorySingleton* p = MirrorDirectorySingleton::getInstance();
        auto info = p->list();
        for (auto item : info) {
            p->remove(item.first);
        }
    }
protected:
    void needsub();
    void badsub();
    
    void listall_1();
    void listall_2();
    void listall_3();
    
    void partial_1();    
};

CPPUNIT_TEST_SUITE_REGISTRATION(mcmdtest);

// Mirror Command exists but we need a subcommand.
void mcmdtest::needsub()
{
    // Check for command existence:
    
    Tcl_CmdInfo info;
    EQ(1, Tcl_GetCommandInfo(m_pInterp->getInterpreter(), "mirror", &info));
    
    // We need a subcommand else CTCLException is thrown:
    
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("mirror"),
        CTCLException
    );
}
// Bad subcommand throws too:

void mcmdtest::badsub()
{
    CPPUNIT_ASSERT_THROW(
        m_pInterp->GlobalEval("mirror nos-such-subcommand"),
        CTCLException
    );
}
// Empty list:

void mcmdtest::listall_1()
{
    std::string result("junk");
    CPPUNIT_ASSERT_NO_THROW(result = m_pInterp->GlobalEval("mirror list"));
    EQ(std::string(""), result);
}
// list all after we put one key in the directory:

void mcmdtest::listall_2()
{
    auto pDict = MirrorDirectorySingleton::getInstance();
    pDict->put("genesis", "ABCD");
    
    std::string info = m_pInterp->GlobalEval("mirror list");
    
    // Start it out as a string then  shimmer it into a dict. for the tests:
    
    CTCLObject list;
    list.Bind(*m_pInterp);
    list = info;
    
    // This is a list with only one element.
    
    EQ(1, list.llength());
    
    // THe only item is a dict with two keys, host and shmkey that have values
    // genesis and "ABCD" respectively.
    
    CTCLObject item; item.Bind(*m_pInterp);
    item = list.lindex(0);
    
    // Dicts look like lists of the form key value key value... so:
    
    EQ(4, item.llength());
    
    CTCLObject hostkey = item.lindex(0);
    hostkey.Bind(*m_pInterp);
    CTCLObject hostvalue = item.lindex(1);
    hostvalue.Bind(*m_pInterp);
    
    EQ(std::string("host"), std::string(hostkey));
    EQ(std::string("genesis"), std::string(hostvalue));
    
    CTCLObject shmkey = item.lindex(2);
    CTCLObject shmval = item.lindex(3);
    shmkey.Bind(*m_pInterp);
    shmval.Bind(*m_pInterp);
    
    EQ(std::string("shmkey"), std::string(shmkey));
    EQ(std::string("ABCD"), std::string(shmval));
}
// list all from several. Note that we assume that the
// listing will sort out in alpha order by host due to the fact the
// (white box here) underlying directory is a map<host, key>).
//
void
mcmdtest::listall_3()
{
    auto pDict = MirrorDirectorySingleton::getInstance();
    pDict->put("genesis", "ABCD");
    pDict->put("anspdaq", "EFGH");
    pDict->put("daqdev_deb10", "IJKL");
    pDict->put("spdaq05", "MNOP");
    
    CTCLObject result;
    result.Bind(*m_pInterp);
    result = m_pInterp->GlobalEval("mirror list");
    
    EQ(4, result.llength());
    
    // So we can loop:
    
    std::vector<std::string> hosts = {"anspdaq", "daqdev_deb10", "genesis", "spdaq05"};
    std::vector<std::string> keys =
        {"EFGH", "IJKL", "ABCD", "MNOP"};
        
    // We're going to assume the dict keys are all ok
    
    for (int i =0; i < result.llength(); i++) {
        CTCLObject item; item.Bind(*m_pInterp); item = result.lindex(i);
        EQ(4, item.llength());
        CTCLObject host; host.Bind(*m_pInterp); host = item.lindex(1);
        CTCLObject key;  key.Bind(*m_pInterp); key = item.lindex(3);
        
        EQ(hosts[i], std::string(host));
        EQ(keys[i], std::string(key));
    }    
}
// Partial listing using host pattern:
void mcmdtest::partial_1()
{
    auto pDict = MirrorDirectorySingleton::getInstance();
    pDict->put("genesis", "ABCD");
    pDict->put("anspdaq", "EFGH");
    pDict->put("daqdev_deb10", "IJKL");
    pDict->put("spdaq05", "MNOP");
    
    CTCLObject result;
    result.Bind(*m_pInterp);
    result = m_pInterp->GlobalEval("mirror list g*");
    
    EQ(1, result.llength());
    CTCLObject item;
    item.Bind(*m_pInterp);
    item = result.lindex(0);
    
    CTCLObject host; host.Bind(*m_pInterp); host = item.lindex(1);
    CTCLObject key ; key.Bind(*m_pInterp) ; key = item.lindex(3);
    
    EQ(std::string("genesis"), std::string(host));
    EQ(std::string("ABCD"), std::string(key));
}
