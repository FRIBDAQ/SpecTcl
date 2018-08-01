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



//  CDeletedGate.h:
//
//    This file defines the CDeletedGate class.
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

#ifndef DELETEDGATE_H  //Required for current class
#define DELETEDGATE_H
                               //Required for base classes
#include "FalseGate.h"
#include <string>

                               
class CDeletedGate  : public CFalseGate        
{
  
public:
			//Default constructor

  CDeletedGate () : CFalseGate() 
  { } 
  ~ CDeletedGate ( ) { }       //Destructor
	
			//Copy constructor

  CDeletedGate (const CDeletedGate& aCDeletedGate )   : 
    CFalseGate (aCDeletedGate) 
  { 
                
  }                                     

			//Operator= Assignment Operator

  CDeletedGate& operator= (const CDeletedGate& aCDeletedGate)
  { 
    if (this == &aCDeletedGate) return *this;          
    CFalseGate::operator= (aCDeletedGate);
  
    return *this;
  }                                     


			//Operator== Equality Operator
private:
  int operator== (const CDeletedGate& aCDeletedGate);
                       
  virtual   std::string Type ()  ;
 

};

#endif
