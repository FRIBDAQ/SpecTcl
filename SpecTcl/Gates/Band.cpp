//  CBand.cpp
// Encapsulates a Band gate.  A band gate is a
// set of limit points.  The gate is made if the parameter
// space point lies below the limit point line.
// A choice had to be made about how to construct the
// limit list when the band point list doesn't extend to the
// ends of the parameter range.  Arbitrarily, the decision was
// made to extend the gate horizontally to the ends of the parameter
//  space range.
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

#include "Band.h"                               
#include <algorithm>
#include <assert.h>
#include <iostream.h>

static const char* Copyright = 
"Band.cpp: Copyright 1999 NSCL, All rights reserved\n";

// X ordering comparison for CPoints:
//

class Xorder {
public:
  int operator()(const CPoint& p1,const CPoint& p2)
  {
    return (p1.X() < p2.X());
  }
};

// Functions for class CBand

//////////////////////////////////////////////////////////////////////////
//  
// Function:
//   CBand (UInt_t nXid, UInt_t nYid,
//	    vector<CPoint>& points)
// Operation Type:
//    Constructor.
//
CBand::CBand(UInt_t nXid, UInt_t nYid,const vector<CPoint>& points)   :
  CPointListGate(nXid, nYid, points)
{
  CreateLimits();
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CBand (UInt_t nXid, UInt_t nYid,
//  	     UInt_t nPts, CPoint* pPoints);
// Operation Type:
//    Constructor.
//
CBand::CBand(UInt_t nXid, UInt_t nYid,
	     UInt_t nPts, CPoint* pPoints) :
  CPointListGate(nXid, nYid, nPts, pPoints)
{
  CreateLimits();
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CBand (UInt_t nXid, UInt_t nYid,
//	     UInt_t nPts, UInt_t* pX, UInt_t* pY)
// Operation Type:
//   Constructor
//
CBand::CBand(UInt_t nXid, UInt_t nYid,
	     UInt_t nPts, UInt_t* pX, UInt_t* pY) :
  CPointListGate(nXid, nYid, nPts, pX, pY)
{
  CreateLimits();
}

Bool_t
CBand::operator()(CEvent& rEvent)
{
  vector<UInt_t> Params;
  if(!wasChecked()) {
    Params.push_back(getxId());
    Params.push_back(getyId());
    Set(inGate(rEvent, Params));
  }
  
  return getCachedValue();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t inGate ( CEvent& rEvent )
//  Operation Type:
//     Evaulator
//
Bool_t
CBand::inGate(CEvent& rEvent, vector<UInt_t>& Params)
  // Determines if the parameter space point
  //  ( rEvent[m_nxId], rEvent[m_nyId])
  //  is under the gate defined by m_aLimits.
  //  In essence whether or not rEvent[m_nyId) < m_aLimits[m_nxId].
  //
  //  Implemented so that if a cached result is available
  //  it will be used, and if not, will be created.
  //  
  //   If for some reason, m_nXid >= m_aLimits.size(),
  //   or either parameter index does not fall into the current
  //   parameter array, the gate is assumed to have failed.
  //
  // Formal Parameters:
  //      CEvent& rEvent:
  //          Refers to the event to check.
  //      vector<UInt_t>& Params
  //          Refers to the vector of parameters in the gate (empty)
{

  UInt_t xPar = Params[0];
  UInt_t yPar = Params[1];
  if( (xPar >= rEvent.size()) || (yPar >= rEvent.size())) {
    return kfFALSE;
  }
  else {
    // The gate array is scaled to parameter space, however the user
    // may not have drawn the gate all the way to the end of the 
    // spectrum. So if we're outside the limits array, the gate is
    // not made
    if(rEvent[xPar].isValid() && rEvent[yPar].isValid()) {
      UInt_t x = rEvent[xPar];
      UInt_t y = rEvent[yPar];
      if(x < (getLimits()).size()) {
	return (y < (getLimits())[x]);
      }
      else {
	return kfFALSE;
      }
    }
    else {
      return kfFALSE;
    }
  }
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CGate* clone (  )
//  Operation Type:
//     Virtual construction
//
CGate* 
CBand::clone() 
{
// Returns a new gate which is identical
// to this one.
// Exceptions:  

  return new CBand(*this);
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string Type (  )
//  Operation Type:
//     Selector.
//
std::string 
CBand::Type() const
{
// Returns the gate type.  For a band, this is:
//   std::string("b")
//
// Exceptions:  

  return std::string("b");
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//   void CreateLimits()
// Operation Type:
//   Utilities:
//
void
CBand::CreateLimits()
{
  //  Takes the gate points and turns them into a limit array.
  //  The gate points have been extended by the client as required
  //  (e.g. either horizontally to the ends of the spectrum or at the
  //  zero level or whatever.

  //  The gate points may be out of order, so first they are sorted in
  //  order of ascending X.
  //

  vector<CPoint>::iterator b = getBegin();
  vector<CPoint>::iterator e = getEnd();
  Xorder c;
  sort(b,e, c);

  // There should be at least two points in a band.

  b = getBegin();
  assert(Size() > 1);

  // Now construct the limits array by interpolating pairwise.  There are
  // special cases to consider, in particular vertical  leaps of the band.
  //

  // Run from chan zero to first channel on the
  // x axis.

  CPoint p1((*b).X(),0);
  for(UInt_t i = 0; i <= p1.X(); i++) {
    m_aLimits.push_back(0);
  }


  CPoint p2;
  for(; b != getEnd(); b++) {
    p2 = *b;
    if(p1.X() == p2.X()) { // Vertical leap:
      m_aLimits[Size() - 1] = p2.Y();
    }
    else {			// Draw the line.
      float y = (float)p1.Y();
      int   x = p1.X();
      float m = ((float)p2.Y() - y)/((float)p2.X() - (float)x);
      while(x < p2.X()) {
	m_aLimits.push_back((UInt_t)y);
	y += m;
	x++;
      }
    }
      p1 = p2;
  } 
  
  // The only point we have not filled in is the last one and it is 
  // given by the Y coordinate of the last point:

  m_aLimits.push_back(p2.Y());

}
  
