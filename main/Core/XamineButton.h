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
//  CXamineButton.h:
//
//    This file defines the CXamineButton class.
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

#ifndef XAMINEBUTTON_H  //Required for current class
#define XAMINEBUTTON_H
                               
                               //Required for 1:1 associated classes
#include <string>
#include <histotypes.h>
#include <Xamine.h>

class CXamineButtonPrompt;	// Forward class reference.
class CXaminePushButton;
class CXamineToggleButton;

class CXamineButton      
{
  int                  m_nReturnCode;    // Identifier for button in event.
  std::string          m_sLabel;         // textual label on the button.
  Bool_t               m_fEnabled;   // kfTRUE if button is initially enabled. 
  ButtonSensitivity    m_eWhenSensitive; // Indicates when button is available.
  CXamineButtonPrompt* m_pPrompter;
  
public:
  // Constructor:

  CXamineButton (  int am_nReturnCode,  
		   const std::string& am_sLabel,  
		   Bool_t am_fEnabled,  
		   ButtonSensitivity am_eWhenSensitive,
		   CXamineButtonPrompt& rPrompter) :
    m_nReturnCode(am_nReturnCode),
    m_sLabel(am_sLabel),
    m_fEnabled(am_fEnabled),
    m_eWhenSensitive(am_eWhenSensitive),
    m_pPrompter(&rPrompter)
  { }
  CXamineButton(  int am_nReturnCode,  
		   const char* pLabel,  
		   Bool_t am_fEnabled,  
		   ButtonSensitivity am_eWhenSensitive,
		   CXamineButtonPrompt& rPrompter) :
    m_nReturnCode(am_nReturnCode),
    m_sLabel(pLabel),
    m_fEnabled(am_fEnabled),
    m_eWhenSensitive(am_eWhenSensitive),
    m_pPrompter(&rPrompter)
  { }
  virtual  ~ CXamineButton ( ) { }       //Destructor
	
			//Copy constructor

  CXamineButton (const CXamineButton& aCXamineButton ) :
    m_pPrompter(aCXamineButton.m_pPrompter)
  {   
    m_nReturnCode = aCXamineButton.m_nReturnCode;
    m_sLabel = aCXamineButton.m_sLabel;
    m_fEnabled = aCXamineButton.m_fEnabled;
    m_eWhenSensitive = aCXamineButton.m_eWhenSensitive;
  }                                     

			//Operator= Assignment Operator
			//Update to access 1:M part class attributes
			//Update to access 1:1 associated class attributes
			//Update to access 1:M associated class attributes      
  CXamineButton& operator= (const CXamineButton& aCXamineButton)
  { 
    if (this == &aCXamineButton) return *this;          
    
    m_nReturnCode    = aCXamineButton.m_nReturnCode;
    m_sLabel         = aCXamineButton.m_sLabel;
    m_fEnabled       = aCXamineButton.m_fEnabled;
    m_eWhenSensitive = aCXamineButton.m_eWhenSensitive;
    m_pPrompter      = aCXamineButton.m_pPrompter;
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CXamineButton& aCXamineButton)
  { 
    return (
	    (m_nReturnCode    == aCXamineButton.m_nReturnCode) &&
	    (m_sLabel         == aCXamineButton.m_sLabel) &&
	    (m_fEnabled       == aCXamineButton.m_fEnabled) &&
	    (m_eWhenSensitive == aCXamineButton.m_eWhenSensitive)  &&
	    (m_pPrompter      == aCXamineButton.m_pPrompter)
	    );
  } 
  // Selectors:

public:
  int getReturnCode() const
  {
    return m_nReturnCode;
  }
  const std::string& getLabel() const
  {
    return m_sLabel;
  }
  Bool_t getEnabled() const
  {
    return m_fEnabled;
  }
  ButtonSensitivity getWhenSensitive() const
  {
    return m_eWhenSensitive;
  }
  CXamineButtonPrompt* getPrompter() const
  {
    return m_pPrompter;
  } 
  // Mutators:

protected:
  void setReturnCode (int am_nReturnCode)
  { 
    m_nReturnCode = am_nReturnCode;
  }
  void setLabel (const std::string& am_sLabel)
  { 
    m_sLabel = am_sLabel;
  }
  void setEnabled (Bool_t am_fEnabled)
  { 
    m_fEnabled = am_fEnabled;
  }
  void setWhenSensitive (ButtonSensitivity am_eWhenSensitive)
  { 
    m_eWhenSensitive = am_eWhenSensitive;
  }
  void setPrompter (CXamineButtonPrompt* am_pPrompter)
  { 
    m_pPrompter = am_pPrompter;
  }
  // Operations:

public:
  virtual   void FormatMessageBlock (ButtonDescription& rButton) const  ;
  virtual   CXaminePushButton*   PushButton();   // Type safe upcasts
  virtual   CXamineToggleButton* ToggleButton(); // Type safe upcasts.
};

#endif
