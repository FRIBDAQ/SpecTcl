/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#include "RingFormatHelper10.h"
#include <BufferTranslator.h>
#include <unistd.h>
#include <RingItemFactoryBase.h>
#include <NSCLDAQFormatFactorySelector.h>
#include <CRingItem.h>
#include <CRingStateChangeItem.h>
#include <CRingTextItem.h>
#include <CRingScalerItem.h>
#include <CRingPhysicsEventCountItem.h>
#include <v10/DataFormat.h>
#include <memory>

/**
 * constructor
 *    Select the correct factory and stow its reference in m_Factory
 *    where it can be used to pull stuff out of ring items. as required.
 *    Note wew don't own the factory, the factory selector does so we don't
 *    need to delete it on destruction.
 */
CRingFormatHelper10::CRingFormatHelper10() :
  m_Factory(FormatSelector::selectFactory(FormatSelector::v10))
{}

/**
 * hasBodyHeader
 *
 * No version 10.x ring items have body headers:
 *
 * @param pItem - Actually an v10::pRingItem
 *
 * @return bool -false.
 */
bool
CRingFormatHelper10::hasBodyHeader(void* pItem) const
{
    return false;
}
/**
 * getBodyPointer
 *
 * Returns a pointer to the body of the ring item.  This is unconditionally
 * the storage unit that just follows the ring item header.
 *
 * @param pItem - Actually an v10::pRingItem.
 *
 * @return void* - Pointer to the ring item body... regardless of what it
 *                 looks like.
 */
void*
CRingFormatHelper10::getBodyPointer(void* pItem)
{
   v10::pRingItem pI = reinterpret_cast<v10::pRingItem>(pItem);
   return pI->s_body;

}
/**
 * getSourceId
 *    @param pItem - pointer to the full item.
 *    @param pTranslator - buffer translator
 *    @return uint32_t - hard-coded zero since v10 items have no source id.
 */
uint32_t
CRingFormatHelper10::getSourceId(void* pItem, BufferTranslator* pTranslator)
{
  return 0;
}  
/**
 * getBodyHeaderPointer
 *
 * Since no items in NSCLDAQ-10.0 have a body header, a null pointer is
 * returned.
 *
 * @param pItem - actually an v10::pRingItem
 *
 * @return void* null.
 */
void*
CRingFormatHelper10::getBodyHeaderPointer(void* pItem)
{
    return reinterpret_cast<void*>(0);
}

// State change specific methods:

/**
 * getTitle
 *    Return the title string from a state transition item.
 *
 * @param pItem - Pointer to what should be a state transition item.
 *
 * @return std::string - the title.
 *
 * @throw std::string - if the item is not a state transition item.
 */
std::string
CRingFormatHelper10::getTitle(void* pItem)
{
    const ::RingItem* pRaw = reinterpret_cast<const ::RingItem*>(pItem);
    std::unique_ptr<::CRingItem> pBase(m_Factory.makeRingItem(pRaw));
    try {
      std::unique_ptr<::CRingStateChangeItem> pStateChange(
        m_Factory.makeStateChangeItem(*pBase));   // Throws if bad type.
        return pStateChange->getTitle();
    }
    catch (...) {
        throw std::string("CRingFormatHelper10::getTitle - not state change item");  
    }
    
}
/**
 * getRunNumber
 *
 *   Return the run number from a state transition item.
 *
 * @param pItem       - pointer to the state change item.
 * @param pTranslator - The translator that understands byte order conversions.
 *
 * @return unsigned - Run nunmber.
 *
 * @throw std::string - if the item is nto a state transition item.
 */
unsigned
CRingFormatHelper10::getRunNumber(void* pItem, BufferTranslator* pTranslator)
{
    const ::RingItem* pRaw = reinterpret_cast<const ::RingItem*>(pItem);
    std::unique_ptr<::CRingItem> pBase(m_Factory.makeRingItem(pRaw));
    try {
      std::unique_ptr<::CRingStateChangeItem> pStateChange(
        m_Factory.makeStateChangeItem(*pBase));   // Throws if bad type.
        return pStateChange->getRunNumber();
    }
    catch (...) {
        throw std::string("CRingFormatHelper10::getRunNumber - not state change item");  
    }
    
}

// String item specific methods.

/**
 * getStringCount
 *
 * Returns the number of strings in a string item.
 *
 * @param pItem       - Pointer to the item.
 * @param pTranslator - Pointer to a translator that knows how to convert byte
 *                      order diffefrences between the source and the host.
 * @return unsigned
 *
 * @throw std::string - The item is not a string item type.
 */
unsigned
CRingFormatHelper10::getStringCount(void* pItem, BufferTranslator* pTranslator)
{
    return getStrings(pItem, pTranslator).size();
}
/**
 * getStrings
 *    Return a vector of strings from a strings item.
 * @param pItem - actually a pointer to a v10::TextItem.
 * @param pTranslator - pointer to a translator that knows how to do byte
 *                 order conversion.
 * @return std::vector<std::string>  - the strings in the item.
 */
