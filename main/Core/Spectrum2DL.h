/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


//  CSpectrum2dL.h:
//
//    This file defines the CSpectrum2dL class.
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

#ifndef __SPECTRUM2DL_H  //Required for current class
#define __SPECTRUM2DL_H
                               //Required for base classes
#ifndef __SPECTRUM_H
#include "Spectrum.h"
#endif                               

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif 
#endif


#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif


#ifndef __CAXIS_H
#include "CAxis.h"
#endif

//  Foward Class definitions:

class CParameter;               

class CSpectrum2DL  : public CSpectrum
{
  UInt_t m_nXScale;		// Number of channels on  x axis.
  UInt_t m_nYScale;		// Number of channels on  y Axis.
  UInt_t m_nXParameter;		// Number of parameter on X axis.
  UInt_t m_nYParameter;         // Number of parameter on Y axis.
  
public:

			//Constructor(s) with arguments

  CSpectrum2DL(const std::string& rName, UInt_t nId,
	       const CParameter& rXParameter,
	       const CParameter& rYParameter,
	       UInt_t nXScale, UInt_t nYScale);

  CSpectrum2DL(const std::string& rName, UInt_t nId,
	       const CParameter& rXParameter,
	       const CParameter& rYParameter,
	       UInt_t nXChannels, Float_t fxLow, Float_t fxHigh,
	       UInt_t nYChannels, Float_t fyLow, Float_t fyHigh);
  


  virtual  ~ CSpectrum2DL( ) { }       //Destructor	
private:
			//Copy constructor [illegal]

  CSpectrum2DL(const CSpectrum2DL& acspectrum1dl); 

			//Operator= Assignment Operator [illegal] 

  CSpectrum2DL operator= (const CSpectrum2DL& aCSpectrum1D);

			//Operator== Equality Operator [Not too useful but:]
public:
  int operator== (const CSpectrum2DL& aCSpectrum)
  { return (
	    (CSpectrum::operator== (aCSpectrum)) &&
	    (m_nXScale == aCSpectrum.m_nXScale) &&
	    (m_nXParameter == aCSpectrum.m_nXParameter) &&
	    (m_nYScale == aCSpectrum.m_nYScale) &&
	    (m_nYParameter == aCSpectrum.m_nYParameter) 
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

  //
  //  Operations:
  //   
public:                 
  virtual   void Increment (const CEvent& rEvent)  ;
  virtual   ULong_t operator[](const UInt_t* pIndices) const;
  virtual   void    set(const UInt_t* pIndices, ULong_t nValue);
  virtual   Bool_t UsesParameter (UInt_t nId) const;
  virtual   Size_t Dimension (UInt_t n) const;

  virtual   UInt_t Dimensionality () const {
    return 2;
  }
  virtual void GetParameterIds(std::vector<UInt_t>& rvIds);
  virtual void GetResolutions(std::vector<UInt_t>&  rvResolutions);

  // Utility functions:
protected:
  //!  Create an axis vector describing axis mappings.
  static Axes  CreateAxisVector(const CParameter& xParam,
				 UInt_t      nxChannels,
				 Float_t     fxLow, Float_t fxHigh,
				 const CParameter& yParam,
				 UInt_t      nYChannels,
				 Float_t     fyLow, Float_t fyHigh);
  void CreateStorage();

 
};

#endif
