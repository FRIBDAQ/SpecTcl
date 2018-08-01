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

#ifndef INTERIORROW_H  //Required for current class
#define INTERIORROW_H

#include <vector>   //Required for include files, eg <CList.h>
#include <histotypes.h>   //Required for include files, eg <CList.h>
                                                               
class CInteriorRow      
{
  std::vector<UInt_t> m_vRow;  // Contains the scanline interior flags.
  
public:
			// 'normal' constructor.


  CInteriorRow() {}
  CInteriorRow (UInt_t nSize) {
    m_vRow.reserve(nSize);	// Size the std::vector appropriately...
    fill(m_vRow.begin(), m_vRow.end(), 0); // And init to zero.
  }
  virtual ~ CInteriorRow ( ) { }       //Destructor
			//Constructor with pre-existing row.

  CInteriorRow (  std::vector<UInt_t> am_vRow  )       
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
  std::vector<UInt_t> getRow() const
  {
    return m_vRow;
  }
  // Mutators:

protected:                   
                       //Set accessor function for attribute
  void setRow (std::vector<UInt_t> am_vRow)
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
    std::vector<UInt_t>::iterator Begin ()  ;
    std::vector<UInt_t>::iterator End ()  ;
    UInt_t& operator[] (UInt_t i)  ;

};

#endif
