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


//  CXamineConfirmPrompt.h:
//
//    This file defines the CXamineConfirmPrompt class.
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

#ifndef XAMINECONFIRMPROMPT_H  //Required for current class
#define XAMINECONFIRMPROMPT_H
                               //Required for base classes
#include "XamineButtonPrompt.h"
#include <string>


class CXamineConfirmPrompt  : public CXamineButtonPrompt        
{
  
public:

  CXamineConfirmPrompt (const std::string& rPrompt ) :
    CXamineButtonPrompt(rPrompt)
    { }        
  CXamineConfirmPrompt(const char* pPrompt) :
    CXamineButtonPrompt(pPrompt)
  { }
  virtual  ~ CXamineConfirmPrompt ( ) { }       //Destructor
	
			//Copy constructor

  CXamineConfirmPrompt (const CXamineConfirmPrompt& aCXamineConfirmPrompt ) : 
    CXamineButtonPrompt (aCXamineConfirmPrompt) 
  { 
                
  }                                     

			//Operator= Assignment Operator

  CXamineConfirmPrompt& operator= 
                          (const CXamineConfirmPrompt& aCXamineConfirmPrompt)
  { 
    if (this == &aCXamineConfirmPrompt) return *this;          
    CXamineButtonPrompt::operator= (aCXamineConfirmPrompt);
  
    return *this;
  }                                     

			//Operator== Equality Operator
			//Update to access 1:M part class attributes
			//Update to access 1:1 associated class attributes
			//Update to access 1:M associated class attributes      
  int operator== (const CXamineConfirmPrompt& aCXamineConfirmPrompt)
  { 
    return (
	    (CXamineButtonPrompt::operator== (aCXamineConfirmPrompt))
	    );
  }                             
                       
  virtual void FormatPrompterBlock (ButtonDescription& rButton) const  ;
 
};

#endif
