//  CSingleItemIterator.cpp
// Encapsulates an interator for a 'list' containing a single item.
// This is used to make single itemed, and 0 itemed gates
//  look like they have constituent lists.

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


#include "SingleItemIterator.h"                               

static const char* Copyright = 
"SingleItemIterator.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CSingleItemIterator

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CAConstituentIterator& operator++ (  )
//  Operation Type:
//     pre-increment
//
CAConstituentIterator& 
CSingleItemIterator::operator++() 
{

// Exceptions:  

  m_fNotEnd = kfFALSE;             // Next is always end.
  return *this;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int operator== ( CAConstituentIterator& rRhs )
//  Operation Type:
//     comparison
//
int 
CSingleItemIterator::operator==(const CAConstituentIterator& rRhs)  const
{
// Compares us to rRhs.  Note that we *assume*
// that rRhs is another CSingleItemIterator.
// BUGBUGBUG - later use RTTI to ensure this
// BUGBUGBUG - currently no way to know the 
//                           iterators 'point' to the same object.
//
// Exceptions:  

  CSingleItemIterator& rhs((CSingleItemIterator&)rRhs);
  return m_fNotEnd == rhs.m_fNotEnd;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CAConstituentIterator* clone (  )
//  Operation Type:
//     Construction
//
CAConstituentIterator* 
CSingleItemIterator::clone() 
{
// Returns a dynamically created clone of
// this.
// Exceptions:  

  return new CSingleItemIterator(*this);
}







