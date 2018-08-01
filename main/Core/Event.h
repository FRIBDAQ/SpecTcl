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

//  CEvent.h:
//
//    This file defines the CEvent class.
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

#ifndef EVENT_H //Required for current class
#define EVENT_H

#include <histotypes.h>
#include <vector>
#include <ValidValue.h>

typedef DFloat_t ParamType;
typedef std::vector<DopedValidValue<ParamType> > CParameterVector;
typedef DopedValidValue<ParamType>               CParameterValue;
typedef CParameterVector::iterator               CEventIterator;

class CEvent {
 private:
  DopeVector       m_dopeVector;
  CParameterVector m_rvParameters; // The data parameters.
  Int_t            m_nSize;        // Current vector size (optimization).
  ULong_t          m_nSerial;      // Serial number for bulk validity.

 public:
  // Canonical functions, construction, destruction, assignment and comparison.
  // Ensure initial values entered.
  CEvent();
  virtual ~CEvent() {} // Destructor
  CEvent(UInt_t nInitialSize);
  CEvent(const CEvent& aEvent);

  // Operators.
  CEvent& operator=(const CEvent& aEvent);
  int operator==(const CEvent& aEvent);
  int operator!=(const CEvent& anEvent) {
    return !(operator==(anEvent));
  }
  // Profiling suggests this indexing operator should be
  // inlined.
  //
  CParameterValue& operator[](UInt_t nParam) {
    if(nParam >= m_nSize) {	// Only resize if necessary.
      Resize(nParam+1);
    }
    return m_rvParameters[nParam];
  }

  // Exported operations:
  CEventIterator begin();
  CEventIterator end();
  UInt_t size() const {
    return m_nSize;
  }
  void clear() {
    m_nSerial++;
    m_dopeVector.reset();
  }

  DopeVector& getDopeVector() {
    return m_dopeVector;
  }

  // Utility protected functions:
 protected:
  void DoAssign(const CEvent& anEvent);
  void Resize(UInt_t nElements); // Resizes to hold at least nElements.
  void ExpandGranule(); // Expands vector by knExpandSize.
};

#endif
