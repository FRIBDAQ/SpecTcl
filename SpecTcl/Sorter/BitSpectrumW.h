//  BitSpectrumW.h
//
//    This file defines the CBitSpectrumW class.  That class maintains
//    a 1-d spectrum which is incremented by bits set in bitmasks.
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

#ifndef __BITSPECTRUMW_H  //Required for current class
#define __BITSPECTRUMW_H
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
                

class CBitSpectrumW  : public CSpectrum
{
  UInt_t m_nChannels;		// Spectrum size in channels.
  UInt_t m_nParameter;		// Number parameter which is histogrammed
  
public:

			//Constructor(s) with arguments

  CBitSpectrumW(const std::string& rName, UInt_t nId,
	       const CParameter& rParameter,
	       UInt_t nScale);
  virtual  ~ CBitSpectrumW( ) { }       //Destructor	
private:
			//Copy constructor [illegal]

  CBitSpectrumW(const CBitSpectrumW& acspectrum1dl); 

			//Operator= Assignment Operator [illegal] 

  CBitSpectrumW operator= (const CBitSpectrumW& aCSpectrum1D);

			//Operator== Equality Operator [Not too useful but:]
public:
  int operator== (const CBitSpectrumW& aCSpectrum1D)
  { return (
	    (CSpectrum::operator== (aCSpectrum1D)) &&
	    (m_nChannels == aCSpectrum1D.m_nChannels) &&
	    (m_nParameter == aCSpectrum1D.m_nParameter)
	    );
  }                             
  // Selectors 

public:
  UInt_t getChannels() const
  {
    return m_nChannels;
  }
  UInt_t getParameter() const
  {
    return m_nParameter;
  }
  virtual SpectrumType_t getSpectrumType() {
    return keBitmask;
  }
  // Mutators (available to derived classes:

protected:
  void setChannels (UInt_t am_nChans)
  { 
    m_nChannels = am_nChans;
  }
  void setParameter (UInt_t am_nParameter)
  { 
    m_nParameter = am_nParameter;
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
    return ((n == 0) ? (m_nChannels) : 0);
  }
  virtual   UInt_t Dimensionality () const {
    return 1;
  }
  virtual void GetParameterIds(vector<UInt_t>& rvIds);
  virtual void GetResolutions(vector<UInt_t>&  rvResolutions);
  virtual Int_t getScale(UInt_t index);
};

#endif
