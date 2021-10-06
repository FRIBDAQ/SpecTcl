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

/** @file:  MirrorDirectory.cpp
 *  @brief: Directory of shared memory mirrors (implementation).
 */
#include "MirrorDirectory.h"
#include <CTCLMutex.h>
#include <stdexcept>

/**
 * constructor
 *    - Create the mutex that will be used to guard the critical regions.
 */
MirrorDirectory::MirrorDirectory() : m_pGuard(new CTCLMutex)
{
    
}
/**
 * destructor
 *  - Clean up the mutex.
 */
MirrorDirectory::~MirrorDirectory()
{
    delete m_pGuard;
}

/**
 * put
 *    Put a new directory entry.
 *  @param host - host to add.
 *  @param key  - SYSV key to associated with the host.
 *  @throws std::logic_error - if the key already exists.
 */
void
MirrorDirectory::put(std::string& host, std::string& key)
{
    CriticalSection entry(*m_pGuard);      // Only one thread at a time.
    if (m_directory.count(host) > 0) {
        throw std::logic_error("Duplicate key inster attempted");
    }
    m_directory[host] = key;
}
/**
 * get
 *    Return the key associated with a host.
 * @param host - the host.
 * @return std::string the associated key.
 * @throws std::logic_error if there is no match to the host.
 */
std::string
MirrorDirectory::get(std::string host)
{
    CriticalSection entry(*m_pGuard);
    
    auto p = m_directory.find(host);
    if (p == m_directory.end()) {
        throw std::logic_error("No matching key");
    }
    return p->second;
}
