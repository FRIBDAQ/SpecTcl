//  CSummarySpectrumW.h:
//
//    This file defines the CSummarySpectrumW class.
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

#ifndef __SUMMARYSPECTRUMW_H  //Required for current class
#define __SUMMARYSPECTRUMW_H
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

//  Foward Class definitions:

class CParameter;               
                

class CSummarySpectrumW  : public CSpectrum
{
  struct ParameterDef {
    UInt_t      nParameter;
    Int_t       nScale;
    int operator==(const ParameterDef& r) const {
      return (nParameter == r.nParameter) && (nScale == r.nScale);
    }
  };
  UInt_t              m_nYScale;		// Log(2) x axis.
  UInt_t              m_nXChannels;
  vector<ParameterDef> m_vParameters;
public:

			//Constructor(s) with arguments

  CSummarySpectrumW(const std::string& rName, UInt_t nId,
		    vector<CParameter> rrParameters,
		    UInt_t nYScale);

  virtual  ~ CSummarySpectrumW( ) { }       //Destructor	
private:
			//Copy constructor [illegal]

  CSummarySpectrumW(const CSummarySpectrumW& acspectrum1dl); 

			//Operator= Assignment Operator [illegal] 

  CSummarySpectrumW operator= (const CSummarySpectrumW& aCSpectrum1D);

			//Operator== Equality Operator [Not too useful but:]
public:
  int operator== (const CSummarySpectrumW& aCSpectrum)
  { return (
	    (CSpectrum::operator== (aCSpectrum))         &&
	    (m_nYScale      == aCSpectrum.m_nYScale)     &&
	    (m_nXChannels   == aCSpectrum.m_nXChannels)  &&
	    (m_vParameters  == aCSpectrum.m_vParameters) 
	    );
  }                             
  // Selectors 

public:
  UInt_t getYScale() const {
    return m_nYScale;
  }
  UInt_t getXChannels() const {
    return m_nXChannels;
  }
  UInt_t getnParams() const {
    return m_vParameters.size();
  }
  UInt_t getParameterId(UInt_t n) const {
    return m_vParameters[n].nParameter;
  }
  Int_t getScaleDifference(UInt_t n) const {
    return m_vParameters[n].nScale;
  }
  virtual SpectrumType_t getSpectrumType() {
    return keSummary;
  }

  // Mutators (available to derived classes:

protected:
  void setYScale(UInt_t Scale) {
    m_nYScale = Scale;
  }
  void setXChannels(UInt_t Chans) {
    m_nXChannels = Chans;
  }
  //
  //  Operations:
  //   
public:                 
  virtual   void Increment (const CEvent& rEvent)  ;
  virtual   ULong_t operator[](const UInt_t* pIndices) const;
  virtual   void    set(const UInt_t* pIndices, ULong_t nValue);
  virtual   Bool_t UsesParameter (UInt_t nId) const;
  virtual   UInt_t Dimension (UInt_t n) const;

  virtual   UInt_t Dimensionality () const {
    return 2;
  }
  virtual void GetParameterIds(vector<UInt_t>& rvIds);
  virtual void GetResolutions(vector<UInt_t>&  rvResolutions);
  virtual Int_t getScale(UInt_t index);
 
};

#endif
