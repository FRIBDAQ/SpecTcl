//  CGateListIterator.h:
//
//    This file defines the CGateListIterator class.
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

#ifndef __GATELISTITERATOR_H  //Required for current class
#define __GATELISTITERATOR_H
                               //Required for base classes
#ifndef __ACONSTITUENTITERATOR_H
#include "ActualIterator.h"
#endif                     

#ifndef __GATECONTAINER_H
#include "GateContainer.h"
#endif          
                 
#ifndef __STL_LIST
#include <list>
#define __STL_LIST
#endif


              
class CGateListIterator  : public CAConstituentIterator        
{
  std::list<CGateContainer*>::iterator m_iGates;  // Gates array iterator.
  
public:
			// Constructors

  CGateListIterator (std::list<CGateContainer*>::iterator rIt);

  CGateListIterator(std::list<CGateContainer*>& rArray) :
    m_iGates(rArray.begin())
  { }
   ~ CGateListIterator ( ) { }       //Destructor
			//Copy constructor

  CGateListIterator (const CGateListIterator& aCGateListIterator )   : 
    CAConstituentIterator (aCGateListIterator) 
  {   
    m_iGates = aCGateListIterator.m_iGates;
                
  }                                     

			//Operator= Assignment Operator

  CGateListIterator& operator= (const CGateListIterator& aCGateListIterator)
  { 
    if (this == &aCGateListIterator) return *this;          
    CAConstituentIterator::operator= (aCGateListIterator);
    m_iGates = aCGateListIterator.m_iGates;
        
  return *this;
  }                                     

			//Operator== Equality Operator

  virtual
  int operator== (const CAConstituentIterator& aCGateListIterator) const
  { 
    const CGateListIterator& it((const CGateListIterator&)aCGateListIterator);
    return (
	    (m_iGates == it.m_iGates) 
	    );
  }                             
  // Selectors:

public:
  std::list<CGateContainer*>::iterator getGates() const
  {
    return m_iGates;
  }
                       
  // Mutators:

protected:
  void setGates (std::list<CGateContainer*>::iterator& am_iGates)
  { 
    m_iGates = am_iGates;
  }
  // Operations on the object:
public:                   
  virtual   CAConstituentIterator& operator++ ()  ;
  virtual   int operator== (const CAConstituentIterator& rRhs)  ;
  virtual   CAConstituentIterator* clone ()  ;
 

};

#endif
