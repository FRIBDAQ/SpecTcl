// Class: CSpectrumStandardFormatters
// Exists solely to ensure the initial population of the
// CSpectrumFormatterFactory formatter list with
// the NSCL formatters.
//
// Implemented in this file, however the
// header for this class is in SpectrumFormatterFactory.h
// which also declares a static instance of this class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
//
//////////////////////////.cpp file/////////////////////////////////////////////////////
#include "SpectrumFormatterFactory.h"
#include "NSCLAsciiSpectrumFormatter.h"
#include "NSCLBinarySpectrumFormatter.h"


static char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved SpectrumStandardFormatters.cpp \n";

Bool_t CSpectrumStandardFormatters::m_fFirstTime = kfTRUE; //Static data member initialization


CNSCLAsciiSpectrumFormatter   g_NSCLAscii;
CNSCLBinarySpectrumFormatter  g_NSCLBinary;

struct RegistrationEntry {
  string               Keyword;
  CSpectrumFormatter*  Formatter;
};

static RegistrationEntry RegistrationTable[] =
{
  { string("nsclascii"),  &g_NSCLAscii }, // Standard items...
  { string("nsclbinary"), &g_NSCLBinary },
  { string("ascii"),      &g_NSCLAscii },  // Convenience items
  { string("binary"),     &g_NSCLBinary } 
};

static unsigned int nStandard = 4;
//  If there is pad between elements of the table, the below may not
//  work.
//                   sizeof(RegistrationTable)/sizeof(RegistrationEntry);


CSpectrumFormatterFactory  FirstFactory;

///////////////////////////////////////////////////////////////////////////
// 
// Function:
//    CSpectrumStandardFormatters
// Operation Type:
//    Constructor.
//
CSpectrumStandardFormatters::CSpectrumStandardFormatters()
{
  // Register the standard formatters if not yet done.

  if(m_fFirstTime) {
    m_fFirstTime = 0;
    for(unsigned i = 0; i < nStandard; i++) {
      CSpectrumFormatterFactory::AddFormatter(RegistrationTable[i].Keyword,
					    (RegistrationTable[i].Formatter));
    }
  }
}
