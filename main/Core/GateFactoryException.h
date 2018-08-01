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


// Class: CGateFactoryException                     //ANSI C++
//
// Exceptions which can be thrown by the gate factory.

//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
// 
// (c) Copyright NSCL 1999, All rights reserved .h
//

#ifndef GATEFACTORYEXCEPTION_H  //Required for current class
#define GATEFACTORYEXCEPTION_H

                               //Required for base classes
#include <Exception.h>
#include "GateFactory.h"
#include <string>

class CGateFactoryException  : public CException        
{        
public:
  enum GateFactoryExceptionReason {
    WrongConstructor,
    WrongGateCount,
    WrongParameterCount,
    WrongPointCount,
    NoSuchParameter,
    MustBeBand,
    NoSuchGate,
    MustBeSameParams,
    NoSuchSpectrum
  };
private:
  GateFactoryExceptionReason m_eReason; //Enum containing reason of exception.
  CGateFactory::GateType m_eGateType; //Type of gate being constructed        
  static const char**  m_svReasonTexts; //Pointer to mappings between reason 
				// enumerator and textual std::strings.
  static const char**  m_svGateTypeStrings;


public:
  CGateFactoryException(GateFactoryExceptionReason eReason,
			CGateFactory::GateType     eType,
			const std::string&              rDoing) :
    CException(rDoing),
    m_eReason(eReason),
    m_eGateType(eType)
    {}

   virtual ~ CGateFactoryException ( )  // Destructor 
     { }  

  
   //Copy constructor 

  CGateFactoryException (const CGateFactoryException& aCGateFactoryException )   : CException (aCGateFactoryException) 
  {
       
    m_eReason = aCGateFactoryException.m_eReason;
    m_eGateType = aCGateFactoryException.m_eGateType;
     
  }                                     

   // Operator= Assignment Operator 

  CGateFactoryException& operator= (const CGateFactoryException& aCGateFactoryException);
 
   //Operator== Equality Operator 

  int operator== (const CGateFactoryException& aCGateFactoryException) const;
	
// Selectors:

public:

  GateFactoryExceptionReason getm_eReason() const
  { return m_eReason;
  }
  CGateFactory::GateType getGateType() const
  { return m_eGateType;
  }
                       
  const char**  getReasonTexts()
  { 
    return m_svReasonTexts;
  }
                       
// Mutators:

protected:

  void setReason (const GateFactoryExceptionReason am_eReason)
  { m_eReason = am_eReason;
  }
  void setGateType (const CGateFactory::GateType am_eGateType)
  { m_eGateType = am_eGateType;
  }
       
  static void setReasonTexts (const  char**  am_svReasonTexts)
  { m_svReasonTexts = am_svReasonTexts;
  }
                       
public:
  
  virtual   const char*  ReasonText ()   const ;
  virtual   Int_t ReasonCode ()    ;
  static    const char*  ReasonCodeToText (GateFactoryExceptionReason eReason)    ;
 
protected:

private:

};

#endif
