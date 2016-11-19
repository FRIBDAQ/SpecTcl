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

/**
 * @file RingFormatHelper.cpp
 * @brief Empty implementations of the supported canonicals.
 * @note The comparison ops are pathalogical and should not be chained to for
 *       obvious reasons.
 */

#include "RingFormatHelper.h"
#include "DataFormatPre11.h"
#include <unistd.h>


/**
 * constructor - default
 *  NO-OP.
 */
CRingFormatHelper::CRingFormatHelper() {}

/**
 * destructor
 */
CRingFormatHelper::~CRingFormatHelper() {}

/**
 * Copy constructor
 */
CRingFormatHelper::CRingFormatHelper(const CRingFormatHelper& rhs)
{}

/**
 * assignment
 */
CRingFormatHelper&
CRingFormatHelper::operator=(const CRingFormatHelper& rhs)
{
    return *this;
}
/**
 * Comparison for ==
 *
 * Pathalogical - all empty objects are equal to all others.
 */
int
CRingFormatHelper::operator==(const CRingFormatHelper& rhs) const
{
    return 1;
}
/**
 * Comparison for !=
 *  Pathalogical - All empty objects are unequal too.
 */
int
CRingFormatHelper::operator!=(const CRingFormatHelper& rhs) const
{
    return 1;
}

/*-----------------------------------------------------------------------------
 * Private utilities:
 */
/**
 * itemType
 *  Returns the type of a ring item.
 *
 *  @param pItem - pointer to a ring item.
 *
 *  @return uint16_t the item type
 */
uint16_t
CRingFormatHelper::itemType(void* pItem)
{
    NSCLDAQ10::pRingItemHeader p =
        reinterpret_cast<NSCLDAQ10::pRingItemHeader>(pItem);
    
    // Note that since the most significant word of this is zero,
    // the type is loworder | swab(highorder).
    
    uint16_t type = p->s_type;
    uint16_t top  = p->s_type >> 16;
    uint16_t tops;
    swab(&top, &tops, 2);
    
    return type | tops;
}
