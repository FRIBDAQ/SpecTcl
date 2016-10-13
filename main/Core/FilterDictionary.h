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

/*
  FilterDictionary.h
*/

// Preprocessor includes, definitions.
#ifndef __FILTERDICTIONARY_H //Required for current class
#define __FILTERDICTIONARY_H

#ifndef __DICTIONARY_H
#include "Dictionary.h"
#define __DICTIONARY_H
#endif

#ifndef __GATEDEVENTFILTER_H
#include <GatedEventFilter.h>
#define __GATEDEVENTFILTER_H
#endif

// Class. (Singleton Pattern)
class CFilterDictionary : public CDictionary<CGatedEventFilter*> {
  // Attributes.
 private:
  static CFilterDictionary* m_pInstance;

  // Constructors. (Destructor is never called.)
 protected:
  CFilterDictionary();

  // Additional functions.
 public: //protected:
  static CFilterDictionary* GetInstance();
  static void onExit();		// Called at app exit.
}; // CFilterDictionary.

typedef CFilterDictionary::DictionaryIterator CFilterDictionaryIterator;

#endif
