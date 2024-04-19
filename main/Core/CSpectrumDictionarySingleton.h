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
 * CSpectrumDictionarySingleton is a wrapper for 
 * the singleton Spectrujm Dictionary.  To obtain a pointer to the spectrum dictionary
 * inside of SpecTcl, one should CSpectrumDictionarySingleton::getInstance()
*/

#ifndef CSPECTRUMDICTIONARYSINGLETON_H 
#define CSPECTRUMDICTIONARYSINGLETON_H 

#include "Dictionary.h"
#include "Spectrum.h"

typedef CDictionary<CSpectrum*>                 SpectrumDictionary;
typedef SpectrumDictionary::DictionaryIterator  SpectrumDictionaryIterator;
typedef SpectrumDictionary::ConstDictionaryIterator  ConstSpectrumDictIterator;
typedef DictionaryObserver<CSpectrum*>          SpectrumDictionaryObserver;

class CSpectrumDictionarySingleton {
private:
    static SpectrumDictionary* m_pInstance;
private:
    CSpectrumDictionarySingleton();
    ~CSpectrumDictionarySingleton();
    CSpectrumDictionarySingleton(const CSpectrumDictionarySingleton& rhs);
    CSpectrumDictionarySingleton& operator=(const CSpectrumDictionarySingleton rhs);
    int operator==(const CSpectrumDictionarySingleton& rhs);
public:
    static SpectrumDictionary* getInstance();
};

#endif