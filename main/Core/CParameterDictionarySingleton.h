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
 * CParameterDictionarySingleton is the SpecTcl singleton class that
 * contains parameter dictionaries.  You get the instance of it
 * by invoking CParameterDictionarySingleton::getInstnace()
 * 
*/
#ifndef CPARAMETERDICTIONARYSINGLETON_H
#define CPARAMETERDICTIONARYSINGLETON_H
#include "Dictionary.h"
#include "Parameter.h"

typedef CDictionary<CParameter>                 ParameterDictionary;
typedef ParameterDictionary::DictionaryIterator ParameterDictionaryIterator;
typedef DictionaryObserver<CParameter>          ParameterDictionaryObserver;

class CParameterDictionarySingleton {
private:
    static ParameterDictionary* m_pInstance;
private:
    CParameterDictionarySingleton();
    ~CParameterDictionarySingleton();
    CParameterDictionarySingleton(const CParameterDictionarySingleton& rhs);
    CParameterDictionarySingleton& operator=(const CParameterDictionarySingleton& rhs);
public:
    static ParameterDictionary* getInstance();
};

#endif