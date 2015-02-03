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
 * @file CRingFormatHelperFactory.cpp
 * @brief Implementation of CringFormatHelperFactory, see the header for more.
 */

#include "RingFormatHelperFactory.h"
#include "RingFormatHelper.h"
#include "RingFormatHelperCreator.h"
#include "DataFormat.h"

/**
 * default constructor:
 * 
 * Default constructor does nothing useful.  Provide this as a placeholder
 * in case that changes
 */
CRingFormatHelperFactory::CRingFormatHelperFactory()
{}

/**
 * copy constructor:
 * 
 * Copy constructor.  Since the creator map is filled with pointers to
 * dynamically allocated creators this copyies must be dynamically
 * constructed.
 *
 * @param rhs - The factory that is a template for this creation.
 */
CRingFormatHelperFactory::CRingFormatHelperFactory(
    const CRingFormatHelperFactory& rhs
)
{
    CreatorMap::const_iterator p = rhs.m_Creators.begin();
    while(p != rhs.m_Creators.end()) {
        m_Creators[p->first]  = p->second->clone();
        p++;
    }
}
/**
 * destructor
 *
 * Again, since we're holding pointers to creators, we need to delete the
 * creators in the map.  Then the map can cleanly destroy itself.
 */
CRingFormatHelperFactory::~CRingFormatHelperFactory()
{
    CreatorMap::iterator p = m_Creators.begin();
    while (p != m_Creators.end()) {
        delete p->second;
        p++;
    }
}

/**
 * create
 *
 * Create a ring format helper object given the major/minor version of the
 * ring data format.
 *
 * @param major - Major part of data format version.
 * @param minor - Minor part of data format version.
 *
 * @return CRingFormatHelper*
 * @retval Null if there is no match.
 * @retval Pointer to the appropriate ring format creator.  This is dynamically
 *         allocated and must be deleted at the appropriate time by the caller.
 */
CRingFormatHelper*
CRingFormatHelperFactory::create(uint32_t major, uint32_t minor)
{

    CreatorMap::iterator p = m_Creators.find(MajorMinorVersion(major, minor));
    if (p == m_Creators.end()) {
        return reinterpret_cast<CRingFormatHelper*>(0);
    } else {
        return p->second->create();
    }
}
/**
 * create
 *   Overload where the parameter is pointing to a DataFormat item.
 *
 *   @param pItem - pointer to a pDataFormat item.
 *
 *   @return CRingFormatHelper*
 *
 */
CRingFormatHelper*
CRingFormatHelperFactory::create(void* pItem)
{
    pDataFormat p = reinterpret_cast<pDataFormat>(pItem);
    
    // TODO: Figure out what to do if p->s_header.s_type is not RING_FORMAT
    // TODO: Need to use a translator here because these may need swapping!
    
    return create(p->s_majorVersion, p->s_minorVersion);
}
/**
 * addCreator
 *
 * Adds support for another format version by adding a creator
 * for the new version.
 *
 * @param major - Version major part.
 * @param minor - Version minor part.
 * @param creator - Creator.
 */
void
CRingFormatHelperFactory::addCreator(
    uint32_t major, uint32_t minor, const CRingFormatHelperCreator& creator
)
{
    removeCreator(major, minor);   // In case this is a replacement.
    m_Creators[MajorMinorVersion(major, minor)] = creator.clone();
}
/**
 * removeCreator
 *
 *  Remove support for a format by removing its creator.
 *
 *  @param major - Format revision major part.
 *  @param minor - Format revision minor part.
 *
 *  @note it is a no-op to remove a nonexistent creator.
 */
void
CRingFormatHelperFactory::removeCreator(uint32_t major, uint32_t minor)
{
    CreatorMap::iterator p = m_Creators.find(MajorMinorVersion(major, minor));
    if (p != m_Creators.end()) {
        delete p->second;
        m_Creators.erase(p);
    }
}