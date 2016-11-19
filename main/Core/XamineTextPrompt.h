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
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif
                               
class CXamineTextPrompt  : public CXamineButtonPrompt        
{
  
public:

			//Constructor with arguments

  CXamineTextPrompt (const std::string& rPrompt)  : 
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
