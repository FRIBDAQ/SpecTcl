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

/** @file:  scalerprocessortests.cpp
 *  @brief: 
 */
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>
#include "Asserts.h"
#include "CScalerProcessor.h"
#include "TCLInterpreter.h"
#include <time.h>
#include <string>
#include <tcl.h>
#include <sstream>
#include

class sptest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(sptest);
    CPPUNIT_TEST(statechange_1);
    CPPUNIT_TEST(statechange_2);
    CPPUNIT_TEST(statechange_3);
    CPPUNIT_TEST(statechange_4);
    
    CPPUNIT_TEST(scaler_1);
    CPPUNIT_TEST(scaler_2);
    CPPUNIT_TEST(scaler_3);
    CPPUNIT_TEST_SUITE_END();
    
private:
    CTCLInterpreter* m_pInterp;
    ScalerProcessor* m_pProcessor;
public:
    void setUp() {
        m_pInterp = new CTCLInterpreter;
        m_pProcessor = new ScalerProcessor(*m_pInterp);
        
        // Define the procs called by the package and has been called flags.
        
        m_pInterp->GlobalEval("set ::begin 0");
        m_pInterp->GlobalEval("proc BeginRun {} {set ::begin 1}");
        
        m_pInterp->GlobalEval("set ::end 0");
        m_pInterp->GlobalEval("proc EndRun {} {set ::end 1}");
        
        m_pInterp->GlobalEval("set ::paused 0");
        m_pInterp->GlobalEval("proc PauseRun {} {set ::paused 1}");
        
        m_pInterp->GlobalEval("set ::resumed 0");
        m_pInterp->GlobalEval("proc ResumeRun {} {set ::resumed 1}");
        
        m_pInterp->GlobalEval("set ::updated 0");
        m_pInterp->GlobalEval("proc Update {} {incr ::updated}");
    }
    void tearDown() {
        delete m_pProcessor;
        delete m_pInterp;
    }
protected:
    void statechange_1();
    void statechange_2();
    void statechange_3();
    void statechange_4();
    
    void scaler_1();
    void scaler_2();
    void scaler_3();
};

CPPUNIT_TEST_SUITE_REGISTRATION(sptest);

// Begin run
void sptest::statechange_1()
{
    m_pProcessor->onStateChange(
        CAnalysisBase::Begin, 123, time(nullptr), 0, "This is a title", nullptr
    );
    // Run number, elasped time run number and the run state should be correct:
    
    std::string value;
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "RunNumber", TCL_GLOBAL_ONLY);
    EQ(std::string("123"), value);
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "ElapsedRunTime", TCL_GLOBAL_ONLY);
    EQ(std::string("0"), value);
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "RunTitle", TCL_GLOBAL_ONLY);
    EQ(std::string("This is a title"), value);
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "ScalerRunState", TCL_GLOBAL_ONLY);
    EQ(std::string("Active"), value);
    
    // The begin runshould have been called
    
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "begin", TCL_GLOBAL_ONLY);
    EQ(std::string("1"), value);
}
// endrun
void sptest::statechange_2()
{
    m_pProcessor->onStateChange(
        CAnalysisBase::End, 666, time(nullptr), 120, "A title", nullptr
    );
    
    std::string value;
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "RunNumber", TCL_GLOBAL_ONLY);
    EQ(std::string("666"), value);
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "ElapsedRunTime", TCL_GLOBAL_ONLY);
    EQ(std::string("120"), value);
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "RunTitle", TCL_GLOBAL_ONLY);
    EQ(std::string("A title"), value);
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "ScalerRunState", TCL_GLOBAL_ONLY);
    EQ(std::string("Halted"), value);
    
    // EndRunshoulid have been called.
    
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "end", TCL_GLOBAL_ONLY);
    EQ(std::string("1"), value);
    
    
}
// Pauserun.
void sptest::statechange_3()
{
    m_pProcessor->onStateChange(
        CAnalysisBase::Pause, 42, time(nullptr), 60, "Some Title", nullptr
    );
    
    std::string value;
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "RunNumber", TCL_GLOBAL_ONLY);
    EQ(std::string("42"), value);
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "ElapsedRunTime", TCL_GLOBAL_ONLY);
    EQ(std::string("60"), value);
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "RunTitle", TCL_GLOBAL_ONLY);
    EQ(std::string("Some Title"), value);
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "ScalerRunState", TCL_GLOBAL_ONLY);
    EQ(std::string("Paused"), value);
    
    // EndRunshoulid have been called.
    
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "paused", TCL_GLOBAL_ONLY);
    EQ(std::string("1"), value);
    
}
void sptest::statechange_4()
{
    m_pProcessor->onStateChange(
        CAnalysisBase::Resume, 24, time(nullptr), 70, "Last Title", nullptr
    );
    
    std::string value;
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "RunNumber", TCL_GLOBAL_ONLY);
    EQ(std::string("24"), value);
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "ElapsedRunTime", TCL_GLOBAL_ONLY);
    EQ(std::string("70"), value);
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "RunTitle", TCL_GLOBAL_ONLY);
    EQ(std::string("Last Title"), value);
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "ScalerRunState", TCL_GLOBAL_ONLY);
    EQ(std::string("Active"), value);
    
    // EndRunshoulid have been called.
    
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "resumed", TCL_GLOBAL_ONLY);
    EQ(std::string("1"), value);
}

