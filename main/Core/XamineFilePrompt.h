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

#ifndef __XAMINEFILEPROMPT_H  //Required for current class
#define __XAMINEFILEPROMPT_H
                               //Required for base classes
#ifndef __XAMINEBUTTONPROMPT_H
#include "XamineButtonPrompt.h"
#endif               
#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif                

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif
                               
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
