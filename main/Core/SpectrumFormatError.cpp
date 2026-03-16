
/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321
(c) Copyright FRIB/MSU 2026 All rights reserved.
*/
static const char* Copyright = "(C) Copyright Michigan State University 2009, All rights reserved";
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
#include <config.h>
#include "SpectrumFormatError.h"    				
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


static const char* pErrorStrings[] = {
  "Spectrum section has invalid header",
  "Spectrum section has an incompatible format level",
  "The spectrum section appears to be corrupt",
  "The channel subsection has channel numbers which are not valid",
  "The channel subsection has channel values which would cause overflows",
  "Error reported from the HDF5 library"
};

const char** CSpectrumFormatError::m_svErrorStrings = pErrorStrings; 

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

  return (m_svErrorStrings[(Int_t)m_eError]);
  
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



