//  CGamma2DW.h:
//
//    This file defines the CGamma2DW class.
//
// Author:
//    Jason Venema
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:venemaja@msu.edu
//
//  Copyright 2001 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef __GAMMA2D_H  //Required for current class
#define __GAMMA2D_H
                               //Required for base classes
#ifndef __SPECTRUM_H
#include "Spectrum.h"
#endif                               

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __STL_VECTOR
#include <vector>
#define __STL_VECTOR
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

//  Forward class definition:

class CParameter;


class CGamma2DW : public CSpectrum
{
  struct ParameterDef {
    UInt_t nParameter;
    UInt_t nScale;
    int operator==(const ParameterDef& r) const {
      return (nParameter == r.nParameter) && (nScale == r.nScale);
    }
  };
  UInt_t m_nXScale;		// Log(2) x axis.
  UInt_t m_nYScale;		// Log(2) y Axis.
  vector<ParameterDef> m_vParameters; // Vector of parameters
  
public:

			//Constructor(s) with arguments

  CGamma2DW(const std::string& rName, UInt_t nId,
	       vector<CParameter>& rParameters,
	       UInt_t nXScale, UInt_t nYScale);

  virtual  ~ CGamma2DW( ) { }       //Destructor	
private:
			//Copy constructor [illegal]

  CGamma2DW(const CGamma2DW& acspectrum1dl); 

			//Operator= Assignment Operator [illegal] 

  CGamma2DW operator= (const CGamma2DW& aCGamma1D);

			//Operator== Equality Operator [Not too useful but:]
public:
  int operator== (const CGamma2DW& aCGamma)
  { return (
	    (CGamma2DW::operator== (aCGamma)) &&
	    (m_nXScale == aCGamma.m_nXScale) &&
	    (m_vParameters == aCGamma.m_vParameters) &&
	    (m_nYScale == aCGamma.m_nYScale)
	    );
  }
  // Selectors 

public:
  UInt_t getXScale() const
  {
    return m_nXScale;
  }
  UInt_t getYScale() const
  {
    return m_nYScale;
  }
  UInt_t getnParams() const
  {
    return m_vParameters.size();
  }
  virtual SpectrumType_t getSpectrumType() {
    return keG2D;
  }
  // Mutators (available to derived classes:

protected:
  void setXScale (UInt_t am_nScale)
  { 
    m_nXScale = am_nScale;
  }
  void setYScale(UInt_t nScale)
  {
    m_nYScale = nScale;
  }

  //
  //  Operations:
  //   
public:                 
  virtual   void Increment(const CEvent& rEvent)  ;
  virtual void GammaGateIncrement(const CEvent& Event, std::string sGateType);
  virtual   ULong_t operator[](const UInt_t* pIndices) const;
  virtual   void    set(const UInt_t* pIndices, ULong_t nValue);
  virtual   Bool_t UsesParameter (UInt_t nId) const;
  virtual   UInt_t Dimension (UInt_t n) const;

  virtual   UInt_t Dimensionality () const {
    return 2;
  }
  virtual void GetParameterIds(vector<UInt_t>& rvIds);
  virtual void GetResolutions(vector<UInt_t>&  rvResolutions);
  virtual Int_t getScale(UInt_t nIndex);
  
};

#endif
