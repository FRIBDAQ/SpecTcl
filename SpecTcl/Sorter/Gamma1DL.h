//  CGamma1DL.h:
//
//    This file defines the CGamma1DL class.
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

#ifndef __GAMMA1DL_H  // Required for current class
#define __GAMMA1DL_H

#ifndef __SPECTRUM_H  // Required for base class
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

//  Foward Class definition:

class CParameter;

class CGamma1DL : public CSpectrum
{
  struct ParameterDef {
    UInt_t nParameter;
    UInt_t nScale;
    int operator==(const ParameterDef& r) const {
      return (nParameter == r.nParameter) && (nScale == r.nScale);
    }
  };
  UInt_t m_nScale;                          // Log(2) the spectrum size
  vector<ParameterDef> m_vParameters;       // Vector of parameters

 public:

  //Constructor(s) with arguments

  CGamma1DL(const string& rName, UInt_t nId,
	    vector<CParameter> rrParameters,
	    UInt_t nScale);

  virtual ~CGamma1DL( ) { }      //Destructor

 private:

  //Copy constructor [illegal]
  CGamma1DL(const CGamma1DL& aCGamma1DL);

  //Operator= Assignment operator [illegal]
  CGamma1DL operator= (const CGamma1DL& aCGamma1DL);

 public:

  //Operator== Equality operator [Not too useful still]
  int operator== (const CGamma1DL& aCGamma1D)
    {
      return (
	      (CSpectrum::operator==(aCGamma1D)) &&
	      (m_nScale == aCGamma1D.m_nScale) &&
	      (m_vParameters == aCGamma1D.m_vParameters)
	      );
    }

  // Selectors

 public:

  UInt_t getScale() const
    {
      return m_nScale;
    }
  UInt_t getnParams() const
    {
      return m_vParameters.size();
    }
  UInt_t getParameterId (UInt_t n) const
    {
      return m_vParameters[n].nParameter;
    }
  virtual SpectrumType_t getSpectrumType()
    {
      return keG1D;
    }

  // Mutators (available to derived classes):

 protected:

  void setScale (UInt_t am_nScale)
    {
      m_nScale = am_nScale;
    }

  // Operations:

 public:

  virtual void Increment (const CEvent& rEvent);
  virtual void GammaGateIncrement(const CEvent& rEvent, std::string sGateType);
  virtual ULong_t operator[] (const UInt_t* pIndices) const;
  virtual void set (const UInt_t* pIndices, ULong_t nValue);
  virtual Bool_t UsesParameter (UInt_t nId) const;
  
  virtual UInt_t Dimension (UInt_t n) const
    {
      return ((n == 0) ? (1 << m_nScale) : 0);
    }
  virtual UInt_t Dimensionality () const
    {
      return 1;
    }
  virtual void GetParameterIds(vector<UInt_t>& rvIds);
  virtual void GetResolutions(vector<UInt_t>& rvResolutions);
  virtual Int_t getScale(UInt_t nIndex);
};

#endif
