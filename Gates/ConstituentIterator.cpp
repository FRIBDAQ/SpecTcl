//  CConstituentIterator.cpp
// This is a virtualizer for constituent iterators.
// Constituent iterators are highly polymorphic objects
// which select constituents of gates which consist of
// subcomponents which may either be a point or
// another gate.  They are able to be this flexible by
// textualizing the actual contituent.
//   It is convenient to be able to assign, copy and
// otherwise diddle constituent iterators, however this
// cannot be done directly since slicing problems would
// arise.  Therefore, the CConstituent iterator
// contains a reference to an actual iterator CAConstituent iterator
// which knows how to clone itself in copy etc. operations.
// The fact that the actual iterator is maintained by reference,
// and its clone() are used on copies allows CConstituent iterators
// to get around the normal slicing problems.
// Facade pattern.

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


#include "ConstituentIterator.h"                               
#include "ActualIterator.h"

static const char* Copyright = 
"ConstituentIterator.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CConstituentIterator

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CConstituentIterator& operator++ (  )
//  Operation Type:
//     Pre-increment.
//
CConstituentIterator& 
CConstituentIterator::operator++() 
{
// Increments the actual iterator and returns 
// a reference to this.
// Note, this is the most efficient form of ++
//
// Exceptions:  

  m_pActualIterator->operator++();
  return *this;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CConstituentIterator operator++ ( int  )
//  Operation Type:
//     Post increment
//
CConstituentIterator 
CConstituentIterator::operator++(int ) 
{
// Increments the actual iterator, but returns
// a temporary copy of this prior to the increment.
// Exceptions:  

  CConstituentIterator retval(*this);        // Return prior to increment.
  m_pActualIterator->operator++();
  return retval;
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    int operator!= ( CConstituentIterator& rRhs )
//  Operation Type:
//     comparison
//
int 
CConstituentIterator::operator!=(CConstituentIterator& rRhs) const
{
// Just returns the inverse of the == operator.
// Exceptions:  

  return !(operator==(rRhs));
}

////////////////////////////////////////////////////////////////////////////
//
// Function:
//     CConstituentIterator (CAConstituentIterator& rIt)
// Operation Type:
//     Constructor.
//
CConstituentIterator::CConstituentIterator(CAConstituentIterator& rIt) :
  m_pActualIterator(rIt.clone())
{
}
////////////////////////////////////////////////////////////////////////////
//
// Function:
//   CConstituentIterator (const CConstituentIterator& aCConstituentIterator )
// Operation Type:
//   Copy Constructor.
//
CConstituentIterator::CConstituentIterator 
     (const CConstituentIterator& aCConstituentIterator ) :
       m_pActualIterator(aCConstituentIterator.m_pActualIterator->clone())
{ }
///////////////////////////////////////////////////////////////////////////
//
// Function:
//   CConstituentIterator& 
//     operator= (const CConstituentIterator& aCConstituentIterator)
// Operation Type:
//     Assignment.
// 
CConstituentIterator&
CConstituentIterator::operator=
            (const CConstituentIterator& aCConstituentIterator)
{
  if(this != &aCConstituentIterator) {
    delete m_pActualIterator;
    m_pActualIterator = aCConstituentIterator.m_pActualIterator->clone();
  }
  return *this;
}
////////////////////////////////////////////////////////////////////////////
//
//  Function:
//    int operator== 
//      (const CConstituentIterator& aCConstituentIterator);
//  Operation Type:
//    Comparison.
//
int
CConstituentIterator::operator==
   (const CConstituentIterator& aCConstituentIterator) const
{
  // Delegates to the actual constituents.. 
  // BUGBUGBUG -- this presupposes the actual constituents are the
  //              same type.  In the future, RTTI may be used to
  //              determine if this is, in fact, the case.
  //

  return 
    m_pActualIterator->operator==(*aCConstituentIterator.m_pActualIterator);
}



