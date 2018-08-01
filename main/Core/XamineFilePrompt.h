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
//  CXamineFilePrompt.h:
//
//    This file defines the CXamineFilePrompt class.
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

#ifndef XAMINEFILEPROMPT_H  //Required for current class
#define XAMINEFILEPROMPT_H
                               //Required for base classes
#include "XamineButtonPrompt.h"
#include <histotypes.h>
#include <string>
                               
class CXamineFilePrompt  : public CXamineButtonPrompt        
{
  
public:

  CXamineFilePrompt (const std::string& rPrompt) :
    CXamineButtonPrompt(rPrompt)
  { }        
  CXamineFilePrompt(const char* pPrompt) :
    CXamineButtonPrompt(pPrompt)
  { }
  virtual ~ CXamineFilePrompt ( ) { }       //Destructor
  
			//Copy constructor

  CXamineFilePrompt (const CXamineFilePrompt& aCXamineFilePrompt )   : 
    CXamineButtonPrompt (aCXamineFilePrompt) 
  { 
                
  }                                     

			//Operator= Assignment Operator

  CXamineFilePrompt& operator= (const CXamineFilePrompt& aCXamineFilePrompt)
  { 
    if (this == &aCXamineFilePrompt) return *this;          
    CXamineButtonPrompt::operator= (aCXamineFilePrompt);
  
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CXamineFilePrompt& aCXamineFilePrompt)
  { 
    return (
	    (CXamineButtonPrompt::operator== (aCXamineFilePrompt))
	    );
  }                             
                       
  virtual   void FormatPrompterBlock (ButtonDescription& rButton) const  ;
 
};

#endif
