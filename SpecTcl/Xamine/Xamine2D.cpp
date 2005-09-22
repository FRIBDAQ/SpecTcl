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

static const char* Copyright = "(C) Copyright Michigan State University 2005, All rights reserved";
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

#include <config.h>
#include "Xamine2D.h"                               
#include <assert.h>
#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

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
//    CXamine2D(volatile Xamine_shared* pXamine, UInt_t nSlot)
// Operation Type:
//    Constructor
//
CXamine2D::CXamine2D(volatile Xamine_shared* pXamine, UInt_t nSlot) :
  CXamineSpectrum(pXamine, nSlot)
{
  // Darned well better be a 2-d:

  assert(  (pXamine->dsp_types[nSlot] == twodword) ||
	   (pXamine->dsp_types[nSlot] == twodbyte) ||
	   (pXamine->dsp_types[nSlot] == twodlong));

  switch (pXamine->dsp_types[nSlot]) {
  case twodbyte:
    m_nType = 1;
    break;
  case twodword:
    m_nType = 0;
    break;
  case twodlong:
    m_nType = 2;
  }

  m_nXchannels = pXamine->dsp_xy[nSlot].xchans;
  m_nYchannels = pXamine->dsp_xy[nSlot].ychans;

}


