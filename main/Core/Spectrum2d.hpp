/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2024.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins
             Giordano Cerizza
             Aaron Chester
             Jin-Hee (Genie) Chang
             Simon Giraud
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321

 */

/**
 *  @file Specctrum2d.hpp
 *  @brief Implementation of the CSpectrum2d template 
 * @note We don't include Spectrum2d.h becuase _we_ are included in it.
 */

#include <config.h>                             
#include "Parameter.h"
#include "RangeError.h"
#include "Event.h"
#include "CAxis.h"
#include "CParameterMapping.h"
#include <TH2I.h>
#include <TDirectory.h>

#include <type_traits>
#include <typeinfo>
#include <stdexcept>

/// Constructors:

/** 
 * constructor
 *    @param rName - name of the spectrum.
 *    @param nId  - Spectrum id
 *    @param  rxParameter Referencdes the X axis parameter object.
 *    @param rYParameter  References the Y axis parameter object.
 *    @param nXScale - number of X bins exclusive of under/overflows.
 *    @param nYScale - number of Y bins exclusive of under/overflows.
 */
template <typename T>
CSpectrum2D<T>::CSpectrum2D(const std::string& rName, UInt_t nId,
			   const CParameter& rXParameter, 
			   const CParameter& rYParameter,
			   UInt_t nXScale, UInt_t nYScale) :
  CSpectrum(rName, nId,
	    CreateAxisVector(rXParameter, 
			     nXScale, 0.0, (Float_t)(nXScale-1),
			     rYParameter, 
			     nYScale, 0.0, (Float_t)(nYScale-1))),
  m_nXScale(nXScale + 2),                 // Account for Root needing 2
  m_nYScale(nYScale + 2),                 // extra chans on each axis.
  m_nXParameter(rXParameter.getNumber()),
  m_nYParameter(rYParameter.getNumber())

{
  AddAxis(nXScale, 0.0, (Float_t)(nXScale), 
	  rXParameter.getUnits());
  AddAxis(nYScale, 0.0, (Float_t)(nYScale), 
	  rYParameter.getUnits());

  this->CreateRootSpectrum(
    rName.c_str(), 
    0.0, static_cast<Double_t>(nXScale), nXScale, 
    0.0, static_cast<Double_t>(nYScale), nYScale
  );
  
}

/**
 *  constructor
 *    @param rName - name of the spectrum (also used for root title).
 *    @param nId - Spectrum id (number).
 *    @param rxParameter - Referencdes the x parameter object
 *    @param ryParameter - References the y parameter object.
 *    @param nXChannels - number of x bins (exclusive of under/overflow).
 *    @param fxLow, fxHigh -  World coordinates limits  of X axis [fxLow, fxHigh)
 *    @param nYChannels - Number of y bins.
 *   @param fyLow, fyHigh - World coordinate limits of YT axis [fyLow, fyHigh])
 * 
 */
template <typename T>
CSpectrum2D<T>::CSpectrum2D(const std::string& rName, UInt_t nId,
			    const CParameter& rXParameter,
			    const CParameter& rYParameter,
			    UInt_t nXChannels, 
			    Float_t fxLow, Float_t fxHigh,
			    UInt_t nYChannels, 
			    Float_t fyLow, Float_t fyHigh) :
  CSpectrum(rName, nId,
	    CreateAxisVector(rXParameter, nXChannels,
			     fxLow, fxHigh,
			     rYParameter, nYChannels,
			     fyLow, fyHigh)),
  m_nXScale(nXChannels + 2),
  m_nYScale(nYChannels + 2),
  m_nXParameter(rXParameter.getNumber()),
  m_nYParameter(rYParameter.getNumber())
  
{
  AddAxis(nXChannels, fxLow, fxHigh, rXParameter.getUnits());
  AddAxis(nYChannels, fyLow, fyHigh, rYParameter.getUnits());
  
  // CreateROot spectrum has all the common code to make the root
  // spectrum between all constructors.
  
  this->CreateRootSpectrum(
    rName.c_str(), 
    fxLow, fxHigh, nXChannels, 
    fyLow, fyHigh, nYChannels);
  
}

/**
 *  destructor 
*/
template <typename T>
CSpectrum2D<T>::~CSpectrum2D() {}

/////  Object operations.

/**
 *  Increment
 *    Increment the appropriate bin of the spectrum.
 *  @param rE - referencdes the event for to increment.
 * 
 */
template <typename T>
void
CSpectrum2D<T>::Increment(const CEvent& rE) 
{
// Increments channel number rEvent[m_nParameter] >> m_nScaleDifference
// Formal Parameters:
//
//          const CEvent& rEvent:
//               Event which drives the histogramming
//
  CEvent& rEvent((CEvent&)rE);
  CParameterValue& xParam(rEvent[m_nXParameter]);
  CParameterValue& yParam(rEvent[m_nYParameter]);


  if(xParam.isValid()  && // Require the parameters be in event
     yParam.isValid()) {
    getRootSpectrum()->Fill(xParam, yParam);
  }
}
/**
 * UsesParameter
 *    @param nId - the id of a parameter to check.
 *    @return Bool_t - true if nId is one of the parameters used.
 * 
 */
