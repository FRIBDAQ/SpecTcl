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
#ifndef CWAVEFORM_H
#define CWAVEFORM_H
/**
 * @file CWaveForm.h
 * @brief Definition of class to provide waveform objects (issue #201).
 */
#include <NamedItem.h>               // parent class.
#include <stdint.h>

#include <string>                    // for metadata e.g.
#include <map>

#include <vector>                    // Holds the data.
#include <CMetadata.h>               // Issue #229 - haul metadata into class.

class CWaveform : public CNamedItem {
public:
    typedef CMetadata::Metadata_t Metadata_t;   // For pre Issue #229 compatibility
    typedef std::map<std::string, size_t>      FitMap_t;
    typedef std::vector<uint16_t>  WaveForm_t;
    typedef std::vector<double>    Fit_t;       // Fits are floating point.
private:
    CMetadata  m_metadata;
    WaveForm_t  m_trace;
    FitMap_t    m_fitDictionary;     // Issue #212 - Fit name -> index.
    std::vector<Fit_t> m_fits;  // Issue #212 - Vector of fit traces.
    
    static unsigned m_idIndex;     // Used to assign ids we don't use.
public:
    // Canonicals:

    CWaveform(const char* name, size_t nSamples);
    virtual ~CWaveform();

    // Other canonicals automatics should work for us.

    // Post construction operations:
public:
    void setMetadata(const char* name, const char* value);
    std::string getMetadata(const char* name) const;
    const Metadata_t& getMetadata() const;

    void update(const uint16_t* data);    // Update the waveform.
    const WaveForm_t& trace() const;
    void resize(unsigned nSample);        // Change # of samples.
    size_t size() const;

    // Issue #212 - add fits to waveforms:

    size_t addFit(const char* fitName);
    size_t findFit(const char* fitName) const;
    const Fit_t& getFit(size_t fitno) const;
    const Fit_t& getFit(const char* fitName)const ;
    void fillFit(size_t fitno, const double* pData);
    size_t fillFit(const char* fitName, const double* pData);   // not recommended (better by index).
    std::vector<std::pair<std::string, size_t> > listFits() const;

private:
    static unsigned getid();
};
 #endif