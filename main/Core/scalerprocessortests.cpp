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
#include <string.h>
#include <tcl.h>
#include <sstream>
#include "Globals.h"
#define private public
#include "CRingBufferDecoder.h"   // need access to current item pointer.
#undef private
#include "RingFormatHelper10.h"
#include "RingFormatHelper11.h"
#include "RingFormatHelper12.h"
#include "BufferTranslator.h"
#include <memory>

/** @todo we really need to expand the tests for all formats :-(
 */

#include "DataFormat12.h"    


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
#define private public
    CRingBufferDecoder* m_pDecoder;   // Need to set m_pCurrentRingItem
#undef private
    CRingFormatHelper10* m_pHelper10;
    CRingFormatHelper11* m_pHelper11;
    CRingFormatHelper12* m_pHelper12;
    BufferTranslator*    m_pTranslator;
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
        
        m_pDecoder = new CRingBufferDecoder;
        m_pTranslator = new NonSwappingBufferTranslator;
        m_pDecoder->m_pTranslator = m_pTranslator;
        gpBufferDecoder = m_pDecoder;
        
        m_pHelper10     = new CRingFormatHelper10;
        m_pHelper11     = new CRingFormatHelper11;
        m_pHelper12     = new CRingFormatHelper12;
    }
    void tearDown() {
        delete m_pProcessor;
        delete m_pInterp;
        delete m_pHelper10;
        delete m_pHelper11;
        delete m_pHelper12;
        gpBufferDecoder = nullptr;
        m_pDecoder->setFormatHelper(nullptr);
        delete m_pDecoder;
        delete m_pTranslator;
    }
protected:
    void statechange_1();
    void statechange_2();
    void statechange_3();
    void statechange_4();
    
    void scaler_1();
    void scaler_2();
    void scaler_3();
private:
    NSCLDAQ12::pStateChangeItem makeStateChange12(
        uint32_t type, unsigned run, uint32_t runOffset, uint32_t divisor,
        const char* title, uint32_t sid, uint32_t osid
    );
    NSCLDAQ12::pScalerItem makeScalerItem12(
            const std::vector<uint32_t>& data, uint32_t start, uint32_t end,
            uint32_t divisor, uint32_t sid, uint32_t osid
    );
};
/////////////////////  Private utilities ////////////////////////////////////

struct free_del {
    void operator()(void* p) {free(p);}
};

/**
 * makeScalerItem12
 *    Returna pointer to a V12 scaler item.  The item has a body header.
 *    See commens about std::unique_ptr in  makeStateChange12 below.
 * @param data - vector of scalers.
 * @param start - start of counting interval.
 * @param end   - end of counting interval.
 * @param divisor - {start,end}/divisor is seconds (floating point).
 * @param sid    - Source Id in body header.
 * @param osid   - Source di in body's s_originalSid field.
 * @return NSCLDA12::pScalerItem - dynamically allocated.
 * @note since new is not so good at making scaler items without knowing
 *        in advance then number of scalers we'll use malloc.  This requires
 *        the caller have a deleter for the unique_ptr that understands this.
 *        one such is supplied above (free_del)
 */
NSCLDAQ12::pScalerItem
sptest::makeScalerItem12(
    const std::vector<uint32_t>& data, uint32_t start, uint32_t end,
    uint32_t divisor, uint32_t sid, uint32_t osid
)
{
    // how big an item do we need:
    
    size_t totalSize = sizeof(NSCLDAQ12::RingItemHeader) +
        sizeof(NSCLDAQ12::BodyHeader) +
        sizeof(NSCLDAQ12::ScalerItemBody) +
        (data.size()-1)*sizeof(uint32_t);
    NSCLDAQ12::pScalerItem p =
        reinterpret_cast<NSCLDAQ12::pScalerItem>(malloc(totalSize));
    p->s_header.s_size = totalSize;
    p->s_header.s_type = NSCLDAQ12::PERIODIC_SCALERS;
    p->s_body.u_hasBodyHeader.s_bodyHeader.s_size = sizeof(NSCLDAQ12::BodyHeader);
    p->s_body.u_hasBodyHeader.s_bodyHeader.s_timestamp = 0;
    p->s_body.u_hasBodyHeader.s_bodyHeader.s_sourceId = sid;
    p->s_body.u_hasBodyHeader.s_bodyHeader.s_barrier =  0;
    p->s_body.u_hasBodyHeader.s_body.s_intervalStartOffset = start;
    p->s_body.u_hasBodyHeader.s_body.s_intervalEndOffset    = end;
    p->s_body.u_hasBodyHeader.s_body.s_intervalDivisor      = divisor;
    p->s_body.u_hasBodyHeader.s_body.s_timestamp = time(nullptr);
    p->s_body.u_hasBodyHeader.s_body.s_scalerCount= data.size();
    p->s_body.u_hasBodyHeader.s_body.s_isIncremental = 1;
    p->s_body.u_hasBodyHeader.s_body.s_originalSid = osid;
    memcpy(
        p->s_body.u_hasBodyHeader.s_body.s_scalers,
        data.data(), data.size()*sizeof(uint32_t)
    );
    
    return p;
}


/**
 * makeStateChange12
 *    Return a pointer to a V12 state change item.  The item has a body header
 *    for testing it's recommended to use this pointer to construct a
 *    std::unique_ptr e.g. so that even if tests fail it will get killed off.
 * @param type - type of state change (e.g. NSCLDAQ12::BEGIN_RUN)
 * @param run - run number.
 * @param runOffset - when in the run the change occured.
 * @param divisor - runOffset/divisor is seconds (floating point).
 * @param title - the run title.
 * @param sid   - Source id ( in body header).
 * @param osid  - original source id (in body's s_originalSid field).
 * @return NSCLDAQ12::pStateChangeItem - pointer to the item (dynamically allocated).
 * @note we assume title can fit in the title field...this is, after all, test code.
 */