std::vector<std::string>
CRingFormatHelper10::getStrings(void* pItem, BufferTranslator* pTranslator)
{
    const ::RingItem* pRaw = reinterpret_cast<const ::RingItem*>(pItem);
    std::unique_ptr<::CRingItem> pBase(m_Factory.makeRingItem(pRaw));
    try {
      std::unique_ptr<::CRingTextItem> p(m_Factory.makeTextItem(*pBase));
      return p->getStrings();
    }
    catch (...) {
      throw std::string("CRingFormatHelper10::getStrings - not a text item.");
    }
    
}
// Scaler item specific methods.

/**
 * getScalerCount
 *
 * Returns the number of scalers in a scaler item.
 *
 * @param pItem       - Pointer to the item.
 * @param pTranslator - Pointer to the byte order translator object.
 *
 * @return unsigned - Number of scalers.
 *
 * @throw std::string - If the item is not a scaler item.
 */
unsigned
CRingFormatHelper10::getScalerCount(void* pItem, BufferTranslator* pTranslator)
{
    return getScalers(pItem, pTranslator).size();
}
/**
 * getScalers
 *    Return a vector of the scaler counts from a scaler item.
 * @param pItem - pointer to the item.
 * @param pTranslator - pointer to the byte order translator.
 * @return std::vector<uint32_t> the scalers from the item.
 */
std::vector<uint32_t>
CRingFormatHelper10::getScalers(void* pItem, BufferTranslator* pTranslator)
{
    const ::RingItem* pRaw = reinterpret_cast<const ::RingItem*>(pItem);
    std::unique_ptr<::CRingItem> pBase(m_Factory.makeRingItem(pRaw));
    try {
      std::unique_ptr<::CRingScalerItem> p(m_Factory.makeScalerItem(*pBase));
      return p->getScalers();
    }
    catch (...) {
      throw std::string("CRingFormatHelper10::getScalers - not a scaler item");
    }
    
    
}
/**
 * getScalerOriginalSourceId
 *   @param pItem -full item pointer.
 *   @param pTranslator - buffer translator
 *   @return uint32_t 0 - there's no concept of a source id in v10.
 */
uint32_t
CRingFormatHelper10::getScalerOriginalSourceId(void* pItem, BufferTranslator* pTranslator)
{
  return 0;
}
// Trigger count specific methods:

/**
 * getTriggerCount
 *
 *  Return the number of triggers seen so far from a trigger count item.
 *
 *  @param pItem       - Pointer to the item.
 *  @param pTranslator - Pointer to the appropriate byte order translator object.
 *
 *  @return uint64_t trigger count.
 *
 *  @throw std::string - if the item is not a trigger count item.
 */
uint64_t
CRingFormatHelper10::getTriggerCount(void* pItem, BufferTranslator* pTranslator)
{
    const ::RingItem* pRaw = reinterpret_cast<const ::RingItem*>(pItem);
    std::unique_ptr<::CRingItem> pBase(m_Factory.makeRingItem(pRaw));
    try {
      std::unique_ptr<::CRingPhysicsEventCountItem> p(    // Will check
        m_Factory.makePhysicsEventCountItem(*pBase)       // correct type.
      );
      return p->getEventCount();
    }
    catch (...) {
      throw std::string("CRingFormatHelper10::getTriggerCount - not trigger count item");
    }
    
}
/*-----------------------------------------------------------------------------
 * Private utilites:
 */


/**
 * isStateTransition
 *
 *  Returns true if the item is a state transition item.
 *
 *  @param pItem - pointer to the item.
 *
 *  @return bool - true for state transitions false if not.
 */
bool
CRingFormatHelper10::isStateTransition(void* pItem)
{
    uint16_t type = itemType(pItem);
    
    return (
        (type == v10::BEGIN_RUN)
        || (type == v10::END_RUN)
        || (type == v10::PAUSE_RUN)
        || (type == v10::RESUME_RUN)
     );
}
/**
 * isTextItem
 *
 *  @param pItem - Pointer to a ring item.
 *  @return bool - true if the item is a text item, false otherwise.
 */
bool
CRingFormatHelper10::isTextItem(void* pItem)
{
    uint16_t type = itemType(pItem);
    
    return (
        (type == v10::PACKET_TYPES)
        || (type == v10::MONITORED_VARIABLES)
    );
}
/**
 * isScalerItem
 *
 *  @param pItem - Pointer to a ring item.
 *  @return bool - true if the item is a scaler item false otherwise.
 *
 *  @note Only the INCREMENTAL_SCALER type satisfies this because
 *        the shape of the S800 scaler events are so much diferent.
 */
bool
CRingFormatHelper10::isScalerItem(void* pItem)
{
    uint16_t type = itemType(pItem);
    
    return type == v10::INCREMENTAL_SCALERS;
}
/**
 * isTriggerItem
 *
 * @param pItem - Pointer to a ring item.
 * @return bool - True if the item type is PHYSICS_EVENT_COUNT.
 *                false otherwise.
 */
bool CRingFormatHelper10::isTriggerItem(void* pItem)
{
    return itemType(pItem) == v10::PHYSICS_EVENT_COUNT;
}
