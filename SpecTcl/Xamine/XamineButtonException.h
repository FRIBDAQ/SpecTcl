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

#ifndef __XAMINEBUTTONEXCEPTION_H  //Required for current class
#define __XAMINEBUTTONEXCEPTION_H
                               //Required for base classes
#ifndef __EXCEPTION_H
#include "Exception.h"
#endif    

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __XAMINE_XAMINE_H                           
#include <Xamine.h>   //Required for include files, eg 
#define __XAMINE_XAMINE_H
#endif

#ifndef __CXX_STRSTREAM_H
#include <strstream.h>
#define __CXX_STRSTREAM_H
#endif

#ifndef __XAMINEBUTTON_H
#include "XamineButton.h"
#endif


class CXamineButtonException   : public CException
{
  Int_t          m_nError;	// Error code responsible for the exception
  CXamineButton* m_pButton;	// Button which is causing the problem.
  Bool_t         m_fFormatted;
  strstream      m_ReasonText; 
  
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
  virtual   const char* ReasonText ()  ;
  virtual   Int_t ReasonCode ()  ;
 
  // Protected utilities:

protected:
  void SetButton(const CXamineButton& rButton);
};

#endif
