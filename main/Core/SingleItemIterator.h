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


//  CSingleItemIterator.h:
//
//    This file defines the CSingleItemIterator class.
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

#ifndef SINGLEITEMITERATOR_H  //Required for current class
#define SINGLEITEMITERATOR_H

#include "ActualIterator.h"
#include <histotypes.h>
      
class CSingleItemIterator  : public CAConstituentIterator        
{
  Bool_t m_fNotEnd;      // kfTRUE if not 'end' of list.
  
public:
			//Default constructor
			//Update to access base class attributes 
			//Update to access 1:1 part class attributes 
			//Update to access 1:M part class attributes
			//Update to access 1:1 associated class attributes
			//Update to access 1:M associated class attributes
			//Ensure initial values entered
  CSingleItemIterator () :  
    m_fNotEnd(kfTRUE)   
  { 
  }
  virtual   ~CSingleItemIterator ( ) { }       //Destructor  

			//Constructor with arguments
  CSingleItemIterator (Bool_t am_fNotEnd  )  :
    m_fNotEnd (am_fNotEnd)  
  { 
  }        
	
			//Copy constructor

  CSingleItemIterator (const CSingleItemIterator& aCSingleItemIterator )   : 
    CAConstituentIterator (aCSingleItemIterator) 
  {   
    m_fNotEnd = aCSingleItemIterator.m_fNotEnd;
                
  }                                     

			//Operator= Assignment Operator

  CSingleItemIterator& operator= 
                        (const CSingleItemIterator& aCSingleItemIterator)
  { 
    if (this == &aCSingleItemIterator) return *this;          
    CAConstituentIterator::operator= (aCSingleItemIterator);
    m_fNotEnd = aCSingleItemIterator.m_fNotEnd;        
    return *this;                                                                                                 
  }                                     
  // Selectors:
                       //Get accessor function for attribute
  Bool_t getNotEnd() const
  {
    return m_fNotEnd;
  }
  // Mutators:
                       //Set accessor function for attribute
  void setNotEnd (Bool_t am_fNotEnd)
  { 
    m_fNotEnd = am_fNotEnd;
  }
  //
  // operations:
  //
public:                   
  virtual   CAConstituentIterator& operator++ ()  ;
  virtual   int operator== (const CAConstituentIterator& rRhs)  const;
  virtual   CAConstituentIterator* clone ()  ;

};

#endif
