/*
  FilterDictionary.cpp
*/

// Copyright Notice.
static const char* Copyright =
  "FilterDictionary.cpp: Copyright 2002 NSCL, All rights reserved\n";

// Header Files.

#include <config.h>
#include <vector>

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
  }
  return m_pInstance;
  //static CFilterDictionary instance; // Not to worry; Destroyed on program termination.
  //return &instance;
}
