/**

#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2013.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Author:
#            Ron Fox
#            NSCL
#            Michigan State University
#            East Lansing, MI 48824-1321

##
# @file   CAnalysisEventProcessor.cpp
# @brief  Implement the SpecTcl base analysis framework.
# @author <fox@nscl.msu.edu>
#
  TODO:  There are numerous cases where we have separate code chunks for
         the DAQ version the data comes from.  A helper class hierarchy
         could make the code paths 'straighter' along with a factory for
         producing the right one based on the decoder and, if necessary,
         it's helper.
*/

#include "CAnalysisEventProcessor.h"

#include <NSCLBufferDecoder.h>
#include <CRingBufferDecoder.h>
#include <TCLAnalyzer.h>
#include <RingFormatHelper10.h>
#include <RingFormatHelper11.h>

namespace DAQ11Format {
#include <DataFormat.h>
}


#include <DataFormatPre11.h>


#include <buffer.h>
#include <buftypes.h>


#include <string.h>
#include <stdlib.h>
#include <iostream>

/*---------------------------------------------------------------------------
 *  Dispatch methods:
 */

/**
 * constructor
 *
 *   @param pProcessor - the analysis processor object we are going to be calling.
 *   @param pClientData - Data that is passed in to the analysis processorl
 */
CAnalysisEventProcessor::CAnalysisEventProcessor(
    CAnalysisBase* pProcessor, void* pClientData
) :
    m_pUserCode(pProcessor),
    m_pUserClientData(pClientData),
    m_pCurrentEvent(0),
    m_pDecoder(0),
    m_pAnalyzer(0)
{}
/**
 * destructor
 */
CAnalysisEventProcessor::~CAnalysisEventProcessor() {}

/**
 * operator()
 *    Invoked for a physics event.
 *    -   Marshall stuff into the saved pointers.
 *    -   Marshall the client data.
 *    -   Call the method in a try catch block to deal with exceptions.
 *    -   If the data was a ring item ignore the return data and use the
 *        ring item body size to set the event size,
 *    -   If not, use the return value to set the event size.
 *
 * @param pEvent    - Pointer to the raw data.
 * @param rEvent    - Reference to the event 'array'.
 * @param rAnalyzer - Reference to the analysis driver.
 * @param rDecoder  - Reference to the buffer decoder.
 *
 * @return Bool_t   - kfTrue if successful or kfFalse if not.
 */
Bool_t
CAnalysisEventProcessor::operator()(
    Address_t pEvent, CEvent& rEvent, CAnalyzer& rAnalyzer,
    CBufferDecoder& rDecoder
)
{
    // Marshall the saved pointers and client data:
    
    m_pCurrentEvent = &rEvent;
    m_pDecoder      = &rDecoder;
    m_pAnalyzer     = &rAnalyzer;
    
    ClientData cd = {m_pUserClientData, this};
    
    try {
        // Call the user's method:
        
        unsigned size = m_pUserCode->onEvent(pEvent, &cd);
        CTclAnalyzer* pAnalyzer = dynamic_cast<CTclAnalyzer*>(m_pAnalyzer);
        
        if (dynamic_cast<CNSCLBufferDecoder*>(m_pDecoder)) {
            pAnalyzer->SetEventSize(size);
        } else {
            // Rings might be 11.x or 10.x
            
            pAnalyzer->SetEventSize(computeRingPayloadSize());
        }
        resetPointers();
        return kfTRUE;
        
    }
    catch (CAnalysisBase::NonFatalException& e) {
        // Can continue with next event:
        
        std::cerr << "Error processing event: " << e.what() << std::endl;
        resetPointers();
        return kfFALSE;
        
    }
    catch (CAnalysisBase::FatalException& e) {
        std::cerr << "*Fatal error processing event:*\n";
        std::cerr << e.what() << std::endl;
        exit(-1);
        
    }
    catch(...) {
        resetPointers();
        throw;
    }
    // Can't land here so:
    
    return kfFALSE;
}
/**
 * OnBegin
 *     Called at the beginning of a run. We're really just going to dispatch to
 *     the common state change method passing the correct state change type.
 *
 *   @param rAnalyzer  - References the SpecTcl Analyzer object.
 *   @param rDecoder   - References the SpecTcl buffer decoder object./
 *   @return Bool_t    - See DispatchStateChange
 */
