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
# @file   CAnalysisEventProcessor.h
# @brief  Event processor definition to drive CAnalysisBase derived objects.
# @author <fox@nscl.msu.edu>
*/
#ifndef CANALYSISEVENTPROCESSOR_H
#define CANALYSISEVENTPROCESSOR_H

#include <EventProcessor.h>
#include "CAnalysisBase.h"
#include <histotypes.h>

#include <stdint.h>
#include <vector>
#include <string>

/**
 * @class CAnalysisEventProcessor
 *    This class is an event processor that can be added to the SpecTcl event
 *    processor pipeline.  It jackets a CAnalysisBase (derived) object which
 *    performs ad-hoc analysis that may or may not interface with SpecTcl.
 *    The class is smart enough to be able to invoke the methods in its
 *    encapsulated object passing the correct parameters for the following
 *    NSCLDAQ data versions:
 *   -  Pre 10.x  (buffered data).
 *   -  10.x      (ring buffers with no body header provision)
 *   -  11.x+     (ring buffers with body header provision).
 *
 *  @note the client data passed to the encapsulated object includes client data
 *        specified by our constructor as well as a pointer to this so that the
 *        analysis object can call back into our informational methods.
 */
class CAnalysisEventProcessor : public CEventProcessor
{
    // Private data:
private:
    CAnalysisBase*   m_pUserCode;
    void*            m_pUserClientData;
    
    CEvent*          m_pCurrentEvent;
    CBufferDecoder*  m_pDecoder;
    CAnalyzer*       m_pAnalyzer;
    
    // Public data types:
public:
    typedef struct _ClientData {
        void*                      s_pUserData;
        CAnalysisEventProcessor*   s_pCaller;
    } ClientData, *pClientData;
    
    // Construction and other canonicals
public:
    CAnalysisEventProcessor(CAnalysisBase* pProcessor, void* pClientData = 0);
    virtual ~CAnalysisEventProcessor();
    
    // The interface
    
    virtual Bool_t operator()(
        const Address_t pEvent, CEvent& rEvent, CAnalyzer& rAnalyzer,
        CBufferDecoder& rDecoder
    );
    virtual Bool_t OnBegin(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder);
    virtual Bool_t OnEnd(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder);
    virtual Bool_t OnPause(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder);
    virtual Bool_t OnResume(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder);
    virtual Bool_t OnOther(
        UInt_t nType, CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder
    );
    
    
    
    // Queries available to the user code:
    
public:

    CEvent*         getEvent();
    CBufferDecoder* getDecoder();
    CAnalyzer*      getAnalyzer();
    
    // Utilities:
    
private:
    void resetPointers();
    unsigned computeRingPayloadSize();
    Bool_t   DispatchStateChange(
        CAnalysisBase::StateChangeType type,
        CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder
    );
    Bool_t DispatchOtherPre10(int nType);
    Bool_t DispatchOther10Plus(int nType);
    time_t getStateChangeAbsTime(CBufferDecoder& rDecoder);
    float getStateChangeRunTime(CBufferDecoder& rDecoder);
    void   StringBuffer8(CAnalysisBase::StringListType type);
    void   StringBuffer10(CAnalysisBase::StringListType type);
    void   StringBuffer11(CAnalysisBase::StringListType type);
    
    void   ScalerBuffer10();
    void   ScalerBuffer11();
    
    std::vector<std::string> marshallUnpaddedStrings(int nStrings, const char* pStrings);
    std::vector<unsigned>    marshallScalers(int nScalers,  const uint32_t* pScalers);
    
    
};

#endif

