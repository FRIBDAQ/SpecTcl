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
#include <config.h>
#include "Band.h"                               
#include <algorithm>
#include <assert.h>
#include <iostream>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/* 
  Change log:
  $Log$
  Revision 5.2  2005/06/03 15:19:04  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.1  2004/12/21 17:51:20  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:56:01  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.7  2003/04/15 19:15:46  ron-fox
  To support real valued parameters, primitive gates must be internally stored as real valued coordinate pairs.

*/

// X ordering comparison for CPoints:
//

class Xorder {
public:
  int operator()(const FPoint& p1,const FPoint& p2)
  {
    return (p1.X() < p2.X());
  }
};

// Functions for class CBand

/*!
  Constructor.  Creates a band given a set of points:
  \param  <TT>nXid (UInt_t [in])</TT> 
     Id of the x parameter involved in the gate.
  \param  <TT>nYid (UInt_t [in])</TT>
     Id of the y parameter involved in the gate.
  \param  <TT>points (const vector<FPoint>& points [in])</TT>
     array of points to make up the band.
*/
CBand::CBand(UInt_t nXid, UInt_t nYid,
	     const vector<FPoint>& points)   :
  CPointListGate(nXid, nYid, points)
{
  GetLRLimits();
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CBand (UInt_t nXid, UInt_t nYid,
//  	     UInt_t nPts, FPoint* pPoints);
// Operation Type:
//    Constructor.
//
CBand::CBand(UInt_t nXid, UInt_t nYid,
	     UInt_t nPts, FPoint* pPoints) :
  CPointListGate(nXid, nYid, nPts, pPoints)
{
  GetLRLimits();
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CBand (UInt_t nXid, UInt_t nYid,
//	     UInt_t nPts, Float_t_t* pX, Float_t* pY)
// Operation Type:
//   Constructor
//
CBand::CBand(UInt_t nXid, UInt_t nYid,
	     UInt_t nPts, Float_t* pX, Float_t* pY) :
  CPointListGate(nXid, nYid, nPts, pX, pY)
{
  GetLRLimits();
}

Bool_t
CBand::operator()(CEvent& rEvent)
{
  if(!wasChecked()) {
    Set(CPointListGate::inGate(rEvent));
  }
  
  return getCachedValue();
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
/*!
   Get the left and right most limit points of the band.

   - I have tried to find the x-left/right most and that gives counter 
     intuitive results for pathological bands
   - I am trying the first and last points now.


*/
void
CBand::GetLRLimits()
{
  vector<FPoint>::iterator f = getBegin();
  vector<FPoint>::iterator l = getEnd(); l--;

  if(f->X() < l->X()) {
    m_LeftLimit  = *f;
    m_RightLimit = *l;
  }
  else {
    m_LeftLimit  = *l;
    m_RightLimit = *f;
  }
 
}
/*!
   Return true if the selected point is inside the band.
   \param x   X coord of the point
   \param y   Y coord of the point.
   \return  kfTRUE if the point is interior.
*/
Bool_t
CBand::Interior(Float_t x, Float_t y)
{
    // Now count edges.  
    // First handle the vertical edges separately.

    int nCrosses(0);
    if(y < (m_LeftLimit.Y()) && (x >= m_LeftLimit.X())) nCrosses++;
    if(y < (m_RightLimit.Y()) && (x >= m_RightLimit.X())) nCrosses++;

    // Now tally any additional crossings with the border of the band.

    int nSegments = Size() - 1;
    assert(nSegments >= 1);	// Need at least one segment to make a band.
    vector<FPoint>::iterator first = getBegin();
    vector<FPoint>::iterator second= first; second++;

    for(int seg = 0; seg < nSegments; seg++) {
      nCrosses += Crosses(x,y, first, second);
      first = second;
      second++;
    }
    return ((nCrosses & 1) == 1);
}
/*!
  Equality comparison.  two bands are equal if their point lists are
  equal and if all member data is equal.
*/
int
CBand::operator==(const CBand& rhs) const
{
  return (CPointListGate::operator==(rhs)          &&
	  (m_LeftLimit == rhs.m_LeftLimit)         &&
	  (m_RightLimit== rhs.m_RightLimit));
}
