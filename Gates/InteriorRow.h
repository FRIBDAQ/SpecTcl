//  CInteriorRow.h:
//
//    This file defines the CInteriorRow class.
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

#ifndef __INTERIORROW_H  //Required for current class
#define __INTERIORROW_H

#ifndef __STL_VECTOR                               
#include <vector>   //Required for include files, eg <CList.h>
#define __STL_VECTOR
#endif

#ifndef __STL_ALGORITHM
#include <algorithm>
#define __STL_ALGORITHM
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>   //Required for include files, eg <CList.h>
#endif
                                                               
class CInteriorRow      
{
  vector<UInt_t> m_vRow;  // Contains the scanline interior flags.
  
public:
			// 'normal' constructor.


  CInteriorRow() {}
  CInteriorRow (UInt_t nSize) {
    m_vRow.reserve(nSize);	// Size the vector appropriately...
    fill(m_vRow.begin(), m_vRow.end(), 0); // And init to zero.
  }
  virtual ~ CInteriorRow ( ) { }       //Destructor
			//Constructor with pre-existing row.

  CInteriorRow (  vector<UInt_t> am_vRow  )       
  :   m_vRow (am_vRow)  { }        
	
			//Copy constructor

  CInteriorRow (const CInteriorRow& aCInteriorRow ) 
  {  
    m_vRow = aCInteriorRow.m_vRow;
                
  }                                     

			//Operator= Assignment Operator

  CInteriorRow& operator= (const CInteriorRow& aCInteriorRow)
  { 
    if (this == &aCInteriorRow) return *this;          
  
    m_vRow = aCInteriorRow.m_vRow;
        
    return *this;                                                                                                 
  }                                     

			//Operator== Equality Operator

  int operator== (const CInteriorRow& aCInteriorRow)
  { 
    return (
	    
	    (m_vRow == aCInteriorRow.m_vRow) 
	    );
  }                             
  // Selectors:

public:
  vector<UInt_t> getRow() const
  {
    return m_vRow;
  }
  // Mutators:

protected:                   
                       //Set accessor function for attribute
  void setRow (vector<UInt_t> am_vRow)
  { 
    m_vRow = am_vRow;
  }
public:
  void SetRowSize(UInt_t n) {
    m_vRow.reserve(n);
    m_vRow.insert(Begin(), (const int)n, (UInt_t)0);
  }
  // Class member operations:

public:                   
    UInt_t Size ()  ;
    vector<UInt_t>::iterator Begin ()  ;
    vector<UInt_t>::iterator End ()  ;
    UInt_t& operator[] (UInt_t i)  ;

};

#endif
