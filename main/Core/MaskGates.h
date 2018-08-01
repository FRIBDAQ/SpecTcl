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

//  MaskGates.h:
//
//    This file defines the CMaskGate class.
//
//  Author:
//    Timothy Hoagland
//    NSCL / WIENER
//    s04.thoagland@wittenberg.edu
//
//  Adapted from the CTrueGate class written by:
//    Ron Fox
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:fox@nscl.msu.edu
//
//  Copyright 1999 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef MASKGATES_H         //Required for current class
#define MASKGATES_H
                               //Required for base classes
#include "Gate.h"
#include <histotypes.h>
                             
class CMaskGate  : public CGate        
{
  
public:
			// Constructor

  CMaskGate (UInt_t mask) : 
    CGate(),
    m_cCompare(mask)
  { } 
  ~CMaskGate ( ) { }       //Destructor

	
			//Copy constructor

  CMaskGate (const CMaskGate& aCMaskGate )   : CGate (aCMaskGate) 
  { 
                
  }                                     

			//Operator= Assignment Operator

  CMaskGate& operator= (const CMaskGate& aCMaskGate)
  { 
    if (this == &aCMaskGate) return *this;          
    CGate::operator= (aCMaskGate);
    
    return *this;                                                                                                 
  }                                     

  UInt_t getId() const
  {
    return m_nId;
  }

 long getCompare() const
  {
    return m_cCompare;
  }


			//Operator== Equality Operator
			// Gate comparison makes no sense.
private:
  int operator== (const CMaskGate& aCMaskGate); 
  UInt_t m_nId;
protected:
  UInt_t m_cCompare;
public:
  // Operations on the class.

public:               
  virtual   Bool_t operator() (CEvent& rEvent)  ;
  virtual   CConstituentIterator Begin ()  ;
  virtual   CConstituentIterator End ()  ;
  virtual   UInt_t Size ()  ;
  virtual   std::string GetConstituent (CConstituentIterator& rIterator)  ;
};

#endif
