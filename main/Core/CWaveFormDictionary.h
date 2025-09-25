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
 * @file CWaveFormDictionary.h 
 * @file Defines the singleton dictionary of waveform objects.
 */
#ifndef CWAVEFORMDICTIONARY_H
#define CWAVEFORMDICTIONARY_H
#include "CWaveForm.h"

#include <map>    // The dict.
#include <string>


class CWaveFormDictionary {
public:
    typedef std::map<std::string, CWaveform> Dictionary_t;
private:
    static CWaveFormDictionary*  m_pInstance;   
    Dictionary_t m_dict;
private:
    CWaveFormDictionary();
    ~CWaveFormDictionary();

    // These are completely forbidden:
private:
    CWaveFormDictionary(const CWaveFormDictionary&);
    CWaveFormDictionary& operator=(const CWaveFormDictionary&);
    int operator==(const CWaveFormDictionary&) const;
    int operator!=(const CWaveFormDictionary&) const;

public:
    static CWaveFormDictionary& getInstance();
    void clear();                       // For testing.

    // Dictionary modification

    CWaveform& add(const CWaveform& trace);
    void remove (const char* name);

    // Dictionary queries:

    size_t size() const;
    CWaveform& find(const char* name);
    Dictionary_t::iterator begin();
    Dictionary_t::iterator end();
 };

 #endif