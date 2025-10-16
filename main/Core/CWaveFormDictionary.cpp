/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

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
 * @file CWaveFormDictionary.cpp 
 * @file Implements the singleton dictionary of waveform objects.
*/

#include "CWaveFormDictionary.h"
#include "CNoSuchObjectException.h"
#include "CDuplicateSingleton.h"
#include <stdexcept>


/// Singleton instance:

CWaveFormDictionary* CWaveFormDictionary::m_pInstance(0);

/** 
 * constructor.
 *    Note this is private since this is a singleton object:
 * 
 */
CWaveFormDictionary::CWaveFormDictionary() {
   
}
/**
 *  destructor:
 *     This shoulid never be called except in testing.
 */
CWaveFormDictionary::~CWaveFormDictionary() {
    
}

/** 
 * getInstance
 * @return CWaveFormDictionary& reference to the singleton object:
 */
CWaveFormDictionary&
CWaveFormDictionary::getInstance() {
    if (!m_pInstance) {
        m_pInstance = new CWaveFormDictionary;
    }
    return *m_pInstance;
}

/**
 * add
 *    Add a _copy_ of a waveform object to the dictionary. 
 * @param trace - the waveform to add (it has a name already).
 * @return CWaveform& - references the copy that was created/added to the dict.
 * @throw CDuplicateSingleton if the name already is in the dict.
 */
CWaveform&
CWaveFormDictionary::add(const CWaveform& trace) {
    std::string name = trace.getName();
    if (m_dict.find(name) == m_dict.end()) {
        m_dict.emplace(name, trace);                // Copy done here.
        return m_dict.find(name)->second;
    } else {
        throw CDuplicateSingleton("Inserting a waveform into the waveform dictionary", name);
    }
}

/** 
 * remove
 *    @param name - name of the object to remove.
 *    @throw CNoSuchObjectException if there's no entry for name in the dictionary.
 */
void
CWaveFormDictionary::remove(const char* name) {
    std::string  strName(name);
    auto p = m_dict.find(strName);
    if (p != m_dict.end()) {
        m_dict.erase(p);
    } else {
        throw CNoSuchObjectException(
            "Attempting to delete a waveform from the waveform dictionary", 
            strName
        );
    }
}

/**
 *  @return size_t - number of entries in the dictionary.
 */
size_t
CWaveFormDictionary::size() const {
    return m_dict.size();
}
/**
 * @param name - name of the waveform to find.
 * @return CWaveForm& - reference to the waveform if found.
 * @throw CNoSuchObjectException - if not found.
 */
CWaveform&
CWaveFormDictionary::find(const char* name) {
    std::string strName(name);
    auto p = m_dict.find(strName);
    if (p != m_dict.end()) {
        return p->second;
    } else {
        throw CNoSuchObjectException("Failed to find waveform in dict:", strName);
    }
}

/**
 * @return CWaveFormDictionary::Dictionary_t::iterator  - start of iteration iterator to dict.
 */
CWaveFormDictionary::Dictionary_t::iterator
CWaveFormDictionary::begin() {
    return m_dict.begin();
}
/**
 *  @return CWaveFormDictionary::Dictionary_t::iterator end of iteration iterator to dict.
 */
CWaveFormDictionary::Dictionary_t::iterator
CWaveFormDictionary::end() {
    return m_dict.end();
}