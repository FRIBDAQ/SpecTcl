/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321


  FilterDictionary.cpp
*/

// Copyright Notice.
static const char* Copyright =
  "FilterDictionary.cpp: Copyright 2002 NSCL, All rights reserved\n";

// Header Files.

#include <config.h>
#include <vector>
#include <iostream>
#include "Dictionary.h"

#include "FilterDictionary.h"
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


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
  //static CFilterDictionary instance; // Not to worry; Destroyed on program termination.
  //return &instance;
}
void
CFilterDictionary::onExit()
{
  // Don't need to do anything if no filters

  cerr << "closing off filters\n";

  if(m_pInstance) {
    CFilterDictionaryIterator p = m_pInstance->begin();
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
