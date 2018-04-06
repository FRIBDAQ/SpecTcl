#ifndef __CGAMMASUMMARYSPECTRUM_H
#define __CGAMMASUMMARYSPECTRUM_H
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

#include <TH2I.h>
#include <TH2S.h>
#include <TH2C.h>

// forward definitions:

class CParameter;

/*!
   This class implements a gamma summary spectrum.  Gamma summary spectra are summary spectra,
   however each vertical strip is a gamma spectrum, that is a multiply incremented thing.
   
   Axis scaling: The X axis is always unit-less. the Y axis can have a collection of CAxes items
   for each vertical channel.

   The class is templated by the type of channel   Normally this is Int_t, 
   Short_t, or Char_t.
   
   The second template parameter R must be a root 2d histogram type. All this
   is made somewhat simpler by the typedefs at the bottom of this file.
*/
template <typename T, typename R>
class CGammaSummarySpectrum : public CSpectrum
{

  // Local data type definitions and so on.
private:

  UInt_t                m_nXChannels; // Number of channels in the x direction.
  UInt_t                m_nYChannels; // Number of channels in the y direction.
  std::vector<CAxis>       m_Axes;      // One axis vector per x channel.
  std::vector<std::vector<UInt_t> >  m_Parameters; // Vector of parameters per x channel.

  // Construtors and other canonicals:

public:
  CGammaSummarySpectrum(const std::string name, 
			UInt_t nId,
			UInt_t               nYChannels,
			std::vector<std::vector<CParameter> >* pParameters);
  CGammaSummarySpectrum(const std::string name, UInt_t nId,
			std::vector<std::vector<CParameter> >* pParameters,
			UInt_t               nYChannels,
			Float_t              fYLow,
			Float_t              fYHigh);
  virtual ~CGammaSummarySpectrum();

private:
  CGammaSummarySpectrum(const CGammaSummarySpectrum& rhs);
  CGammaSummarySpectrum& operator=(const CGammaSummarySpectrum& rhs);
  int operator==(const CGammaSummarySpectrum& rhs) const;
  int operator!=(const CGammaSummarySpectrum& rhs) const;
  
public:
  // The interface that must be implemented to produce the spectrum:
  // (implementations of functions that are pure virtual in the base class
  //
public:
  virtual   void Increment (const CEvent& rEvent)  ;
  virtual   ULong_t operator[](const UInt_t* pIndices) const;
  virtual   void    set(const UInt_t* pIndices, ULong_t nValue);
  virtual   Bool_t UsesParameter (UInt_t nId) const;

  virtual void GetParameterIds(std::vector<UInt_t>& rvIds);
  virtual void GetResolutions(std::vector<UInt_t>&  rvResolutions);
  virtual Size_t Dimension(UInt_t n) const;
  virtual UInt_t Dimensionality() const;
  virtual CSpectrum::SpectrumDefinition& GetDefinition();

  virtual Bool_t needParameter() const;

  virtual SpectrumType_t getSpectrumType();

  virtual Float_t GetLow(UInt_t n) const;
  virtual Float_t GetHigh(UInt_t n) const;
  virtual std::string GetUnits(UInt_t n) const;
  
  virtual void setStorage(Address_t pStorage);
  virtual Size_t StorageNeeded() const;
  //  Utility functions:

private:
  void CreateStorage();
  void fillParameterArray(std::vector<std::vector<CParameter> >& params,
			  UInt_t                   xChannels);
  void CreateAxes(std::vector<std::vector<CParameter> >& params,
		  UInt_t                   xChannels,
		  UInt_t                   ychannels,
		  Float_t                  yLow,
		  Float_t                  yHigh);
};



typedef CGammaSummarySpectrum<Int_t,    TH2I>   CGammaSummarySpectrumL;
typedef CGammaSummarySpectrum<Short_t, TH2S> CGammaSummarySpectrumW;
typedef CGammaSummarySpectrum<Char_t,  TH2C>  CGammaSummarySpectrumB;

#ifndef __CGAMMASUMMARYSPECTRUM_CXX
#include "CGammaSummarySpectrum.cpp"
#endif

#endif
