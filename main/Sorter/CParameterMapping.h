// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 

#ifndef __CPARAMETERMAPPING_H  //Required for current class
#define __CPARAMETERMAPPING_H

//
// Include files:
//

#ifndef __HISTOTYPES_H
#include <histotypes.h>        //Required for include files  
#endif

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

class CParameter;

/*!
Provides a mapping between raw parameter coordinates and potentially
mapped coordinates.  There are two types of parameters:
- Reals - these have no mappings. the coordinates of the parameter represent
  actual physical values.  The transformation functions are identity transforms.
- Mapped - these parameters have raw values between 0 and 2^m_nBits - 1 
   which, upon transformation, represent real values between m_fLow and m_fHigh.
*/
class CParameterMapping      
{
private:
  

  Bool_t m_fMapped;		//!  True if parameter is a mapped parameter.  
  UInt_t m_nBits;		//!  Bits of resolution of the raw parameter.  
  Float_t m_fLow;		//!  Low part of range of mapped parameter.  
  Float_t m_fHigh;		//!  High value of mapped parameter range  
  STD(string) m_sUnits;		//!  Parameter units or empty if there are none.  
  
  
public:
  // Constructors and other canonical operations.

  CParameterMapping (const STD(string)& sUnits=STD(string)(""));		//!< Constructs identity mapping.
  CParameterMapping(UInt_t  nBits,
		    Float_t fLow,
		    Float_t fHigh,
		    const STD(string)& sUnits = STD(string)("")); //!< Create mapping.
  CParameterMapping(const CParameter& rParam); //!< Construct from param definition.
  ~ CParameterMapping ( );  //!< Destructor
  CParameterMapping (const CParameterMapping& rhs ); //!<  Copy Constructor 
  
  CParameterMapping& operator= (const CParameterMapping& rhs); //!< assignment
  int operator== (const CParameterMapping& rhs) const; //!< Equality compare
  int operator!= (const CParameterMapping& rhs) const { //!< ineq compare.
    return !(operator==(rhs));
  }

// Selectors:

public:

  Bool_t isMapped() const	//!< Selector for m_fMapped
  { 
    return m_fMapped;
  }  

  UInt_t getBits() const	//!< Selectro fro m_nBits.
  { 
    return m_nBits;
  }  

  Float_t getLow() const	//!< Selector for m_fLow
  { 
    return m_fLow;
  }  

  Float_t getHigh() const	//!< Selector for m_fHigh
  { 
    return m_fHigh;
  }  

  STD(string) getUnits() const	//!< Selector for m_sUnits
  { 
    return m_sUnits;
  }   

// Attribute mutators:

protected:


  void setMapped (const Bool_t am_fMapped) //!< Mutator for m_fMapped.
  {
    m_fMapped = am_fMapped;
  }  

  void setBits (const UInt_t am_nBits) //!< Mutator for m_nBits
  { 
    m_nBits = am_nBits;
  }  

  void setLow (const Float_t am_fLow) //!< Mutator for m_fLow
  { 
    m_fLow = am_fLow;
  }  

  void setHigh (const Float_t am_fHigh)	//!< Mutator for m_fHigh
  { 
    m_fHigh = am_fHigh;
  }  

  void setUnits (const STD(string) am_sUnits) //!< Mutator for m_sUnits
  { 
    m_sUnits = am_sUnits;
  }   

  // Class operations:

public:

  // Profiling suggests this hsould be inlined.

  Float_t RawToMapped (Float_t fRaw) {
    if(!m_fMapped) return fRaw;	// Unmapped parameters don't change
    Float_t RawMax = (Float_t)((1 << m_nBits) - 1);
    return m_fLow + (m_fHigh - m_fLow)*fRaw/RawMax; 
    
  } 
  Float_t MappedToRaw (Float_t fMapped)   ; //!< Convert Mapped to raw

};

#endif
