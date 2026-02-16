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
#include "CDuplicateSingleton.h"
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

    m_metadata.set(name, value);
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
    return m_metadata.get(name);
}
/**
 * getMetadata
 *    This  overload returns a const reference to the entire metadata dict:
 * 
 * @return const CWaveform::Metadata_t which is actually a map<std::string, std::string>
 */
const CWaveform::Metadata_t&
CWaveform::getMetadata() const {
    return m_metadata.get_all();
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
 * trace
 *    @return const Waveform_t& - reference the trace last saved.
 */
const CWaveform::WaveForm_t&
CWaveform::trace() const {
    return m_trace;
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
    memset(m_trace.data(), 0, nSamples*sizeof(uint16_t));

    // Need to also resize all the fits:

    for(auto& namedfit : m_fitDictionary) {
        m_fits.at(namedfit.second).resize(nSamples);

        // Assumes 0.0 is all bytes zero:
        memset(m_fits.at(namedfit.second).data(), 0, nSamples*sizeof(double));  // FIll with zeros.
    }
}

////////////////////////// Support for fits Issue #212 ///////////////////////

/**
 * addFit
 *    Add a new fit to the  list of fits.  A new fit waveform is allocated
 * with the same size as the parent waveform.  Name of the new fit is entered into the
 * fit dictionary and associated with the index of the added fit waveform.
 *    @param name - name of the new fit, must be unique within the fits for the waveform.
 *                  can be duplicated in a different waveform.. e.g. more than one waveform
 *                  can have a fit named 'fit'.
 *    @returns size_t - id of the fit. Passing this to e.g. fillFit will fill the fit
 *                  named by this call.
 *    @throw CDuplicateSingleton - the fit already exists.
 */
size_t 
CWaveform::addFit(const char* name) {
    std::string sname(name);
    if (m_fitDictionary.find(sname) == m_fitDictionary.end()) {
        // we can enter it.

        Fit_t fit;
        fit.resize(size());        // It has our size... .this fills with 0s.
        m_fits.push_back(fit);
        size_t id = m_fits.size() - 1;
        m_fitDictionary[name] = id;
        return id;
    } else {
        // Duplicate:

        throw CDuplicateSingleton("Waveform already has a fit with this name", name);
    }

}
/**
 * findFit
 *    Get the id fof a fit from its name. 
 * 
 * @param name - name seeking.
 * @return size_t id of the fit.
 * @throw CNoSuchObjectException - no such fit name exists.
 */

 size_t
 CWaveform::findFit(const char* name) const {
    std::string sname(name);
    auto p = m_fitDictionary.find(sname);
    if (p != m_fitDictionary.end()) {
        return p->second;
    } else {
        throw CNoSuchObjectException("No fit exists with that name", name);
    }
 }

 /** 
  * getFit
  *    Return a readonly reference to the fit data for a fit given its index.
  * 
  * @param fitno - id of the fit, returned from addFit or findFit e.g.
  * @return const CWaveform::Fit_t& References the waveform data.
  * @throw CNoSuchObjectException- the fitno is invalid.
  */
 const CWaveform::Fit_t&
 CWaveform::getFit(size_t fitno) const {
    
    if (fitno < m_fits.size()) {
        return  m_fits[fitno];
    } else {
        throw CNoSuchObjectException("Fit id does not reference an existing fit.", std::to_string(fitno));
    }
    
 }

 /**
  * getFit
  *    Same as above but given the name not the id.  So essentially a findFit followed by
  * the above.
  * 
  * @param fitnname - name of a fit.
  * @return const CWaveform::Fit_t& References the waveform data.
  * @throw CNoSuchObjectException- the fitno is invalid.
  */
 const CWaveform::Fit_t&
 CWaveform::getFit(const char* name) const {
    return getFit(findFit(name));    // Takes care of the exception too.
 }

 /**
  * fillFit
  *    Fills a fit with data.
  *   @param fitno - fit id from e.g. addFit.
  *   @param pData - Pointer to double data contaning the fit points.  There must be
  *                 at least size() points in pData.
  *   @throw CNoSuchObjectExceptoin - invalid fitno.
  */
 void 
 CWaveform::fillFit(size_t fitno, const double* pData) {
    const Fit_t& cfit = getFit(fitno);   // Takes care of exceptions too...
    Fit_t& fit = const_cast<Fit_t&>(cfit);   // So we can modifity it.

    memcpy(fit.data(), pData, fit.size()*sizeof(double));
 }

 /**
  * fillFit
  *    Fills fit with data given its name.  This is not recommended because the
  * search time for the fit goes like log(n) n number of fits.  Better to select the
  * fit by number as that's constant time.
  * 
  * @param fitName - name of the fit.
  * @param pData   - Pointer to the data to fill the fit.
  * @return size_t - The fit index so you only need to do this once.
  * @throw CNoSuchObjectException - no matching fit.
  */
size_t
CWaveform::fillFit(const char* fitName, const double* pData) {
    size_t id = findFit(fitName);    // Does our throw for us.
    fillFit(id, pData);

    return id;
}

/**
 * listFits
 *    Returns a vector of pairs containig the name and ids of the
 * defined fits
 * 
 * @return std::vector<std::string, size_t>
 */
std::vector<std::pair<std::string, size_t> >
CWaveform::listFits() const {
    std::vector<std::pair<std::string, size_t>> result;
    for (auto p : m_fitDictionary) {
        result.push_back({p.first, p.second});
    }
    return result;
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