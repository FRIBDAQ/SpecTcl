

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

#ifndef __DELETEDGATE_H  //Required for current class
#define __DELETEDGATE_H
                               //Required for base classes
#ifndef __FALSEGATE_H
#include "FalseGate.h"
#endif                               
                               
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
