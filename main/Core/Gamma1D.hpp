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

// Don't includee Gamma1D.h since it includes us!

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
#include "CParameterMapping.h"
#include "CAxis.h"
#include <assert.h>
#include <TH1I.h>
#include <TDirectory.h>
#include <typeinfo>
#include <type_traits>

 /**
  * constructor
  *    @param rName - name of the spectrum.
  *    @param nId   - Id of the spectrum.
  *    @param rrParameters - references a fector of CParameters that will be on the X axis of the specturm.
  *    @param nScale - Number of X bins on the X axis of the spectrum.  X axis goes [0, nScale)
  */
 template <typename T>
 CGamma1D<T>::CGamma1D(const std::string& rName, UInt_t nId,
		     std::vector<CParameter>& rrParameters,
		     UInt_t nScale) :
  CGammaSpectrum(rName, nId,
	    MakeAxesVector(rrParameters, nScale,
			   0.0, (Float_t)(nScale)), rrParameters),
  m_nScale(nScale + 2)
{
  // The assumption is that all axes have the same units.

  this->AddAxis(nScale, 0.0, (Float_t)(nScale - 1), rrParameters[0].getUnits());
  this->CreateRootSpectrum(rName.c_str(), 0.0, (Double_t)(nScale), nScale);
  
}
/**
 *  constructor - with world coordinate transform for the axis:
 * 
 *   @param rName - name of the spectrum.
 *   @param nId   - Id of the spectrum.
 *   @param rrParameters - references a vector of parameters that will be histogramed.
 *   @param UInt_t nChannels - Number of X axis bins.
 *   @param fLow, fHigh - limit of the x axis world coordinates the axis goes [fLow, fHigh)
 *  * 
 */
template <typename T>
CGamma1D<T>::CGamma1D(const std::string& rName, UInt_t nId,
		     std::vector<CParameter>& rrParameters,
		     UInt_t nChannels,
		     Float_t fLow, Float_t fHigh) :
  CGammaSpectrum(rName, nId,
		 MakeAxesVector(rrParameters, nChannels, fLow, fHigh), rrParameters),
  m_nScale(nChannels + 2)
{
  this->AddAxis(nChannels, fLow, fHigh, rrParameters[0].getUnits());
  this->CreateRootSpectrum(rName.c_str(), fLow, fHigh, nChannels);
  
  
}

/** destructor
 *   - Note the ultimate base class Cspectrum will call deleteRootSpectrum which is all we need:
 */
template <typename T>
CGamma1D<T>::~CGamma1D()  {
    if (getRootSpectrum()) {
        deleteRootSpectrum();
    }
}


/**
 *  operator[] - readonly indexing.
 * 
 * @param  pIndices - pointer to the x index (bin coordinates).
 * @return ULong_t - number of counts in the bin selected.  This is exclusive of the underflow/overflow
 */
template <typename T>
ULong_t
CGamma1D<T>::operator[] (const UInt_t* pIndices) const
{
  
  Int_t n = pIndices[0];
  
  Int_t bin = getRootSpectrum()->GetBin(n + 1);
  return static_cast<ULong_t>(getRootSpectrum()->GetBinContent(bin));
}

/**
 * set
 *   Modify the value of a bin in the spectrum.
 *  @param pIndicies - pointer to the bin number to modify.  This is exclusive of the under/overflow bins.
 *  @param nValue - new contents for the selected bin.
 */
template <typename T>
void
CGamma1D<T>::set(const UInt_t* pIndices, ULong_t nValue)
{
  auto rootSpectrum = this->getRootSpectrum();
  Double_t n = pIndices[0];
  Int_t  bin = rootSpectrum->GetBin(n+1);
  rootSpectrum->SetBinContent(n, static_cast<Double_t>(nValue));
}

/**
 * getSpectrumType
 *    Return the Actual spectrum type.
 * @return SpectrumType_t
 * @retval keG1D
 */
template <typename T>
SpectrumType_t
CGamma1D<T>::getSpectrumType() {
    
        return keG1D;
}
/**
 * GetResolutions
 *   @param rvResolutions - reference to a vector that will be cleared and set to a single element
 * containing the number of bins in the spectrum.
 * 
 */
template <typename T>
void
CGamma1D<T>::GetResolutions(std::vector<UInt_t>& rvResolutions) {
    rvResolutions.clear();
    rvResolutions.push_back(m_nScale);
}

/**
 * CreateStorage 
 *    Create local storage for the spectrum, and set its storage type for the base class.
 * @throw std::bad_typeid if T is not a supported Gamma 1d spectrum type.
 */
template <typename T>
void 
CGamma1D<T>::CreateStorage() {
    if (std::is_same<T, uint32_t>::value) {
        this->setStorageType(keLong);
    } else if (std::is_same<T, uint16_t>::value) {
        this->setStorageType(keWord);
    } else {
        throw std::bad_typeid();
    }
    // Now allocate the storage.. this can be cone in type indpendent manner:

    ULong_t nBytes = this->StorageNeeded();
    uint8_t* pStorage = new uint8_t[nBytes];
    this->ReplaceStorage(pStorage);
    this->Clear();
    this->createStatArrays(1);
}

 /**
  *  MakeAxisVector
  *   Utility function to make the axis definition vector.
  * 
  * @param params - vector of parameter definitions.
  * @param nChannels - Number of axis bins.
  * @param fLow, fHIgh - Channel range in world coordinates (the axis goes [fLow, fHigh)).
  * @return Axes a vector with one element per parameter.  
  * */
