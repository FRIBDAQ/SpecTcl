//  CXamine1D.cpp
// Encapsulates a 1-d spectrum description (readonly) for Xamine
// Supplies the type safe upcasts for Oned and Twod appropriate
// to this type.
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//

#include "Xamine1D.h"                               

static const char* Copyright = 
"CXamine1D.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CXamine1D

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CXamine1D* Oned (  )
//  Operation Type:
//     Type safe upcast.
//
CXamine1D* 
CXamine1D::Oned() 
{
// Returns (CXamine1D*)this
// Exceptions:  

  return (CXamine1D*)this;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CXamine2D* Twod (  )
//  Operation Type:
//     Type safe upcast
//
CXamine2D* 
CXamine1D::Twod() 
{
// Returns kpNULL cast to CXamine2D*
// Exceptions:  

  return (CXamine2D*)kpNULL;
}

/////////////////////////////////////////////////////////////////////////
//
// Function:
//    CXamine1D(volatile Xamine_shared* pXamine, UInt_t nSlot)
// Operation Type:
//    Parameterized constructor.
//
CXamine1D::CXamine1D(volatile Xamine_shared* pXamine, UInt_t nSlot) :
  CXamineSpectrum(pXamine, nSlot),
  m_fWord(kfFALSE),
  m_nChannels(0)
{
  // This really must be a 1d  spectrum:

  assert( (pXamine->dsp_types[nSlot] == onedlong)  ||
	  (pXamine->dsp_types[nSlot] == onedword));

  m_fWord     = (pXamine->dsp_types[nSlot] == onedword);
  m_nChannels = pXamine->dsp_xy[nSlot].xchans; 
}




