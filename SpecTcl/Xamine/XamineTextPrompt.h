//  CXamineTextPrompt.h:
//
//    This file defines the CXamineTextPrompt class.
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

#ifndef __XAMINETEXTPROMPT_H  //Required for current class
#define __XAMINETEXTPROMPT_H
                               //Required for base classes
#ifndef __XAMINEBUTTONPROMPT_H
#include "XamineButtonPrompt.h"
#endif                               

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif
                               
class CXamineTextPrompt  : public CXamineButtonPrompt        
{
  
public:

			//Constructor with arguments

  CXamineTextPrompt (const std::string& rPrompt ) : 
    CXamineButtonPrompt(rPrompt)
  { }        
  CXamineTextPrompt(const char* pPrompt) :
    CXamineButtonPrompt(pPrompt)
  { }
  virtual ~ CXamineTextPrompt ( ) { }       //Destructor
	
			//Copy constructor
			//Update to access 1:M part class attributes
			//Update to access 1:1 associated class attributes
			//Update to access 1:M associated class attributes      
  CXamineTextPrompt (const CXamineTextPrompt& aCXamineTextPrompt )   : 
    CXamineButtonPrompt (aCXamineTextPrompt) 
  { 
                
  }                                     

			//Operator= Assignment Operator

  CXamineTextPrompt& operator= (const CXamineTextPrompt& aCXamineTextPrompt)
  { 
    if (this == &aCXamineTextPrompt) return *this;          
    CXamineButtonPrompt::operator= (aCXamineTextPrompt);
  
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CXamineTextPrompt& aCXamineTextPrompt)
  { 
    return (
	    (CXamineButtonPrompt::operator== (aCXamineTextPrompt))
	    
	    );
  }                             
                       
  virtual   void FormatPrompterBlock (ButtonDescription& rButton) const  ;
 
};

#endif