Bool_t
CAnalysisEventProcessor::OnBegin(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)
{
   
    return DispatchStateChange(CAnalysisBase::Begin, rAnalyzer, rDecoder);
}
/**
 * OnEnd
 *    Called at end of run, see OnBegin
 *
 *   @param rAnalyzer  - References the SpecTcl Analyzer object.
 *   @param rDecoder   - References the SpecTcl buffer decoder object./
 *   @return Bool_t    - See DispatchStateChange
 */
Bool_t
CAnalysisEventProcessor::OnEnd(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)
{
    return DispatchStateChange(CAnalysisBase::End, rAnalyzer, rDecoder);
}
/**
 * OnPause
 *   Called when the run pauses, see OnBegin
 *
 *   @param rAnalyzer  - References the SpecTcl Analyzer object.
 *   @param rDecoder   - References the SpecTcl buffer decoder object./
 *   @return Bool_t    - See DispatchStateChange
 */
Bool_t
CAnalysisEventProcessor::OnPause(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)
{
    return DispatchStateChange(CAnalysisBase::Pause, rAnalyzer, rDecoder);
}
/**
 * OnResume
 *   Called when the run resumes, see OnBegin
 *
 *   @param rAnalyzer  - References the SpecTcl Analyzer object.
 *   @param rDecoder   - References the SpecTcl buffer decoder object./
 *   @return Bool_t    - See DispatchStateChange
 */
Bool_t
CAnalysisEventProcessor::OnResume(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)
{
    return DispatchStateChange(CAnalysisBase::Resume, rAnalyzer, rDecoder);
}
/**
 * OnOther
 *    Called for data types not described above.  
 *
 *   @param  type      - type code of the data item.
 *   @param  rAnalyzer - the analyzer object.
 *   @param  rDecoder  - The buffer decoder object.
 *
 *   @return Bool_t    - kfTRUE to continue with the pipe otherwise kfFalse
 *  
 */
Bool_t
CAnalysisEventProcessor::OnOther(
    UInt_t nType, CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder
)
{
    m_pDecoder  = &rDecoder;
    m_pAnalyzer = &rAnalyzer;
    
    // How we dispatch depends on the DAQ version:
    
    Bool_t result;
    if(dynamic_cast<CNSCLBufferDecoder*>(m_pDecoder)) {
        result = DispatchOtherPre10(nType);
    } else {
        result = DispatchOther10Plus(nType);
    }
    
    resetPointers();
    
    return result;
}
/*----------------------------------------------------------------------------
 *  Informational methods(Selectors)
 */

/**
 * getEvent
 *    Returns a pointer to the current event object.  Note that if any thing
 *    out of the call tree from operator() calls this a null pointer is returned
 *    and, as we know, dereferencing that leads to disaster.
 *
 * @return CEvent*
 */
CEvent*
CAnalysisEventProcessor::getEvent()
{
    return m_pCurrentEvent;
}
/**
 * getDecoder
 *    Return a pointer to the current buffer decoder object.  This must
 *    be called in the call tree below our methods or else a null pointer is
 *    returned (the internal pointers are set to null after each method processes)
 *
 * @return CBufferDecoder*
 */
CBufferDecoder*
CAnalysisEventProcessor::getDecoder()
{
    return m_pDecoder;
}
/**
 * getAnalyzer
 *    Returns a pointer to the current analyzer object.  This must
 *    be called in the call tree below our methods or else a null pointer is
 *    returned (the internal pointers are set to null after each method processes)
 *
 * @return CAnalyzer*
 */
CAnalyzer*
CAnalysisEventProcessor::getAnalyzer()
{
    return m_pAnalyzer;
}

/*------------------------------------------------------------------------------
 * Private utility methods.
 */


/**
 * resetPointers
 *    Resets the pointers to SpecTcl objects that are set prior to invoking
 *    a user method.  This ensures that if a method requests an object that is
 *    not valid it will be rewarded with a null pointer.
 */
void
CAnalysisEventProcessor::resetPointers()
{
    m_pCurrentEvent = 0;
    m_pDecoder      = 0;
    m_pAnalyzer     = 0;
}
/**
 * computeRingPayloadSize
 *   Given that the current item is from a ring buffer based system, computes
 *   the size of the payload of the item:
 *   -  For 10.x this is just the ring item body size.
 *   -  For 11.x this is the part after the body header or mbz long.
 *
 *  @return unsigned
 */
