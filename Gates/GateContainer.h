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

#ifndef __GATECONTAINER_H  //Required for current class
#define __GATECONTAINER_H
                               //Required for base classes
#ifndef __NAMEDITEM_H
#include "NamedItem.h"
#endif                               

#ifndef __GATE_H
#include "Gate.h"
#endif

#ifndef __EVENT_H
#include "Event.h"
#endif
                  
#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif             

  
// Forward class definitions:


                                                               
class CGateContainer  : public CNamedItem        
{
  
  CGate* m_pGate;
  
public:
			//Default constructor

  CGateContainer() : m_pGate(0) {} // Default constructor used by stl.
  CGateContainer (const std::string& sName, UInt_t nNumber, 
		  CGate& rGate);  
  virtual ~ CGateContainer ( );

			//Copy constructor

  CGateContainer (const CGateContainer& aCGateContainer );

			//Operator= Assignment Operator

  CGateContainer& operator= (const CGateContainer& aCGateContainer);


  //  While gates don't compare, containers compare as named items do:
  //:
  int operator== (const CGateContainer& aCGateContainer) const {
    return CNamedItem::operator==(aCGateContainer);
  }

                       
                       //Get accessor function for 1:1 association

  // Selectors:

public:
  CGate* getGate() const
  {
    return m_pGate;
  }
  // Mutators:

public:                   
                       //Set accessor function for 1:1 association
  void setGate (CGate* am_pGate)
  { 
    delete m_pGate;		// This is always dynamic.
    m_pGate = am_pGate->clone();
  }
public:
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
