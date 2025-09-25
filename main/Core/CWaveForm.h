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

class CWaveform : public CNamedItem {
public:
    typedef std::map<std::string, std::string> Metadata_t;
private:

    typedef std::vector<uint16_t>  WaveForm_t;
private:
    Metadata_t  m_metadata;
    WaveForm_t  m_trace;
    
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
    void resize(unsigned nSample);        // Change # of samples.
    size_t size() const;
private:
    static unsigned getid();
};
 #endif