/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/**
* @file DBEvents.cpp
* @brief Implement the classes defined in DBEvents.cpp.
*/
#include "DBEvents.h"
#include "CDBEvents.h"
#include <EventList.h>
#include <Event.h>
#include <SpecTcl.h>
#include <CRingBufferDecoder.h>
#include <stdexcept>

/////////////////////////////////////////////////////////////////////////
// CDBSink implementation:

/**
 * constructor
 *    @param pWriter - points to the object to which data will be written.
 *                     caller retains ownership.
 */
CDBSink::CDBSink(CDBEventWriter* pWriter) :
    m_pWriter(pWriter),
    m_saving(false)
{}

/**
 * operator()
 *    Called for every event list sent to the sink.  For each event
 *    in the event list; if we are enabled, write the event to the database.
 *  @param rEvents - reference to the event list.  Note that if not filled, trailing
 *                   elements will be nullptr.
 */
void
CDBSink::operator()(CEventList& rEvents)
{
    if (m_saving) {
        if (rEvents.size() > 0) {
            for (int i = 0; i < rEvents.size(); i++) {
                if (rEvents[i]) {
                    m_pWriter->event(rEvents[i]);          
                } else break;              // No more events.
            }
        }
    }
}
/**
 * enable
 *    Turn on saving.
 *    Note this must be done between runs or else saving events
 *    will throw.
 */
void
CDBSink::enable()
{
    m_saving = true;
}
/**
 * disable
 *   Turn off saving.
 *   This should be done between runs else a run will not be completely saved.
 */
void
CDBSink::disable()
{
    m_saving = false;
}
/**
 * setWriter
 *    Sets a new database writer.  This should be done between runs.
 *  @param pWriter - new writer.
*/
void
CDBSink::setWriter(CDBEventWriter* pWriter)
{
    m_pWriter = pWriter;
}

////////////////////////////////////////////////////////////////////////////
// Implementation of CDBProcessor.

/**
 * constructor
 *   - Create/install the event sink.
 *   - Save the writer, set disabled and not in run.
 * @param pWriter - pointer to the database writer.
 */
CDBProcessor::CDBProcessor(CDBEventWriter* pWriter) :
    m_pWriter(pWriter),
    m_pSink(nullptr),
    m_enabled(false),
    m_inRun(false)
{
    CDBSink* pSink = new CDBSink(pWriter);
    SpecTcl* pApi = SpecTcl::getInstance();
    pApi->AddEventSink(*pSink, "sqlite-writer");
    m_pSink = pSink;   
}
/**
 * OnBegin
 *    Called when a begin run has been received. Note that in event built
 *    data we can get more than one begin run.  Therefore we only
 *    register the first begin run, using the m_inRun flag to indicate
 *    we've already seen a begin.
 *  @param rAnalyzer - unused.
 *  @param rDecoder  - Buffer decoder.  We assume it's a ring decoder and we
 *                     use it to get the state transition ring item.
 */
Bool_t
CDBProcessor::OnBegin(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)
{
    if (m_enabled && (!m_inRun)) {
        // Have to enter the record... for that we need to original ring item
        
        CRingBufferDecoder* pDecoder =
            dynamic_cast<CRingBufferDecoder*>(&rDecoder);
        if (pDecoder) {
            pRingItem pItem = static_cast<pRingItem>(pDecoder->getItemPointer());
            m_pWriter->beginRun(pItem);
            m_inRun = true;             // We've seen the first begin run.
        } else {
            throw std::invalid_argument("Buffer decoder is not a ring item decoder");
        }
    }
    return kfTRUE;
}
/**
 * OnEnd
 *    Called when an end run is received.
 *      This is registered with the writer and we mark that we're no longer in
 *      a run.
 *
  *  @param rAnalyzer - unused.
 *   @param rDecoder  - Buffer decoder.  We assume it's a ring decoder and we
 *                     use it to get the state transition ring item.
 */
Bool_t
CDBProcessor::OnEnd(CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder)
{
    if (m_enabled && m_inRun) {
        CRingBufferDecoder* pDecoder =
            dynamic_cast<CRingBufferDecoder*>(&rDecoder);
        if (pDecoder) {
            pRingItem pItem = static_cast<pRingItem>(pDecoder->getItemPointer());
            m_pWriter->endRun(pItem);
            m_inRun = false;
        } else {
            throw std::invalid_argument("Buffer decoder is not a ring item decoder");
        }
    }
    return kfTRUE;
}
/**
 *OnOther
 *  We use this for scaler items
 *  
 *  @param rDecoder - references the buffer decoder.
 */
Bool_t
CDBProcessor::OnOther(
    UInt_t nType, CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder
)
{
    if (m_enabled &&  m_inRun) {
        CRingBufferDecoder* pDecoder =
            dynamic_cast<CRingBufferDecoder*>(&rDecoder);
        if (pDecoder) {
            pRingItem pItem = static_cast<pRingItem>(pDecoder->getItemPointer());
            m_pWriter->scaler(pItem);
            m_inRun = false;
        } else {
            throw std::invalid_argument("Buffer decoder is not a ring item decoder");
        }
    }
    
    return kfTRUE;
}
/**
 * Enable writing data.
 */
void
CDBProcessor::enable()
{
    m_enabled = true;
    m_pSink->enable();
}
/**
 * setWriter
 *   Set a new writer.
 *
 *   @param pWriter - new event writer.
 */
void
CDBProcessor::setWriter(CDBEventWriter* pWriter)
{
    m_pWriter = pWriter;
    m_pSink->setWriter(pWriter);
}
