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
//  CXaminePushButton.h:
//
//    This file defines the CXaminePushButton class.
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

#ifndef __XAMINEPUSHBUTTON_H  //Required for current class
#define __XAMINEPUSHBUTTON_H
                               //Required for base classes
#ifndef __XAMINEBUTTON_H
#include "XamineButton.h"
#endif
                               
#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

class CXamineToggleButton;	// forward def.
                               
class CXaminePushButton  : public CXamineButton        
{
  
public:
  
  CXaminePushButton ( int am_nReturnCode,  
		      const std::string& am_sLabel,  
		      Bool_t am_fEnabled,  
		      ButtonSensitivity am_eWhenSensitive,
		      CXamineButtonPrompt& rPrompter) :      
    CXamineButton(am_nReturnCode,
		  am_sLabel,
		  am_fEnabled,
		  am_eWhenSensitive,
		  rPrompter)
  { }        

  CXaminePushButton ( int am_nReturnCode,  
		      const char* pLabel,  
		      Bool_t am_fEnabled,  
		      ButtonSensitivity am_eWhenSensitive,
		      CXamineButtonPrompt& rPrompter) :      
    CXamineButton(am_nReturnCode,
		  pLabel,
		  am_fEnabled,
		  am_eWhenSensitive,
		  rPrompter)
  { }        
  virtual  ~ CXaminePushButton ( ) { }       //Destructor
	
			//Copy constructor
			//Update to access 1:M part class attributes
			//Update to access 1:1 associated class attributes
			//Update to access 1:M associated class attributes      
  CXaminePushButton (const CXaminePushButton& aCXaminePushButton )   : 
    CXamineButton (aCXaminePushButton) 
  { 
                
  }                                     

			//Operator= Assignment Operator

  CXaminePushButton& operator= (const CXaminePushButton& aCXaminePushButton)
  { 
    if (this == &aCXaminePushButton) return *this;          
    CXamineButton::operator= (aCXaminePushButton);
  
  return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CXaminePushButton& aCXaminePushButton)
  { 
    return (
	    (CXamineButton::operator== (aCXaminePushButton)) 
	    );
  }                             
                       
  virtual   void FormatMessageBlock (ButtonDescription& rButton) const  ;

  virtual   CXaminePushButton*   PushButton(); // Type safe upcasts.
  virtual   CXamineToggleButton* ToggleButton();

};

#endif
