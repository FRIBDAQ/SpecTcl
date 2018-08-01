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


//  CGateContainer.h:
//
//    This file defines the CGateContainer class.
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

#ifndef GATECONTAINER_H //Required for current class
#define GATECONTAINER_H

#include "NamedItem.h"
#include "Gate.h"
#include "Event.h"
#include <string>



class CGateContainer : public CNamedItem {
  CGate* m_pGate;

 public:
  //Default constructor
  CGateContainer() : m_pGate(0) {} // Default constructor used by stl.
  CGateContainer (const std::string& sName, UInt_t nNumber, 
		  CGate& rGate);  
  virtual ~CGateContainer();

  //Copy constructor
  CGateContainer(const CGateContainer& aCGateContainer);

  //Operator= Assignment Operator
  CGateContainer& operator=(const CGateContainer& aCGateContainer);

  //  While gates don't compare, containers compare as named items do:
  int operator== (const CGateContainer& aCGateContainer) const {
    return CNamedItem::operator==(aCGateContainer);
  }

  //Get accessor function for 1:1 association

  // Selectors:
 public:
  CGate* getGate() const {
    return m_pGate;
  }

  // Mutators:
 public:                   
  //Set accessor function for 1:1 association
  void setGate (CGate* am_pGate) { 
    delete m_pGate; // This is always dynamic.
    m_pGate = am_pGate->clone();
  }

  CGate& operator* () {
    return *m_pGate;
  }

  CGate* operator-> () {
    return m_pGate;
  }

  Bool_t operator() (CEvent& rEvent) {
    return m_pGate->operator()(rEvent);
  }
};

#endif
