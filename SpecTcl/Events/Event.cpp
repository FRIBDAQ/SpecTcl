//  Event.cpp
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
static const char* Copyright = 
"Event.cpp: Copyright 1999 NSCL, All rights reserved\n";


#include "Event.h"                               
#include <histotypes.h>


#define UNDEFINED -1
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
      m_nSize(m_rvParameters.size())
{
}

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
	: m_rvParameters()
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
	//

	return (m_rvParameters == anEvent.m_rvParameters);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Int_t& operator[] ( UInt_t nParam )
//  Operation Type:
//     Selector/mutator.
//
// Int_t& 
// CEvent::operator[](UInt_t nParam) 
// {
// Returns a reference to the n'th parameter.
//  n is numbered from zero.
// 
// Formal parameters:
//      UInt_t  nParam:
//         Number of the parameter to return a reference to. 
//         Note that if the event is not yet big enough to
//         hold that many parameters it is expanded.
//         If parameter nParam doesn't exist, then
//        an uninitialized one is generated and a reference
//        returned to it.
// Returns:
//        Reference to the parameter value.
//   Resize(nParam);
//   return m_rvParameters[nParam];
// 
// }
CParameterValue&
CEvent::operator[](UInt_t nParam) 
{
  if(nParam >= m_nSize) {	// Only resize if necessary.
    Resize(nParam+1);
  }
  return m_rvParameters[nParam];
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    EventIterator begin (  )
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
//    EventIterator end (  )
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
//

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
  //

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
  //
  int undefined = UNDEFINED;
  for(UInt_t i = 0; i < knExpandSize; i++) {
    m_rvParameters.push_back(undefined);
  }
  m_nSize = m_rvParameters.size();
}








