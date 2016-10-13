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


static const char* Copyright = "(C) Copyright Michigan State University 2006, All rights reserved";

// Event.cpp
// Encapsulates an event.  An event is modelled as a dynamic array of parameters.
     
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
#include "Event.h"
#include <histotypes.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

#define UNDEFINED 0.0
static const Size_t knExpandSize = 256;      // Vector expansion granularity.

static void 
fill(vector<Int_t>& rv, UInt_t n, Int_t value)
{
  for(UInt_t i = 0; i < n; i++) {
    rv.push_back(value);
  }
}

// Functions for class Event

///////////////////////////////////////////////////////////////////////////
//
// Function:
//    CEvent()
// Operation Type:
//    Default Constructor.
//
CEvent::CEvent()
  :  m_rvParameters(),
     m_nSize(0),
     m_nSerial(0)
{}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//     CEvent(UInt_t nInitialSize)
// Operation Type:
//    Constructor.[parameterized]
//
//
//  BUGBUGBUG - On g++ with digital unix anyway, the vector constructor
//                   vector(size, value) seems to recurse and
//                   exhaust memory.
CEvent::CEvent(UInt_t nInitialSize) 
  : m_rvParameters(),
    m_nSize(0),
    m_nSerial(0)
{
  Resize(nInitialSize);
  m_nSize = m_rvParameters.size();
}

//////////////////////////////////////////////////////////////////////////
// 
//  Function:
//     CEvent(const CEvent& anEvent)
//  Operation Type:
//     Copy Constructor.
//
CEvent::CEvent(const CEvent& anEvent) 
  :  m_rvParameters()
{
  m_rvParameters.reserve(anEvent.size());
  DoAssign(anEvent);                      // Assign this to anEvent.
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CEvent& operator=(const CEvent& anEvent)
// Operation Type:
//    Assignment operator.
//
CEvent&
CEvent::operator=(const CEvent& anEvent)
{
  if(this != &anEvent) {
    DoAssign(anEvent);
  }
  return *this;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//   int operator==(const CEvent& anEvent)
// Operation Type:
//   Equality comparison operator.
//
int
CEvent::operator==(const CEvent& anEvent)
{
  // Relies on the fact that STL Vectors define an operator== iff
  // the underlying type does, and Int_t does.
  return (m_rvParameters == anEvent.m_rvParameters);
}


//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    EventIterator begin()
//  Operation Type:
//     // Selector.
//
CEventIterator 
CEvent::begin()
{
  // Returns an interator which cooresponds
  // to the first parameter of the event.
  // The iterator can be used as if it
  //  were a pointer with the operator*,
  //  operator++, operator-- and operator==
  // operator!=
  //  operations defined.
  //
  return m_rvParameters.begin();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    EventIterator end()
//  Operation Type:
//     selector
//
CEventIterator
CEvent::end()
{
  // Returns an iterator which corresponds
  // to the end of the vector, e.g.:
  //    for(EventIterator i = event.begin();
  //             i != event.end(); i++) {...}
  // processes the entire event one parameter
  // at a time.
  return m_rvParameters.end();
}

//////////////////////////////////////////////////////////////////////////
// 
// Function:
//    void DoAssign(const CEvent& anEvent)
// Operation Type:
//    Utility.
//
void
CEvent::DoAssign(const CEvent& anEvent)
{
  // Assigns the value of anEvent to this.
  m_rvParameters = anEvent.m_rvParameters;
  m_nSize = m_rvParameters.size();
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//     void Resize(UInt_t nElements)
// Operation Type:
//     Mutator/protected utility.
//
void
CEvent::Resize(UInt_t nElements)
{
  // Ensures that the event has at least nElement slots
  while(m_rvParameters.size() < nElements) 
    ExpandGranule();
}

////////////////////////////////////////////////////////////////////////
//
// Function:
//   void ExpandGranule()
// Operation type:
//    Mutator/protected utility.
//
void
CEvent::ExpandGranule()
{
  // Expands the size of an array by knExpandSize


  for(UInt_t i = 0; i < knExpandSize; i++) {
    int index = m_nSize + i;
    m_rvParameters.push_back(CParameterValue(m_dopeVector, index, &m_nSerial));
  }
  m_nSize = m_rvParameters.size();
}
