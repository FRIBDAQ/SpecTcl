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

#ifndef EVENTLIST_H // Required for current class
#define EVENTLIST_H

#include <histotypes.h>
#include "Event.h"
#include <vector>

typedef std::vector<CEvent*>   CEventVector;
typedef CEventVector::iterator CEventListIterator;
typedef CEvent *pCEvent;

class CEventList {
  // Class Attributes:
  CEventVector       m_rvEvents;
  
 public:
  // Constructors and destructors:
  CEventList();
  CEventList(UInt_t nEvents); // Sized construction.
  CEventList(const CEventList& aEventList);
  ~CEventList();

  // Other canonical operations:
  CEventList& operator= (const CEventList& aEventList);
  int operator== (const CEventList& aEventList);
  int operator!=(const CEventList& anEventList) {
    return !(operator==(anEventList));
  }

  // Exported operations:
 public:
  pCEvent& operator[](UInt_t nEvent);
  CEventVector& getVector() {return m_rvEvents;}
  CEventListIterator begin();
  CEventListIterator end();
  UInt_t size() const;
  void clear();

  
  // Utility functions (Available to derived classes too):
 protected:
  void DoAssign(const CEventList& anEventList);
  void Expand(); // BUG: Do not run on construction, or else CAnalyzer::CreateEvent will fail.

};

#endif
