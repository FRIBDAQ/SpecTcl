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

static const char* Copyright = "(C) Copyright Michigan State University 2007, All rights reserved";
//  CContour.cpp
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
#include "Contour.h"                               
#include <algorithm>
#include <histotypes.h>
#include <Iostream.h>
#include <assert.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif
// Local data types:

class YCompare {
public:
  int operator()(const FPoint& p1, const FPoint& p2) {
    return (p1.Y() < p2.Y());
  }
};
class XCompare {
public:
  int operator()(const FPoint& p1, FPoint& p2) {
    return (p1.X() < p2.X());
  }
};




// functions for class CContour

/*!
   Equality comparison.
*/
int
CContour::operator==(const CContour& rhs) const
{
  return (CPointListGate::operator==(rhs)         &&
	  (m_LowerLeft == rhs.m_LowerLeft)        &&
	  (m_UpperRight== rhs.m_UpperRight));
}

/*!
  Construct a contour from an array of floating point points.
  The points define a polygon (last point connected to first 
  point).

  \param <TT>nXId (UInt_t [in]): </TT>
     Id of the parameter on the x axis of the gate.
  \param <TT>nYId (UInt_t [in]): </TT>
     Id of the parameter on the y axis of the gate.
  \param <TT>Points (vector<FPoint>& [in]):
     Vector of points that define the exterior boundary of the gate.



*/
CContour::CContour(UInt_t nXId, UInt_t nYId, 
		   const  vector<FPoint>& Points) :
  CPointListGate(nXId, nYId, Points)
{
  GenerateInterior();
}
/*!
   Overloaded contour contructor:

  \copydoc CContour(UInt_t,UInt_t,const vector<FPoint>&)
  \param <TT>nPts    [UInt_t [in]):</TT>
      Number of points that define the boundary.  The 
      xCoords and yCoords arrays must have at least this
      many points.
  \param <TT>xCoords [UFloat_t* [in]):</TT>
      Pointer to the array of xcoordinates of the boundary
      points.
  \param <TT>yCoords (UFloat_t* [in]):</TT>
      Pointer to the array of y coordinates of the boundary
      points
  
Only the first nPts points in the xCoords and yCoords arrays
are used to construct the contour.
*/
CContour::CContour (UInt_t nXId, UInt_t nYId, UInt_t nPts,
		    Float_t *xCoords, Float_t *yCoords) :
  CPointListGate(nXId, nYId, nPts, xCoords, yCoords)
{
  GenerateInterior();
}
/*!
  Overloaded contour constructor:

  \copydoc CContour(UInt_t,UInt_t,const vector<FPoint>&)
  \param <TT>nPts (UInt_t [in]):
     Number of points that define the boundary.
  \param <TT>pPoints (FPoint* [in]):</TT>
     A pointer to an array of FPoint objects.  Each FPoint
     defines a boundary point for the contour.
*/
CContour::CContour (UInt_t nXId, UInt_t nYId, UInt_t nPts,
		    FPoint* pPoints) :
  CPointListGate(nXId, nYId, nPts, pPoints)
{
  GenerateInterior();
}
/*!
  Evaluate the contour.  Note that the gate has a cached
  value associated with it.  Once evaluated, it will stubbornly
  return the same value without rechecking the gate until
  the cache is cleare via a call to: CGate::Reset().
  \param <TT>rEvent (CEvent& [in]):</TT>
     The event for which the gate is evaluated.
  \retval Bool_t
     - kfTRUE if the gate was made.
     - kfFALSE if the gate was not made.
*/
Bool_t
CContour::operator()(CEvent& rEvent)
{
  if(!wasChecked()) {
    Set(inGate(rEvent));
  }
  return getCachedValue();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t inGate ( CEvent& rEvent, vector<UInt_t>& Params )
//  Operation Type:
//     Evaulator
//
Bool_t
CContour::inGate(CEvent& rEvent, const vector<UInt_t>& Params)
// Evaluates the gate.  This determines
// if the parameter has made the gate.
// Note that if there is a cached evaulation
// we use that instead.  Otherwise the gate is
// evaluated and cached for future use.
//
// Formal Parameters:
//     CEvent& rEvent:
//           The event against which the gate is being checked.
//     vector<UInt_t>& Params
//           The vector of params in the gate (empty)
//
// Exceptions:  
{
  return inGate(rEvent);
}

/*!
   Determines if the event is inside the gate.  This is done by countingin two 
   steps:
   - The point must be inside the bounding box of the contour.  If not,
     the gate is clearly not made.
   - We check to see if there are an even or odd number of edge crossing
     to the left of this point (extend a horizontal line to the left from
     the point -> infinity:  Pairs of points define line segments.  
 the point or on the
       point, there's a crossing, otherwise, not
   - If the number of edge crossings is odd, the point is inside the contour
     otherwise outside.
       
     \param rEvent (CEvent& [in]): The event to check against the gate.
     The gate's x/y id's determine which elemetns of the event are the
     x/y point coordinates.

     \retval Bool_t
        - kfTRUE if gate's parameters for rEvent are inside the 
	  contour as described above.
        - kfFALSE if the gate's parameters for rEvent are outside
	  the contour.
*/

Bool_t
CContour::inGate(CEvent& rEvent)
{
  UInt_t nx = getxId();
  UInt_t ny = getyId();
  if((nx < rEvent.size()) && (ny < rEvent.size())) {
    if(rEvent[nx].isValid() && rEvent[ny].isValid()) {
      Float_t x = rEvent[nx];
      Float_t y = rEvent[ny];
      return Inside(x,y);
    }
    else {			// Some parameters are not valid.
      return kfFALSE;
    }                             // Event array not even big enough
    return kfFALSE;
  }
  return kfFALSE;
}
  /*!
   USed by this and subclasses to determine if a point is in the interior
   of the region described by the gate:
*/
Bool_t
CContour::Inside(Float_t x, Float_t y)
{
  if((x < m_LowerLeft.X())   || (x > m_UpperRight.X()) ||
     (y < m_LowerLeft.Y())   || (y > m_UpperRight.Y())) {
    return kfFALSE;		// Outside bounding box.
  }
  else {			// Here we need to count left crossings.
    int nSegments = Size();
    assert(nSegments > 2);	// Required to make a contour.
    vector<FPoint>::iterator first = getBegin();
    vector<FPoint>::iterator second= first; second++;
    int nCrossings = 0;
    for(int seg = 0; seg < nSegments; seg++) {
      
      // Increment crossings if appropriate.
      
      nCrossings += Crosses(x,y, first, second);
      
      //
      // Get iterators to the the next segment endpoints.
      //
      first = second;
      second++;
      if(second == getEnd()) {
	second = getBegin();
      }
      
    }
      return ((nCrossings & 1) == 1); // True if odd number of crossings.
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
CContour::clone() 
{
// Constructs a copy of this gate.  
// This must eventually be explicitly deleted.
//
// Exceptions:  

  return new CContour(*this);
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string Type (  ) const
//  Operation Type:
//     Selector
//
std::string 
CContour::Type() const
{
// Return the gate type which in this
// case is "c" for contour.
//
// Exceptions:  

  return std::string("c");
}
/*!
   
  In this version the only thing we do is figure out the corners of the
  region's bounding box.  The corners are used to do a quick check on 
  insidedness:  If points are outside this box, there's no point in the
  more time consuming edge crossing check algorithm.  For more information
  see inGate.
*/
void
CContour::GenerateInterior()
{
  //  First figure out the corners of the interior region,
  //  from that, we'll be able to size the interior, as well as set the
  //  corner members.
  //
  vector<FPoint> Polygon = getPoints();
  vector<FPoint>::iterator p;
  YCompare yc;
  XCompare xc;

  p = min_element(Polygon.begin(), Polygon.end(), yc);
  Float_t yl = (*p).Y();
  p = max_element(Polygon.begin(), Polygon.end(),  yc);
  Float_t yh = (*p).Y();

  p = min_element(Polygon.begin(), Polygon.end(), xc);
  Float_t xl = (*p).X();
  p = max_element(Polygon.begin(), Polygon.end(), xc);
  Float_t xh = (*p).X();
  m_LowerLeft   = FPoint(xl, yl);
  m_UpperRight  = FPoint(xh, yh);

}
