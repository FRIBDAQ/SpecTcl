//  CPointListIterator.h:
//
//    This file defines the CPointListIterator class.
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

#ifndef __POINTLISTITERATOR_H  //Required for current class
#define __POINTLISTITERATOR_H
                               //Required for base classes
#ifndef __ACONSTITUENTITERATOR_H
#include "ActualIterator.h"
#endif                               


#ifndef __POINT_H
#include <Point.h>
#endif

#ifndef __STL_VECTOR
#include <vector>   
#define __STL_VECTOR
#endif  
                          
class CPointListIterator  : public CAConstituentIterator        
{
  std::vector<CPoint>::iterator m_iPoints;  // Point array iterator.
  
public:
			// Constructors.

  CPointListIterator (std::vector<CPoint>::iterator rStart) : 
    m_iPoints(rStart)   
  { } 
  virtual  ~ CPointListIterator ( ) { }       //Destructor

			//Copy constructor

  CPointListIterator (const CPointListIterator& aCPointListIterator )   : 
    CAConstituentIterator (aCPointListIterator) 
  {   
    m_iPoints = aCPointListIterator.m_iPoints;
                
  }                                     

			//Operator= Assignment Operator

  CPointListIterator& operator= (const CPointListIterator& aCPointListIterator)
  { 
    if (this == &aCPointListIterator) return *this;          
    CAConstituentIterator::operator= (aCPointListIterator);
    m_iPoints = aCPointListIterator.m_iPoints;
    
    return *this;
  }                                     

  // Selectors:

public:

  std::vector<CPoint>::iterator getPointIterator() const
  {
    return m_iPoints;
  }

  // Mutators:

protected:
                       //Set accessor function for attribute

  void setPointIterator (std::vector<CPoint>::iterator am_iPoints)
  { 
    m_iPoints = am_iPoints;
  }
  // Operations on the class.

public:
  virtual   CAConstituentIterator& operator++ ()  ;
  virtual   int operator== (const CAConstituentIterator& rRhs)  const;
  virtual   CAConstituentIterator* clone ()  ;
 
};

#endif
