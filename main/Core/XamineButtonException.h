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
//  CXamineButtonException.h:
//
//    This file defines the CXamineButtonException class.
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

#ifndef XAMINEBUTTONEXCEPTION_H  //Required for current class
#define XAMINEBUTTONEXCEPTION_H
                               //Required for base classes
#include "Exception.h"
#include <histotypes.h>
#include <string>
#include <Xamine.h>   //Required for include files, eg 
#include <sstream>
#include "XamineButton.h"


class CXamineButtonException   : public CException
{
  Int_t          m_nError;	// Error code responsible for the exception
  CXamineButton* m_pButton;	// Button which is causing the problem.
  mutable Bool_t         m_fFormatted;
  mutable std::stringstream      m_ReasonText; 
  
public:

			//Constructor with arguments

  CXamineButtonException(Int_t nStatus,
			 const CXamineButton& rButton,
			 const std::string&   rDoing);
  virtual ~CXamineButtonException ( ) { delete m_pButton; } // Destructor
	
			//Copy constructor
	
  CXamineButtonException 
                    (const CXamineButtonException& aCXamineButtonException );


			//Operator= Assignment Operator

  CXamineButtonException& operator= 
                    (const CXamineButtonException& aCXamineButtonException);

			//Operator== Equality Operator

  int operator== (const CXamineButtonException& aCXamineButtonException)
  { 
    return (
	    (CException::operator== (aCXamineButtonException)) &&
	    (m_nError == aCXamineButtonException.m_nError) &&
	    (*m_pButton == *aCXamineButtonException.m_pButton) 
	    );
  }                    
  // Selectors:         
               
public:
  Int_t getError() const
  {
    return m_nError;
  }

                       //Get accessor function for attribute
  const CXamineButton* getButton() const
  {
    return m_pButton;
  }
  // Mutators:

protected:                   
  void setError (Int_t am_nError)
  { 
    m_nError = am_nError;
  }
  void setButton (CXamineButton* am_pButton)
  { 
    m_pButton = am_pButton;
  }
  // Member operations:

public:                   
  virtual   const char* ReasonText ()  const;
  virtual   Int_t ReasonCode ()  const;
 
  // Protected utilities:

protected:
  void SetButton(const CXamineButton& rButton);
};

#endif
