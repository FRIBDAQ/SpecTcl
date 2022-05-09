/*
    This software is Copyright by the Board of Trustees of Michigan
    
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  RingFormatHelper12.cpp
 *  @brief: Implement the ring buffer decoder helper for version 12.
 */
#include "RingFormatHelper12.h"
#include "DataFormat12.h"
#include <BufferTranslator.h>


/**
 * constructor
 */
CRingFormatHelper12::CRingFormatHelper12() : CRingFormatHelper11()
{}

// destructor:

CRingFormatHelper12::~CRingFormatHelper12() {}

/**
 * getTitle
 *    Return the title from a state change item.
 *    This must override the 11 method because the title field is past the
 *    original source id field.
 * @param pItem - pointer to the item.
 * 
 * @return std::string - title
 * @throws std::string if this is not a valid state change item.
 */
std::string
CRingFormatHelper12::getTitle(void* pItem)
{
    if(!isStateTransition(pItem)) {
        throw std::string("CRingFormatHelper12::getTitle - item is not a state transition item");
    }
    const NSCLDAQ12::StateChangeItemBody* pBody =
        reinterpret_cast<const NSCLDAQ12::StateChangeItemBody*>(getBodyPointer(pItem));
    std::string result = pBody->s_title;     // NSCLDAQ ensures this is null terminated.
    return result;
}
/**
 * getStrings
 *    Returns a vector of strings that are the strings in a TextItem.
 *  @param pItem - pointer to the item.
 *  @param pTranslator - Pointer to the buffer translator associated with the
 *                  item.
 *  @return std::vector<std::string>
 *  @throws std::string - if the item is not a text item.
 */
std::vector<std::string>
CRingFormatHelper12::getStrings(void* pItem, BufferTranslator* pTranslator)
{
    // this throws std::string if item is not text:
    
    unsigned nStrings = getStringCount(pItem, pTranslator);
    const NSCLDAQ12::TextItemBody* pBody =
        reinterpret_cast<NSCLDAQ12::TextItemBody*>(getBodyPointer(pItem));
    std::vector<std::string> result =
        stringListToVector(nStrings, pBody->s_strings);
    return result;
}
/**
 * getScalers
 *    @param pItem - pointer to a scaler item.
 *    @param pTranslator - pointer to the associated buffer translator.
 *    @return std::vector<uint32_t> - the scalers in the scaler item.
 *    @throws std::string - if pItem does not point to a scaler item.
 */
std::vector<uint32_t>
CRingFormatHelper12::getScalers(void* pItem, BufferTranslator* pTranslator)
{
    // Throws if not scaler item:
    
    unsigned nScalers = getScalerCount(pItem, pTranslator);
    
    const NSCLDAQ12::ScalerItemBody* pBody =
        reinterpret_cast<const NSCLDAQ12::ScalerItemBody*>(getBodyPointer(pItem));
    std::vector<uint32_t> result = marshallScalers(
        nScalers, pBody->s_scalers, pTranslator
    );
    return result;
    
}
/**
 * getScalrOriginalSourceId
 *   @param pItem - pointer to the entire item.
 *   @param pTranslator - buffer translator.
 *   @return uint32_t - s_originalSid field of the body
 */
uint32_t
CRingFormatHelper12::getScalerOriginalSourceId( void* pItem, BufferTranslator* pTranslator)
{
  NSCLDAQ12::ScalerItemBody* pBody =
    reinterpret_cast<NSCLDAQ12::ScalerItemBody*>(getBodyPointer(pItem));
  return pTranslator->TranslateLong(pBody->s_originalSid);
}
/**
 * getTriggerCount
 *    @param pItem - pointer to a physics trigger count item.
 *    @param pTranslator - buffer translator that knows how to deal with byte order
 *          differences.
 *    @return uint64_t - number of triggers.
 */
uint64_t
CRingFormatHelper12::getTriggerCount(void* pItem, BufferTranslator* pTranslator)
{
    if(!isTriggerCountItem(pItem)) {
        throw std::string("CRingFormatHelper12::getTriggerCount - not a PHYSICS_EVENT_COUNT item");
    }
    // This is made complex by the fact that glom is trying to produce
    // trigger count items that sum over all of the physics items it emits.
    // those are the ones that SpecTcl wants to compute analysis efficiency.
    // - No body header - just return the event count.
    // - Body header that does not match the saved physics item source id
    //   (see v11's helper), return the last event count total.
    // -  Body header that matches the saved source id:
    // - update the saved event count total with the one from the item
    //   and return it.
    
    const NSCLDAQ12::PhysicsEventCountItemBody* pBody =
        reinterpret_cast<const NSCLDAQ12::PhysicsEventCountItemBody*>(
            getBodyPointer(pItem)
        );
        
    if (hasBodyHeader(pItem)) {
        const NSCLDAQ12::BodyHeader* pBodyHeader =
            reinterpret_cast<const NSCLDAQ12::BodyHeader*>(
                getBodyHeaderPointer(pItem)
            );
        if (pBodyHeader->s_sourceId == m_glomSourceId) {
            m_nLastEventCount = pTranslator->getQuad(pBody->s_eventCount);            
        }
        return m_nLastEventCount;              // Covers non-matching sid too.
    } else {
        return pTranslator->getQuad(pBody->s_eventCount);
    }
    
}
