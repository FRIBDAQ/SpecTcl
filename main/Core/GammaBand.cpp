/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321
*/
static const char* Copyright = "(C) Copyright Michigan State University 2007, All rights reserved";
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

#include <config.h>
#include "GammaBand.h"
#include <assert.h>
#include <algorithm>
#include <iostream>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif
/*
   Change Log:
   $Log$
   Revision 5.2  2005/06/03 15:19:20  ron-fox
   Part of breaking off /merging branch to start 3.1 development

   Revision 5.1.2.1  2004/12/21 17:51:21  ron-fox
   Port to gcc 3.x compilers.

   Revision 5.1  2004/11/29 16:56:02  ron-fox
   Begin port to 3.x compilers calling this 3.0

   Revision 4.5  2003/08/25 16:25:31  ron-fox
   Initial starting point for merge with filtering -- this probably does not
   generate a goo spectcl build.

   Revision 4.4  2003/04/15 19:15:45  ron-fox
   To support real valued parameters, primitive gates must be internally stored as real valued coordinate pairs.

*/

// Functions for class CGammaBand
/*!
   Equality comparison.  Two gamma bands are equal if their underlying
   bands are equal and their spectrum list are equal:
 
*/
int
CGammaBand::operator==(const CGammaBand& rhs) const
{
  return (CBand::operator==(rhs)                  &&
	  (m_vSpecs   == rhs.m_vSpecs));
}

////////////////////////////////////////////////////////////////////////
//
//  Function:
//    CGammaBand (const vector<FPoint>& rPoints)
//  Operation Type:
//    Constructor
//  Note:
//    Function CreateLimits() is defined in the base class
//
CGammaBand::CGammaBand(const vector<FPoint>& rPoints) :
  CBand(0, 0, rPoints)
{
  vector<string> empty;
  m_vSpecs = empty;

}

////////////////////////////////////////////////////////////////////////
//
//  Function:
//    CGammaBand (const vector<FPoint>& rPoints,
//                const vector<UInt_t> rIds)
//  Operation Type:
//    Constructor
//
CGammaBand::CGammaBand(const vector<FPoint>& rPoints,
		       const vector<string> rSpecs) :
  CBand(0, 0, rPoints)
{
  m_vSpecs = rSpecs;

}

////////////////////////////////////////////////////////////////////////
//
//  Function:
//    CGammaBand (UInt_t nPts, FPoint* pPoints)
//  Operation Type:
//    Constructor
//
CGammaBand::CGammaBand(UInt_t nPts, FPoint* pPoints) :
  CBand(0, 0, nPts, pPoints)
{

  vector<string> empty;
  m_vSpecs = empty;

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
  if(xPar >= rEvent.size() || yPar >= rEvent.size()) {
    return kfFALSE;
  }
  if(rEvent[xPar].isValid() && rEvent[yPar].isValid()) {
    Float_t x = rEvent[xPar];
    Float_t y = rEvent[yPar];
    return Interior(x,y);
  }
  else {
    return kfFALSE;
  }
}
