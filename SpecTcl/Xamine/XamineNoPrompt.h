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

#ifndef __XAMINENOPROMPT_H  //Required for current class
#define __XAMINENOPROMPT_H
                               //Required for base classes
#ifndef __XAMINEBUTTONPROMPT_H
#include "XamineButtonPrompt.h"
#endif                               
                               
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
