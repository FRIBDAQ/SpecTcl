//  CParameter.h:
//
//    This file defines the CParameter class.
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

#ifndef __PARAMETER_H  //Required for current class
#define __PARAMETER_H
                               //Required for base classes
#ifndef __NAMEDITEM_H
#include "NamedItem.h"
#endif                               

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __CRT_LIMITS_H
#include <limits.h>
#define __CRT_LIMITS_H
#endif
                               
class CParameter  : public CNamedItem        
{
  UInt_t  m_nScale;      // Log(2) of maximum parameter size.
  Float_t m_nLow;        // Low limit of the parameter scaling
  Float_t m_nHigh;       // High limit of the parameter scaling
  std::string m_sUnits;  // Optional units of this parameter scaling
  
public:
  // Constructors with arguments.

  CParameter() : 
    CNamedItem(std::string("Undefined"), UINT_MAX),
    m_nScale(0),
    m_nLow(0.0),
    m_nHigh(0.0)
    { }

  CParameter(UInt_t am_nScale, 
	     const std::string& rName, UInt_t nNumber) :
    CNamedItem(rName, nNumber),
    m_nScale (am_nScale),
    m_nLow(0.0),
    m_nHigh(0.0)
    { }

  CParameter(UInt_t am_nScale,
	     const std::string& rName, UInt_t nNumber,
	     Float_t nLow, Float_t nHigh, std::string am_sUnits) :
    CNamedItem(rName, nNumber),
    m_nScale(am_nScale),
    m_nLow(nLow),
    m_nHigh(nHigh)
    {
      m_sUnits.resize(strlen(am_sUnits.c_str()) + 2, 0);
      m_sUnits = am_sUnits;
    }

  virtual ~CParameter ( ) { }       //Destructor
	
  //Copy constructor
  CParameter (const CParameter& aCParameter )   : 
    CNamedItem (aCParameter) 
    {   
      m_nScale = aCParameter.m_nScale;
      m_nLow   = aCParameter.m_nLow;
      m_nHigh  = aCParameter.m_nHigh;
      m_sUnits = aCParameter.m_sUnits;
    }                                     

  //Operator= Assignment Operator
  CParameter operator= (const CParameter& aCParameter)
    { 
      if(this == &aCParameter) return *this;
      CNamedItem::operator=(aCParameter);
      m_nScale = aCParameter.m_nScale;
      m_nLow   = aCParameter.m_nLow;
      m_nHigh  = aCParameter.m_nHigh;
      m_sUnits = aCParameter.m_sUnits;

      return *this;
    }       

  //Operator== Equality Operator
  int operator== (const CParameter& aCParameter) const
  { return (
	    (CNamedItem::operator== (aCParameter)) &&	    
	    (m_nScale == aCParameter.m_nScale) &&
	    (m_nLow == aCParameter.m_nLow) &&
	    (m_nHigh == aCParameter.m_nHigh) &&
	    (m_sUnits == aCParameter.m_sUnits)
	    );
  }
  // Selectors:

public:
  UInt_t getScale() const
  {
    return m_nScale;
  }
  Float_t getLow() const
    {
      return m_nLow;
    }
  Float_t getHigh() const 
    {
      return m_nHigh;
    }
  std::string getUnits() const
    {
      return m_sUnits;
    }
  // Protected mutators:

protected:
  void setScale (UInt_t am_nScale)
  { 
    m_nScale = am_nScale;
  }
  void setLow(Float_t am_nLow) 
    {
      m_nLow = am_nLow;
    }
  void setHigh(Float_t am_nHigh) 
    {
      m_nHigh = am_nHigh;
    }
  void setUnits(std::string am_sUnits) 
    {
      m_sUnits = am_sUnits;
    }
};

#endif






