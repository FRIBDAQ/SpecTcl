//  EventList.h:
//
//    This file defines the EventList class.
//
// Author:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef __EVENTLIST_H  //Required for current class
#define __EVENTLIST_H

#ifndef __HISTODEF_H            // Get primitive type definitions.
#include <histotypes.h>
#endif                               
                               //Required for 1:M associated classes
#ifndef __EVENT_H
#include "Event.h"
#endif                                                               

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif

typedef std::vector<CEvent*>   CEventVector;
typedef CEventVector::iterator CEventListIterator;

typedef CEvent *pCEvent;
class CEventList      
{
	// Class Attributes:

  CEventVector m_rvEvents;
  UInt_t       m_nSize;
  
public:
	// Constructors and destructors:

 CEventList ();                   // Default construction
 CEventList (UInt_t nEvents);     // Sized construction.
 CEventList (const CEventList& aEventList ); // Copy Construction.
 ~ CEventList ( );                //Destructor

 // Other canonical operations:

			//Operator= Assignment Operator

 CEventList& operator= (const CEventList& aEventList);

            //Operator== Equality Operator

 int operator== (const CEventList& aEventList);
 int operator!=(const CEventList& anEventList) {
	 return !(operator==(anEventList));
 }
 
 // Exported operations:
public:
 CEventVector& getVector() { return m_rvEvents; }
    pCEvent& operator[] (UInt_t nEvent)  ;
    CEventListIterator begin ();
    CEventListIterator end ();
    UInt_t size ()  const;
    void clear ()  ;

	// Utility functions (Available to derived classes too):
protected:
	void DoAssign(const CEventList& anEventList);
	void Expand();
 
};

#endif
