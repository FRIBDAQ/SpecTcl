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
//  CXamineToggleButton.h:
//
//    This file defines the CXamineToggleButton class.
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

#ifndef XAMINETOGGLEBUTTON_H  //Required for current class
#define XAMINETOGGLEBUTTON_H
                               //Required for base classes
#include "XamineButton.h"
#include <histotypes.h>
#include <string>
 
class CXamineToggleButton  : public CXamineButton        
{
  Bool_t m_fInitialState;  // Initial state of toggle.
  
public:

  CXamineToggleButton (int am_nReturnCode,  
		       const std::string& am_sLabel,  
		       Bool_t am_fEnabled,  
		       ButtonSensitivity am_eWhenSensitive,
		       CXamineButtonPrompt& rPrompter,
		       Bool_t am_fInitialState = kfFALSE  )  :
    CXamineButton(am_nReturnCode, am_sLabel, am_fEnabled, 
		  am_eWhenSensitive, rPrompter),
    m_fInitialState (am_fInitialState)  
  { }        

  CXamineToggleButton (int am_nReturnCode,  
		       const char*  pLabel,  
		       Bool_t am_fEnabled,  
		       ButtonSensitivity am_eWhenSensitive,
		       CXamineButtonPrompt& rPrompter,
		       Bool_t am_fInitialState = kfFALSE  )  :
    CXamineButton(am_nReturnCode, pLabel, am_fEnabled, 
		  am_eWhenSensitive, rPrompter),
    m_fInitialState (am_fInitialState)  
  { }        

   virtual ~ CXamineToggleButton ( ) { }       //Destructor
	
			//Copy constructor

  CXamineToggleButton (const CXamineToggleButton& aCXamineToggleButton )   : 
    CXamineButton (aCXamineToggleButton) 
  {   
    m_fInitialState = aCXamineToggleButton.m_fInitialState;
                
  }                                     

			//Operator= Assignment Operator

  CXamineToggleButton& operator= 
                             (const CXamineToggleButton& aCXamineToggleButton)
  { 
    if (this == &aCXamineToggleButton) return *this;          
    CXamineButton::operator= (aCXamineToggleButton);
    m_fInitialState = aCXamineToggleButton.m_fInitialState;
        
  return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CXamineToggleButton& aCXamineToggleButton)
  { 
    return (
	    (CXamineButton::operator== (aCXamineToggleButton)) &&
	    (m_fInitialState == aCXamineToggleButton.m_fInitialState) 
	    );
  }                             
  // Selectors:

public:
  Bool_t getInitialState() const
  {
    return m_fInitialState;
  }
  // Mutators:

protected:
  void setInitialState (Bool_t am_fInitialState)
  { 
    m_fInitialState = am_fInitialState;
  }
  // Operations:

public:
  virtual   void FormatMessageBlock (ButtonDescription& rButton) const  ;

  virtual   CXaminePushButton*   PushButton();   // Type safe upcasts.
  virtual   CXamineToggleButton* ToggleButton();
};

#endif

