//  CAConstituentIterator.cpp
// Abstract base class for actual constituent iterators.
// This class defines the common iterfaces for the
// ConstituentIterator classes.  Typically these will be
// conatined within  a CConstituentIterator.
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


#include "ActualIterator.h"                               
#include <assert.h>

static const char* Copyright = 
"AConstituentIterator.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CAConstituentIterator


//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int operator!= ( CAConstituentIterator& rRhs )
//  Operation Type:
//     comparison.
//
int 
CAConstituentIterator::operator!=(CAConstituentIterator& rRhs) const 
{
// Inverse of operator==,
// Relies on virtual operator==.
// Exceptions:  

  return !(operator==(rRhs));
}
//////////////////////////////////////////////////////////////////////////////
//
// Function:
//    int operator== (const CAConstituentIterator& aCAConstituentIterator)
// Operation Type:
//    Comparison.
//
int
CAConstituentIterator::operator==(const CAConstituentIterator& rhs) const
{
   assert(0);                     // Should never call actual comparison!!
}
