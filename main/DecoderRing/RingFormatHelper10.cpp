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
#include "DataFormatPre11.h"    // Version 10.x ring formats.
#include <BufferTranslator.h>
#include <unistd.h>

/**
 * hasBodyHeader
 *
 * No version 10.x ring items have body headers:
 *
 * @param pItem - Actually an NSCLDAQ10::pRingItem
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
 * @param pItem - Actually an NSCLDAQ10::pRingItem.
 *
 * @return void* - Pointer to the ring item body... regardless of what it
 *                 looks like.
 */
void*
CRingFormatHelper10::getBodyPointer(void* pItem)
{
    NSCLDAQ10::pRingItem p = reinterpret_cast<NSCLDAQ10::pRingItem>(pItem);
    return reinterpret_cast<void*>((p->s_body));
}
/**
 * getBodyHeaderPointer
 *
 * Since no items in NSCLDAQ-10.0 have a body header, a null pointer is
 * returned.
 *
 * @param pItem - actually an NSCLDAQ10::pRingItem
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
    if (isStateTransition(pItem)) {
        NSCLDAQ10::pStateChangeItem p =
            reinterpret_cast<NSCLDAQ10::pStateChangeItem>(pItem);
        return std::string(p->s_title);
    } else {
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
    if (isStateTransition(pItem)) {
        NSCLDAQ10::pStateChangeItem p =
            reinterpret_cast<NSCLDAQ10::pStateChangeItem>(pItem);
        return pTranslator->TranslateLong(p->s_runNumber);
    } else {
        throw std::string("CRingFormatHelper10::getRunNumber - not state transition item");
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
    if (isTextItem(pItem)) {
        NSCLDAQ10::pTextItem p = reinterpret_cast<NSCLDAQ10::pTextItem>(pItem);
        return pTranslator->TranslateLong(p->s_stringCount);
    } else {
        throw std::string("CRingFormatHelper10::getStringCount - not text item");
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
    if (isScalerItem(pItem)) {
        NSCLDAQ10::pScalerItem p = reinterpret_cast<NSCLDAQ10::pScalerItem>(pItem);
        return pTranslator->TranslateLong(p->s_scalerCount);
    } else {
        throw std::string("CRingFormatHelper10::getScalerCount - Not scaler item");
    }
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
    if (isTriggerItem(pItem)) {
        NSCLDAQ10::pPhysicsEventCountItem p =
            reinterpret_cast<NSCLDAQ10::pPhysicsEventCountItem>(pItem);
        return pTranslator->getQuad(p->s_eventCount);
    } else {
        throw std::string(
            "CRingFormatHelper10::getTriggerCount - not trigger count item"
        );
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
        (type == NSCLDAQ10::BEGIN_RUN)
        || (type == NSCLDAQ10::END_RUN)
        || (type == NSCLDAQ10::PAUSE_RUN)
        || (type == NSCLDAQ10::RESUME_RUN)
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
        (type == NSCLDAQ10::PACKET_TYPES)
        || (type == NSCLDAQ10::MONITORED_VARIABLES)
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
    
    return type == NSCLDAQ10::INCREMENTAL_SCALERS;
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
    return itemType(pItem) == NSCLDAQ10::PHYSICS_EVENT_COUNT;
}