// Class: CSpectrumFormatter
// Base class of spectrum formatters.   Derivations
// of this class are responsible for reading and writing
// spectra from C++ streams. 
//
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

#include "SpectrumFormatter.h"    				

static char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved SpectrumFormatter.cpp \n";

// Functions for class CSpectrumFormatter

//////////////////////////////////////////////////////////////////////////////
//
// Function:
//    CSpectrumFormatter& operator=
//                         (const CSpectrumFormatter& aCSpectrumFormatter) 
// Operation type:
//    Assignment operator.
//
const CSpectrumFormatter& 
CSpectrumFormatter::operator=(const CSpectrumFormatter& aCSpectrumFormatter) 
  
{
  m_Description = aCSpectrumFormatter.m_Description;
  m_FileType    = aCSpectrumFormatter.m_FileType;
  return *this;
}
///////////////////////////////////////////////////////////////////////////
//
// Function:
//    int operator== (const CSpectrumFormatter& aCSpectrumFormatter) const
// Operation type:
//    Equality comparison.
//
int
CSpectrumFormatter::operator== (const CSpectrumFormatter& aCSpectrumFormatter) 
  const
{
  return ((m_Description == aCSpectrumFormatter.m_Description)   &&
	  (m_FileType    == aCSpectrumFormatter.m_FileType));
}
