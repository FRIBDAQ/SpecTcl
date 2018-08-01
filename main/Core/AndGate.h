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

#ifndef ANDGATE_H  //Required for current class
#define ANDGATE_H
                               //Required for base classes
#include "CompoundGate.h"
#include "GateContainer.h"
#include <histotypes.h>
#include <vector>
#include <list>
#include <string>

                    
class CAndGate  : public CCompoundGate        
{
  
public:
			//Default constructor

  CAndGate () : CCompoundGate() { } 
  CAndGate(std::list<CGateContainer*>& rGates); 
  CAndGate(std::vector<CGateContainer*>& rGates) :
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
  virtual   Bool_t inGate(CEvent& rEvent, const STD(vector)<UInt_t>& Params );
  virtual   Bool_t inGate(CEvent& rEvent);

};

#endif
