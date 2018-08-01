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

#ifndef SUMMARYSPECTRUMW_H  //Required for current class
#define SUMMARYSPECTRUMW_H
                               //Required for base classes
#include "Spectrum.h"
#include <string>
#include <vector>
#include <histotypes.h>

//  Foward Class definitions:

class CParameter;               
class TH2S;
/*!
   A summary spectrum is a special 2-d spectrum that allows you to 
   view several parameters simultaneously.  Each x axis channel
   represents a parameter and the Y axis the spectrum for that 
   parameter.  The idea is that you can take the detectors for a large
   detector array and histogram them all in a summary spectrum.  The
   channels that are not working for some reason or are not gain matched
   will stand out like sore thumbs against the relatively uniform
   pattern of gainmatched, and working detectors.

   Axis scaling is managed a bit differently than for 'ordinary' spectra.
   - The X axis is always a unit-less axis that goes from [0, nParam).
   - The Y axis can be an arbitrary fixed cut (same for all parameters), 
     and will therefore have a separate CAxis item for each parameter
     as the mapping between that cut and the parameter may be different
     for each parameter (suppose for example, the user decides to use
     scaled parameters with the scaling doing gain matching e.g.).

*/
class CSummarySpectrumW  : public CSpectrum
{
  UInt_t              m_nYScale;     //!< Number of Y channels. 
  UInt_t              m_nXChannels;  //!< Number of X chanels.  
  std::vector<UInt_t>      m_vParameters; //!< STD(Vector) of parameter id's.
public:

			//Constructor(s) with arguments

  CSummarySpectrumW(const std::string& rName, UInt_t nId,
		    std::vector<CParameter> rrParameters,
		    UInt_t nYScale); //!< axis represents [0,nYScale-1]
  CSummarySpectrumW(const std::string& rName, UInt_t nId,
		    std::vector<CParameter> rrParameters,
		    UInt_t nYScale,
		    Float_t fYLow,
		    Float_t fYHigh); //!< Axis represents [fYlow, fYHigh].


  virtual  ~ CSummarySpectrumW( );
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
    return m_vParameters[n];
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
  virtual void GetParameterIds(std::vector<UInt_t>& rvIds);
  virtual void GetResolutions(std::vector<UInt_t>&  rvResolutions);
  virtual CSpectrum::SpectrumDefinition& GetDefinition();
  virtual Bool_t  needParameter() const;
  virtual void setStorage(Address_t pStorage);
  virtual Size_t StorageNeeded() const;
  virtual Size_t Dimension(UInt_t axis) const;
  
  // Utility functions.
protected:
  void CreateStorage();
  void FillParameterArray(std::vector<CParameter> Params);
  CSpectrum::Axes CreateAxes(std::vector<CParameter> Parameters,
			     UInt_t             nChannels,
			     Float_t fyLow, Float_t fyHigh);
  
};

#endif
