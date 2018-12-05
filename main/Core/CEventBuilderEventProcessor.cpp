/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  CEventBuilderEventProcessor.cpp
 *  @brief: Implement the event builder that unpacks event built data.
 *  
 */

#include <config.h>
#include "CEventBuilderEventProcessor.h"
#include "CTreeParameter.h"
#include "FragmentIndex.h"

#include <algorithm>
#include <sstream>

/*------------------------------------------------------------------------------
 * Implement the canonicals:
 */

/**
 * constructor:
 *   - Set the clock tick to seconds conversion factor.
 *   - Create the fixed set of tree parameters.
 *
 *   @param clockMhz   - Clock frequency in MHz.
 *   @param baseName   - Base part of the name that's used to construct
 *                       parameter names.
 */
CEventBuilderEventProcessor::CEventBuilderEventProcessor(
    double clockMHz, std::string baseName
) :
    m_ClockMHz(clockMHz), m_sourceCount(0), m_unrecognizedSourceCount(0),
    m_eventNumber(0), m_seconds(0), m_baseName(baseName), m_nEvents(0)
{
    // Make the tree parameters.
    
    m_sourceCount             = new CTreeParameter(baseName + ".sources");
    m_unrecognizedSourceCount =
        new CTreeParameter(baseName + ".unrecognized_source");
    m_eventNumber             = new CTreeParameter(baseName + ".event_no");
    m_seconds                 =
        new CTreeParameter(baseName + ".run_time", "seconds");
    
}
/**
 * destructor
 *    Release all dynamic storage.
 */
CEventBuilderEventProcessor::~CEventBuilderEventProcessor()
{
    // Delete the simple stuff first:
    
    delete m_sourceCount;
    delete m_unrecognizedSourceCount;
    delete m_eventNumber;
    delete m_seconds;
    
    
    // Delete tree parameters in the HandlerMap - the event processors
    // are not our problem -- they may be pointers to something static:
    
    std::for_each(
        m_sourceHandlers.begin(), m_sourceHandlers.end(),
        [](std::pair<const unsigned, SourceData>& item) {    // lamda function.
            delete item.second.s_SourcePresent;
        }
    );
    m_sourceHandlers.clear();
    
    // Delete the tree parameters in the matrix of time differences.
    
    std::for_each(
        m_timeDifferenceParams.begin(), m_timeDifferenceParams.end(),
        [](std::pair<const unsigned, TimeDifferences>& item) {
            std::for_each(
                item.second.begin(), item.second.end(),
                [](std::pair<const unsigned, CTreeParameter*>& param) {
                    delete param.second;
                }
            );
            item.second.clear();
        }
    );
    
    m_timeDifferenceParams.clear();

}
/*------------------------------------------------------------------------------
 *  Implement the CEventProcessor interface methods.
 */


/**
 * operator()
 *    Iterate over the fragments:
 *    - Collect the time stamps.
 *    - Invoke each event processor for recognized source ids.
 *    - Set all appropriate parameters.
 *
 * @param pEvent    - points to the event body.
 * @param rEvent    - References the event object we fill in (indirectly?).
 * @param rAnalyzer - References the analyzer that's controlling program flow.
 * @param rDecoder  - References the buffer decoder that's controlling program flow.
 * @return Bool_t   - If any event processor fails, we return kfFALSE immediately
 *                    otherwise kfTrue is returned.
 *                    Since kfFALSE aborts histogramming, we can fill in the
 *                    parameter values optimistically (as we know their values).
 */