// single scaler with no data.

void sptest::scaler_1()
{
    std::vector<unsigned> empty;
    m_pProcessor->onScalers(time(nullptr), 12, 22, empty, true, nullptr);
    
    std::string value;
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "ElapsedRunTime",TCL_GLOBAL_ONLY);
    EQ(std::string("22"), value);
    
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "ScalerDeltaTime",TCL_GLOBAL_ONLY);
    EQ(std::string("10"), value);
    
    // Update was called once:
    
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "updated", TCL_GLOBAL_ONLY);
    EQ(std::string("1"), value);
}
// Test 1 scaler item with data:
void sptest::scaler_2()
{
    std::vector<unsigned> data = {1, 2, 3, 4, 5, 6};
    m_pProcessor->onScalers(time(nullptr), 0, 10, data, true, nullptr);
    
    std::string value;
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "ElapsedRunTime",TCL_GLOBAL_ONLY);
    EQ(std::string("10"), value);
    
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "ScalerDeltaTime",TCL_GLOBAL_ONLY);
    EQ(std::string("10"), value);
    
    // Update was called once:
    
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "updated", TCL_GLOBAL_ONLY);
    EQ(std::string("1"), value);
    
    for (int i =0; i < 6; i ++) {
        std::stringstream iname;
        std::stringstream tname;
        std::stringstream shouldbe;
        
        iname << "Scaler_Increments(" << i << ")";
        tname << "Scaler_Totals(" << i << ")";
        
        shouldbe << i+1;
        
        value = Tcl_GetVar(
            m_pInterp->getInterpreter(), iname.str().c_str(),  TCL_GLOBAL_ONLY
        );
        EQ(shouldbe.str(), value);
        
        value = Tcl_GetVar(
            m_pInterp->getInterpreter(), tname.str().c_str(), TCL_GLOBAL_ONLY
        );
        EQ(shouldbe.str(), value);
    }
    
}
// test two calls to scalers should sum the scalers:

void sptest::scaler_3()
{
    std::vector<unsigned> data = {1, 2, 3, 4, 5, 6};
    m_pProcessor->onScalers(time(nullptr), 0, 10, data, true, nullptr);
    m_pProcessor->onScalers(time(nullptr), 10, 20, data, true, nullptr);
    std::string value;
    value = Tcl_GetVar(m_pInterp->getInterpreter(), "updated", TCL_GLOBAL_ONLY);
    EQ(std::string("2"), value);
    
    for (int i =0; i < 6; i++) {
        std::stringstream iname;
        std::stringstream tname;
        std::stringstream ishouldbe;
        std::stringstream tshouldbe;
        iname << "Scaler_Increments(" << i << ")";
        tname << "Scaler_Totals(" << i << ")";
        
        ishouldbe << i+1;
        tshouldbe << 2*(i+1);
        
        value = Tcl_GetVar(
            m_pInterp->getInterpreter(), iname.str().c_str(),  TCL_GLOBAL_ONLY
        );
        EQ(ishouldbe.str(), value);
        
        value = Tcl_GetVar(
            m_pInterp->getInterpreter(), tname.str().c_str(), TCL_GLOBAL_ONLY
        );
        EQ(tshouldbe.str(), value);
    }
}
