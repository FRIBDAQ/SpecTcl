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


/**
 * @file Gamma2D.hpp
 * @brief - implementation of Gamma2D<T>
 * @note we don't include Gamma2D.h since it includes us.
 */

#include <config.h>
#include "Parameter.h"
#include <Exception.h>
#include <RangeError.h>
#include "Event.h"
#include "GateContainer.h"
#include "Gate.h"
#include "CGammaCut.h"
#include "CGammaBand.h"
#include "CGammaContour.h"
#include <assert.h>
#include <TH2I.h>
#include <TDirectory.h>

#include <typeinfo>
#include <type_traits>
#include <stdexcept>

//// Canonicals:

/**
 * constructor
 *   @param rName -name of the spectrum.
 *   @param nId   -Id of the spectrum.
 *   @param rParameters - vector of parameter definitions.
 *   @param nXScale  - Number of xbins (covering [0, nXScale))
 *   @param nYScale   - Number of yBins (Covering [0, nYScale)).
 */
template <typename T>
CGamma2D<T>::CGamma2D(const std::string& rName, UInt_t nId,
		     std::vector<CParameter>& rParameters,
		     UInt_t nXScale, UInt_t nYScale) :
  CGammaSpectrum(rName, nId,
	    CreateAxisVector(rParameters,
			     nXScale, nYScale,
			     0.0,    (Float_t)(nXScale ),
			     0.0,    (Float_t)(nYScale)),
		 rParameters),
  m_nXScale(nXScale + 2),
  m_nYScale(nYScale + 2)
{

    // The assumption is all parameters have the same units.

    AddAxis(nXScale, 0.0, (Float_t)(nXScale - 1), rParameters[0].getUnits());
    AddAxis(nYScale, 0.0, (Float_t)(nYScale - 1), rParameters[0].getUnits());

    createRootSpectrum(
        rName.c_str(), 
        0.0, static_cast<Double_t>(nXScale), nXScale, 
        0.0, static_cast<Double_t>(nYScale), nYScale
    );
    
}
/**
 * constructor
 *    @param rName - Name of the spectrum.;
 *    @param nId   - Spectrum Id.
 *    @param rParameters - vector of parameter definitions.
 *    @param nXScale   - Number of xbins.
 *    @param nYScale   - Number of ybins.
 *    @param xLow, xHigh - range in world coords of the x axis [xLow, xHigh)
 *    @param yLow, yHigh - Range in world coords of the y axis.
 * 
 */
template <typename T>
CGamma2D<T>::CGamma2D(const std::string& rName, UInt_t nId,
		     std::vector<CParameter>& rParameters,
		     UInt_t nXScale, UInt_t nYScale,
		     Float_t xLow, Float_t xHigh,
		     Float_t yLow, Float_t yHigh) :
  CGammaSpectrum(rName, nId,
	    CreateAxisVector(rParameters, nXScale, nYScale,
			     xLow, xHigh, yLow, yHigh), rParameters),
  m_nXScale(nXScale + 2),
  m_nYScale(nYScale + 2)
{
    // The assumption is all paramters have the same units.
    AddAxis(nXScale, xLow, xHigh, rParameters[0].getUnits());
    AddAxis(nYScale, yLow, yHigh, rParameters[0].getUnits());
    createRootSpectrum(rName.c_str(), xLow, xHigh, nXScale, yLow, yHigh, nYScale);
}

/**
 *  destructor
 */
template <typename T>
CGamma2D<T>::~CGamma2D() {
    deleteRootSpectrum();
}

/// Object operations.

/**
 *  operator[] (readonly)
 *     Fetches a the value of a bin from the spectrum given bin coordinates.
 * 
 * @param pIndicies - pointer to the x and y bin numbers in that order.
 * @return ULong_t 
 */
template <typename T>
ULong_t
CGamma2D<T>::operator[] (const UInt_t* pIndices) const
{
  
  Double_t nx = pIndices[0];
  Double_t ny = pIndices[1];

  auto spec = getRootSpectrum();

  Int_t bin = spec->GetBin(nx+1, ny+1);
  return static_cast<ULong_t>(spec->GetBinContent(bin));
}

