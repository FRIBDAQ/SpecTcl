//  CAndGate.h:
//
//    This file defines the CAndGate class.
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

#ifndef __ANDGATE_H  //Required for current class
#define __ANDGATE_H
                               //Required for base classes
#ifndef __COMPOUNDGATE_H
#include "CompoundGate.h"
#endif                               
           
#ifndef __GATECONTAINER_H
#include "GateContainer.h"
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif

#ifndef __STL_LIST
#include <list>
#define __STL_LIST
#endif
                    
class CAndGate  : public CCompoundGate        
{
  
public:
			//Default constructor

  CAndGate () : CCompoundGate() { } 
  CAndGate(list<CGateContainer*>& rGates); 
  CAndGate(vector<CGateContainer*>& rGates) :
    CCompoundGate(rGates)
  {}
  CAndGate(UInt_t nGates, CGateContainer** ppGates) :
    CCompoundGate(nGates, ppGates)
  {}
  ~ CAndGate ( ) { }       //Destructor

                           // Copy constructor

  CAndGate (const CAndGate& aCAndGate )   : 
    CCompoundGate (aCAndGate) 
  { 
                
  }                                     

			//Operator= Assignment Operator

  CAndGate& operator= (const CAndGate& aCAndGate)
  { 
    if (this == &aCAndGate) return *this;          
    CCompoundGate::operator= (aCAndGate);
  
    return *this;
  }                                     

			//Operator== Equality Operator [not allowed]
private:
  int operator== (const CAndGate& aCAndGate);
public:

  virtual   CGate* clone ()  ;
  virtual   std::string Type ()  const;
  virtual   Bool_t inGate(CEvent& rEvent, const vector<UInt_t>& Params );
  virtual   Bool_t inGate(CEvent& rEvent);

};

#endif