Bool_t
CEventBuilderEventProcessor::operator()(
    const Address_t pEvent, CEvent& rEvent, CAnalyzer& rAnalyzer,
    CBufferDecoder& rDecoder
)
{
    *m_eventNumber = ++m_nEvents;
    FragmentIndex frags(reinterpret_cast<uint16_t*>(pEvent));
    
    // Source cout will be the number of fragments -- note an event is allowed
    // to have more than one fragment from the same source id....this
    // would play hob with the registered event processors.
    
    *m_sourceCount = frags.getNumberFragments();
    *m_unrecognizedSourceCount = 0;
    
    std::vector<std::pair<unsigned, uint64_t> > timestamps;  // srcid/timestamp pairs.
    try {
        for (auto pFrag = frags.begin(); pFrag != frags.end(); pFrag++) {
            // Save the timestamp/sourceid pair for the differences:
            
            unsigned sid = pFrag->s_sourceId;
            timestamps.push_back(
                std::pair<unsigned, uint64_t>(sid, pFrag->s_timestamp)
            );
            // Call the event processor if one was registered and indicate
            // the source had data:
            
            SourceData& info = m_sourceHandlers[sid];
            if (info.s_processor) {           // in case we just made an empty.
                (*info.s_SourcePresent) = 1;
                Bool_t ok = (*info.s_processor)(
                    pFrag->s_itembody, rEvent, rAnalyzer, rDecoder
                );
                if (!ok) throw ok;                    // Failure.
            } else {
                *m_unrecognizedSourceCount = *m_unrecognizedSourceCount + 1;
            }
        }
    }
    catch(Bool_t notOk) {
        return notOk;
    }
    
    // If there were fragments, update the seconds, and the time difference
    // counters:
    
    if (timestamps.size()) {
        *m_seconds =
            static_cast<double>(timestamps[0].second) / (1.0E6 * m_ClockMHz);
        computeTimestampDifferences(timestamps);
    }
    
    return kfTRUE;
}
/**
 * OnAttach
 *    Calls the OnAttache of all registered processors.  This means that
 *    processors should be registered before the event processor is registered.
 *    Any registered after that time must arrange for their own OnAttach to be
 *    called if needed.
 *
 * @param rAnalyzer  - References the analyzer.
 * @return Bool_t    - only kfFALSE if an event processor fails.
 */
Bool_t
CEventBuilderEventProcessor::OnAttach(CAnalyzer& rAnalyzer)
{
    try {
        std::for_each(
            m_sourceHandlers.begin(), m_sourceHandlers.end(),
            [&rAnalyzer](std::pair<const unsigned, SourceData>& item) {
                if (item.second.s_processor)  {
                    Bool_t result = item.second.s_processor->OnAttach(rAnalyzer);
                    if (!result) throw result;
                }
            }
        );
    }
    catch (Bool_t ok) {
        return ok;
    }
    return kfTRUE;
}
/**
 * OnDetach
 *   Processes detachment from the analyzer (e.g. pipeline switch).
 *
 * @param rAnalyzer -the analyzer.
 * @return Bool_t - success flag.
 */
Bool_t
CEventBuilderEventProcessor::OnDetach(CAnalyzer& rAnalyzer)
{
    try {
        std::for_each(
            m_sourceHandlers.begin(), m_sourceHandlers.end(),
            [&rAnalyzer](std::pair<const unsigned, SourceData>& item) {
                if (item.second.s_processor)  {
                    Bool_t result = item.second.s_processor->OnDetach(rAnalyzer);
                    if (!result) throw result;
                }
            }
        );
    }
    catch (Bool_t ok) {
        return ok;
    }
    return kfTRUE;
}    

/**
 * OnBegin
 *    Called when a run begins.  We initialze the event number to zero
 *    and call the OnBegin methods of all registered event processors.
 *
 * @param rAnalyzer - references the analyzer controlling the flow of analyzis.
 * @param rDecoder  - references the buffer decoder that declared the begin run.
 * @return Bool_t   - kfFALSE if any event processor fails else kfTRUE
 */
Bool_t
CEventBuilderEventProcessor::OnBegin(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)
{
    m_nEvents = 0;               // Reset event count.
    try {
        std::for_each(
            m_sourceHandlers.begin(), m_sourceHandlers.end(),
            [&rAnalyzer, &rDecoder](std::pair<const unsigned, SourceData>& item) {
                if (item.second.s_processor)  {
                    Bool_t result = item.second.s_processor->OnBegin(rAnalyzer, rDecoder);
                    if (!result) throw result;
                }
            }
        );
    }
    catch(Bool_t ok) {
        return ok;
    }
    return kfTRUE;
}
/**
 * OnEnd
 *    Same as OnBegin but called at th eend of a run.
 */
Bool_t
CEventBuilderEventProcessor::OnEnd(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)
{
    try {
        std::for_each(
            m_sourceHandlers.begin(), m_sourceHandlers.end(),
            [&rAnalyzer, &rDecoder](std::pair<const unsigned, SourceData>& item) {
                if (item.second.s_processor)  {
                    Bool_t result = item.second.s_processor->OnEnd(rAnalyzer, rDecoder);
                    if (!result) throw result;
                }
            }
        );
    } catch(Bool_t ok) {
        return ok;
    }
    return kfTRUE;
}
/**
 * OnPause
 *    Same but for a pause run if that's supported by the underling DAQ.
 */
Bool_t
CEventBuilderEventProcessor::OnPause(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)
{
    try {
        std::for_each(
            m_sourceHandlers.begin(), m_sourceHandlers.end(),
            [&rAnalyzer, &rDecoder](std::pair<const unsigned, SourceData>& item) {
                if (item.second.s_processor)  {
                    Bool_t result = item.second.s_processor->OnPause(rAnalyzer, rDecoder);
                    if (!result) throw result;
                }
            }
        );
    } catch(Bool_t ok) {
        return ok;
    }
    return kfTRUE;
}
/**
 * OnResume
 */
