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
 *  CGateDictionarySingleton manages the singleton Gate dictionary.  The way to get a pointer
 * to SpecTcl's gate dictionary is with CGateDictionarySingleton::getInstance().
 * Note that the dict itself is not a singleton.  This is just a class to manage a singleton intance
 * of a GateDictionary.
*/

#ifndef   CGATEDICTIONARYSINGLETON_H
#define CGATEDICTIONARYSINGLETON_H

#include "Dictionary.h"
#include "GateContainer.h"

typedef CDictionary<CGateContainer>             CGateDictionary;
typedef CGateDictionary::DictionaryIterator     CGateDictionaryIterator;
typedef DictionaryObserver<CGateContainer>      GateDictionaryObserver;


class CGateDictionarySingleton {
private:
    static CGateDictionary* m_pInstance;

private:
    CGateDictionarySingleton();
    ~CGateDictionarySingleton();
    CGateDictionarySingleton(const CGateDictionarySingleton& rhs);
    CGateDictionarySingleton operator=(const CGateDictionarySingleton& rhs);
    int operator==(const CGateDictionarySingleton& rhs);

public:
    static CGateDictionary* getInstance();
};
#endif