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

//  CFalseGate.h:
//
//    This file defines the CFalseGate class.
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

#ifndef FALSEGATE_H  //Required for current class
#define FALSEGATE_H
                               //Required for base classes
#include "Gate.h"
#include <vector>
#include <string>

// Forward class definitions:

class CEvent;
                               
class CFalseGate  : public CGate        
{
  
public:
			//Default constructor

  CFalseGate () : CGate()
  { } 
  virtual ~ CFalseGate ( ) { }       //Destructor

  CFalseGate (const CFalseGate& aCFalseGate )   : CGate (aCFalseGate) 
  { 
                
  }                                     

			//Operator= Assignment Operator

  CFalseGate& operator= (const CFalseGate& aCFalseGate)
  { 
    if (this == &aCFalseGate) return *this;          
    CGate::operator= (aCFalseGate);
  
    return *this;
  }     
           // There is no gate comparison which makes sense.
private:
  int operator== (const CFalseGate& aCFalseGate);
public:
  // Operations on the object.

public:
                       
  virtual   Bool_t operator() (CEvent& rEvent)  ;
  virtual   CConstituentIterator Begin ()  ;
  virtual   CConstituentIterator End ()  ;
  virtual   UInt_t Size ()  ;
  virtual   std::string GetConstituent (CConstituentIterator& rIterator)  ;
  virtual   CGate* clone ()  ;
  virtual   std::string Type () const;
  virtual   Bool_t inGate(CEvent& rEvent, const std::vector<UInt_t>& Params);
  virtual Bool_t inGate(CEvent& rEvent);
  virtual   Bool_t caches();
};

#endif
