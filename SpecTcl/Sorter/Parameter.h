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
  UInt_t m_nScale;      // Log(2) of maximum parameter size.
  
public:
  // Constructors with arguments.

  CParameter() : 
    CNamedItem(std::string("Undefined"), UINT_MAX),
    m_nScale(0)
  {}
  CParameter (  UInt_t am_nScale, 
		const std::string& rName, UInt_t nNumber  ) :
    CNamedItem(rName, nNumber),
    m_nScale (am_nScale) 
  { }        
  virtual ~CParameter ( ) { }       //Destructor
	
			//Copy constructor

  CParameter (const CParameter& aCParameter )   : 
    CNamedItem (aCParameter) 
  {   
    m_nScale = aCParameter.m_nScale;            
  }                                     

			//Operator= Assignment Operator

  CParameter operator= (const CParameter& aCParameter)
  { 
    if (this == &aCParameter) return *this;          
    CNamedItem::operator= (aCParameter);
    m_nScale = aCParameter.m_nScale;
    
    return *this;
  }                                     

			//Operator== Equality Operator

  int operator== (const CParameter& aCParameter) const
  { return (
	    (CNamedItem::operator== (aCParameter)) &&	    
	    (m_nScale == aCParameter.m_nScale) 
	    );
  }                             
  // Selectors:

public:
  UInt_t getScale() const
  {
    return m_nScale;
  }
  // Protected mutators:

protected:
  void setScale (UInt_t am_nScale)
  { 
    m_nScale = am_nScale;
  }
                       
};

#endif






