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


//  COrGate.h:
//
//    This file defines the COrGate class.
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

#ifndef ORGATE_H  //Required for current class
#define ORGATE_H
                               //Required for base classes
#include "CompoundGate.h"
#include <histotypes.h>
#include <GateContainer.h>
#include <list>
#include <vector>
#include <string>

                 
class COrGate  : public CCompoundGate        
{
  
public:
			//Default constructor

  COrGate ()  
  { }
  COrGate(std::list<CGateContainer*>& rGates);
  COrGate(std::vector<CGateContainer*>& rGates) :
    CCompoundGate(rGates)
  {}
  COrGate(UInt_t nGates, CGateContainer** ppGates) :
    CCompoundGate(nGates, ppGates)
  {}
  ~ COrGate ( ) { }       //Destructor

	
			//Copy constructor

  COrGate (const COrGate& aCOrGate )   : CCompoundGate (aCOrGate) 
  { 
                
  }                                     

			//Operator= Assignment Operator

  COrGate& operator= (const COrGate& aCOrGate)
  { 
    if (this == &aCOrGate) return *this;          
    CCompoundGate::operator= (aCOrGate);
    
    return *this;                                                                                                 
  }                                     

			//Operator== Equality Operator
private:
  int operator== (const COrGate& aCOrGate);
public:

  // Class operations.

public:                       
  virtual   CGate* clone ()  ;
  virtual   std::string Type ()  const; 
  virtual   Bool_t inGate(CEvent& rEvent, const std::vector<UInt_t>& Params);
  virtual   Bool_t inGate(CEvent& rEvent);
};

#endif
