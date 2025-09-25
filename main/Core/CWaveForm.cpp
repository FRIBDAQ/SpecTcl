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
 * @file CWaveForm.cpp
 * @brief Implement the CWaveForm class to contain traces (issue #201).
 */
#include "CWaveForm.h"
#include <CNoSuchObjectException.h>
#include <string.h>                 // memcpy.

/**
 * m_idIndex is an indes to be passed to the NamedObject constructor
 * and is assigned by our static method getid().
 */
unsigned CWaveform::m_idIndex(0);

/**
 *  constructor:
 *    @param  name - name of the waveform.
 *    @param nSamples - size of the waveform in samples.
 */
CWaveform::CWaveform(const char* name, size_t nSamples) : 
    CNamedItem(std::string(name), CWaveform::getid()) {

    // Set the size/capacity of the trace to nSamples:

    m_trace.resize(nSamples);
}
/**
 * destructor is null for now as all the members can properly
 * destroy themselves.
 */
CWaveform::~CWaveform() {}

/**
 *  setMetdata 
 *    @param name - name of the metadata to set.
 *    @param value- Value to give that metadata item.
 * 
 * @note if 'name' is already metadata its value is replaced.  If not, a new metadata item is created.
 */
void
CWaveform::setMetadata(const char* name, const char* value) {
    std::string strName(name);
    std::string strValue(value);

    m_metadata[name] = value;
}

/**
 *  getMetadata
 *    Return the value of a  named piece of metadata.
 * 
 * @param name - name of the metadata to return.
 * @throw CNoSuchObjectException if that name does not name a metadata.
 * @return std::string - value of that metadata.
 */
std::string
CWaveform::getMetadata(const char* name) const {
    std::string strName (name);
    auto p = m_metadata.find(strName);
    if (p != m_metadata.end()) {
        return p->second;
    } else {
        // not found.

        throw CNoSuchObjectException("Retrieving waveform metadata", name);
    }
}
/**
 * getMetadata
 *    This  overload returns a const reference to the entire metadata dict:
 * 
 * @return const CWaveform::Metadata_t which is actually a map<std::string, std::string>
 */
const CWaveform::Metadata_t&
CWaveform::getMetadata() const {
    return m_metadata;
}

/**
 * update
 *    Update the contents of the trace.  
 * 
 * @param data - pointer to at least the number of samples required to fill the waveform.
 *   The first m_trace.size() values are placed in the trace.
 * 
 * If there's not sufficient storage pointed to by data a SEGFAULT may happen or the end of the
 * waveform could be filled with garbage.
 */
void
CWaveform::update(const uint16_t* data) {
    memcpy(m_trace.data(), data, m_trace.size()*sizeof(uint16_t));
}
/**
 * @return size_t - waveform current size.
 */
size_t
CWaveform::size() const {
    return m_trace.size();
}
/**
 *  resize
 *    Change the size of the waveform.  The waveform values are zeroed.
 * 
 * @param nSamples - new number of samples the trace can accomodtae.
 */
void
CWaveform::resize(unsigned nSamples) {
    m_trace.resize(nSamples);
    memcpy(m_trace.data(), 0, nSamples*sizeof(uint16_t));
}


//////////////////////////////// static methods ////////////////////////////////

/**
 *  getid
 *    Get a new unique waveform id:
 * 
 * @return unsigned.
 */
unsigned
CWaveform::getid() {
    return m_idIndex++;
}