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

#ifndef __COMPOUNDGATE_H  //Required for current class
#define __COMPOUNDGATE_H
                               //Required for base classes
#ifndef __GATE_H
#include "Gate.h"
#endif                               
                               
                               //Required for 1:M associated classes
#ifndef __GATECONTAINER_H
#include "GateContainer.h"
#endif                                                               


#ifndef __STL_LIST
#include <list>
#define __STL_LIST
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif

class CCompoundGate  : public CGate        
{
  
  list<CGateContainer*> m_vConstituents;
  
public:
			//Default constructor

  CCompoundGate () {}
  CCompoundGate(list<CGateContainer*> pGates);
  CCompoundGate(vector<CGateContainer*> pGates);
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
  list<CGateContainer*>& GetConstituents()  {
    return m_vConstituents;
  }
  //  Mutators:
protected:
  void SetConstituents(list<CGateContainer*> gates) {
    m_vConstituents = gates;
  }
  // Class operations:

public:

  virtual   CConstituentIterator Begin ()  ;
  virtual   CConstituentIterator End ()  ;
  virtual   UInt_t Size ()  ;
  virtual   std::string GetConstituent (CConstituentIterator& rIterator)  ;
  virtual   Bool_t inGate(CEvent& rEvent, vector<UInt_t>& Params) { }

  // Support for incrementally built up compound gates.

  void      AddGateFront(CGateContainer& rGate);
  void      AddGateBack(CGateContainer& rGate);
  void      AddGate(CConstituentIterator& rSpot, CGateContainer& rGate);
  //
  // Utilities.
  //
protected:
  list<CGateContainer*>::iterator 
                         ConvertIterator(CConstituentIterator& rIterator);
};

#endif
