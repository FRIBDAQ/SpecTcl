

// Class: C2Bands
//    Models a contour which is formed from 2 bands.  The idea is
// that in many detector models, particle id is available in a de/e
// spectrum as hyperbollic bands of counts.  Placing a bunch of 
// hyperbollic band lines allows separation by treating the area
// between adjacent bands as 'in a contour gate formed from these
// bands.'
//  This gate therefore acts just like a contour *however*:
//  1.  The point list is initialized from two bands, the 'upper'
//       and 'lower' band.
//  2.  The Y coordinates in the 'upper' band are decreased by
//       1 in order to make the gate exclusive of points on the
//       upper band to prevent double counting if these are
//       stacked.
//
//   The point list is built up in a clockwise manner in that the
//    we start with the first point of the upper band and continue
//    with the last point of the lower band going in reverse order.
//  
//   A crude attempt is made to ensure that this clockwise ordering is correct:
//   If the X coordinate of each band's first point is less than that of its
//   last point, then the band points are reversed.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//
//
//////////////////////////.cpp file/////////////////////////////////////////////////////
#include "C2Bands.h" 
#include <algorithm>   				

static char* pCopyrightNotice = 
"(C) Copyright 1999 NSCL, All rights reserved .cpp \n";

// Functions for class C2Bands
//////////////////////////////////////////////////////////////////////////
//
//  Function:
//      C2Bands (UInt_t nXId, UInt_t nYId,
//	   vector<CPoint>& rLowBand, 
//	   vector<CPoint>& rHiBand)
// Operation Type:
//   Constructor.
C2Bands:: C2Bands (UInt_t nXId, UInt_t nYId,
		   vector<CPoint>& rLowBand, 
		   vector<CPoint>& rHiBand) :
  CContour(nXId, nYId, MarshallPoints(rLowBand, rHiBand))
  
{ 
  
} 
  

//  Function:       Type()
//  Operation Type: Selector.
std::string C2Bands::Type()  
{
  // Returns the type of gate.. .in this case it's the text:
  //   cb2 indicating a contour from 2 bands.
  //
  return std::string("cb2");
}

/////////////////////////////////////////////////////////////////////////////
//
// Function:
//   std::vector<CPoint> MarshalPoints(vector<CPoint> rLow,
//				       vector<CPoint> rHi) 
// Operation Type:
//    Utility.
//
std::vector<CPoint> 
C2Bands::MarshallPoints(vector<CPoint>& rLow, vector<CPoint>& rHi)
{
  std::vector<CPoint> MarshalledPoints;

  // First reverse the points if needed.
  //   The algorithm is rather naive... if the X coord of the
  // first point is > than that of the last point, the array is reversed.
  //
  UInt_t nPts = rLow.size();
  if(rLow[0].X() > rLow[nPts-1].X()) {
    reverse(rLow.begin(), rLow.end());
  }

  nPts = rHi.size();
  if(rHi[0].X() > rHi[nPts-1].X()) {
    reverse(rHi.begin(), rHi.end());
  }

  // Add the points in the upper band left to right.

    MarshalledPoints.insert(MarshalledPoints.end(), 
			    rHi.begin(),
			    rHi.end());


  // Add the points in the lower band right to left.

    std::vector<CPoint>::reverse_iterator p = rLow.rbegin();
    for(; p != rLow.rend(); p++) {
      CPoint pt = *p;
      MarshalledPoints.push_back(pt);
    }

    return MarshalledPoints;

}
