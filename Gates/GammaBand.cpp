// CGammaBand.cpp
// Encapsulates a GammaBand gate. A gamma band gate is a
// set of limit points. The gate is made if the parameter
// space point lies below the limit point line.
// A choice had to be made about how to construct the
// limit list when the band point list doesn't extend to
// the ends of the parameter range. Arbitrarily, the decision
// was made to extend the gate horizontally to the ends of the
// parameter space range.
//
//  Author:
//    Jason Venema
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:venemaja@msu.edu
//
/////////////////////////////////////////////////////////////////////////

#include "GammaBand.h"
#include <assert.h>
#include <algorithm>
#include <iostream.h>

static const char* Copyright = 
"GammaBand.cpp: Copyright 2001 NSCL, All rights reserved\n";

// Functions for class CGammaBand

////////////////////////////////////////////////////////////////////////
//
//  Function:
//    CGammaBand (const vector<CPoint>& rPoints)
//  Operation Type:
//    Constructor
//  Note:
//    Function CreateLimits() is defined in the base class
//
CGammaBand::CGammaBand(const vector<CPoint>& rPoints) :
  CBand(0, 0, rPoints)
{
  vector<string> empty;
  m_vSpecs = empty;
  CreateLimits();
}

////////////////////////////////////////////////////////////////////////
//
//  Function:
//    CGammaBand (const vector<CPoint>& rPoints,
//                const vector<UInt_t> rIds)
//  Operation Type:
//    Constructor
//
CGammaBand::CGammaBand(const vector<CPoint>& rPoints,
		       const vector<string> rSpecs) :
  CBand(0, 0, rPoints)
{
  m_vSpecs = rSpecs;
  CreateLimits();
}

////////////////////////////////////////////////////////////////////////
//
//  Function:
//    CGammaBand (UInt_t nPts, CPoint* pPoints)
//  Operation Type:
//    Constructor
//
CGammaBand::CGammaBand(UInt_t nPts, CPoint* pPoints) :
  CBand(0, 0, nPts, pPoints)
{
  vector<string> empty;
  m_vSpecs = empty;
  CreateLimits();
}

//////////////////////////////////////////////////////////////////////
//
//  Function:
//    CGate* clone ()
//  Operation Type:
//    Virtual construction
//  Purpose:
//    Returns a new gate which is
//    identical to this one.
//
CGate*
CGammaBand::clone()
{
  return new CGammaBand(*this);
}

//////////////////////////////////////////////////////////////////////
//
//  Function:
//    std::string Type()
//  Operation Type:
//    Selector
//  Purpose:
//    Returns the gate type. In this case, this
//    is the string "gb"
//
std::string
CGammaBand::Type() const
{
  return std::string("gb");
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t inGate ( CEvent& rEvent, vector<UInt_t>& Params )
//  Operation Type:
//     Evaluator
//
Bool_t
CGammaBand::inGate(CEvent& rEvent, const vector<UInt_t>& Params)
{
  // We may assume that the parameter has been validated already
  UInt_t xPar = Params[0];
  UInt_t yPar = Params[1];
  if(xPar >= rEvent.size() || yPar >= rEvent.size())
    return kfFALSE;
  else {
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
    else
      return kfFALSE;
  }
}
