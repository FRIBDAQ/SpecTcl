//  CSpectrum2DW.h:
//
//    This file defines the CSpectrum2DW class.
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

#ifndef __SPECTRUM2D_H  //Required for current class
#define __SPECTRUM2D_H
                               //Required for base classes
#ifndef __SPECTRUM_H
#include "Spectrum.h"
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
                

class CSpectrum2DW  : public CSpectrum
{
  UInt_t m_nXScale;		// Log(2) x axis.
  UInt_t m_nYScale;		// Log(2) y Axis.
  UInt_t m_nXParameter;		// Number of parameter on X axis.
  UInt_t m_nYParameter;         // Number of parameter on Y axis.
  Int_t  m_nXScaleDifference;	// Shift count scaling parameter to X spectrum.
  Int_t  m_nYScaleDifference;   // Shift count scaling parameter to Y spectrum.
  
public:

			//Constructor(s) with arguments

  CSpectrum2DW(const std::string& rName, UInt_t nId,
	       const CParameter& rXParameter,
	       const CParameter& rYParameter,
	       UInt_t nXScale, UInt_t nYScale);

  virtual  ~ CSpectrum2DW( ) { }       //Destructor	
private:
			//Copy constructor [illegal]

  CSpectrum2DW(const CSpectrum2DW& acspectrum1dl); 

			//Operator= Assignment Operator [illegal] 

  CSpectrum2DW operator= (const CSpectrum2DW& aCSpectrum1D);

			//Operator== Equality Operator [Not too useful but:]
public:
  int operator== (const CSpectrum2DW& aCSpectrum)
  { return (
	    (CSpectrum::operator== (aCSpectrum)) &&
	    (m_nXScale == aCSpectrum.m_nXScale) &&
	    (m_nXParameter == aCSpectrum.m_nXParameter) &&
	    (m_nXScaleDifference == aCSpectrum.m_nXScaleDifference) &
	    (m_nYScale == aCSpectrum.m_nYScale) &&
	    (m_nYParameter == aCSpectrum.m_nYParameter) &&
	    (m_nYScaleDifference == aCSpectrum.m_nYScaleDifference)
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
  UInt_t getXParameter() const
  {
    return m_nXParameter;
  }
  UInt_t getYParameter() const
  {
    return m_nYParameter;
  }
  Int_t getXScaleDifference() const
  {
    return m_nXScaleDifference;
  }
  Int_t getYScaleDifference() const
  {
    return m_nYScaleDifference;
  }
  virtual SpectrumType_t getSpectrumType() {
    return ke2D;
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
  void setXParameter (UInt_t am_nParameter)
  { 
    m_nXParameter = am_nParameter;
  }
  void setYParameter(UInt_t nParameter)
  {
    m_nYParameter = nParameter;
  }
  void setXScaleDifference (Int_t am_nScaleDifference)
  { 
    m_nXScaleDifference = am_nScaleDifference;
  }
  void setYScaleDifference(Int_t nScaleDiff)
  {
    m_nYScaleDifference = nScaleDiff;
  }
  //
  //  Operations:
  //   
public:                 
  virtual   void Increment(const CEvent& rEvent)  ;
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
