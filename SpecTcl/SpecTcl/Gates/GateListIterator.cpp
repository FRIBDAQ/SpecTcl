//  CGateListIterator.cpp
// Encapsulates an iterator for a list of gates
// under the common CAConstituentIterator
// umbrella.
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

#include "GateListIterator.h"                               

static const char* Copyright = 
"GateListIterator.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CGateListIterator
//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CGateListIterator (std::list<CGateContainer*>::iterator rIt)
// Operation Type:
//    Constructor.
//
CGateListIterator::CGateListIterator (std::list<CGateContainer*>::iterator rIt)
  :
  m_iGates(rIt)
{ } 

  
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CAConstituentIterator& operator++ (  )
//  Operation Type:
//     pre-increment.
//
CAConstituentIterator& 
CGateListIterator::operator++() 
{
// Increments the iterator and returns *this.
// Exceptions:  

  m_iGates++;
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
CGateListIterator::operator==(const CAConstituentIterator& rRhs) 
{
// Returns true if this iterator  is
// the same as the rRhs one. rRhs is assumed
// to be the same type as us. BUGBUGBUG
// use RTTI later to check this.
// Exceptions:  

  CGateListIterator& rhs((CGateListIterator&)rRhs);
  return m_iGates == rhs.m_iGates;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CAConstituentIterator* clone (  )
//  Operation Type:
//     Virtual construction.
//
CAConstituentIterator* 
CGateListIterator::clone() 
{
// Returns a dynamically allocated CAConstituentIterator
// which is an exact coyp of us.
// Exceptions:  

  return new CGateListIterator(*this);
}