template <typename T>
Bool_t
CSpectrum2D<T>::UsesParameter(UInt_t nId) const
{
// Returns nParameter == m_nParameter
//
// Formal Parameters:
//      UInt_t nParameter:
//        The parameter being checked.

  return (m_nXParameter == nId || 
	  m_nYParameter == nId);

}

/**
 *  operator[] 
 *    Readonly indexing of the spectrum
 * 
 * @param pIndices - pointer to an x then y index.
 * @return ULong_t - Value in the bin.
 */
template <typename T>
ULong_t
CSpectrum2D<T>::operator[](const UInt_t* pIndices) const
{
  // Provides the value of an element of the spectrum.
  // note:  This is not a 'normal' indexing operation in that
  // references are not returned.
  //
  
  Int_t   nx = pIndices[0];
  Int_t   ny = pIndices[1];
  
  const TH1* pRootSpectrum = getRootSpectrum();
  Int_t bin = pRootSpectrum->GetBin(nx + 1, ny + 1);
  return (ULong_t)(pRootSpectrum->GetBinContent(bin));
		      
}

/** set
 * 
 *   Write a spectrum bin.
 * 
 * @param pIndices - sames as for operastor[].
 * @param nValue - value to load into that spectrum bin.
 * 
 */
template <typename T>
void
CSpectrum2D<T>::set(const UInt_t* pIndices, ULong_t nValue)
{
  // Provides write access to a channel of the spectrum.
  //
  
  UInt_t   nx = pIndices[0];
  UInt_t   ny = pIndices[1];

  TH1* pRootSpectrum = getRootSpectrum();
  Int_t bin = pRootSpectrum->GetBin(nx + 1, ny + 1);
  pRootSpectrum->SetBinContent(bin, static_cast<Double_t>(nValue));

  
}
/**
 *  GetParameterIds
 *    @param rvIds - references a vector that will be filled with the
 * x and y parameter ids in that order.  Prior vector contents are lost.
 */
template <typename T>
void
CSpectrum2D<T>::GetParameterIds(std::vector<UInt_t>& rvIds)
{
  
  rvIds.clear();
  rvIds.push_back(m_nXParameter);
  rvIds.push_back(m_nYParameter);
}
/**
 *  GetResolutions
 *    @param rvResolutions - vector that will be cleared and receive the
 * number of bins in the X and Y axes in that order.
 */
template <typename T>
void
CSpectrum2D<T>::GetResolutions(std::vector<UInt_t>&  rvResolutions)
{
  rvResolutions.clear();
  rvResolutions.push_back(m_nXScale);
  rvResolutions.push_back(m_nYScale);
}

/**
 * CreateStorage
 *    Creates the spectrum storage and
 *  - Sets the type, 
 *  - Replaces the storage in the histogram.
 *  - Clears the spectrum.
 *  - creats the statistics array.
 * 
 * @throws std::bad_typeid - if T is not one of uint32_t, uin16_t uint8_t
 */
template <typename T>
void
CSpectrum2D<T>::CreateStorage() {
    // Type dependent code:

    size_t units = StorageNeeded()/sizeof(T);
   
    if (std::is_same<T, uint32_t>::value) {
        setStorageType(keLong);
        
    } else if (std::is_same<T, uint16_t>::value) { 
        setStorageType(keWord);
        
    } else if (std::is_same<T, uint8_t>::value) {
        setStorageType(keByte);
        
    } else {
        throw std::bad_typeid();     // Unsupported SpecTcl bin type.
    }
    //  The rest is unconditional for all types.
    T* pStorage = new T[units];
    ReplaceStorage(pStorage);
    Clear();
    this->createStatArrays(2);
}
/**
 * CreateAxisVector
 *    Creates an axis vector for  the spectrum constructor.
 * 
 * @param xParam - references the parameter on the X axis.
 * @param nxChannels, fxLow, fxHigh - describes the x axis.
 * @param nyChannnels, fyLow, fyHigh - describves the y axis.
 * @return an axis array with the appropriate mappings.
 *   Element 0 for th X and element 1 for the Y
 */
template <typename T>
CSpectrum::Axes
CSpectrum2D<T>::CreateAxisVector(const CParameter& xParam,
		       UInt_t      nxChannels,
		       Float_t     fxLow, Float_t fxHigh,
		       const CParameter& yParam,
		       UInt_t      nyChannels,
		       Float_t     fyLow, Float_t fyHigh)
{
  CSpectrum::Axes aMappings;
  CAxis xMap(fxLow, fxHigh, nxChannels, 
	     CParameterMapping(xParam));
  CAxis yMap(fyLow, fyHigh, nyChannels,
	     CParameterMapping(yParam));

  aMappings.push_back(xMap);
  aMappings.push_back(yMap);

  return aMappings;
}

/**
 *  Dimension
 *    @param n - axis number (0 for X, 1 for Y)
 *    @return the number of bins on that axis. 
 *    @throw invalid_argument if n is not 0 or 1.
 * @note throwing is new for SpecTcl 7
 */
