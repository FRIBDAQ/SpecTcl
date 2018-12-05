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

/** @file  CPipelineEventprocessor.cpp
 *  @brief implement an event processor using pipelines.
 *  @note exceptions are thrown to abort the analysis pipeline because I _think_
 *        returns from the lambdas just run the next pass of the for_each.
 */
#include "CPipelineEventProcessor.h"
#include <algorithm>

/**
 * constructor
 *    Save the pipeline we'll process.
 *
 * @param pipe - the event processing pipeline we encapsulate. Note that we
 *               dont' take storage ownerhip of the pipe or any of the event
 *               processors in the pipe.
 */
CPipelineEventProcessor::CPipelineEventProcessor(
   CTclAnalyzer::EventProcessingPipeline* pipe 
) :
    m_pPipeline(pipe)
{}

/**
 * destructor - currently empty.
 */
CPipelineEventProcessor::~CPipelineEventProcessor()
{}


/**
 * operator()
 *    Called to process an event.  operator() is called for each
 *    element of the event processing pipeline in m_pPipeline.
 *    If any of them fail (return kfFALSE), we stop processing right
 *    then and there and return kfFalse.
 *
 * @param pEvent - pointer to the raw data.
 * @param rEvent - Output event 'array'.
 * @param rAnalyzer - References the analyzer that's controlling the flow.
 * @param rDecoder - Buffer decoder.
 */
Bool_t
CPipelineEventProcessor::operator()(
    const Address_t pEvent,
    CEvent& rEvent,
    CAnalyzer& rAnalyzer,
    CBufferDecoder& rDecoder
)
{
    try {
        std::for_each(m_pPipeline->begin(), m_pPipeline->end(),
            [pEvent, &rEvent, &rAnalyzer, &rDecoder](std::pair<std::string, CEventProcessor*> ep) {
                if(!(*ep.second)(pEvent, rEvent, rAnalyzer, rDecoder)) {
                    throw 1;                      // return kfFALSE/abort pipe.
                }
            }
            );
    }
    catch (...) {
        return kfFALSE;
    }
    
    return kfTRUE;
}
/**
 * OnAttach
 *    Called when the event processor is attached to the analyzer.
 *    Each pipeline element's OnAttach is called.
 *
 *  @param rAnalyzer - reference to the analyzer.
 *  @return Bool_t - success flag.
 */
Bool_t
CPipelineEventProcessor::OnAttach(CAnalyzer& rAnalyzer)
{
    try {
        std::for_each(m_pPipeline->begin(), m_pPipeline->end(),
            [&rAnalyzer](std::pair<std::string, CEventProcessor*> ep) {
                if (!(ep.second)->OnAttach(rAnalyzer)) {
                    throw 1;
                }
            }
        );
    }
    catch (...) {
        return kfFALSE;
    }
    return kfTRUE;
}
/**
 * OnDetach
 *   Same as above but called when detached from the analyzer.
 *
 *  @param rAnalyzer - reference to the analyzer.
 *  @return Bool_t - success flag.
 */
Bool_t
CPipelineEventProcessor::OnDetach(CAnalyzer& rAnalyzer)
{
    try {
        std::for_each(m_pPipeline->begin(), m_pPipeline->end(),
            [&rAnalyzer](std::pair<std::string, CEventProcessor*> ep) {
                if (!(ep.second)->OnDetach(rAnalyzer)) {
                    throw 1;
                }
            }
        );
    }
    catch (...) {
        return kfFALSE;
    }
    return kfTRUE;
}
/**
 * OnBegin
 *    Invoked on a begin run item (if such exist).
 *    Again just iterate over the pipeline
 *  @param rAnalyzer the analyzer object running the show.
 *  @param rDecoder the buffer decoder.
 *  @return Bool_t success flag.
 */
Bool_t
CPipelineEventProcessor::OnBegin(
    CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder
)
{
    try {
        std::for_each(m_pPipeline->begin(), m_pPipeline->end(),
            [&rAnalyzer, &rDecoder](std::pair<std::string, CEventProcessor*> ep) {
                if (!(ep.second)->OnBegin(rAnalyzer, rDecoder)) {
                    throw 1;
                }
            }
        );
                                 
    }
    catch (...) {
        return kfFALSE;
    }
    return kfTRUE;
}
/**
 * OnEnd
 *    Same as OnBegin but end of run.
 *  @param rAnalyzer the analyzer object running the show.
 *  @param rDecoder the buffer decoder.
 *  @return Bool_t success flag.
 */
