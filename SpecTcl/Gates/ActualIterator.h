//  CAConstituentIterator.h:
//
//    This file defines the CAConstituentIterator class.
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

#ifndef __ACONSTITUENTITERATOR_H  //Required for current class
#define __ACONSTITUENTITERATOR_H
                               
class CAConstituentIterator      
{
  
public:
			//Default constructor

  CAConstituentIterator ()   { } 
  virtual  ~ CAConstituentIterator ( ) { }       //Destructor

                        // Copy Constructor.

  CAConstituentIterator (const CAConstituentIterator& aCAConstituentIterator ) 
  { 
                
  }                                     

			//Operator= Assignment Operator

  CAConstituentIterator& 
  operator= (const CAConstituentIterator& aCAConstituentIterator)
  {   
    return *this;  
  }                                     

			//Operator== Equality Operator
                        // Pure Virtual
  virtual 
  int operator== (const CAConstituentIterator& aCAConstituentIterator) const;

  // Member functions: [most are pure virtual and implemented in derived
  //                    classes].

public:
                   
  int operator!= (CAConstituentIterator& rRhs) const ;
  virtual   CAConstituentIterator& operator++ ()   = 0;
  virtual   CAConstituentIterator* clone ()   = 0;

};

#endif
