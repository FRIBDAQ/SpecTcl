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
//  CPointListGate.cpp
// Base class for the set of gates which can be expressed as 
// a list of x/y points in a 2-d parameter space.
// In addition to parameter indices, these contain a set of points.
// which repsresent the original set of gate points in parameter space.
//

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
/*
  Change log:
  $Log$
  Revision 5.4  2007/02/23 20:38:17  ron-fox
  BZ291 enhancement... add gamma deluxe spectrum type (independent x/y
  parameter lists).

  Revision 5.3  2005/09/22 12:41:47  ron-fox
  2dl spectra in Xamine and other misc stuff.. including making
  void functions return values in all paths, including exception
  exits since g++3.x and higher likes that.

  Revision 5.2  2005/06/03 15:19:20  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.2  2005/05/27 17:47:36  ron-fox
  Re-do of Gamma gates also merged with Tim's prior changes with respect to
  glob patterns.  Gamma gates:
  - Now have true/false values and can therefore be applied to spectra or
    take part in compound gates.
  - Folds are added (fold command); and these perform the prior function
      of gamma gates.

  Revision 5.1.2.1  2004/12/21 17:51:22  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:56:03  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.5  2003/08/25 16:25:32  ron-fox
  Initial starting point for merge with filtering -- this probably does not
  generate a goo spectcl build.

  Revision 4.4  2003/04/15 19:15:42  ron-fox
  To support real valued parameters, primitive gates must be internally stored as real valued coordinate pairs.

*/
#include <config.h>
#include "PointlistGate.h"                               
#include "PointlistIterator.h"
#include <stdio.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif
/*!
   Comparison.  Two point list gates are equal if the base class
   indicates equality, and all components of the two bojects are
   equal:
   \param  <TT>rhs (const CPointListGate& [in])</TT>
      The gate to which this will be compared.
   \retval <TT>int</TT>
   - kfTRUE  if the comparison indicated equality.
   - kfFALSE if the comparison indicated inequality.
*/
int
CPointListGate::operator==(const CPointListGate& rhs) const
{
  return (CGate::operator==(rhs)                  &&
	  (m_nxId     == rhs.m_nxId)              &&
	  (m_nyId     == rhs.m_nyId)              &&
	  (m_aConstituents == rhs.m_aConstituents));
}

// Functions for class CPointListGate
//////////////////////////////////////////////////////////////////////////
//
// Function:
//   CPointListGate(UInt_t nXId, UInt_t nYId, UINt_t nPts,
//  		    CPoint* pPoints)
// Operation Type:
//   Construction
//
CPointListGate::CPointListGate(UInt_t nXId, UInt_t nYId, UInt_t nPts,
			       FPoint* pPoints) :
  m_nxId(nXId),
  m_nyId(nYId)
{
  for(UInt_t n = 0; n < nPts; n++) {
    m_aConstituents.push_back(*pPoints);
    pPoints++;
  }
}

//////////////////////////////////////////////////////////////////////////
//
// Function
//   CPointListGate(UInt_t nXId, UInt_t nYId, UInt_t nPts,
//		    UInt_t *xCoords, UInt_t *yCoords)
// Operation Type:
//   Construction
//
CPointListGate::CPointListGate(UInt_t nXId, UInt_t nYId, UInt_t nPts,
			       Float_t *xCoords, Float_t *yCoords) :
  m_nxId(nXId),
  m_nyId(nYId)
{
  for(UInt_t n = 0; n < nPts; n++) {
    FPoint pt(*xCoords, *yCoords);
    m_aConstituents.push_back(pt);
    xCoords++;
    yCoords++;
  }
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CConstituentIterator Begin (  )
//  Operation Type:
//     Selector.
//
CConstituentIterator
CPointListGate::Begin() 
{
// Returns an iterator which represents the first element of
// the point array (A CPointListIterator).
// Exceptions:  

  CPointListIterator p(m_aConstituents.begin());
  return CConstituentIterator(p);
 }
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CConstituentIterator End (  )
//  Operation Type:
//     
//
CConstituentIterator 
CPointListGate::End() 
{
// Returns an iterator representing the end of the
// point list.
// Exceptions:  

  CPointListIterator p(m_aConstituents.end());
  return CConstituentIterator(p);
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t Size (  )
//  Operation Type:
//     Selector
//
UInt_t 
CPointListGate::Size() 
{
// Returns the number of elements
// in the point list.
// Exceptions:  

  return m_aConstituents.size();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string GetConstituent ( CConstituentIterator& rIterator )
//  Operation Type:
//     Selector.
//
std::string 
CPointListGate::GetConstituent(CConstituentIterator& rIterator) 
{
// Returns the textual representation
// of the constituent represented by the
// iterator.
//
// Formal Parameters:
//     CConstituentIterator& rIterator:
//         Represents the item to return.
// Returns:
//   The textual representation of a point is the
//   string: x y
//   where x is the ascii representation of the x coordinate and
//              y is the ascii representation of the y coordinate.


  char Formatted[100];

  CPointListIterator* pI = (CPointListIterator*)rIterator.getActualIterator();
  
  std::vector<FPoint>::iterator p = pI->getPointIterator();

  sprintf(Formatted, "%f %f", (*p).X(), (*p).Y());
  return std::string(Formatted);
  
}
/*!
   Utility function to determine if there was a line crossing between the
   horizontal ray extended to the left of the point (x,y), and the
   segment defined by f,s.


   Crossing is determined as follows:
   # If the point is above or below both endpoints, there's no line crossing
   # If the line enpoints are both to the right of the point there's no
     crossing for a line extended to the left.
   # If the line's enpoints are both to the left (and test1 passed) trivially
     there's a crossing.
   # If the line's enpoints 'straddle' the x position of the point,
     the x coordinate of the point on the line at the same y coordinate as
     the point is computed.  If it's to the left of

   See inGate for information about how this is determined.
   \param x  (int) X coordinate of the point to check.
   \param y  (int) Y coordinate of the point to check.
   \param f (vector<CPoint>::iterator [in]) Iterator to the segment's first point.
   \param s (vector<CPoint>::iterator[in]) Iterator to the segment's second pt.

   \return 0 if there's no intersection, 1 if there is.
*/
int
CPointListGate::Crosses(Float_t x, Float_t y, 
		      vector<FPoint>::iterator f,
		      vector<FPoint>::iterator s)
{

  Float_t y1 = f->Y();
  Float_t y2 = s->Y();

  if ((y < y1) && (y < y2)) return 0; // Segement is above point.
  if ((y >= y1) && (y >= y2)) return 0; // Segment is below point.

  Float_t x1 = f->X();
  Float_t x2 = s->X();

  if(( x < x1) && (x < x2)) return 0; // Segment is to right of point.
  if(( x>= x1) && (x >= x2)) return 1; // Segment is wholly left of point.

  // Need to see where the line segment is at y.
  //

  if(y2 == y1) {
    return y == y2;		// If flat crosses if on th eline.
  }
  float invslope = float(x2 - x1)/float(y2 - y1);
  float xp = float(x1) + invslope*float(y - y1);
  return (xp <= float(x));

}
/*!
   Determines if an event is in a gate.  This delegates to
  the Inside function however we do figure out if the parameters
  are present for it:
*/
Bool_t
CPointListGate::inGate(CEvent& rEvent)
{
  size_t nParams = rEvent.size();
  if((m_nxId < nParams) && (m_nyId < nParams)) {
    if (rEvent[m_nxId].isValid() && rEvent[m_nyId].isValid()) {
      return Inside(rEvent[m_nxId], rEvent[m_nyId]);
    }
  }
  return kfFALSE;
}
