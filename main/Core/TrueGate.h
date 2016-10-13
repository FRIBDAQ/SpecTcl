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

//  CTrueGate.h:
//
//    This file defines the CTrueGate class.
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

#ifndef __TRUEGATE_H  //Required for current class
#define __TRUEGATE_H
                               //Required for base classes
#ifndef __GATE_H
#include "Gate.h"
#endif                               
  

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif
                             
class CTrueGate  : public CGate        
{
  
public:
			// Constructor

  CTrueGate () : CGate() 
  { } 
  ~CTrueGate ( ) { }       //Destructor

	
			//Copy constructor

  CTrueGate (const CTrueGate& aCTrueGate )   : CGate (aCTrueGate) 
  { 
                
  }                                     

			//Operator= Assignment Operator

  CTrueGate& operator= (const CTrueGate& aCTrueGate)
  { 
    if (this == &aCTrueGate) return *this;          
    CGate::operator= (aCTrueGate);
    
    return *this;                                                                                                 
  }                                     

			//Operator== Equality Operator
			// Gate comparison makes no sense.
private:
  int operator== (const CTrueGate& aCTrueGate);
public:

  // Operations on the class.

public:               
  virtual   Bool_t operator() (CEvent& rEvent)  ;
  virtual   CConstituentIterator Begin ()  ;
  virtual   CConstituentIterator End ()  ;
  virtual   UInt_t Size ()  ;
  virtual   std::string GetConstituent (CConstituentIterator& rIterator)  ;
  virtual   CGate* clone ()  ;
  virtual   std::string Type ()  const;
  virtual   Bool_t inGate(CEvent& rEvent, const std::vector<UInt_t>& Params);
  virtual   Bool_t inGate(CEvent& rEvent);
  virtual   Bool_t caches();
};

#endif
