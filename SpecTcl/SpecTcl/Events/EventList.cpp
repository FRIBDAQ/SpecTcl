//  EventList.cpp
// Encapsulates a list of events. 
//  A list of events is what is dispatched to the histogrammer.
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

#include "EventList.h"                               

static const char* Copyright = 
"EventList.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Constants:

static const UInt_t    knExpandSize = 512;      // Events in an expansion.


//////////////////////////////////////////////////////////////////////////
//
// Function:
//   CEventList()
// Operation Type:
//    Default Constructor.
//
CEventList::CEventList() :
  m_nSize(0)
{
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//   CEventList(UInt_t nEvents)
// Operation Type:
//   Parameterized constructor.
//
CEventList::CEventList(UInt_t nEvents)
: m_rvEvents(
#ifdef OSF1
	     (unsigned long int)
#endif
	     nEvents, (CEvent*)kpNULL),
  m_nSize(m_rvEvents.size())
{
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CEventList(const CEventList& anEventList)
// Operation type:
//    Copy Constructor.
//
CEventList::CEventList(const CEventList& anEventList)
: m_rvEvents(
#ifdef OSF1
	     (unsigned long int)
#endif
	     knExpandSize, (CEvent*)kpNULL)
{
  DoAssign(anEventList);
}

/////////////////////////////////////////////////////////////////////////
//
// Function:
//     ~CEventList()
// Operation Type:
//    Destructor
//
CEventList::~CEventList()
{
  clear();			// Delete the events.
  delete &m_rvEvents;		// and the array too.
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:
//     CEventList& operator=(const CEventList& anEventList)
//  Operation Type:
//     Assignment operator.
//
CEventList&
CEventList::operator=(const CEventList& anEventList)
{
	if(this != &anEventList) {
		clear();                           // First clear the current event set.
		DoAssign(anEventList);             // Then complete the assignment.
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CEvent& operator[] (UInt_t  nEvent)
//  Operation Type:
//     Selector and mutator.
//
pCEvent&
CEventList::operator[](UInt_t nEvent) 
{
//  Returns a reference to the selected event.
//  
// Formal Parameters:
//    UInt_t  nEvent:
//      Number of the event to return.
//  NOTE:
//      If nEvent does not yet exist, then one will be
//     generated and inserted in the clear state, and
//     a reference returned.

	while(m_nSize < nEvent) Expand();   // Expand vector as needed.
	return m_rvEvents[nEvent];

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    EventListIterator begin (  )
//  Operation Type:
//     Selector
//
CEventListIterator 
CEventList::begin()
{
// Returns an event list iterator which 'points'
// to the first event in the event list.
//  Iterators are like pointers which support:
//  operator++, operator--, operator*,
//  operator== and operator!=
//
// Exceptions:  

	return m_rvEvents.begin();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    EventListIterator& end (  )
//  Operation Type:
//     Selector
//
CEventListIterator 
CEventList::end()
{
// Returns an event list iterator which can
// be compared against to determine when an
// event list iterator has hit the end of the EventList
//  e.g.:
//    for(CEventListIterator i = eventlist.begin(); 
//         i != eventlist.end(); i++) { ... }
//  processes all of the events in the event list.
//

	return m_rvEvents.end();

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void size (  )
//  Operation Type:
//     selector.
//
UInt_t 
CEventList::size()  const
{
// Returns the number of events in the event list.
//

	return m_nSize;
} 
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void clear (  )
//  Operation Type:
//     mutator
//
void 
CEventList::clear() 
{
// All of the events in the event list are deleted, and the
// pointers to them set to NULL.

	for(CEventListIterator p = begin(); p != end(); p++) {
		delete *p;
		*p = (CEvent*)kpNULL;
	}
	
}
///////////////////////////////////////////////////////////////////////
// Function:
//    void DoAssign(const CEventList& anEvent)
// Operation Type:
//    Utility available to derived classes.
//
void
CEventList::DoAssign(const CEventList& Evt)
{
	// Our vector is assumed to already be cleared.
	//
	// >>>BUGBUGBUG<<< the Event vectors are copied.  This can be
	//                 time consuming.  Therefore at a later time, it
	//                 maybe worthwhile wrapping the event pointers in
	//                 a reference counted pointer class.
	//

  CEventList& anEvent((CEventList&)Evt);
  while(m_rvEvents.size() < anEvent.size()) 
    Expand();   // Make dest. size big enough.
  
  CEventListIterator pDest = begin();
  for(CEventListIterator pSrc = anEvent.begin(); 
      pSrc != anEvent.end(); pSrc++) {
    *pDest = new CEvent(*(*pSrc)); // we get copies of the events
    pDest++;		         // in the rhs.
  }                                // Copies are needed for now to deal
  // with deletions
  m_nSize = m_rvEvents.size();
		                               

}
///////////////////////////////////////////////////////////////////////
//
// Function:
//    void Expand()
// Operation Type:
//    Utility available to derived classes
//
void
CEventList::Expand()
{
	m_rvEvents.insert(end(), (long int)knExpandSize, (CEvent*)kpNULL);
	m_nSize = m_rvEvents.size();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:
//    operator==(const CEventList& aEventList)
//  Operation type:
//    Equality comparision.
//
int
CEventList::operator==(const CEventList& EvtList)
{
  // Equality requires that the event lists are the same size and
  // that the contents of all corresponding events in the both lists 
  // are the same.
  //
  CEventList& aEventList((CEventList&)EvtList);
  if(size() != aEventList.size()) return 0;
  for(int i = 0; i < size(); i++) {
    if(*(operator[](i)) != *(aEventList[i]) ) return 0;
  }
  return -1;
}
