//  CXamine2D.cpp
// Encapsulates an Xamine 2d spectrum description
// (readonly).

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


#include "Xamine2D.h"                               
#include <assert.h>

static const char* Copyright = 
"CXamine2D.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CXamine2D

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CXamine1D* Oned (  )
//  Operation Type:
//     Type safe upcast
//
CXamine1D* 
CXamine2D::Oned() 
{

// Exceptions:  

  return (CXamine1D*)kpNULL;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CXamine2D* Twod (  )
//  Operation Type:
//     type safe upcast
//
CXamine2D* 
CXamine2D::Twod() 
{

// Exceptions:  

  return (CXamine2D*)this;
}
///////////////////////////////////////////////////////////////////////
//
// Function:
//    CXamine2D(Xamine_shared* pXamine, UInt_t nSlot)
// Operation Type:
//    Constructor
//
CXamine2D::CXamine2D(Xamine_shared* pXamine, UInt_t nSlot) :
  CXamineSpectrum(pXamine, nSlot)
{
  // Darned well better be a 2-d:

  assert(  (pXamine->dsp_types[nSlot] == twodword) ||
	   (pXamine->dsp_types[nSlot] == twodbyte));

  m_fByte      = (pXamine->dsp_types[nSlot] == twodbyte);
  m_nXchannels = pXamine->dsp_xy[nSlot].xchans;
  m_nYchannels = pXamine->dsp_xy[nSlot].ychans;

}