/**
 *  set
 *    Sets the value of a bin in the spectrum given bin coords and new value.
 * 
 * @param pIndices - As for operator[].
 * @param nValue  - New value to set for the bin.
 */
template <typename T>
void
CGamma2D<T>::set (const UInt_t* pIndices, ULong_t nValue)
{
  Double_t nx = pIndices[0];
  Double_t ny = pIndices[1];
  if (nx >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, nx,
		      std::string("Indexing 2DW gamma spectrum x axis"));
  }
  if (ny >= Dimension(1)) {
    throw CRangeError(0, Dimension(1)-1, ny,
		      std::string("Indexing 2DW gamma spectrum y axis"));
  }

  auto spec = getRootSpectrum();
  Int_t bin = spec->GetBin(nx+1, ny+1);
  spec->SetBinContent(nx, ny, static_cast<Double_t>(nValue));
}
/**
 * GetResolutions
 *    Return the number of bins on each axis.
 * 
 * @param rvResolutions - vector that will be fill in with [xbins, ybins].
 */
template <typename T>
void
CGamma2D<T>::GetResolutions (std::vector<UInt_t>& rvResolutions)
{
  rvResolutions.clear();
  rvResolutions.push_back(m_nXScale);
  rvResolutions.push_back(m_nYScale);
}

/**
 * CreateStorage
 *     Create the storage for the spectrum and set it as the root storage.
 *  Also set the data type for the channels.
 * 
 * @throw std::bad_typeid if T is not supported for spectrum bins.
 */
template <typename T>
void
CGamma2D<T>::CreateStorage() {
    // Set the storage type of throw:

    if (std::is_same<T, uint32_t>::value) {
        setStorageType(keLong);
    } else if (std::is_same<T, uint16_t>::value) {
        setStorageType(keWord);
    } else if (std::is_same<T, uint8_t>::value) {
        setStorageType(keByte);
    } else {
        throw std::bad_typeid();
    }

  Size_t nBytes = StorageNeeded();
  uint8_t* pStorage = new uint8_t[size_t(nBytes)];

  ReplaceStorage(pStorage);
  Clear();
  createStatArrays(2);
}

/**
 *  CreateAxisVector
 *    Make the vector of axis definitions
 * 
 *  @param rParams the parameter definitions
 * @param nXchan, nYchan - number of x, y channels.
 * @param xLow, xHigh - X axis range.
 * @param yLow, yHigh - Y axis range.
 */
template <typename T>
CSpectrum::Axes 
CGamma2D<T>::CreateAxisVector(std::vector<CParameter>& rParams,
			    UInt_t nXchan, UInt_t nYchan,
			    Float_t xLow, Float_t xHigh,
			    Float_t yLow, Float_t yHigh)
{
  CSpectrum::Axes Result;			// Build up the axis vector here.

  // Fill in the x transforms...

  std::vector<CParameter>::iterator i = rParams.begin();
  while(i != rParams.end()) {
    Result.push_back(CAxis(xLow, xHigh, nXchan,
			   CParameterMapping(*i)));
			   
    i++;
  }
  // And then the y transforms..

  i = rParams.begin();
  while(i != rParams.end()) {
    Result.push_back(CAxis(yLow, yHigh, nYchan,
			   CParameterMapping(*i)));

    i++;
  }

  return Result;
}
/**
 *  Dimension
 *    Return the number of bins on an axis.
 * 
 * @param n - axis number 0 - x, 1 y
 * @return Size_t
 * @throw  std::invalid_argument if n is not 0 or 1.
 */
template <typename T>
Size_t
CGamma2D<T>::Dimension(UInt_t n) const
{
  switch(n) {
  case 0:
    return m_nXScale;
  case 1:
    return m_nYScale;
  default:
    throw std::invalid_argument("Axis selector must be 1 (x) or two (y).");
  }
}

/**
 * Increment
 *    Increment callback after fold has been applied.
 * @param rParameters - vector of pairs of id/value that survived the fold.
 */
