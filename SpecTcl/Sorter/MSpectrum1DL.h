//
//  CMSpectrum1DL.h:
//
//    This file defines the CMSpectrum1D class.
//
// Author:
//    Jason Venema
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:venema@nscl.msu.edu
//
//  Copyright 2002 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

#ifndef __MSPECTRUM1DL_H  //Required for current class
#define __MSPECTRUM1DL_H
                               //Required for base classes
#ifndef __SPECTRUM1DL_H
#include "Spectrum1DL.h"
#endif

#ifndef __STL_STRING
#include <string>
#define __STL_STRING
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

//  Foward Class definitions:

class CParameter;

class CMSpectrum1DL  : public CSpectrum1DL
{
  struct MParameterDef {
    UInt_t  nScale;
    Float_t nLow;
    Float_t nHigh;
    std::string sUnits;
    int operator== (const MParameterDef& r) const {
      return ((nLow == r.nLow) && (nHigh == r.nHigh) && (nScale == r.nScale)
	      && (sUnits == r.sUnits));
    }
  };
  Float_t m_nLow;             // The low limit to use on the mapping
  Float_t m_nHigh;            // The upper limit to use on the mapping
  UInt_t  m_nChannels;        // The stepsize to use
  MParameterDef m_Parameter;  // Our parameter's attribs.
  
 public:

  //Constructor(s) with arguments
  CMSpectrum1DL(const std::string& rName, UInt_t nId,
		const CParameter& rParameter,
		Float_t nLow, Float_t nHigh, 
		UInt_t nChannels);

  // Destructor
  virtual ~CMSpectrum1DL() { }

 private:
  //Copy constructor [illegal]
  CMSpectrum1DL(const CMSpectrum1DL& acspectrum1dl); 
  
  //Operator= Assignment Operator [illegal] 
  CMSpectrum1DL operator= (const CMSpectrum1DL& aCSpectrum1D);
  
  //Operator== Equality Operator [Not too useful but:]
 public:
  int operator== (const CMSpectrum1DL& aCMSpectrum1D)
    { return (
	      (CMSpectrum1DL::operator== (aCMSpectrum1D)) &&
	      (m_nLow == aCMSpectrum1D.m_nLow) &&
	      (m_nHigh == aCMSpectrum1D.m_nHigh) &&
	      (m_nChannels == aCMSpectrum1D.m_nChannels) &&
	      (m_Parameter == aCMSpectrum1D.m_Parameter)
	      );
    }
                             
  // Selectors 
 public:
  Float_t getLow() const
    {
      return m_nLow;
    }
  Float_t getHigh() const
    {
      return m_nHigh;
    }
  UInt_t getChannels() const
    {
      return m_nChannels;
    }
  MParameterDef getParameterDef() const
    {
      return m_Parameter;
    }
  std::string getUnits() const
    {
      return m_Parameter.sUnits;
    }
  virtual SpectrumType_t getSpectrumType() 
    {
      return keM1D;
    }

  // Mutators (available to derived classes:
 protected:
  void setLow (Float_t am_nLow)
    { 
      m_nLow = am_nLow;
    }
  void setHigh (Float_t am_nHigh)
    { 
      m_nHigh = am_nHigh;
    }
  void setChannels (UInt_t am_nChannels)
    { 
      m_nChannels = am_nChannels;
    }
  //
  //  Operations:
  //   
 public:                 
  virtual void Increment(const CEvent& rEvent);
  UInt_t Dimension(UInt_t n) const {
    return ((n == 0) ? m_nChannels : 0);
  }
  UInt_t       Randomize(Float_t nChannel);
  UInt_t       SpecToParamPoint(UInt_t nSpecPoint);
  UInt_t       ParamToSpecPoint(UInt_t nParamPoint);
  UInt_t       RandomizeToMultipleBins(Float_t nChannel, Float_t nRatio);
};

#endif
