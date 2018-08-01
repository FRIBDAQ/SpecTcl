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


//  CXamineSpectrumIterator.h:
//
//    This file defines the CXamineSpectrumIterator class.
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

#ifndef XAMINESPECTRUMITERATOR_H  //Required for current class
#define XAMINESPECTRUMITERATOR_H
                               
                               //Required for 1:1 associated classes
#include <Xamine.h>
#include <histotypes.h>


class CXamineSpectrum;
                                                               
class CXamineSpectrumIterator      
{
  volatile Xamine_shared*   m_pXamineMemory; // Xamine shared memory pointer.
  UInt_t           m_nSlot;	// Current spectrum slot [0,XAMINE_MAXSPEC]
  CXamineSpectrum* m_pSpectrum;	// Xamine shared memory.
  
public:

			//Constructor with arguments:
                        // Initialized to first used slot on or after am_nSlot.
  CXamineSpectrumIterator (volatile   Xamine_shared* am_pXamineMemory,  
			     UInt_t am_nSlot = 0); 
  virtual ~ CXamineSpectrumIterator ( ) { 
    unbind();
  }       //Destructor

			//Copy constructor

  CXamineSpectrumIterator 
  (const CXamineSpectrumIterator& aCXamineSpectrumIterator ) {
    DoAssign(aCXamineSpectrumIterator);
  }

			//Operator= Assignment Operator

  CXamineSpectrumIterator& operator= 
                  (const CXamineSpectrumIterator& aCXamineSpectrumIterator)
  { 
    if(this != &aCXamineSpectrumIterator)
      DoAssign(aCXamineSpectrumIterator);

    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CXamineSpectrumIterator& aCXamineSpectrumIterator)
  { 
    return (
	    (m_pXamineMemory == aCXamineSpectrumIterator.m_pXamineMemory) &&
	    (m_nSlot == aCXamineSpectrumIterator.m_nSlot) 
	    );
  }                             
  // Selectors:

public:
  const volatile Xamine_shared* getXamineMemory() const
  {
    return m_pXamineMemory;
  }
  UInt_t getSlot() const
  {
    return m_nSlot;
  }
  const CXamineSpectrum* getSpectrum() const
  {
    return m_pSpectrum;
  }
  // Mutators (only available to derived classes):

protected:
  void XamineMemory (volatile Xamine_shared* am_pXamineMemory)
  { 
    m_pXamineMemory = am_pXamineMemory;
  }
  void setSlot (UInt_t am_nSlot)
  { 
    m_nSlot = am_nSlot;
  }
  void setSpectrum (CXamineSpectrum* am_pSpectrum)
  { 
    m_pSpectrum = am_pSpectrum;
  }
  // Public operations:

public:
  CXamineSpectrum& operator* ()  ;
  CXamineSpectrumIterator& operator++ ()  ;     // preinc.
  CXamineSpectrumIterator  operator++(int);     // postinc.
  int operator!= (const CXamineSpectrumIterator& rRhs)  ;
  CXamineSpectrum* operator-> ()  ;

  // Utility functions:

protected:
  void bind ()  ;		// Bind iterator to spectrum.
  void unbind ()  ;		// unbind iterator from spectrum.
  void next();			// locate next used spectrum.
  void DoAssign(const CXamineSpectrumIterator& rRhs); // perform assignment.
  
};

#endif