template <typename T>
void
CGamma2D<T>::Increment(std::vector<std::pair<UInt_t, Float_t> >& rParameters)
{

  TH1* pSpec = getRootSpectrum();
  if (rParameters.size() > 0) {
    for(int i = 0; i < rParameters.size() - 1; i++) {
      for(int j = i+1; j < rParameters.size(); j++ ) {
        Double_t xval = rParameters[i].second;
        Double_t yval = rParameters[j].second;
        
        pSpec->Fill(xval, yval);
      }
    }
  }
}
/**
 *  Increment
 *    This increment should only be called for Gamma Deluxe spectra  throw.
 */
template<typename T>
void
CGamma2D<T>::Increment(std::vector<std::pair<UInt_t, Float_t> >& xParameters,
		     std::vector<std::pair<UInt_t, Float_t> >& yParameters)
{
  throw CException("Gamma 2d Deluxe increment called on CGamma2DL");
}
/**
 * SetStorage
 *    Given a pointer to spectrum storage, sets the storage for the spectrum
 * to point to that.  This has type dependent code.
 * 
 * @param pStorage - pointer to the storage.
 * @throw std::bad_typeid if T is not a supported spectrum channel type for gamm2d
 */
template <typename T>
void
CGamma2D<T>::setStorage(Address_t pStorage) {
  TH2* pRawSpec = reinterpret_cast<TH2*>(getRootSpectrum());
  if (std::is_same<T, uint32_t>::value) {    // Long
    TH2I* pSpectrum = reinterpret_cast<TH2I*>(pRawSpec);
    pSpectrum->fArray = reinterpret_cast<Int_t*>(pStorage);
    pSpectrum->fN     = getXScale() * getYScale();
  } else if (std::is_same<T, uint16_t>::value) {   // Short
    TH2S* pSpectrum = reinterpret_cast<TH2S*>(pRawSpec);
    pSpectrum->fArray = reinterpret_cast<Short_t*>(pStorage);
    pSpectrum->fN     = getXScale() * getYScale();
  } else if (std::is_same<T, uint8_t>::value) {    // Byte
    TH2C* pSpectrum = reinterpret_cast<TH2C*>(pRawSpec);
    pSpectrum->fArray = reinterpret_cast<Char_t*>(pStorage);
    pSpectrum->fN     = getXScale() * getYScale();
  } else {
    throw std::bad_typeid(); 
  }
}
/**
 *  StorageNeeded
 *    @return Size_t - number of bytes of storage needed by the spetrum.
 */
template <typename T>
Size_t
CGamma2D<T>::StorageNeeded() const {
  return getXScale()* getYScale() * sizeof(T);
}

/**
 *  createRootSpectrum
 * 
 *     Create the appropriate underlying root spectrum, do the appropriate directory
 * gymnastics and book keeping to associate the root spectrum with this spectrum.
 * 
 * @param name - name of the spectrum.
 * @param xlow, xhigh,xbins  - Describe the x axis.
 * @param ylow, high, ybins  - Describe the y axis.
 */
template <typename T>
void
CGamma2D<T>::createRootSpectrum(
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
                xbins, xlow, xhigh, ybins, ylow, yhigh
            );
            pRootSpectrum->Adopt(0, nullptr);
            pS = pRootSpectrum;
        } else if (std::is_same<T, uint16_t>::value) {
            TH2S* pRootSpectrum = new TH2S(
                name, name,
                xbins, xlow, xhigh, ybins, ylow, yhigh
            );
            pRootSpectrum->Adopt(0, nullptr);
            pS = pRootSpectrum;
        } else if (std::is_same<T, uint8_t>::value) {
            TH2C* pRootSpectrum = new TH2C(
                name, name,
                xbins, xlow, xhigh, ybins, ylow, yhigh
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
 *     Destroy the root spectrum safely (so that root doesn't try to e.g. dispose of Xamine storage
 * that might be mapped to the spectum's storage).  The main work we have to do is figure out the
 * actual TH2x type and set it's fArray member to a null pointer so that when ROOT deletes it
 * nothing will happen.  It goes without saying that someone must dispose of the storage associated with 
 * the spectrum if it is dynamic or return it to Xamine's shared memory free storage if the spectrum is bound.
 * 
 */
template <typename T>
void
CGamma2D<T>::deleteRootSpectrum() {
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