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
//  CXamineButtonPrompt.h:
//
//    This file defines the CXamineButtonPrompt class.
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

#ifndef XAMINEBUTTONPROMPT_H  //Required for current class
#define XAMINEBUTTONPROMPT_H

#include <Xamine.h>
#include <string>
                               
class CXamineButtonPrompt      
{
  std::string m_sPromptString;  // Text associated with the prompter.
  
public:


			//Constructor with arguments

  CXamineButtonPrompt (const std::string& am_sPromptString  )       
  :   m_sPromptString (am_sPromptString)  
  { }
  CXamineButtonPrompt(const char* pPrompt) :
    m_sPromptString(pPrompt)
  {}
  virtual  ~ CXamineButtonPrompt ( ) { }       //Destructor
	
  // Copy Constructor

  CXamineButtonPrompt (const CXamineButtonPrompt& aCXamineButtonPrompt ) 
  {   
    m_sPromptString = aCXamineButtonPrompt.m_sPromptString;
                
  }                                     

			//Operator= Assignment Operator

  CXamineButtonPrompt& operator= 
                         (const CXamineButtonPrompt& aCXamineButtonPrompt)
  { 
    if (this == &aCXamineButtonPrompt) return *this;          
  
    m_sPromptString = aCXamineButtonPrompt.m_sPromptString;
        
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CXamineButtonPrompt& aCXamineButtonPrompt)
  { 
    return (
	    (m_sPromptString == aCXamineButtonPrompt.m_sPromptString) 
	    );
  }           
  // Selectors:

public:

  std::string getPromptString() const
  {
    return m_sPromptString;
  }
  // Mutators - available only to derived classes:

protected:
  void setPromptString (std::string am_sPromptString)
  { 
    m_sPromptString = am_sPromptString;
  }
  // Operations:
  //
public:                       
  virtual   void FormatPrompterBlock(ButtonDescription& rButton) const   = 0;
  
  // Protected utilities:

protected:
  void SetPromptString(ButtonDescription& rButton) const;
};

#endif