NSCLDAQ12::pStateChangeItem
sptest::makeStateChange12(
    uint32_t type, unsigned run, uint32_t runOffset, uint32_t divisor,
    const char* title, uint32_t sid, uint32_t osid
)
{
    NSCLDAQ12::pStateChangeItem p = new NSCLDAQ12::StateChangeItem;
    
    p->s_header.s_size = sizeof(NSCLDAQ12::RingItemHeader) +
        sizeof(NSCLDAQ12::BodyHeader) + sizeof(NSCLDAQ12::StateChangeItemBody);
    p->s_header.s_type = type;
    p->s_body.u_hasBodyHeader.s_bodyHeader.s_size = sizeof(NSCLDAQ12::BodyHeader);
    p->s_body.u_hasBodyHeader.s_bodyHeader.s_timestamp = 0;
    p->s_body.u_hasBodyHeader.s_bodyHeader.s_sourceId  = sid;
    p->s_body.u_hasBodyHeader.s_bodyHeader.s_barrier   = 0;
    
    NSCLDAQ12::pStateChangeItemBody pB = &(p->s_body.u_hasBodyHeader.s_body);
    pB->s_runNumber = run;
    pB->s_timeOffset = runOffset;
    pB->s_Timestamp = time(nullptr);
    pB->s_offsetDivisor = divisor;
    pB->s_originalSid   = osid;
    strcpy(pB->s_title, title);     // See comment note.
    
    return p;
}

CPPUNIT_TEST_SUITE_REGISTRATION(sptest);

//////////////////// Tests /////////////////////////////////////////////////

// Begin run
void sptest::statechange_1()
{
    // need to set up the decoder/helper or else stuff will go very sideways:
    
    m_pDecoder->setFormatHelper(m_pHelper12);     // See todo near top of file.
    std::unique_ptr<NSCLDAQ12::StateChangeItem> pItem(makeStateChange12(
        NSCLDAQ12::BEGIN_RUN, 123, 0, 1, "This is a title", 1, 2
    ));
    m_pDecoder->m_pCurrentRingItem = pItem.get();
    
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
    m_pDecoder->setFormatHelper(m_pHelper12);     // See todo near top of file.
    std::unique_ptr<NSCLDAQ12::StateChangeItem> pItem(makeStateChange12(
        NSCLDAQ12::BEGIN_RUN, 123, 0, 1, "This is a title", 1, 2
    ));
    m_pDecoder->m_pCurrentRingItem = pItem.get();
    
    m_pProcessor->onStateChange(
        CAnalysisBase::Begin, 123, time(nullptr), 0, "This is a title", nullptr
    );
    
    m_pDecoder->setFormatHelper(m_pHelper12);     // See todo near top of file.
    std::unique_ptr<NSCLDAQ12::StateChangeItem> pItem2(makeStateChange12(
        NSCLDAQ12::END_RUN, 123, 120, 1, "This is a title", 1, 2
    ));
    m_pDecoder->m_pCurrentRingItem = pItem2.get();
    
    
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
    m_pDecoder->setFormatHelper(m_pHelper12);     // See todo near top of file.
    std::unique_ptr<NSCLDAQ12::StateChangeItem> pItem(makeStateChange12(
        NSCLDAQ12::PAUSE_RUN, 123, 60, 1, "Some title", 1, 2
    ));
    m_pDecoder->m_pCurrentRingItem = pItem.get();
    
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
    m_pDecoder->setFormatHelper(m_pHelper12);     // See todo near top of file.
    std::unique_ptr<NSCLDAQ12::StateChangeItem> pItem(makeStateChange12(
        NSCLDAQ12::RESUME_RUN, 24, 70, 1, "Last title", 1, 2
    ));
    m_pDecoder->m_pCurrentRingItem = pItem.get();
    
    
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
    
    m_pDecoder->setFormatHelper(m_pHelper12);     // See todo near top of file.
    std::unique_ptr<NSCLDAQ12::ScalerItem, free_del> pItem(
        makeScalerItem12(
            empty, 12, 22, 1, 1,2
        )
    );
    m_pDecoder->m_pCurrentRingItem = pItem.get();
    
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
    
    m_pDecoder->setFormatHelper(m_pHelper12);     // See todo near top of file.
    std::unique_ptr<NSCLDAQ12::ScalerItem, free_del> pItem(
        makeScalerItem12(
            data, 0, 10, 1, 1,2
        )
    );
    m_pDecoder->m_pCurrentRingItem = pItem.get();
    
    
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
    
    m_pDecoder->setFormatHelper(m_pHelper12);     // See todo near top of file.
    std::unique_ptr<NSCLDAQ12::ScalerItem, free_del> pItem(
        makeScalerItem12(
            data, 0, 10, 1, 1,2
        )
    );
    m_pDecoder->m_pCurrentRingItem = pItem.get();
    
    
    m_pProcessor->onScalers(time(nullptr), 0, 10, data, true, nullptr);
    
    std::unique_ptr<NSCLDAQ12::ScalerItem, free_del> pItem2(
        makeScalerItem12(
            data, 10, 20, 1, 1,2
        )
    );
    m_pDecoder->m_pCurrentRingItem = pItem2.get();
    
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