unsigned
CAnalysisEventProcessor::computeRingPayloadSize()
{
    // Get the buffer decoder and ask it which version we're dealing with:
    
    CRingBufferDecoder* pDecoder = dynamic_cast<CRingBufferDecoder*>(getDecoder());
    CRingFormatHelper*  pHelper  = pDecoder->getCurrentFormatHelper();
    
    CRingFormatHelper10* p10     = dynamic_cast<CRingFormatHelper10*>(pHelper);
    unsigned size  = pDecoder->getBodySize();    // Already right for 10.x
    if (!p10) {                                  // 11.x
        if (pDecoder->hasBodyHeader()) {
            size -= sizeof(DAQ11Format::BodyHeader);
        } else {
            size -= sizeof(uint32_t);
        }
    }
    return size;
}
/**
 * DispatchStateChange
 *     State change code is common except for the type sent to the user code.
 *     This method extracts the common code for dispatching state change
 *     items.
 *
 * @param type    - The type of state transition being taken.
 * @param rAnalyzer - Refers to the analyzer.
 * @param rDecoder - refers to the buffer decoder.
 * @return Bool_t  - kfTRUE continue with the pipeline esle stop.
 */

Bool_t
CAnalysisEventProcessor::DispatchStateChange(
    CAnalysisBase::StateChangeType type, CAnalyzer& rAnalyzer,
    CBufferDecoder& rDecoder
                                             )
{
    m_pAnalyzer = &rAnalyzer;
    m_pDecoder  = &rDecoder;
    
    int         run    = rDecoder.getRun();
    std::string title  = rDecoder.getTitle();
    time_t      absTime= getStateChangeAbsTime(rDecoder);
    float       runTime= getStateChangeRunTime(rDecoder);
    ClientData  cd     = {m_pUserClientData, this};
    Bool_t result;
    try {
        m_pUserCode->onStateChange(type, run, absTime, runTime, title, &cd);
        result = kfTRUE;
    }
    catch (CAnalysisBase::NonFatalException& e) {
        std::cerr << "Non fatal error processing state change: " << e.what()
            << std::endl;
        result = kfFALSE;
    }
    catch (CAnalysisBase::FatalException& e) {
        std::cerr << "** Fatal error processing state change **\n";
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
    catch (...) {
        resetPointers();
        throw;
    }
    
    
    resetPointers();
    return result;
    
}
/**
 * DispatchOtherPre10
 *    Dispatches to OnOther for a pre10 (buffer based) NSCLDAQ.  Other means:
 *    
 *    - SCALERBF  -> OnScaler
 *    - STATEVARBF -> OnStringLists
 *    - RUNVARBF   -> OnStringLists
 *    - PKTDOCBUF   -> OnStringLists
 *
 * @param nType - Buffer type.
 * @note m_pAnalyzer and m_pDecoder have been setup already.
 * @return Bool_t - kfTRUE on successful processing kfFALSE if the
 *                  pipeline should be aborted.
 */
Bool_t
CAnalysisEventProcessor::DispatchOtherPre10(int nType)
{
    Bool_t result = kfTRUE;
    
    try {
        switch (nType) {
        case SCALERBF: {
            CNSCLBufferDecoder* pDecoder = dynamic_cast<CNSCLBufferDecoder*>(m_pDecoder);
            ClientData cd = {m_pUserClientData, this};
            sclbody* pBody    = reinterpret_cast<sclbody*>(pDecoder->getBody());
            unsigned nScalers = pDecoder->getEntityCount();
            m_pUserCode->onScalers(
                0, pBody->btime, pBody->etime,
                marshallScalers(
                    nScalers,
                    reinterpret_cast<uint32_t*>(pBody->scalers)), true, &cd
            );
            break;
        }
        case STATEVARBF:
            StringBuffer8(CAnalysisBase::MonitoredVariables);
            break;
        case RUNVARBF:
            StringBuffer8(CAnalysisBase::RunVariables);
            break;
        case PKTDOCBF:
            StringBuffer8(CAnalysisBase::PacketTypes);
            break;
        default:
            break;
        }
    }
    catch (CAnalysisBase::NonFatalException& e) {
        std::cerr << "Non fatal error analyzing other data: " << e.what() << std::endl;
        result = kfFALSE;
    }
    catch (CAnalysisBase::FatalException& e) {
        std::cerr << "** Fatal error analyzing other data:\n";
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
    catch (...) {
        resetPointers();                   // In case this is not fatal.
        throw;
    }
    
    resetPointers();
    return result;

}
/**
 * DispatchOther10Plus
 *    Dispatch for OnOther for nscldaq 10.x and later.
 *
 *  @param nType - the ringbuffer type of the item to dispatch:
 *                 - PERIODIC_SCALERS - Dispatch to onScalers
 *                 - PACKET_TYPES - Dispatch to on other with CAnalysis::PacketTypes.
 *                 - MONITORED_VARIABLES - Dispatch to other with
 *                   CAnalysis::MonitoredVariables.
 *  @note m_pAnalyzer and m_pDecoder have been set at this time.  We will
 *        call resetPointers on code paths that could lead to us
 *        being used again.
 *  @return Bool_t kfTRUE - for success kfFALSE to abort the pipeline.
 */
Bool_t
CAnalysisEventProcessor::DispatchOther10Plus(int nType)
{
    Bool_t result = kfTRUE;
    CRingBufferDecoder* pDecoder = dynamic_cast<CRingBufferDecoder*>(m_pDecoder);
    bool is11 =
        dynamic_cast<CRingFormatHelper10*>(pDecoder->getCurrentFormatHelper()) == 0;
    // The ring buffer decoder maps types to 8 types for compatibility
    try {
        switch (nType)  {
        case SCALERBF:  // assumption types are same 10->11
            if(is11) {
                ScalerBuffer11();
            } else {
                ScalerBuffer10();
            }
            break;
        case PKTDOCBF:
            if (is11) {
                StringBuffer11(CAnalysisBase::PacketTypes);
            } else {
                StringBuffer10(CAnalysisBase::PacketTypes);
            }
            break;
        case RUNVARBF:
            if (is11) {
                StringBuffer11(CAnalysisBase::MonitoredVariables);
                
            } else {
                StringBuffer10(CAnalysisBase::MonitoredVariables);
            }
            break;
        default:
            break;
        }
    }
    catch (CAnalysisBase::NonFatalException& e) {
        
    }
    catch (CAnalysisBase::FatalException) {
        
    }
    catch (...) {
        resetPointers();
        throw;
    }
    resetPointers();
    return result;
}
/**
 * getStateChangeAbsTime
 *    Return a time_t representing the absolute time for a state change item.
 *    Note that all versions of NSCLDAQ (8.x, 10, 11) produce this information.
 *
 *  @param rDecoder - References the buffer decoder object.
 *  @return time_t  - system time.
 *  
 */
time_t
CAnalysisEventProcessor::getStateChangeAbsTime(CBufferDecoder& rDecoder)
{
    // first branch is on the type of the buffer decoder:
    
    CNSCLBufferDecoder* pBufDecoder = dynamic_cast<CNSCLBufferDecoder*>(&rDecoder);
    if (pBufDecoder) {
        ctlbody* pBody = reinterpret_cast<ctlbody*>(pBufDecoder->getBody());
        bftime*  pTime = &(pBody->tod);
        struct tm timeStruct = {
            pTime->sec, pTime->min, pTime->hours,
            pTime->day, pTime->month-1, pTime->year - 1900,
            0,0,0
        };
        return mktime(&timeStruct);
    } else {
        // where this all is depends on the 10/11-ness of the stuff:
        
        CRingBufferDecoder* pDecoder = dynamic_cast<CRingBufferDecoder*>(&rDecoder);
        if (dynamic_cast<CRingFormatHelper10*>(pDecoder->getCurrentFormatHelper())) {
            // 10.x
            
            NSCLDAQ10::pStateChangeItem pItem =
                reinterpret_cast<NSCLDAQ10::pStateChangeItem>(pDecoder->getItemPointer());
            return pItem->s_Timestamp;
        
        } else {

            DAQ11Format::pStateChangeItemBody pItem =
                reinterpret_cast<DAQ11Format::pStateChangeItemBody>(pDecoder->getBody());
            return pItem->s_Timestamp;
        }
    }
    // Should not get here:
    
    return 0;
}
/**
 * getStateChangeRunTime
 *    Returns the number of floating point seconds since the start of a run at which
 *    a state change was emitted:
 *    - pre nscldaq 10 - this is just seconds in the body.
 *    - nscldaq10 - This is seconds in the body.
 *    - nscldaq11 - this is floating point seconds/divisor.
 *
 * @param rDecoder - References the decoder object.
 * @return float   - Floating point seconds (for 11, this is not necessarily an
 *                   integer)
 */
float
CAnalysisEventProcessor::getStateChangeRunTime(CBufferDecoder& rDecoder)
{
    CNSCLBufferDecoder* pBufferDecoder =
        dynamic_cast<CNSCLBufferDecoder*>(&rDecoder);
    if(pBufferDecoder) {
        // pre 10
        
        ctlbody* pBody = reinterpret_cast<ctlbody*>(rDecoder.getBody());
        return pBody->sortim;
        
    } else {
        // 10 or 11.
        
        CRingBufferDecoder* pDecoder = dynamic_cast<CRingBufferDecoder*>(&rDecoder);
        if (dynamic_cast<CRingFormatHelper10*>(pDecoder->getCurrentFormatHelper())) {
            // 10
            
            NSCLDAQ10::pStateChangeItem pItem =
                reinterpret_cast<NSCLDAQ10::pStateChangeItem>(pDecoder->getItemPointer());
            return pItem->s_timeOffset;
            
        } else {
            // 11
            
            DAQ11Format::pStateChangeItemBody pItem =
                reinterpret_cast<DAQ11Format::pStateChangeItemBody>(pDecoder->getBody());
            int divisor = pItem->s_offsetDivisor ? pItem->s_offsetDivisor : 1;
            
            return ((double)(pItem->s_timeOffset) )/ divisor;
        }
    }
}

/**
 *  StringBuffer8
 *     Dispatch string list buffers for nscldaq8 and earlier.
 *     Note that the body if of type usrevt and therefore does noth ave
 *     many of the features expected by onStringLists including:
 *     -  absoluteTime - filled in as 0.
 *     -  runTime - filled in as -1.0
 *
 *  @param type - the type of string list item that is presented to the
 *                onStringLists method of our analyzer.
 */
void
CAnalysisEventProcessor::StringBuffer8(CAnalysisBase::StringListType type)
{
    std::vector<std::string> strings;
    CNSCLBufferDecoder* pDecoder = dynamic_cast<CNSCLBufferDecoder*>(m_pDecoder);
    unsigned  nStrings           = pDecoder->getEntityCount();
    
    // Note that in nscldaq8 strings are, if needed padded out to word boundaries.
    // Thus even length strings (remember the null terminator makes them take an
    // odd number of bytes) will be padded with an extra byte.
    
    const char* pStrings = reinterpret_cast<char*>(pDecoder->getBody());
    
    for (int i =0; i < nStrings; i++) {
        strings.push_back(std::string(pStrings));
        int increment = strlen(pStrings);
        if(increment % 2) {
            increment++;
        }
        pStrings += increment + 1;     // +1 the required null terminator.
    }
    // Now that the strings are marshalled we can invoke the callback method:
    
    ClientData cd = {m_pUserClientData, this};
    m_pUserCode->onStringLists(type, 0, -1.0, strings, &cd);
}
/**
 * StringBuffer10
 *   Marshall information about an NSCLDAQ-10.0 string buffer for
 *   onStringsLists
 *
 * @param type - Type of string buffer to feed to the callback method.
 */
void
CAnalysisEventProcessor::StringBuffer10(CAnalysisBase::StringListType type)
{
    CRingBufferDecoder* pDecoder = dynamic_cast<CRingBufferDecoder*>(m_pDecoder);
    NSCLDAQ10::pTextItem pItem =
        reinterpret_cast<NSCLDAQ10::pTextItem>(pDecoder->getItemPointer());
    
    // non string data:
    
    time_t stamp   = pItem->s_timestamp;
    float  offset  = pItem->s_timeOffset;
    int nStrings   = pItem->s_stringCount;
    char* pStrings = pItem->s_strings;
    
    // Perform the callback:
    
    ClientData cd = {m_pUserClientData, this};
    m_pUserCode->onStringLists(
        type, stamp, offset, marshallUnpaddedStrings(nStrings, pStrings), &cd
    );
}
/**
 * StringBuffer11
 *   Marshall information about an NSCLDAQ-11.0 string buffer for onStringLists
 *
 * @param type - Type of string buffer to feed to the callback method.
 */
void
CAnalysisEventProcessor::StringBuffer11(CAnalysisBase::StringListType type)
{
    CRingBufferDecoder* pDecoder = dynamic_cast<CRingBufferDecoder*>(m_pDecoder);
    DAQ11Format::pTextItemBody pItem =
        reinterpret_cast<DAQ11Format::pTextItemBody>(pDecoder->getBody());
    
    // Non string data:
    
    time_t stamp    = pItem->s_timestamp;
    float  offset   = pItem->s_timeOffset;
    if (pItem->s_offsetDivisor > 1) offset = offset / pItem->s_offsetDivisor;
    int nStrings    = pItem->s_stringCount;
    char* pStrings  = pItem->s_strings;
    
    //  Perform the callback:
    
    ClientData cd = {m_pUserClientData, this};
    m_pUserCode->onStringLists(
        type, stamp, offset, marshallUnpaddedStrings(nStrings, pStrings), &cd
    );
    
}
/**
 * ScalerBuffer10
 *    Process a scaler item from an NSCLDAQ-10.x DAQ system into an onScalers
 *    call.
 */
void
CAnalysisEventProcessor::ScalerBuffer10()
{
    CRingBufferDecoder* pDecoder = dynamic_cast<CRingBufferDecoder*>(m_pDecoder);
    NSCLDAQ10::pScalerItem pItem =
        reinterpret_cast<NSCLDAQ10::pScalerItem>(pDecoder->getItemPointer());
    
    // Fish out non scaler info.
    
    time_t stamp   = pItem->s_timestamp;
    float start    = pItem->s_intervalStartOffset;
    float end      = pItem->s_intervalEndOffset;
    int   nScalers = pItem->s_scalerCount;
    uint32_t* pScalers = pItem->s_scalers;
    
    // callback:
    
    ClientData cd = {m_pUserClientData, this};
    m_pUserCode->onScalers(
        stamp, start, end, marshallScalers(nScalers, pScalers), true, &cd
    );    
}
/**
 * ScalerBuffer11
 *  Process a scaler buffer for NSCLDAQ-11 items intoi an onScalers call.
 * 
*/

void
CAnalysisEventProcessor::ScalerBuffer11()
{
    CRingBufferDecoder* pDecoder = dynamic_cast<CRingBufferDecoder*>(m_pDecoder);
    DAQ11Format::pScalerItemBody pItem =
        static_cast<DAQ11Format::pScalerItemBody>(pDecoder->getBody());
    
    // Non scaler info:
    
    time_t stamp = pItem->s_timestamp;
    float  start = pItem->s_intervalStartOffset;
    float  end   = pItem->s_intervalEndOffset;
    int    divisor = pItem->s_intervalDivisor;
    if (divisor > 1) {
        start = start/divisor;
        end   = end/divisor;
    }
    int nScalers     = pItem->s_scalerCount;
    bool incremental = pItem->s_isIncremental != 0;
    uint32_t* pScalers = pItem->s_scalers;
    
    // callback:
    
    ClientData cd = {m_pUserClientData, this};
    m_pUserCode->onScalers(
        stamp, start, end, marshallScalers(nScalers, pScalers), incremental, &cd
    );   
}
/**
 * marshallUnpaddedStrings
 *    Turns a pointer to a bunch of null terminated strings into a vector
 *    of std::string
 *
 *   @param nStrings - number of strings.
 *   @param pStrings - Pointer to the first null terminated string.
 *   @return std::vector<std::string>
 */
std::vector<std::string>
CAnalysisEventProcessor::marshallUnpaddedStrings(int nStrings, const char* pStrings)
{
    std::vector<std::string> result;
    
    for (int i = 0; i < nStrings; i++) {
        result.push_back(std::string(pStrings));
        pStrings += strlen(pStrings) + 1;    // +1 for the null.
    }
    
    return result;
}
/**
 * marshallScalers
 *    Turns a pointer into a bunch of uint32_t's into a vector of same.
 * @param nScalers - number of scalers.
 * @param pScalers - Pointer to the first.
 * @return std::vector<unsigned>
 */
std::vector<unsigned>
CAnalysisEventProcessor::marshallScalers(int nScalers, const uint32_t* pScalers)
{
    std::vector<uint32_t> scalers;
    for (int i = 0; i < nScalers; i++) {
        scalers.push_back(*pScalers++);
    }
    return scalers;
}