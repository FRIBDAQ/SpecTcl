/*
  FilterDictionary.cpp
*/

// Copyright Notice.
static const char* Copyright =
  "FilterDictionary.cpp: Copyright 2002 NSCL, All rights reserved\n";

// Header Files.
#include <vector>
#include <iostream.h>

#include "Dictionary.h"

#include "FilterDictionary.h"

// Initializations.
CFilterDictionary* CFilterDictionary::m_pInstance = (CFilterDictionary*)kpNULL;

// Constructors.
CFilterDictionary::CFilterDictionary() {}
// Destructor is never called for a Singleton pattern.

// Additional functions.
CFilterDictionary* CFilterDictionary::GetInstance() {
  if(m_pInstance == (CFilterDictionary*)kpNULL) {
    m_pInstance = new CFilterDictionary;
    atexit(CFilterDictionary::onExit);
  }
  return m_pInstance;
  
}

// On exit we whip through all of the filters and close them cleanly:
//
typedef CDictionary<CGatedEventFilter*>   FilterDict;
typedef FilterDict::DictionaryIterator    FilterIterator;
void
CFilterDictionary::onExit()
{
  // Don't need to do anything if no filters

  cerr << "closing off filters\n";

  if(m_pInstance) {
    FilterIterator p = m_pInstance->begin();
    while(p != m_pInstance->end()) {
      CGatedEventFilter* pFilter = p->second;
      if(pFilter->CheckEnabled()) {
	pFilter->Disable();
	cerr << "Closed filter: " << p->first << endl;
      }
      p++;
    }
  }
}
