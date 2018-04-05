/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

//  CDictionaryException.h:
//
//    This file defines the CDictionaryException class.
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

#ifndef __DICTIONARYEXCEPTION_H  //Required for current class
#define __DICTIONARYEXCEPTION_H
                               //Required for base classes
#ifndef __EXCEPTION_H
#include "Exception.h"
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
                         
class CDictionaryException  : public CException        
{
private:
  Int_t      m_nReason;		// Code describing reason for exception
  std::string m_sName;		// Name of item being looked up.
  UInt_t      m_nId;		// Id of item being looked up.
  Bool_t      m_fIdValid;	// kfTRUE if m_nId is valid.
  Bool_t      m_fNameValid;	// kfTRUE if m_sName is valid.
  std::string m_sReasonText;	// Most recent reason text.
public:
  enum {			// Possible values for m_nReason:
    knDuplicateKey,		// Insert to dictionary was duplicate key.
    knDuplicateId,		// Duplicate ID insertion attempted. but not
				// allowed by application.
    knNoSuchId,			// Search for Id failed.
    knNoSuchKey,	       	// Search for keyword failed.
    knWrongGateType             // Gate type not compatible with spectrum
  };
  
public:
  // Constructors where name is valid:

  CDictionaryException(Int_t nReason, const char* pDoing, 
		       const char* pName) :
    CException(pDoing),
    m_nReason(nReason),
    m_sName(pName),
    m_nId(0),
    m_fIdValid(kfFALSE),
    m_fNameValid(kfTRUE)
  { UpdateReasonText(); }
  CDictionaryException(Int_t nReason, const char* pDoing, 
		       const std::string& rName) :
    CException(pDoing),
    m_nReason(nReason),
    m_sName(rName),
    m_nId(0),
    m_fIdValid(kfFALSE),
    m_fNameValid(kfTRUE)
  { UpdateReasonText(); }

  CDictionaryException(Int_t nReason, const std::string& rDoing,
		       const char* pName) :
    CException(rDoing),
    m_nReason(nReason),
    m_sName(pName),
    m_nId(0),
    m_fIdValid(kfFALSE),
    m_fNameValid(kfTRUE)
  { UpdateReasonText(); }

  CDictionaryException(Int_t nReason, const std::string& rDoing,
		       const std::string& rName) :
    CException(rDoing),
    m_nReason(nReason),
    m_sName(rName),
    m_nId(0),
    m_fIdValid(kfFALSE),
    m_fNameValid(kfTRUE)
  { UpdateReasonText(); }

  // Constructors for which the ID is valid:
 
  CDictionaryException(Int_t nReason, const char* pDoing,
		       UInt_t nId) :
    CException(pDoing),
    m_nReason(nReason),
    m_sName(""),
    m_nId(nId),
    m_fIdValid(kfTRUE),
    m_fNameValid(kfFALSE)
  { UpdateReasonText(); }
  CDictionaryException(Int_t nReason, const std::string& rDoing,
		       UInt_t nId) :
    CException(rDoing),
    m_nReason(nReason),
    m_sName(""),
    m_nId(nId),
    m_fIdValid(kfTRUE),
    m_fNameValid(kfFALSE)
  { UpdateReasonText(); }

  virtual ~CDictionaryException ( ) { }       //Destructor

  //Copy constructor

  CDictionaryException (const CDictionaryException& aCDictionaryException ) : 
    CException (aCDictionaryException) 
  {   
    m_nReason = aCDictionaryException.m_nReason;
    m_sName = aCDictionaryException.m_sName;
    m_nId = aCDictionaryException.m_nId;
    m_fIdValid = aCDictionaryException.m_fIdValid;
    m_fNameValid = aCDictionaryException.m_fNameValid;        
    UpdateReasonText();
  }                                     

			//Operator= Assignment Operator

  CDictionaryException operator= 
         (const CDictionaryException& aCDictionaryException)
  { 
    if (this != &aCDictionaryException) {
      CException::operator= (aCDictionaryException);
      m_nReason = aCDictionaryException.m_nReason;
      m_sName = aCDictionaryException.m_sName;
      m_nId = aCDictionaryException.m_nId;
      m_fIdValid = aCDictionaryException.m_fIdValid;
      m_fNameValid = aCDictionaryException.m_fNameValid;
      UpdateReasonText();
    }
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CDictionaryException& aCDictionaryException)
  { 
    return (
	    (CException::operator== (aCDictionaryException)) &&
	    (m_nReason == aCDictionaryException.m_nReason)   &&
	    (m_sName == aCDictionaryException.m_sName)       &&
	    (m_nId == aCDictionaryException.m_nId)           &&
	    (m_fIdValid == aCDictionaryException.m_fIdValid) &&
	    (m_fNameValid == aCDictionaryException.m_fNameValid) 
	    );
  }
  //
  // Selectors:
  //
public:
  Int_t getReason() const
  {
    return m_nReason;
  }
  std::string getm_sName() const
  {
    return m_sName;
  }
  UInt_t getID() const
  {
    return m_nId;
  }
  Bool_t getIdValid() const
  {
    return m_fIdValid;
  }
  Bool_t getNameValid() const
  {
    return m_fNameValid;
  }
  //
  // Mutators:  Some are only available to derived classes:
  //                       
protected:
  void setReason (Int_t am_nReason)
  { 
    m_nReason = am_nReason;
    UpdateReasonText();
  }
  void setIdValid (Bool_t am_fIdValid)
  { 
    m_fIdValid = am_fIdValid;
    UpdateReasonText();
  }
  void setNameValid (Bool_t am_fNameValid)
  { 
    m_fNameValid = am_fNameValid;
    UpdateReasonText();
  }
  //
  // Mutators: Others are available to the public:
  //
public:
  void setName (std::string am_sName)
  { 
    m_sName = am_sName;
    m_fNameValid = kfTRUE;
    UpdateReasonText();
  }
  void setID (UInt_t am_nId)
  { 
    m_nId      = am_nId;
    m_fIdValid = kfTRUE;
    UpdateReasonText();
  }

  //  The CException class implementation:

  virtual   const char* ReasonText ()  const;
  virtual   Int_t ReasonCode () const  ;
 
  // Protected utilities:
  //
protected:
  void UpdateReasonText();
};

#endif