Bool_t
CPipelineEventProcessor::OnEnd(
    CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder
)
{
    try {
        std::for_each(m_pPipeline->begin(), m_pPipeline->end(),
            [&rAnalyzer, &rDecoder](std::pair<std::string, CEventProcessor*> ep) {
                if (!(ep.second)->OnEnd(rAnalyzer, rDecoder)) {
                    throw 1;
                }
            }
        );
                                 
    }
    catch (...) {
        return kfFALSE;
    }
    return kfTRUE;
}
/**
 * OnPause
 *    Same as above but for a pause item
 *  @param rAnalyzer the analyzer object running the show.
 *  @param rDecoder the buffer decoder.
 *  @return Bool_t success flag.
 */
Bool_t
CPipelineEventProcessor::OnPause(
    CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder
)
{
    try {
        std::for_each(m_pPipeline->begin(), m_pPipeline->end(),
            [&rAnalyzer, &rDecoder](std::pair<std::string, CEventProcessor*> ep) {
                if (!(ep.second)->OnPause(rAnalyzer, rDecoder)) {
                    throw 1;
                }
            }
        );
                                 
    }
    catch (...) {
        return kfFALSE;
    }
    return kfTRUE;
}
/**
 * OnResume
 *    Same as above but for resume items:
 *  @param rAnalyzer the analyzer object running the show.
 *  @param rDecoder the buffer decoder.
 *  @return Bool_t success flag.
 */
Bool_t
CPipelineEventProcessor::OnResume(
    CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder
)
{
    try {
        std::for_each(m_pPipeline->begin(), m_pPipeline->end(),
            [&rAnalyzer, &rDecoder](std::pair<std::string, CEventProcessor*> ep) {
                if (!(ep.second)->OnResume(rAnalyzer, rDecoder)) {
                    throw 1;
                }
            }
        );
                                 
    }
    catch (...) {
        return kfFALSE;
    }
    return kfTRUE;
}
/**
 * OnOther
 *    Called for an unrecognized item type.
 *
 * @param nType - Type code for the item.
 * @param rAnalyzer the analyzer object running the show.
 * @param rDecoder the buffer decoder.
 * @return Bool_t success flag.
 */
Bool_t
CPipelineEventProcessor::OnOther(
    UInt_t nType, CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder
)
{
    try {
        std::for_each(m_pPipeline->begin(), m_pPipeline->end(),
            [nType, &rAnalyzer, &rDecoder](std::pair<std::string, CEventProcessor*> ep) {
                if (!(ep.second)->OnOther(nType, rAnalyzer, rDecoder)) {
                    throw 1;
                }
            }
        );
                                 
    }
    catch (...) {
        return kfFALSE;
    }
    return kfTRUE;
}
/**
 * OnEventSourceOpen
 *    Called when an event source has been opened.
 *
 * @param name - string that defines the event source.
 */
Bool_t
CPipelineEventProcessor::OnEventSourceOpen(std::string name)
{
    try {
        std::for_each(m_pPipeline->begin(), m_pPipeline->end(),
            [name](std::pair<std::string, CEventProcessor*> ep) {
                if (!ep.second->OnEventSourceOpen(name)) {
                    throw 1;
                }
            }
        );
    }
    catch (...) {
        return kfFALSE;
    }
    return kfTRUE;
}
/**
 * OnEventSourceEOF
 *    Called when an event source hits the EOF (and is therefore closed).
 */
Bool_t
CPipelineEventProcessor::OnEventSourceEOF()
{
    try {
        std::for_each(m_pPipeline->begin(), m_pPipeline->end(),
            [](std::pair<std::string, CEventProcessor*> ep) {
                if (!ep.second->OnEventSourceEOF()) {
                    throw 1;
                }
            }
        );
    }
    catch (...) {
        return kfFALSE;
    }
    return kfTRUE;    
}
/**
 *  OnInitialize - one time initialization.
 */
Bool_t
CPipelineEventProcessor::OnInitialize()
{
    try {
        std::for_each(m_pPipeline->begin(), m_pPipeline->end(),
            [](std::pair<std::string, CEventProcessor*> ep) {
                if (!ep.second->OnInitialize()) {
                    throw 1;
                }
            }
        );
    }
    catch (...) {
        return kfFALSE;
    }
    return kfTRUE;
}