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

// forward definitions:

class CParameter;

/*!
   This class implements a gamma summary spectrum.  Gamma summary spectra are summary spectra,
   however each vertical strip is a gamma spectrum, that is a multiply incremented thing.
   
   Axis scaling: The X axis is always unit-less. the Y axis can have a collection of CAxes items
   for each vertical channel.

   The class is templated by the type of channel   Normally this is UInt_t, 
   UShort_t, or UChar_t.
*/
template <class T>
class CGammaSummarySpectrum<T> : public CSpectrum
{

  // Local data type definitions and so on.
private:

  UInt_t                m_nXChannels; // Number of channels in the x direction.
  Uint_t                m_nYChannels; // Number of channels in the y direction.
  std::vector<CSpectrum::Axis>       m_Axes;      // One axis vector per x channel.
  std::vector<std::vector<UInt_t> >  m_Parameters; // Vector of parameters per x channel.

  // Construtors and other canonicals:

public:
  CGammaSummarySpectrum(const std::string name, UInt_t nId,
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

  // The interface that must be implemented to produce the spectrum:
  // (implementations of functions that are pure virtual in the base class
  //
  virtual   void Increment (const CEvent& rEvent)  ;
  virtual   ULong_t operator[](const UInt_t* pIndices) const;
  virtual   void    set(const UInt_t* pIndices, ULong_t nValue);
  virtual   Bool_t UsesParameter (UInt_t nId) const;

  virtual void GetParameterIds(STD(vector)<UInt_t>& rvIds);
  virtual void GetResolutions(STD(vector)<UInt_t>&  rvResolutions);
  virtual UInt_t Dimension(UInt_t n) const;
  virtual UInt_t Dimensionality() const;
  virtual CSpectrum::SpectrumDefinition& GetDefinition();

  virtual Bool_t needParameter() const;

  //  Utility functions:

private:
  void CreateStorage();
  void fillParameterArray(std::vector<CParameter>* params,
			  UInt_t                   xChannels);
  void CreateAxes(std::vector<CParameter>* params,
		  UInt_t                   xChannels
		  UInt_t                   ychannels,
		  Float_t                  yLow,
		  Float_t                  yHigh);
  void indexCheck(UInt_t x, UInt_t y);
};

#ifndef __CGAMMASUMMARYSPECTRUM_CXX
#include "CGammaSummarySpectrum.cpp"
#endif

typedef CGammaSummarySpectrum<UInt_t>   CGammaSummarySpectrumL;
typedef CGammaSummarySpectrum<UShort_t> CGammaSummarySpectrumW;
typedef CGammaSummarySpectrum<UChar_t>  CGammaSummarySpectrumB;


#endif
