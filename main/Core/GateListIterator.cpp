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
#include <config.h>
#include "GateListIterator.h"                               

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

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