Bool_t
CEventBuilderEventProcessor::OnResume(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)
{
    try {
        std::for_each(
            m_sourceHandlers.begin(), m_sourceHandlers.end(),
            [&rAnalyzer, &rDecoder](std::pair<const unsigned, SourceData>& item) {
                if (item.second.s_processor)  {
                    Bool_t result = item.second.s_processor->OnResume(rAnalyzer, rDecoder);
                    if (!result) throw result;
                }
            }
        );
    } catch(Bool_t ok) {
        return ok;
    }
    return kfTRUE;
}
/**
 * OnOther
 *    Called for non state transition, non physics events
 *
 *  @paarm nType     - The type of item. This is DAQ dependent of course.
 *  @param rAnalyzer - References the analyzer object.
 *  @param rDecoder  - References the decoder object.
 *  @return Bool_t   - kfTRUE unless a processor returns otherwise in which
 *                     case that value is retuurned.
 */
Bool_t
CEventBuilderEventProcessor::OnOther(
    UInt_t nType, CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder
)
{
    try {
        std::for_each(
            m_sourceHandlers.begin(), m_sourceHandlers.end(),
            [nType, &rAnalyzer, &rDecoder](std::pair<const unsigned, SourceData>& item) {
                if (item.second.s_processor)  {
                    Bool_t result = item.second.s_processor->OnOther(
                        nType, rAnalyzer, rDecoder
                    );
                    if (!result) throw result;
                }
            }
        );
    } catch(Bool_t ok) {
        return ok;
    }
    return kfTRUE;    
}
/**
 * OnEventSourceOpen
 *    Called when an event source has been attached by SpecTcl.
 * @param name - name of the event source.  What this is may depend on the
 *               type of event source.
 * @return Bool_t - kfFALSE if an event processor fails.  kfTRUE if allsucceeded.
 * @note SpecTcl does not natively have cluster file support.  That's scripted in.
 *       as a consequence the effect of cluster files on this method is not
 *       well defined.  For example, if the cluster file script does an attach -file
 *       for each file this will be called once per file.  If the cluster file script
 *       does an attach -pipe to a cat of all files in the cluster, this
 *       will only be called once.
 *       Furthermore, the parameter passed will again depend on how the cluster file
 *       script is implemented.
 *
 */
Bool_t
CEventBuilderEventProcessor::OnEventSourceOpen(std::string name)
{
    try {
        std::for_each(
            m_sourceHandlers.begin(), m_sourceHandlers.end(),
            [&name](std::pair<const unsigned, SourceData>& item) {
                if (item.second.s_processor)  {
                    Bool_t result = item.second.s_processor->OnEventSourceOpen(
                        name
                    );
                    if (!result) throw result;
                }
            }
        );
    } catch(Bool_t ok) {
        return ok;
    }
    return kfTRUE;        
}
/**
 * OnEventSourceEOF
 *   Invoked when an end file condition is encountered on an event source.
 *   see OnEventSourceOpen for a note on the meaning of this with respect
 *   to cluster file processing.
 * 
 * @return Bool_t - kfFALSE if an event processor fails.  kfTRUE if allsucceeded.
*/
Bool_t
CEventBuilderEventProcessor::OnEventSourceEOF()
{
    try {
        std::for_each(
            m_sourceHandlers.begin(), m_sourceHandlers.end(),
            [](std::pair<const unsigned, SourceData>& item) {
                if (item.second.s_processor)  {
                    Bool_t result = item.second.s_processor->OnEventSourceEOF();
                    if (!result) throw result;
                }
            }
        );
    } catch(Bool_t ok) {
        return ok;
    }
    return kfTRUE;        
}
/**
 * Called when the event processor is initialized.  This happens after
 * SpecTcl has completed its initialization.  Note that this implies
 * that event processors for event sources should have been registered prior to
 * that fact to make use of this method.
 * 
 * @return Bool_t - kfFALSE if an event processor fails.  kfTRUE if allsucceeded.
*/
Bool_t
CEventBuilderEventProcessor::OnInitialize()
{
    try {
        std::for_each(
            m_sourceHandlers.begin(), m_sourceHandlers.end(),
            [](std::pair<const unsigned, SourceData>& item) {
                if (item.second.s_processor)  {
                    Bool_t result = item.second.s_processor->OnInitialize();
                    if (!result) throw result;
                }
            }
        );
    } catch(Bool_t ok) {
        return ok;
    }
    return kfTRUE;        
}
/*-----------------------------------------------------------------------------
 *  Additional public methods.
 */

 /**
  * addEventProcessor
  *    Registers an event processor with a data source id:
  *    - The processor is entered into the handler map where it will be
  *      called when a fragment from that source is encounterd in an event.
  *    - Tree parameter objects will be created to hold differences between
  *      all other event source timestamps and this one.  These are entered into
  *      the TimeDifference matrix which is sort of like an upper diagonal
  *      2-d array that's indexed by source id pairs.
  *
  * @param sourceId  - Id of the source being registered.
  * @param processor - References the event processor for this fragment body.
  */
