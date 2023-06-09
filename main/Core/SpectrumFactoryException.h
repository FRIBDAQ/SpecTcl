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



//  CSpectrumFactoryException.h:
//
//    This file defines the CSpectrumFactoryException class.
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

#ifndef SPECTRUMFACTORYEXCEPTION_H  //Required for current class
#define SPECTRUMFACTORYEXCEPTION_H
                               //Required for base classes

#include "Exception.h"
#include <string>
#include <string.h>
#include <histotypes.h>

                           
                               

class CSpectrumFactoryException  : public CException        
{
public:
  enum SpectrumErrorReason_t {
    keBadSpectrumType,
    keBadDataType,
    keBadParameterCount,
    keBadResolutionCount,
    keBadTransformCount,
    keBadChannelCount,
    keNoMappingInformation,
    keConstructionFailed,
    keBadGateType
  };
private:
  DataType_t            m_eType;          // Type of data held in spectrum 
  SpectrumType_t        m_eSpectrumType;  // Type of spectrum being created.
  std::string           m_sName;          // Name of the spectrum.
  SpectrumErrorReason_t m_eReason;        // Reason for the exception
  char*                 m_pMessage;
public:
			//Constructor with arguments
  CSpectrumFactoryException (  DataType_t            am_eType,  
			       SpectrumType_t        am_eSpectrumType,  
			       const std::string           am_sName,  
			       SpectrumErrorReason_t am_eReason,  
			       const std::string&          rDoing);
  ~ CSpectrumFactoryException ( ) { delete []m_pMessage; }       //Destructor
	
			//Copy constructor

  CSpectrumFactoryException (
	       const CSpectrumFactoryException& aCSpectrumFactoryException ) :
    CException (aCSpectrumFactoryException) 
  {
    CopyToMe(aCSpectrumFactoryException);
  }                                     

			//Operator= Assignment Operator

  CSpectrumFactoryException& operator= (
             const CSpectrumFactoryException& aCSpectrumFactoryException)
  { 
    if (this == &aCSpectrumFactoryException) return *this;          

    CException::operator= (aCSpectrumFactoryException);
    CopyToMe(aCSpectrumFactoryException);        
    return *this;                                                                                                 
  }                                     

			//Operator== Equality Operator

  int operator== (
		  const CSpectrumFactoryException& aCSpectrumFactoryException)
  { 
    return (
	    (CException::operator== (aCSpectrumFactoryException)) &&
	    
	    (m_eType == aCSpectrumFactoryException.m_eType) &&
	    (m_eSpectrumType == aCSpectrumFactoryException.m_eSpectrumType) &&
	    (m_sName == aCSpectrumFactoryException.m_sName) &&
	    (m_eReason == aCSpectrumFactoryException.m_eReason) 
	    );
  }                             
  // Selectors:
                       //Get accessor function for attribute
  DataType_t getType() const
  {return m_eType;
  }

                       //Get accessor function for attribute
  SpectrumType_t getSpectrumType() const
  {return m_eSpectrumType;
  }

                       //Get accessor function for attribute
  std::string getName() const
  {return m_sName;
  }

                       //Get accessor function for attribute
  SpectrumErrorReason_t getReason() const
  {return m_eReason;
  }

  // Mutators:

protected:
                       //Set accessor function for attribute
  void setType (DataType_t am_eType)
  { 
    m_eType = am_eType;
    UpdateMessage();
  }

                       //Set accessor function for attribute
  void setSpectrumType (SpectrumType_t am_eSpectrumType)
  { 
    m_eSpectrumType = am_eSpectrumType;
    UpdateMessage();
  }

                       //Set accessor function for attribute
  void setName (std::string am_sName)
  { 
    m_sName = am_sName;
    UpdateMessage();
  }

                       //Set accessor function for attribute
  void setReason (SpectrumErrorReason_t am_eReason)
  { 
    m_eReason = am_eReason;
    UpdateMessage();
  }

  // overrides:

public:        
  virtual   const  char* ReasonText () const  ;
  virtual   Int_t ReasonCode () const  ;

  // Utilities:

protected:
  static const char* DataType(DataType_t eType);
  static const char* SpectrumType(SpectrumType_t eType);
  const static char* MessageText(SpectrumErrorReason_t eReason);
  void  UpdateMessage();

  void CopyToMe(const CSpectrumFactoryException& aCSpectrumFactoryException) {
    m_eType = aCSpectrumFactoryException.m_eType;
    m_eSpectrumType = aCSpectrumFactoryException.m_eSpectrumType;
    m_sName = aCSpectrumFactoryException.m_sName;
    m_eReason = aCSpectrumFactoryException.m_eReason;
    m_pMessage = new char[strlen(aCSpectrumFactoryException.m_pMessage) + 1];
    strcpy(m_pMessage, aCSpectrumFactoryException.m_pMessage);
  }
};

#endif
