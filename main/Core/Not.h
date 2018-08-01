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


//  CNot.h:
//
//    This file defines the CNot class.
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

#ifndef NOT_H  //Required for current class
#define NOT_H
                               
                               //Required for 1:1 associated classes
#include "GateContainer.h"
#include "Gate.h"		// Required for base class.
#include "ConstituentIterator.h"
#include <vector>
#include <string>

class CNot : public CGate
{
  
  CGateContainer* m_pConstituent;
  
public:
			// Constructor

  CNot (CGateContainer& rGate) : m_pConstituent(&rGate) 
  { } 
  virtual  ~CNot ( ) { }       //Destructor

  CNot (const CNot& aCNot ) 
  { 
    m_pConstituent = aCNot.m_pConstituent;
  }                                     

			//Operator= Assignment Operator

  CNot& operator= (const CNot& aCNot)
  { 
    if (this == &aCNot) return *this;          
    m_pConstituent = aCNot.m_pConstituent;
    return *this;                                                                                                 
  }                                     

			//Operator== Equality Operator - no such operator.
private:
  int operator== (const CNot& aCNot);
public:

  // Selector:

  const CGateContainer* getGate() const
  {
    return m_pConstituent;
  }
  // Mutator:
protected:
  void setConstituent (CGateContainer* pGate)
  {
    m_pConstituent = pGate;
  }
  // class operations for the object:
  //
public:
  virtual   CConstituentIterator Begin ()  ;
  virtual   CConstituentIterator End ()  ;
  virtual   UInt_t Size ()  ;
  virtual   std::string GetConstituent (CConstituentIterator& rp)  ;
  virtual   CGate* clone ()  ;
  virtual   std::string Type ()  const;
  virtual   Bool_t inGate(CEvent& rEvent, const std::vector<UInt_t>& Params);
  virtual   Bool_t inGate(CEvent& rEvent);
};

#endif