void
CEventBuilderEventProcessor::addEventProcessor(
    unsigned sourceId, CEventProcessor& processor
)
{
    SourceData d;
    d.s_sourceId  = sourceId;
    d.s_processor = &processor;
    
    // Create the parameter name for the present parameter.
    
    std::stringstream sPresentName;
    sPresentName << m_baseName << "." << sourceId << "_present";
    d.s_SourcePresent = new CTreeParameter(sPresentName.str());
    
    // Register the processor.
    
    m_sourceHandlers[sourceId] = d;
    
    addEventSourceParameters(sourceId);        // Make matrix parameters.
 
}
/**
 * haveEventProcessor
 *    Returns true if there's already a handler for the specified
 *    source id.
 * @param sid - source id.
 * @return bool
 */
bool
CEventBuilderEventProcessor::haveEventProcessor(unsigned sid)
{
    return m_sourceHandlers.count(sid) > 0;
}
/*------------------------------------------------------------------------------
 *  Private methods (utilities).
 */

 /**
  * computeTimetampDifferences
  *    Compute timestamp differences for all pairs of source ids and set the
  *    relevant paramters appropriately.  Note that since the
  *    m_timeDifferenceParams are effectively an upper triangular array,
  *    id swapping may be needed to find the time difference parameter.
  *
  * @param stamps  - Vector of source id/timestamp pairs.
  * @note There are guarantees (and therefore no assumptions) about the ordering
  *       of the source ids
  * @note We don't assume there are event processors registered for all source
  *       ids we might encounter.
  */
 void
 CEventBuilderEventProcessor::computeTimestampDifferences(
    const std::vector<std::pair<unsigned, uint64_t> >& stamps
)
{
    for (int i = 0; i < stamps.size(); i++) {
        unsigned sid1 = stamps[i].first;
        int64_t ts1  = stamps[i].second;      // Diffs are signed.
        
        for (int j = i+1; j < stamps.size(); j++) {
            unsigned sid2 = stamps[j].first;
            int64_t ts2  = stamps[j].second;
            
            // Try to find sid1 in the outer map:
            
            if (m_timeDifferenceParams.count(sid1)) {
                if (m_timeDifferenceParams[sid1].count(sid2)) {
                    *(m_timeDifferenceParams[sid1][sid2]) = ts1 - ts2;
                }
            } else {          // Could be flipped or no handler:
                if (m_timeDifferenceParams.count(sid2)) {
                    if (m_timeDifferenceParams[sid2].count(sid1)) {
                        *(m_timeDifferenceParams[sid2][sid1]) = ts2 - ts1;        
                    }
                }
            }
        }
    }
}
/**
 * addEventSourceParams
 *    Creates event source timestamp difference parameters and extends the
 *    upper triangular array implemented in m_timeDifferenceParams.
 *
 * @param id - new source id.
 */
void
CEventBuilderEventProcessor::addEventSourceParameters(unsigned id)
{
    // For all existing registered source IDs we add an element to their map
    // of time differences.
    
    for(auto p = m_timeDifferenceParams.begin();
        p != m_timeDifferenceParams.end(); p++) {
        unsigned otherSid = p->first;
        std::string paramName = tdiffParamName(otherSid, id);
        CTreeParameter* param = new CTreeParameter(paramName, "ticks");
        p->second[id] = param;             // Add the new param.
    }
    // Now add a place holder for our time differences with sids added later:
    
    m_timeDifferenceParams[id];         // I think this adds an empty map.
}
/**
 * tdiffParamName
 *    Compute the name of a time difference parameter.  This will be of the form:
 *    basname.tdiffs.id1-id2.
 * @param sid1   - first source id.
 * @parm sid2    - second sourcid.
 * @return std::string - parameter name.
 */
std::string
CEventBuilderEventProcessor::tdiffParamName(unsigned sid1, unsigned sid2)
{
    std::stringstream sName;             // Stream is the simplest way to build:
    sName << m_baseName << "." << "tdiffs." << sid1 <<"-" << sid2;
    return sName.str();
}