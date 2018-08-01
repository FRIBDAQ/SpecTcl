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

//  CCompoundGate.h:
//
//    This file defines the CCompoundGate class.
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

#ifndef COMPOUNDGATE_H  //Required for current class
#define COMPOUNDGATE_H
                               //Required for base classes
#include "Gate.h"
#include "GateContainer.h"
#include <list>
#include <vector>
#include <string>


class CCompoundGate  : public CGate        
{
  
  std::list<CGateContainer*> m_vConstituents;
  
public:
			//Default constructor

  CCompoundGate () {}
  CCompoundGate(std::list<CGateContainer*> pGates);
  CCompoundGate(std::vector<CGateContainer*> pGates);
  CCompoundGate(UInt_t nGates, CGateContainer** ppGates);
  ~ CCompoundGate ( ) { }       //Destructor

	
			//Copy constructor

  CCompoundGate (const CCompoundGate& aCCompoundGate )   : 
    CGate (aCCompoundGate) 
  { 
    m_vConstituents = aCCompoundGate.m_vConstituents;
  }                                     

			//Operator= Assignment Operator

  CCompoundGate& operator= (const CCompoundGate& aCCompoundGate)
  { 
    if (this == &aCCompoundGate) return *this;          
    CGate::operator= (aCCompoundGate);
    m_vConstituents = aCCompoundGate.m_vConstituents;
    return *this;                                                                                                 
  }                                     

			//Operator== Equality Operator [not allowed]
private:
  int operator== (const CCompoundGate& aCCompoundGate);
public:
  //
  //  Selectors:
  //
  std::list<CGateContainer*>& GetConstituents()  {
    return m_vConstituents;
  }
  //  Mutators:
protected:
  void SetConstituents(std::list<CGateContainer*> gates) {
    m_vConstituents = gates;
  }
  // Class operations:

public:
  virtual void RecursiveReset(); //!< Recursively reset the cache.
  virtual   CConstituentIterator Begin ()  ;
  virtual   CConstituentIterator End ()  ;
  virtual   UInt_t Size ()  ;
  virtual   std::string GetConstituent (CConstituentIterator& rIterator)  ;
//  virtual   Bool_t inGate(CEvent& rEvent, const std::vector<UInt_t>& Params) {
//         return kfFALSE; }
//  virtual   Bool_t inGate(CEvent& rEvent) { return kfFALSE; }
  // Support for incrementally built up compound gates.

  void      AddGateFront(CGateContainer& rGate);
  void      AddGateBack(CGateContainer& rGate);
  void      AddGate(CConstituentIterator& rSpot, CGateContainer& rGate);
  //
  // Utilities.
  //
protected:
  std::list<CGateContainer*>::iterator 
                         ConvertIterator(CConstituentIterator& rIterator);
};

#endif
