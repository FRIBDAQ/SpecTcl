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



//  CGate.h:
//
//    This file defines the CGate class.
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

#ifndef __GATE_H  //Required for current class
#define __GATE_H

#ifndef __CONSTITUENTITERATOR_H
#include "ConstituentIterator.h"
#endif


#ifndef __HISTOTYPES_H                        
#include <histotypes.h>   //Required for include files, eg <CList.h>
#endif

#ifndef __EVENT_H
#include <Event.h>        //Required for include files, eg <CList.h>
#endif
#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

                                                               
class CGate      
{
  Bool_t m_fChecked;  // kfTrue if cached value is valid.
  Bool_t m_fCachedValue;  // Cached gate value.
  
public:
			//Default constructor

  CGate () :  m_fChecked(0),  m_fCachedValue(0)   { } 
  virtual  ~ CGate ( ) { }       //Destructor

			//Copy constructor

  CGate (const CGate& aCGate ) 
  {   
    m_fChecked = aCGate.m_fChecked;
    m_fCachedValue = aCGate.m_fCachedValue;           
  }                                     

			//Operator= Assignment Operator

  CGate& operator= (const CGate& aCGate)
  { 
    if (this == &aCGate) return *this;          
    
    m_fChecked = aCGate.m_fChecked;
    m_fCachedValue = aCGate.m_fCachedValue;
    
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CGate& aCGate) const
  { 
    return (

	    (m_fChecked == aCGate.m_fChecked) &&
	    (m_fCachedValue == aCGate.m_fCachedValue)
	    );
  }           
  // Selectors:

public:
                       //Get accessor function for attribute
  Bool_t getChecked() const
  {
    return m_fChecked;
  }

                       //Get accessor function for attribute
  Bool_t getCachedValue() const
  {
    return m_fCachedValue;
  }

  // Mutators:

protected:                   
                       //Set accessor function for attribute
  void setChecked (Bool_t am_fChecked)
  { 
    m_fChecked = am_fChecked;
  }

                       //Set accessor function for attribute
  void setCachedValue (Bool_t am_fCachedValue)
  { 
    m_fCachedValue = am_fCachedValue;
  }

  // Class operations:

public:                   
  void Set (Bool_t fValue) {
    m_fChecked     = kfTRUE;
    m_fCachedValue = fValue;
  }
  void Reset () {
    m_fChecked = kfFALSE;
  }
  virtual void RecursiveReset(); 

  Bool_t wasChecked () {
    return m_fChecked;
  }
  virtual   Bool_t operator() (CEvent& rEvent)  ;
  virtual   CConstituentIterator Begin ()   = 0;
  virtual   CConstituentIterator End ()   = 0;
  virtual   UInt_t Size ()   = 0;
  virtual   STD(string) GetConstituent (CConstituentIterator& rIterator)   = 0;
  virtual   CGate* clone ()   = 0;
  virtual   STD(string) Type () const   = 0;
  virtual   Bool_t inGate (CEvent& rEvent, const STD(vector)<UInt_t>& Params) = 0;
  virtual   Bool_t inGate (CEvent& rEvent) = 0;
  virtual   Bool_t caches();
};

#endif
