//  CTCLApplication.h:
//
//    This file defines the CTCLApplication class.
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

#ifndef __TCLAPPLICATION_H  //Required for current class
#define __TCLAPPLICATION_H
                               //Required for base classes
#ifndef __TCLINTERPRETEROBJECT_H
#include "TCLInterpreterObject.h"
#endif                               


                               
class CTCLApplication  : public CTCLInterpreterObject        
{
  void* m_pInit;	  // Ensure app init pulled out of link.
public:
			//Default constructor

  CTCLApplication ()   {m_pInit = Tcl_AppInit; } // Ensure TCLApplication.o
				                  // is linked.
  ~CTCLApplication ( ) { }       //Destructor

	
			//Copy constructor

  CTCLApplication (const CTCLApplication& aCTCLApplication )   : 
    CTCLInterpreterObject (aCTCLApplication) 
  { 
                
  }                                     

			//Operator= Assignment Operator

  CTCLApplication& operator= (const CTCLApplication& aCTCLApplication)
  { 
    if (this == &aCTCLApplication) return *this;          
    CTCLInterpreterObject::operator= (aCTCLApplication);
  
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CTCLApplication& aCTCLApplication)
  { 
    return (
	    (CTCLInterpreterObject::operator== (aCTCLApplication))

	    );
  }                             
                       
  virtual   int operator() ()  =0;
 
};

#endif
