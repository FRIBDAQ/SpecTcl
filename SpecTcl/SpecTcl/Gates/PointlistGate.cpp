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


#include "PointlistGate.h"                               
#include "PointlistIterator.h"

#include <stdio.h>

static const char* Copyright = 
"PointListGate.cpp: Copyright 1999 NSCL, All rights reserved\n";

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
			       CPoint* pPoints) :
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
			       UInt_t *xCoords, UInt_t *yCoords) :
  m_nxId(nXId),
  m_nyId(nYId)
{
  for(UInt_t n = 0; n < nPts; n++) {
    CPoint pt(*xCoords, *yCoords);
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
  
  std::vector<CPoint>::iterator p = pI->getPointIterator();

  sprintf(Formatted, "%d %d", (*p).X(), (*p).Y());
  return std::string(Formatted);
  
}
