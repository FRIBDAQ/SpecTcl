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

/* 
  Change Log:
  $Log$
  Revision 5.2  2005/06/03 15:19:20  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.1  2004/12/21 17:51:22  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:56:03  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.4  2003/08/25 16:25:32  ron-fox
  Initial starting point for merge with filtering -- this probably does not
  generate a goo spectcl build.

  Revision 4.3  2003/04/15 19:15:41  ron-fox
  To support real valued parameters, primitive gates must be internally stored as real valued coordinate pairs.

*/


#ifndef POINTLISTITERATOR_H  //Required for current class
#define POINTLISTITERATOR_H
                               //Required for base classes
#include "ActualIterator.h"
#include <Point.h>
#include <vector>   

/*!
   Class to support iteration in point lists.
*/                        
class CPointListIterator  : public CAConstituentIterator        
{
  std::vector<FPoint>::iterator m_iPoints;  // Point array iterator.
  
public:
			// Constructors.

  CPointListIterator (std::vector<FPoint>::iterator rStart) : 
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

  std::vector<FPoint>::iterator getPointIterator() const
  {
    return m_iPoints;
  }

  // Mutators:

protected:
                       //Set accessor function for attribute

  void setPointIterator (std::vector<FPoint>::iterator am_iPoints)
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
