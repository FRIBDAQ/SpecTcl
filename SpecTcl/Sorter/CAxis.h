// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 

#ifndef __CAXIS_H  //Required for current class
#define __CAXIS_H

//
// Include files:
//
#ifndef  __HISTOTYPES_H
#include <histotypes.h>        //Required for include files  
#endif

                               //Required for 1:1 association classes
#ifndef __CPARAMETERMAPPING_H    //CParameterMapping
#include "CParameterMapping.h"
#endif 
 
#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

/*!
Describes the range and granularity of an axis.  If the parameter that 
is displayed on this axis has limits or mapping, this information is also held.
The purpose of CAxis is to provide transformations between spectrum and 
parameter coordinates.  In the case of mapped parameters, the transformation 
is between the spectrum coordinates and raw parameter value.
There are three potential coordinate systems:
- The axis coordinates: these are integers from 0 to m_nChannels - 1.
- The Raw parameter coordinates: These are arbitrary reals, unless
   m_fMappedParam is true in which case they are reals between
   0 and 2^m_nParameterBits - 1.
- The mapped parameter coordinates: Same as raw parameter
   coordinates unless m_fMappedParam is true.. in which case
   these are float values between m_fMappingLow and m_fMappingHigh.
*/
class CAxis      
{
  // Member data (private by coding convention).

private:
  float m_fLow;  //!<  Parameter value corresponding to axis coord 0.  
  float m_fHigh; //!<  Parameter val corresponding to Axis coord m_nChannels-1.
  UInt_t m_nChannels;	       //!<  Number of channels on this axis.  
  CParameterMapping  m_ParameterSpecification; //!< Parameter mapping info.
  
public:

  // Constructors and other canonical functions.

  CAxis (UInt_t nChannels,
	 string sUnits = string(""));	//!< idendtity tansform for [0,nChannels-1].
  CAxis (Float_t fLow,
	 Float_t fHigh,
	 UInt_t  nChannels,
	 string sUnits = string(""));	//!< Unmapped real parameter in [fLow, fHigh].
  CAxis(Float_t fLow,
	Float_t fHigh,
	UInt_t  nChannels,
	CParameterMapping Mapping); //!< Mapped param -> [fLow, fHigh].
  CAxis (const CAxis& aCAxis );	//!< Copy constructor.

  ~ CAxis ( );                      //!< Destructor 
  

  CAxis& operator= (const CAxis& aCAxis); //!< Assignment.
  int operator== (const CAxis& aCAxis) const; //!< Equality comparison
  int operator!= (const CAxis& aCAxis) const { //!< Inequality comparison.
    return !operator==(aCAxis);
  }

  
public:
  // Selectors for member data.
  
  float getLow() const		//!< Return Coordinate of 0 on axis.
  { 
    return m_fLow;
  }  
  float getHigh() const		//!< Return coordinate of axis fullscale.
  { 
    return m_fHigh;
  }  
  int getChannels() const	//!< Return axis channel count.
  { 
    return m_nChannels;
  }   
  const CParameterMapping getParameterMapping() const
  { 
    return m_ParameterSpecification;
  }      

  
protected:
  // Member data direct mutators. Available to derived classes but use
  // with caution as these are not fixed interface definitions.
  

  void setLow (const float am_fLow)
  { 
    m_fLow = am_fLow;
  }  
  void setHigh (const float am_fHigh)
  { 
    m_fHigh = am_fHigh;
  }  
  void setChannels (const int am_nChannels)
  { 
    m_nChannels = am_nChannels;
  }   
  void setParameterMapping (CParameterMapping am_ParameterSpecification)
  { 
    m_ParameterSpecification = am_ParameterSpecification;
  }  
  
  // Class operations:
  
public:
  
  Int_t ParameterToAxis (Float_t fParameter)   ; // 
  Float_t AxisToParameter (UInt_t nAxisValue)   ; // 
  Float_t AxisToMappedParameter (Int_t nAxisValue)   ; // 
  Int_t MappedParameterToAxis (Float_t fParameterValue)   ; // 
  
};

#endif
