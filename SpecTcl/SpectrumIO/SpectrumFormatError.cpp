// Class: CSpectrumFormatError
// Encapsulates a specialized exception class which
// Describes the set of errors on file formats which might
// be encountered when reading a spectrum from file.
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
#include "SpectrumFormatError.h"    				

static char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved SpectrumFormatError.cpp \n";

static char* pErrorStrings[] = {
  "Spectrum section has invalid header",
  "Spectrum section has an incompatible format level",
  "The spectrum section appears to be corrupt",
  "The channel subsection has channel numbers which are not valid",
  "The channel subsection has channel values which would cause overflows"
};

char** CSpectrumFormatError::m_svErrorStrings = pErrorStrings; 

// Functions for class CSpectrumFormatError

//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     ReasonText()
//  Operation Type: 
//     selector.
const char* CSpectrumFormatError::ReasonText()  const
{
  // Returns the reason for the error as a
  // textual string.  The appropriate error
  // string is looked up from the m_svErrorStrings.
  //

  return m_svErrorStrings[(Int_t)m_eError];
  
}
//////////////////////////////////////////////////////////////////////////////
//
//  Function:       
//     ReasonCode()
//  Operation Type: 
//     selector
Int_t CSpectrumFormatError::ReasonCode()  const
{
  // Returns the Encoded reason for the exception.
  // This is just m_eError cast as an int.
  return (Int_t)m_eError;
}



