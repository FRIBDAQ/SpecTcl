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

#ifndef CDISTRIBUTION_H  //Required for current class
#define CDISTRIBUTION_H

#include <histotypes.h>

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
                   
  virtual   Float_t operator() ()   = 0;
 
};

#endif
