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
// overload put with char* see above.
void
MirrorDirectory::put(const char* host, const char* key)
{
    std::string strHost(host);
    std::string strKey(key);
    put(strHost, strKey);                  // Threadsafe.
}

/**
 *   remove
 * Remove a key from the set.  If the key does not exist, this is a no-op.
 *
 * @param host - host to remove.
 */
void
MirrorDirectory::remove(std::string& host)
{
    m_directory.erase(host);
}
/// overload - same but with const char*

void
MirrorDirectory::remove(const char* host)
{
    std::string strHost;
    remove(strHost);
}

/**
 * get
 *    Return the key associated with a host.
 * @param host - the host.
 * @return std::string the associated key.
 * @throws std::logic_error if there is no match to the host.
 */
std::string
MirrorDirectory::get(std::string& host)
{
    CriticalSection entry(*m_pGuard);
    
    auto p = m_directory.find(host);
    if (p == m_directory.end()) {
        throw std::logic_error("No matching key");
    }
    return p->second;
}
// overload, see above.

std::string
MirrorDirectory::get(const char* host)
{
    std::string strHost(host);
    return get(strHost);
}
/**
 * list
 *    List the contents of the directory.
 * @return std::vector<HostKey> - contents of the directory flattened.
 */
std::vector<MirrorDirectory::HostKey>
MirrorDirectory::list()
{
    CriticalSection entry(*m_pGuard);
    std::vector<HostKey> result;
    
    for(auto p = m_directory.begin(); p != m_directory.end(); ++p) {
        result.push_back(*p);
    }
    
    return result;
}
///////////////////////////////////////////////////////////////////////////////
// Singleton implementation:

CTCLMutex MirrorDirectorySingleton::m_creationguard;
MirrorDirectorySingleton* MirrorDirectorySingleton::m_pInstance(0);

// Need the instance for the constructor:
//
MirrorDirectorySingleton::MirrorDirectorySingleton()
{}

/**
 * getInstance
 *   Retrieve the singleton instance (creating it if needed)
 */
MirrorDirectorySingleton*
MirrorDirectorySingleton::getInstance()
{
    CriticalSection entry(m_creationguard);
    if (!m_pInstance) {
        m_pInstance = new MirrorDirectorySingleton;
    }
    return m_pInstance;
}
