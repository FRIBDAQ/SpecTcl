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

/** @file:  CAEHModuleHits.cpp
 *  @brief: Collection of Hits from CAEN modules.
 */
#include "CAENModuleHits.h"

/**
 * constructor:
 *    @param sid - id of the source (module number) from which
 *                 these hits allegedly come.  This is intended for
 *                 use in the stage when the hits we have are
 *                 decoded into parameters.
 */
CAENModuleHits::CAENModuleHits(int sid) : m_nSourceId(sid)
{
    m_Hits.reserve(16);            // Current module size.
}
/**
 * copy constructor
 */
CAENModuleHits::CAENModuleHits(const CAENModuleHits& rhs)
{
    copyIn(rhs);
}
/**
 * assignment
 */
CAENModuleHits&
CAENModuleHits::operator=(const CAENModuleHits& rhs)
{
    if(this != &rhs) copyIn(rhs);
    return *this;
}

/**
 * destructor
 *    Currently empty.
 */
CAENModuleHits::~CAENModuleHits()
{}

/**
 * clear
 *    Clear the hits.  This makes us re-usable.  Note that
 *    the vector's capacity is unchanged so we're not
 *    flailing storage allocation (here).
 */
void
CAENModuleHits::clear()
{
    m_Hits.clear();
}
/**
 * addHit
 *   Adds a pointer to an unspecialized hit to the container.
 *   Note that in practice, all hits added to the container
 *   will be of the same underlying type.
 * @param pHit Pointer to the hit to add.
 * @note that it's up to our client to manage storage.  The
 *       parent retains ownership.
 */
void
CAENModuleHits::addHit(CAENHit* pHit)
{
    m_Hits.push_back(pHit);
}
/**
 * getHits
 *   Returns the set of hits we currently have.  These
 *   are an immutable reference, though theoretically the hits
 *    are mutable.
 * @return const std::vector<CAENHit*> 
 */
const std::vector<CAENHit*>
CAENModuleHits::getHits() const
{
    return m_Hits;
}
/**
 * getSourceId
 *    Gets the id of the hit source.  This is effectively
 *    a module id.
 * @return int - source id.
 */
int
CAENModuleHits::getSourceId() const
{
    return m_nSourceId;
}
 /////////////////////// private methods ///////////
 
 /**
  * copyIn
  *    Copy contents from another object.
  */
 void
 CAENModuleHits::copyIn(const CAENModuleHits& rhs)
 {
    m_nSourceId = rhs.m_nSourceId;
    m_Hits       = rhs.m_Hits;
 }