//  CPointListIterator.cpp
// Encapsulates an iterator which refers to
// one CPoint in a list of points.
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

#include "PointlistIterator.h"                               

static const char* Copyright = 
"CPointListIterator.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CPointListIterator

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CAConstituentIterator& operator++ (  )
//  Operation Type:
//     pre increment.
//
CAConstituentIterator& 
CPointListIterator::operator++() 
{
// Increments the iterator.
//
// Exceptions:  

  ++m_iPoints;
  return *this;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int operator== ( CAConstituentIterator& rRhs )
//  Operation Type:
//     Comparison
//
int 
CPointListIterator::operator==(const CAConstituentIterator& rRhs) const
{
// Compares this with a CAConstituent iterator.
// assumes types are identical.  BUGBUG - later
// use RTTI to confirm this.
//
//
// Exceptions:  

  CPointListIterator& rhs((CPointListIterator&)rRhs);
  return m_iPoints == rhs.m_iPoints;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CAConstituentIterator* clone (  )
//  Operation Type:
//     Construction
//
CAConstituentIterator* 
CPointListIterator::clone() 
{
// Returns a pointer to a point list iterator which
// is identical to this.
// Exceptions:  

  return new CPointListIterator(*this);
}



