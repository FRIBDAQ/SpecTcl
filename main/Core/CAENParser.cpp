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

/** @file:  CAENParser.cpp
 *  @brief: Implement methods of parsing events with CAEN hits.
 */
#include "CAENParser.h"
#include "FragmentIndex.h"
#include <algorithm>
#include <stdexcept>

/**
 * constructor
 *  null for now
 */
CAENParser::CAENParser()
{
    
}
/**
 * Destructor
 *    Just clear to be sure there's no lost storage.
 */
CAENParser::~CAENParser()
{
    clear();
}
/**
 * operator()
 *    Process an event.
 *    @param pEventBody - pointer to an event body.
 *     The first long is the size of the event body.
 *     The event body is assumed to be, and better be,
 *     an event built event.
 */
void
CAENParser::operator()(void* pEventBody)
{
    FragmentIndex frags(static_cast<uint16_t*>(pEventBody));
    for (int i =0; i < frags.getNumberFragments(); i++) {
        FragmentInfo info = frags.getFragment(i);
        
        int sid = info.s_sourceId;    // That's what we care about.
        
        // If there's a module in the map we ask it to
        // parse its hit.  If that's the first hit a pointer
        // to that module is added to the hit modules array.
        
        auto p = m_modules.find(sid);
        if (p != m_modules.end()) {
            CAENHit* pHit = makeHit(p->second);
            pHit->unpack(info.s_itembody);
            if (p->second.s_module.getHits().size() == 0) {
                // First hit:
                
                m_hitModules.push_back(&(p->second.s_module));
            }
            p->second.s_module.addHit(pHit);
        }
    }
}
/**
 * clear
 *     Clears the hits in the module hits array and
 *     the array itself.  This prepares for the next
 *     event.
 */
void
CAENParser::clear()
{
    // Clear the hits in the hit modules.
    std::for_each(
        m_hitModules.begin(), m_hitModules.end(),
        [this](CAENModuleHits* p) {
            destroyHits(*p);
            p->clear();
        }
    );
    // Clear the array of hit modules.
    
    m_hitModules.clear();
}
/**
 * getHitModules
 *   @return  const std::vector<CAENModuleHits*>& - reference to the
 *     array of pointers to hit modules.
 */
const std::vector<CAENModuleHits*>&
CAENParser::getHitModules() const
{
    return m_hitModules;
}
/**
 * addModule
 *    Called to add a module description to the map of modules
 *    that respond to source ids.
 *
 *  @param sid - source id.
 *  @param type - hit type the hit container should use.
 *  @param multiplier - used only for PSD - the CFD multiplier
 *  @note       - we assume you don't put in a duplicate sid.
 */
void
CAENParser::addModule(int sid, CAENHit::HitType type, int multiplier)
{
    ModuleInfo info(sid);
    info.s_hitType = type;
    info.s_multiplier = multiplier;
    m_modules[sid] = info;
}

////////////////////////// private methods ////////////////

/**
 * makeHit
 *    Create a new hit for a module give its info.
 *  @param info - module info reference.
 *  @return CAENHit*  - pointer to the created hit.
 */
CAENHit*
CAENParser::makeHit(const ModuleInfo& info)
{
    CAENHit* pResult;
    switch (info.s_hitType) {
        case CAENHit::PSD: {
            pResult = new CAENPSDHit(info.s_multiplier); 
        }
        break;
        case CAENHit::PHA: {
            pResult = new CAENPHAHit(info.s_multiplier);
        }
        break;
        default: {
            
        }
    }
    return pResult;
}
/**
 * destroyHits
 *    deletes the hits in a hit module.
 * @param module - references the module.
 */
void
CAENParser::destroyHits(CAENModuleHits& module)
{
    auto container = module.getHits();
    std::for_each(
      container.begin(), container.end(),
      [](CAENHit* pHit) {
        delete pHit;
      }
    );
}