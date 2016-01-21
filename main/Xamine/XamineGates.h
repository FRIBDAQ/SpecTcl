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

//  CXamineGates.h:
//
//    This file defines the CXamineGates class.
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

#ifndef XAMINEGATES_H  //Required for current class
#define XAMINEGATES_H
                               
                               //Required for 1:M associated classes
#include "XamineGate.h"

#include <vector>


typedef std::vector<CXamineGate> CXamineGateVector;
typedef CXamineGateVector::iterator CDisplayGateVectorIterator;
 
class CXamineGates      
{   
  CXamineGateVector m_vGates;	// Stores gates retreived from Xamine.

public:
			//Default constructor

  CXamineGates (UInt_t spec);
  virtual ~ CXamineGates ( ) { }       //Destructor

			//Copy constructor

  CXamineGates (const CXamineGates& aCXamineGates ) 
  { 
    m_vGates = aCXamineGates.m_vGates;
  }                                     

			//Operator= Assignment Operator

  CXamineGates& operator= (const CXamineGates& aCXamineGates)
  { 
    if (this == &aCXamineGates) return *this;          
    m_vGates = aCXamineGates.m_vGates;

    return *this;
  }

			//Operator== Equality Operator

  int operator== (const CXamineGates& aCXamineGates) const
  { 
    return (m_vGates == aCXamineGates.m_vGates);
  }                             

  // Operations:

public:
    CDisplayGateVectorIterator begin ()  ;
    CDisplayGateVectorIterator end ()  ;
    CXamineGate& operator[] (UInt_t n)  ;
    UInt_t size ()  ;
 
};


#endif