template <typename T>
Size_t
CSpectrum2D<T>::Dimension(UInt_t n) const
{
  switch(n) {
  case 0:
    return m_nXScale;
  case 1:
    return m_nYScale;
  default:
    throw std::invalid_argument("Invalid axis selectro in Spectrum2D::Dimension");
  }
}
/**
 *  setSTorage
 *    Sets the storage for the underlying histogram.
 * 
 * @param pStorage - pointer to the storage.
 * @throw std::bad_typeid if T is not a valid SpecTcl bin type:
 */
template <typename T>
void
CSpectrum2D<T>::setStorage(Address_t pStorage)
{
    TH1* pRs = getRootSpectrum();

    // The rest of the code depends on T

    if (std::is_same<T, uint32_t>::value) {
        TH2I* pRootSpectrum = reinterpret_cast<TH2I*>(pRs);
        pRootSpectrum->fN = m_nXScale * m_nYScale;
        pRootSpectrum->fArray = reinterpret_cast<Int_t*>(pStorage);
    } else if(std::is_same<T, uint16_t>::value) {
        TH2S* pRootSpectrum = reinterpret_cast<TH2S*>(pRs);
        pRootSpectrum->fN = m_nXScale * m_nYScale;
        pRootSpectrum->fArray = reinterpret_cast<Short_t*>(pStorage);
    } else if (std::is_same<T, uint8_t>::value) {
        TH2C* pRootSpectrum = reinterpret_cast<TH2C*>(pRs);
        pRootSpectrum->fN = m_nXScale * m_nYScale;
        pRootSpectrum->fArray = reinterpret_cast<Char_t*>(pStorage);
    } else {
        throw std::bad_typeid();
    }
}
/**
 *  StorageNeeded
 *    Returns the number of bytes of storage required by the spectrum.
 * 
 * @return Size_t
 */
template <typename T>
Size_t
CSpectrum2D<T>::StorageNeeded() const
{
  return m_nXScale * m_nYScale * sizeof(T);
}

/**
 *  CreateRootSpectrum
 *    Do all the book keeping and fiddlefaddle needed to create
 * the root spectrum.
 * 
 * @param name - name of the new spectrum.
 * @param xlow, xhigh, xbins - X axis description.
 * @param ylow, yhigh, ybins - Y axis description.
 * @throws std::bad_typeid if T is an invalid SpecTcl bin type.
 * @note the histogram is crated in / but the root cwd is restored.
 */
template <typename T>
void
CSpectrum2D<T>::CreateRootSpectrum(
    const char* name, 
    Double_t xlow, Double_t xhigh, UInt_t xbins,
    Double_t ylow, Double_t yhigh, UInt_t ybins
  ) {
    std::string olddir = gDirectory->GetPath();  
    TH1* pS;
    // The try block has all the type specific code:
    try {
        if (std::is_same<T, uint32_t>::value) {
            TH2I* pRootSpectrum = new TH2I(
                name, name,
                xbins, xlow, xhigh, ybins, xlow, xhigh
            );
            pRootSpectrum->Adopt(0, nullptr);
            pS = pRootSpectrum;
        } else if (std::is_same<T, uint16_t>::value) {
            TH2S* pRootSpectrum = new TH2S(
                name, name,
                xbins, xlow, xhigh, ybins, xlow, xhigh
            );
            pRootSpectrum->Adopt(0, nullptr);
            pS = pRootSpectrum;
        } else if (std::is_same<T, uint8_t>::value) {
            TH2C* pRootSpectrum = new TH2C(
                name, name,
                xbins, xlow, xhigh, ybins, xlow, xhigh
            );
            pRootSpectrum->Adopt(0, nullptr);
            pS = pRootSpectrum;
        } else {
            throw std::bad_typeid();
        }
    } catch(...) {   // Restore the root cwd before re-throwing.
        gDirectory->Cd(olddir.c_str());
        throw;
    }
    setRootSpectrum(pS);
    gDirectory->Cd(olddir.c_str());
    this->CreateStorage();
}
/**
 *  deleteRootSpectrum
 *     - Sets the storage for the root spectrum to null
 *     - deletes the root histogram object.
 */
template <typename T>
void
CSpectrum2D<T>::deleteRootSpectrum() {
  auto rs = getRootSpectrum();
  if (std::is_same<T, uint32_t>::value) {
    TH2I* rootSpec = reinterpret_cast<TH2I*>(rs);
    rootSpec->fArray = nullptr;     // So root won't delete that.
  } else if (std::is_same<T, uint16_t>::value) {
    TH2S* rootSpec = reinterpret_cast<TH2S*>(rs);
    rootSpec->fArray = nullptr;     // So root won't delete that.
  } else if (std::is_same<T, uint8_t>::value) {
    TH2C* rootSpec = reinterpret_cast<TH2C*>(rs);
    rootSpec->fArray = nullptr;     // So root won't delete that.
  } else {
    throw std::bad_typeid();
  }
  delete rs; 
}