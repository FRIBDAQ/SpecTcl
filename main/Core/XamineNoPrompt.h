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


//  CXamineNoPrompt.h:
//
//    This file defines the CXamineNoPrompt class.
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

#ifndef XAMINENOPROMPT_H  //Required for current class
#define XAMINENOPROMPT_H
                               //Required for base classes
#include "XamineButtonPrompt.h"
                               
class CXamineNoPrompt  : public CXamineButtonPrompt        
{
  
public:
  CXamineNoPrompt () : CXamineButtonPrompt(" ") {} 
  virtual  ~ CXamineNoPrompt ( ) { }       //Destructor
	
  // Copy constructor

  CXamineNoPrompt (const CXamineNoPrompt& aCXamineNoPrompt )   : 
    CXamineButtonPrompt (aCXamineNoPrompt) 
  { 
                
  }                                     

			//Operator= Assignment Operator

  CXamineNoPrompt& operator= (const CXamineNoPrompt& aCXamineNoPrompt)
  { 
    if (this == &aCXamineNoPrompt) return *this;          
    CXamineButtonPrompt::operator= (aCXamineNoPrompt);
  
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CXamineNoPrompt& aCXamineNoPrompt)
  { 
    return (
	    (CXamineButtonPrompt::operator== (aCXamineNoPrompt)) 
	    );
  }                             
                       
  virtual   void FormatPrompterBlock (ButtonDescription& rPrompt) const  ;

  
 
};

#endif