template <typename T>
CSpectrum::Axes
CGamma1D<T>::MakeAxesVector(std::vector<CParameter> Params,
			  UInt_t             nChannels,
			  Float_t fLow, Float_t fHigh)
{
  CSpectrum::Axes Scales;
  for(int i=0; i < Params.size(); i++) {
    Scales.push_back(CAxis(fLow, fHigh, nChannels,
			   CParameterMapping((Params[i]))));
  }
  return Scales;
}

/**
 *  Increment
 *    Increment the spectrum for all the appropriate parameters.
 * 
 * @param  rParams - reference to a vector fof parameter number, value pairs.
 */
template <typename T>
void
CGamma1D<T>::Increment(std::vector<std::pair<UInt_t, Float_t> >& rParams)
{
  auto pSpec = getRootSpectrum();
  for (auto& p : rParams) {
    pSpec->Fill(p.second);
  }
}
/**
 *  Increment
 *    Increment for Gamma Deluxe spectra so throws an exception.
 * 
 * Never mind the parameters for now.
 */
template <typename T>
void
CGamma1D<T>::Increment(std::vector<std::pair<UInt_t, Float_t> >& xParameters,
			 std::vector<std::pair<UInt_t, Float_t> >& yParameters)
{
  throw CException("Invalid gamma 2d deluxe increment call in CGamma1DL");
}

/**
 *  setStorage
 *    Replace the storage associated with the root spectrum with
 * new storage.  The actual management of the storage is handled by the caller.
 * 
 * @param Address_t pStorage - new storage
 * @throw std::bad_typeid - if T is not a valid Gamma 1 d spectrum bin type.
 */
template <typename T>
void 
CGamma1D<T>::setStorage(Address_t pStorage)
{
  auto pRoot = getRootSpectrum();

  // Because the fArray is in the concete classes this is type dependent code:

  if (std::is_same<T, uint32_t>::value) {
    TH1I* p = reinterpret_cast<TH1I*>(pRoot);
    p->fArray = reinterpret_cast<Int_t*>(pStorage);
    p->fN = m_nScale;
  } else if (std::is_same<T, uint16_t>::value) {
    TH1S* p = reinterpret_cast<TH1S*>(pRoot);
    p->fArray = reinterpret_cast<Short_t*>(pStorage);
    p->fN = m_nScale;
  } else {
    throw std::bad_typeid();
  }
}

/**
 *  StorageNeeded
 *    Returns the number of bytes of spectrum storage required.
 * 
 * @return Size_t
 */
template <typename T>
Size_t 
CGamma1D<T>::StorageNeeded() const {
    return m_nScale * sizeof(T);
}

/**
 *  CreateRootSpectrum
 *    Create the underlying root spectrum and save it.  The appropriate directory
 * wiggle is done to put the spectrum in the root "/" directory and return the
 * working directory to what it was:
 * 
 * @param rName - name of the spectrum
 * @param fLow  - Low axis limit in WC.
 * @param fHigh - High axis limit in WC (the axis covers [fLow, fHigh) in world coordinates)
 * @param nChannels - number of bins on the axis.
 * @throw std::bad_typeid if T is not a valid type.
 */
template <typename T>
void 
CGamma1D<T>::CreateRootSpectrum(const char* rName, Double_t fLow, Double_t fHigh, UInt_t nChannels) {
    // Save the old directory and set the wd to "/":

    std::string olddir = gDirectory->GetPath();
    gDirectory->Cd("/");
    TH1*  pRoot(0);                             // Will hold the spectrum on success:
    // The type dependent code is in a try/catch block so the dir can be restored in case of a throw:

    try {
        if (std::is_same<T, uint32_t>::value) {   // TH1I
            auto p = new TH1I(rName, rName, nChannels, fLow, fHigh);
            p->Adopt(0, nullptr);
            pRoot= p;
        } else if (std::is_same<T, uint16_t>::value) {  // TH1S
            auto p = new TH1S(rName, rName, nChannels, fLow, fHigh);
            p->Adopt(0, nullptr);
            pRoot= p;
        } else {
            throw std::bad_typeid();
        }
    } catch(...) {
        gDirectory->Cd(olddir.c_str());
        throw;
    }
    // type independent book keeping:

    gDirectory->Cd(olddir.c_str());
    setRootSpectrum(pRoot); // CreateStorage needs getRootSpectrum().
    CreateStorage();                  
}
/**
 * deleteRootSpectrum
 *    
 */
template <typename T>
void 
CGamma1D<T>::deleteRootSpectrum() {
  auto rs = getRootSpectrum();
  if (std::is_same<T, uint32_t>::value) {
    TH1I* rootSpec = reinterpret_cast<TH1I*>(rs);
    rootSpec->fArray = nullptr;     // So root won't delete that.
  } else if (std::is_same<T, uint16_t>::value) {
    TH1S* rootSpec = reinterpret_cast<TH1S*>(rs);
    rootSpec->fArray = nullptr;     // So root won't delete that.
  } else {
    throw std::bad_typeid();
  }
  delete rs; 
}

