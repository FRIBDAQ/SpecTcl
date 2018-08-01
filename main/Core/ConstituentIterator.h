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


// Facade pattern
//  CConstituentIterator.h:
//
//    This file defines the CConstituentIterator class.
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

#ifndef CONSTITUENTITERATOR_H  //Required for current class
#define CONSTITUENTITERATOR_H
                               
                               //Required for 1:1 associated classes
#include "ActualIterator.h"


                                                               
class CConstituentIterator      
{
  
  CAConstituentIterator* m_pActualIterator;
  
public:
			// Constructor

  CConstituentIterator (CAConstituentIterator& rIt);
  virtual ~ CConstituentIterator ( ) // Destructor.
  { 
    delete m_pActualIterator; 
  }

	
			//Copy constructor

  CConstituentIterator (const CConstituentIterator& aCConstituentIterator );

			//Operator= Assignment Operator

  CConstituentIterator& 
  operator= (const CConstituentIterator& aCConstituentIterator);

			//Operator== Equality Operator
  int operator== (const CConstituentIterator& aCConstituentIterator) const;
                       
                       //Get accessor function for 1:1 association

  // Selectors:

public:
  CAConstituentIterator* getActualIterator() const
  {
    return m_pActualIterator;
  }
                       
  // Mutators:

protected:
  void setActualIterator (CAConstituentIterator* am_rActualIterator)
  { 
    m_pActualIterator = am_rActualIterator;
  }
  // Operations on the object:

public:

  CConstituentIterator& operator++ ()  ;    // Pre-incremenet.
  CConstituentIterator operator++ (int )  ; // Post-increment.

  int operator!= (CConstituentIterator& rRhs) const;
 
};

#endif





