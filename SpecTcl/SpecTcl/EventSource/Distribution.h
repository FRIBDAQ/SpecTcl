//  CDistribution.h:
//
//    This file defines the CDistribution class, a base class for
//    a set of classes which produce random variables distributed
//    according to some underlying paramterized probability distribution.
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

#ifndef __CDISTRIBUTION_H  //Required for current class
#define __CDISTRIBUTION_H

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

// This class is pure virtual.
                               
class CDistribution      
{
  
public:
			//Default constructor

  CDistribution ()   { } 
  virtual  ~ CDistribution ( ) { }       //Destructor

			//Copy constructor
				// Supplied to allow derivers to not think
				// about what to do with the base class

  CDistribution (const CDistribution& aCDistribution ) 
  { 
                
  }                                     

				// Assignment operator

  CDistribution& operator= (const CDistribution& aCDistribution)
  {
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CDistribution& aCDistribution)
  { 
    return -1;
  }                             
  // Operations:
                   
  virtual   UInt_t operator() ()   = 0;
 
};

#endif
