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


//  CXamineSpectrumPrompt.h:
//
//    This file defines the CXamineSpectrumPrompt class.
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

#ifndef XAMINESPECTRUMPROMPT_H  //Required for current class
#define XAMINESPECTRUMPROMPT_H
                               //Required for base classes
#include "XamineButtonPrompt.h"
#include <histotypes.h>
#include <string>
                           
                               
class CXamineSpectrumPrompt  : public CXamineButtonPrompt        
{
  DialogSpectrumType_t m_eSpectrumType;	// Types of spectra to show in prompter
  
public:

  CXamineSpectrumPrompt (const std::string& rPrompt,
			 DialogSpectrumType_t am_eSpectrumType = keAny)   :
    CXamineButtonPrompt(rPrompt),
    m_eSpectrumType (am_eSpectrumType)  { } 
       
  CXamineSpectrumPrompt(const char* pPrompt,
			DialogSpectrumType_t eSpectrumType = keAny) :
    CXamineButtonPrompt(pPrompt),
    m_eSpectrumType(eSpectrumType)
  { }
  virtual ~ CXamineSpectrumPrompt ( ) { }       //Destructor
	
			//Copy constructor

  CXamineSpectrumPrompt (const CXamineSpectrumPrompt& aCXamineSpectrumPrompt )   : CXamineButtonPrompt (aCXamineSpectrumPrompt) 
  {
    m_eSpectrumType = aCXamineSpectrumPrompt.m_eSpectrumType;
                
  }                                     

			//Operator= Assignment Operator

  CXamineSpectrumPrompt& operator= 
                      (const CXamineSpectrumPrompt& aCXamineSpectrumPrompt)
  { 
    if (this == &aCXamineSpectrumPrompt) return *this;          
    CXamineButtonPrompt::operator= (aCXamineSpectrumPrompt);
    m_eSpectrumType = aCXamineSpectrumPrompt.m_eSpectrumType;
    
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CXamineSpectrumPrompt& aCXamineSpectrumPrompt)
  { 
    return (
	    (CXamineButtonPrompt::operator== (aCXamineSpectrumPrompt)) &&
	    (m_eSpectrumType == aCXamineSpectrumPrompt.m_eSpectrumType) 
	    );
  }         
  // Selectors:

public:
  int getSpectrumType() const
  {
    return m_eSpectrumType;
  }
  // Mutators:

protected:
                       
                       //Set accessor function for attribute
  void setSpectrumType (DialogSpectrumType_t am_eSpectrumType)
  { 
    m_eSpectrumType = am_eSpectrumType;
  }
  // operations

public:                     
  virtual   void FormatPrompterBlock (ButtonDescription& rButton) const  ;
 
};

#endif
