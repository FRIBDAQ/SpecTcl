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

/** @file:  CAENEventProcessor.cpp
 *  @brief: Implement the base class of the CAENEventProcessor.
 */
#include "CAENEventProcessor.h"
#include "CAENParser.h"
#include "CAENParameterMap.h"
#include "CAENHit.h"
#include "CAENModuleHits.h"
#include "TCLAnalyzer.h"

#include <algorithm>
#include <string>
#include <stdexcept>
#include <typeinfo>
#include <Exception.h>
#include <stdint.h>
#include <iostream>

/**
 * constructor
 *    Creates our parrser.
 */
CAENEventProcessor::CAENEventProcessor() :
    m_pParser(nullptr),
    m_testMode(false)
{
    m_pParser = new CAENParser;
}
/**
 * Destructor
 *   For each entry in the parameter map map we call disposeMapEntry
 *   This allows any dynamically allocated map entries to be
 *   deleted.
 */
CAENEventProcessor::~CAENEventProcessor()
{
    std::for_each(
        m_pMaps.begin(), m_pMaps.end(),
        [this](std::pair<int, CAENParameterMap*> p) {
        this->disposeMapEntry(p.first, p.second);
    });
    delete m_pParser;
}

/**
 * operator()
 *    This method is passed each event.  It invokes our parser
 *    to process the event.
 * @param pEvent - pointer to the raw event.  Must be event built.
 * @param rEvent - Parameter 'array' that we're building.
 * @param rAnalyzer - references the analyzer (a CTclAnalyzer).
 * @param rDecoder  - References the buffer decoder. This is assumed
 *                 to be a ring buffer decoder.
 *  @return Bool_t  - we execute in a try/catch block so that any
 *       thrown exception return Bool_t false after reporting what we can.
 *       
 *  @note we assume we have to set the event size but the size is just
 *       the first longword of pEvent so that's simple enough.
 */
Bool_t
CAENEventProcessor::operator()(
    const Address_t pEvent,
    CEvent& rEvent,
    CAnalyzer& rAnalyzer,
    CBufferDecoder& rDecoder
)
{
    // If we have a TCLAnalyzer we need to set it's event
    // size.  Failed dynamic casts result in a nullptr so:
    // Note that in testing, the analyzer may be a null reference
    // but directly testing for that is evidently optimized out because
    // the compiler things that null references are not allowed so
    // this kludge is used:
    
    
    
    
    if (!m_testMode) {    
        CTclAnalyzer* pTclA = dynamic_cast<CTclAnalyzer*>(&rAnalyzer);
        if (pTclA)  {
            uint32_t* p32 = static_cast<uint32_t*>(pEvent);
            pTclA->SetEventSize(*p32);
        }
    }
    // The rest is in a try/catch block to handle exceptions as
    // a failed event processing.
    
    try {
        (*m_pParser)(pEvent);    // Parse event -> hits
        auto moduleHits = m_pParser->getHitModules();
        std::for_each(
            moduleHits.begin(), moduleHits.end(),
            [this](CAENModuleHits* p) {
                int sid = p->getSourceId();
                
                // Ignore modules for which there's no map.
                
                auto pMap = m_pMaps.find(sid);
                if (pMap != m_pMaps.end()) {
                    CAENParameterMap* pParams = pMap->second;
                    pParams->assignParameters(*p);
                }
            }
        );
    }
    catch(std::exception& e) {
        std::cerr << "Exception in event processing: " << e.what() << std::endl;
        return kfFALSE;
    }
    catch (CException& e) {
        std::cerr << "NSCL Exception in event processing: "
                    << e.ReasonText() << std::endl;
        return kfFALSE;
    }
    catch (std::string msg) {
        std::cerr << "String exception caught in event processing: "
            << msg << std::endl;
        return kfFALSE;
    }
    catch (...) {
        std::cerr << "Unanticpated exception type caught in event processing\n";
        return kfFALSE;
    }
    
    // We got here successfully so ensure event processing continues:
    
    return kfTRUE;
    
}
/////////////////////////////////////////////////////////////////
// Protected methods.

/**
 * addParameterMap
 *    Adds a new paramter map, and informs the parser of the sid
 *    and its type. Dynamic casting is used to determine which type
 *    of object the mapper works for.
 * @param sid - source id to associate with this map.
 * @param map - Pointer to the map.
 * @param mult - CFD multiplier (only used for PSD modules)
 * @throw std::bad_cast if the type of the map cannot be determined.
 */
void
CAENEventProcessor::addParameterMap(int sid, CAENParameterMap* map, int mult)
{
    CAENHit::HitType type;
    if (dynamic_cast<CAENPSDMapper*>(map)) {
        type = CAENHit::PSD;
    } else if (dynamic_cast<CAENPHAMapper*>(map)) {
        type = CAENHit::PHA;
    } else {
        throw std::bad_cast();       // Bad map -- can't determine type.
    }
    m_pParser->addModule(sid, type, mult);
    m_pMaps[sid] = map;
    
}
/**
 * disposeMapEntry
 *    destroy map entries.
 *
 *  @param sid  - source id of the map to dispose.
 *  @param pMap - Pointer to the map.
 */
void
CAENEventProcessor::disposeMapEntry(int sid, CAENParameterMap* pMap)
{
    delete pMap;
}
