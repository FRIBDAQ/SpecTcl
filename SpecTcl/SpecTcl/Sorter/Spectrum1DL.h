//  CSpectrum1D.h:
//
//    This file defines the CSpectrum1D class.
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

#ifndef __SPECTRUM1D_H  //Required for current class
#define __SPECTRUM1D_H
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
                

class CSpectrum1DL  : public CSpectrum
{
  UInt_t m_nScale;		// Log[2) the spectrum size.
  UInt_t m_nParameter;		// Number parameter which is histogrammed
  Int_t  m_nScaleDifference;	// Shift count scaling parameter to spectrum.
  
public:

			//Constructor(s) with arguments

  CSpectrum1DL(const std::string& rName, UInt_t nId,
	       const CParameter& rParameter,
	       UInt_t nScale);
  virtual  ~ CSpectrum1DL( ) { }       //Destructor	
private:
			//Copy constructor [illegal]

  CSpectrum1DL(const CSpectrum1DL& acspectrum1dl); 

			//Operator= Assignment Operator [illegal] 

  CSpectrum1DL operator= (const CSpectrum1DL& aCSpectrum1D);

			//Operator== Equality Operator [Not too useful but:]
public:
  int operator== (const CSpectrum1DL& aCSpectrum1D)
  { return (
	    (CSpectrum::operator== (aCSpectrum1D)) &&
	    (m_nScale == aCSpectrum1D.m_nScale) &&
	    (m_nParameter == aCSpectrum1D.m_nParameter) &&
	    (m_nScaleDifference == aCSpectrum1D.m_nScaleDifference) 
	    );
  }                             
  // Selectors 

public:
  UInt_t getScale() const
  {
    return m_nScale;
  }
  UInt_t getParameter() const
  {
    return m_nParameter;
  }
  Int_t getScaleDifference() const
  {
    return m_nScaleDifference;
  }
  virtual SpectrumType_t getSpectrumType() {
    return ke1D;
  }
  // Mutators (available to derived classes:

protected:
  void setScale (UInt_t am_nScale)
  { 
    m_nScale = am_nScale;
  }
  void setParameter (UInt_t am_nParameter)
  { 
    m_nParameter = am_nParameter;
  }
  void setScaleDifference (Int_t am_nScaleDifference)
  { 
    m_nScaleDifference = am_nScaleDifference;
  }
  //
  //  Operations:
  //   
public:                 
  virtual   void Increment (const CEvent& rEvent)  ;
  virtual   ULong_t operator[](const UInt_t* pIndices) const;
  virtual   void    set(const UInt_t* pIndices, ULong_t nValue);
  virtual   Bool_t UsesParameter (UInt_t nId) const;
  virtual   UInt_t Dimension (UInt_t n) const {
    return ((n == 0) ? (1 << m_nScale) : 0);
  }
  virtual   UInt_t Dimensionality () const {
    return 1;
  }
  virtual void GetParameterIds(vector<UInt_t>& rvIds);
  virtual void GetResolutions(vector<UInt_t>&  rvResolutions);
  virtual Int_t getScale(UInt_t nIndex);
};

#endif
