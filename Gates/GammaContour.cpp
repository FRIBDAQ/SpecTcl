//  CGammaContour.cpp:
// Describes a gamma contour gate. The gate is
// represented internally in a manner which makes
// it efficient to look up the result of the gate.
// This is not space efficient, although some
// optimizations have been done:
//  1. m_Lower - is the lowest legal value
//               for the y parameter.
//  2. m_Upper - is the highest legal value
//               for the y parameter.
//    Now let p = rEvent[m_nYid] - m_Upper
//  3. m_aInterior is a vector of structs. Each struct contains:
//        m_LowX  - Lowest valid X parameter for m_aInterior[p]
//        m_HiX   - Highest valid X Parameter for m_aInterior[p]
//        m_vOk   - Bitmask vector where bit number m_nXid is
//                  set if in the interior of the contour.
//  All of this is implemented in the internal class: CInterior
//
//  Note that CInterior allows for arbitrarily shaped contours, with
//  holes and multilobed regions. Construction time uses interior
//  fill algorithms to construct the interior from the point array.
//
//  Author:
//    Jason Venema
//    NSCL
//    Michigan State University
//    East Lansing, MI  48824-1321
//    mailto:venemaja@msu.edu
//
/////////////////////////////////////////////////////////////////////////////

#include "GammaContour.h"
#include <algorithm>
#include <histotypes.h>
#include <iostream.h>

static const char* Copyright =
"CGammaContour.cpp: Copyright 2001 NSCL, All rights reserved\n";

// functions for class CContour

////////////////////////////////////////////////////////////////////////////
//
//  Function:
//    CGammaContour(const vector<CPoint>& rPoints)
//  Operation Type:
//    construction
//  Note:
//    GenerateInterior() is part of the base class
//
CGammaContour::CGammaContour(const vector<CPoint>& rPoints) :
  CContour(0,0, rPoints)
{
  vector<string> empty;
  m_vSpecs = empty;
  GenerateInterior();
}

////////////////////////////////////////////////////////////////////////////
//
//  Function:
//    CGammaContour(const vector<CPoint>& rPoints)
//  Operation Type:
//    construction
//  Note:
//    GenerateInterior() is part of the base class
//
CGammaContour::CGammaContour(const vector<CPoint>& rPoints,
			     const vector<string>& rSpecs) :
  CContour(0,0, rPoints)
{
  m_vSpecs = rSpecs;
  GenerateInterior();
}

///////////////////////////////////////////////////////////////////////////
//
//  Function:
//    CGate* clone()
//  Operation Type:
//    Virtual construction
//  Purpose:
//    Constructs a copy of this gate.
//
CGate*
CGammaContour::clone()
{
  return new CGammaContour(*this);
}

///////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string Type (  ) const
//  Operation Type:
//    Selector
//  Purpose:
//    Return the gate type which in this
//    case is "gc" for gamma contour
// 
std::string
CGammaContour::Type() const
{
  return std::string("gc");
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t inGate ( CEvent& rEvent, vector<UInt_t>& Params )
//  Operation Type:
//     Evaluator
//
Bool_t
CGammaContour::inGate(CEvent& rEvent, const vector<UInt_t>& Params)
{
  // The parameters should have been validated by this point
  UInt_t nx = Params[0];
  UInt_t ny = Params[1];
  if((nx < rEvent.size()) && (ny < rEvent.size())) {
    if(rEvent[nx].isValid() && rEvent[ny].isValid()) {
      Int_t x = rEvent[nx] - getLowerLeft().X();
      Int_t y = rEvent[ny] - getLowerLeft().Y();
      if((x < 0) || (y < 0))
	return kfFALSE;
      else
	return (getInterior().Inside(x, y));
    }
    else 
      return kfFALSE;
  }
  return kfFALSE;
}
