//  CMSpectrum2DW.h:
//
//    This file defines the CMSpectrum2DW class.
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

#ifndef __MSPECTRUM2DW_H  //Required for current class
#define __MSPECTRUM2DW_H
                               //Required for base classes
#ifndef __SPECTRUM2DW_H
#include "Spectrum2DW.h"
#endif                               

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

//  Foward Class definitions:

class CParameter;               

class CMSpectrum2DW  : public CSpectrum2DW
{
  struct MParameterDef {
    UInt_t  nScale;
    Float_t nLow;
    Float_t nHigh;
    STD(string) sUnits;
    int operator== (const MParameterDef& r) const {
      return ((nLow == r.nLow) && (nHigh == r.nHigh) && 
	      (nScale == r.nScale) && (sUnits == r.sUnits));
    }
  };
  Float_t m_nXLow;    // The low x value
  Float_t m_nYLow;    // The low y value
  Float_t m_nXHigh;   // The high x value
  Float_t m_nYHigh;   // The high y value
  UInt_t  m_nXChannels;  // The number of x channels (defines step size)
  UInt_t  m_nYChannels;  // The number of y channels
  MParameterDef m_XParameter;   // The x parameter
  MParameterDef m_YParameter;   // The y parameter

public:

  //Constructor(s) with arguments
  CMSpectrum2DW(const STD(string)& rName, UInt_t nId,
		const CParameter& rXParameter,
		const CParameter& rYParameter,
		Float_t nXLow, Float_t nYLow, Float_t nXHigh, Float_t nYHigh,
		UInt_t nXChans, UInt_t nYChans);

  // Destructor
  virtual  ~CMSpectrum2DW( ) { }

private:

  //Copy constructor [illegal]
  CMSpectrum2DW(const CMSpectrum2DW& acspectrum1dl); 

  //Operator= Assignment Operator [illegal] 
  CMSpectrum2DW operator= (const CMSpectrum2DW& aCSpectrum1D);

  //Operator== Equality Operator [Not too useful but:]
 public:

  int operator== (const CMSpectrum2DW& aCMSpectrum)
  { return (
	    (CSpectrum2DW::operator== (aCMSpectrum)) &&
	    (m_nXLow == aCMSpectrum.m_nXLow) &&
	    (m_nYLow == aCMSpectrum.m_nYLow) &&
	    (m_nXHigh == aCMSpectrum.m_nXHigh) &&
	    (m_nYHigh == aCMSpectrum.m_nYHigh) &&
	    (m_nXChannels == aCMSpectrum.m_nXChannels) &&
	    (m_nYChannels == aCMSpectrum.m_nYChannels) &&
	    (m_XParameter == aCMSpectrum.m_XParameter) &&
	    (m_YParameter == aCMSpectrum.m_YParameter)
	    );
  }            
                 
  // Selectors 
 public:
  Float_t getXLow() const {
    return m_nXLow;
  }
  Float_t getYLow() const {
    return m_nYLow;
  }
  Float_t getXHigh() const {
    return m_nXHigh;
  }
  Float_t getYHigh() const {
    return m_nYHigh;
  }
  UInt_t getXChannels() const {
    return m_nXChannels;
  }
  UInt_t getYChannels() const {
    return m_nYChannels;
  }
  MParameterDef getXParameterDef() const {
    return m_XParameter;
  }
  MParameterDef getYParameterDef() const {
    return m_YParameter;
  }
  STD(string) getXUnits() const {
    return m_XParameter.sUnits;
  }
  STD(string) getYUnits() const {
    return m_YParameter.sUnits;
  }
  virtual SpectrumType_t getSpectrumType() {
    return keM2D;
  }

  // Mutators (available to derived classes:
 protected:
  void setXLow(Float_t x) { 
    m_nXLow = x;
  }
  void setYLow(Float_t y) {
    m_nYLow = y;
  }
  void setXHigh(Float_t x) {
    m_nXHigh = x;
  }
  void setYHigh(Float_t y) {
    m_nYHigh = y;
  }
  void setXChannels(UInt_t xChans) {
    m_nXChannels = xChans;
  }
  void setYChannels(UInt_t yChans) {
    m_nYChannels = yChans;
  }
  
  //
  //  Operations:
  //   
 public:                 
  virtual void Increment(const CEvent& rEvent);
  UInt_t       Randomize(Float_t nChannel);
  UInt_t Dimension(UInt_t n) const;
  UInt_t       SpecToXParamPoint(UInt_t nSpecPoint);
  UInt_t       SpecToYParamPoint(UInt_t nSpecPoint);
  UInt_t       XParamToSpecPoint(UInt_t nParamPoint);
  UInt_t       YParamToSpecPoint(UInt_t nParamPoint);
  UInt_t       RandomizeToMultipleBins(Float_t nChannel, Float_t nRatio);
};

#endif
