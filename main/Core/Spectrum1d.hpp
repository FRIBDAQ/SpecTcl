
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
 * @file Spectrum1D.hpp 
 * @brief Implement the Spectrum1D Template class
 * @note This implicitly generates Spectrum1DL and Spectrum1DW
 * @note Since we are implementing a template class we are included into the
 * header and don't need to include Spectrum1d.h  here.
 * @note - One must construct via the typedefs in the header else
 *     type (std::is_same) comparison may not work properly for figuring out
 * the actual TH1 type.
 */

#include <config.h>
#include "Parameter.h"
#include "RangeError.h"
#include "Event.h"
#include "CAxis.h"
#include <assert.h>
#include <type_traits>    // for std::is_same.
#include <typeinfo>       // for std::bad_typeid if the template type is bad.


#include <TH1.h>
#include <TDirectory.h>
#include <TDirectory.h>                // We need to slosh the root 'wd' around.


    
/////////////////////// Constructors:

// notes on the this->method()s that you'll  see below.  If a 
// template method function, does not have a parameter with that templated
// type you must be explicit about which class the method comes from when
// calling it els you get the error:
//there are no arguments to ‘createRootSpectrum’ that depend on a 
// template parameter, so a declaration of ‘createRootSpectrum’ must be available 
// In our case the type itself is needed for std::is_same macros.

/**
 *  construtor:
 *    @param rName - name of the spectrum.
 *    @param nId   - Id of the spectrrum.
 *    @param rParameter - References the parameter to histogram.
 *    @param nChannels - Number of channels for the range [0,nChannels)
 */
template <typename T>
CSpectrum1D<T>::CSpectrum1D(const std::string& rName, 
			  UInt_t            nId,
			  const CParameter& rParameter,
			  UInt_t            nChannels) :
  CSpectrum(rName, nId,
	    Axes(1,
		  CAxis((Float_t)0.0, (Float_t)(nChannels-1),
			nChannels,
			CParameterMapping(rParameter)))),
  m_nChannels(nChannels+2),
  m_nParameter(rParameter.getNumber())
{
    AddAxis(nChannels, 0.0, (Float_t)(nChannels), 
	    rParameter.getUnits());
    this->CreateRootSpectrum(rName.c_str(), 0.0, static_cast<Double_t>(nChannels), nChannels);
}

/**
 * constructor:
 *  @param  rName - name of the spectrum.
 *  @param  nId  - id of the spectrum.
 *  @param  rParameter - Parameter to be histogrammed.
 *  @param  nChannels - number of channels to bin.
 *  @param  fLow - Parameter coordinate value of the axis origin.
 *  @param  fHigh - Parameter coordinate value of the right of the axis.
 * 
 * @note THe axis covers [fLow, fHigh) in world coordinates.
 * 
 */
template <typename T> 
CSpectrum1D<T>::CSpectrum1D(const std::string&  rName,
			   UInt_t              nId,
			   const   CParameter& rParameter,
			   UInt_t              nChannels,
			   Float_t             fLow, 
			   Float_t             fHigh) :
  CSpectrum(rName, nId,
	    Axes(1, CAxis(fLow, fHigh, nChannels,
			  CParameterMapping(rParameter)))),
  m_nChannels(nChannels+2),
  m_nParameter(rParameter.getNumber())
{
  AddAxis(nChannels, fLow, fHigh, rParameter.getUnits());
  this->CreateRootSpectrum(rName.c_str(), fLow, fHigh, nChannels);
}
///////////////////////////
/**
 * destructor
 *    @note Moved nulling out the fArray field of the Root histogram
 * into the base class constructor and that should be fine.
 */
template <typename T>
CSpectrum1D<T>::~CSpectrum1D() {
  if (getRootSpectrum()) {
    deleteRootSpectrum();
  }
}

//////////////////////////////// Object operations.
/**
 *  Increment
 *    This ius done by the root spectrum.  We just need
 *    to ensure the parameter has been set.
 * 
 * @param const CEvent& rE - the event to increment for.
 */
template <typename T>
void
CSpectrum1D<T>::Increment(const CEvent& rE) {
    CEvent& rEvent((CEvent&)rE);	// Ok since non const  operator[] on rhs only.
  CParameterValue& rParam(rEvent[m_nParameter]);


  if(rParam.isValid()) {  // Only increment if param present.
    getRootSpectrum()->Fill(rParam);
  }
}

/** 
 * UsesParameter
 * @param nId - the id of a parameter.
 * @return Bool_t - True if the parameter is the one histoigramed by this.
*/
template <typename T>
Bool_t
CSpectrum1D<T>::UsesParameter(UInt_t nId) const
{
  return (m_nParameter == nId);

}
/**
 * operator[] - indexing into the spectrum.
 * 
 * @param pIndices array of indices - only the first one is used.
 * @return ULong_t Value of GetBin for the index bin of the spectrum.
 * @note Since for root spectra, channnle 0 are collections of underflows,
 *   we return GetBin(pIndices[0]+1)
 * @note this is a read-only indexing... see
 * set for writing spectrum channels.
 */
template <typename T>
ULong_t
CSpectrum1D<T>::operator[](const UInt_t* pIndices) const
{
  const TH1* pRootSpectrum = getRootSpectrum();
  return static_cast<ULong_t>(pRootSpectrum->GetBinContent(
    pRootSpectrum->GetBin(pIndices[0]+1)));

}

/**
 *  set
 *    Set a bin in the spectrum accordingly.
 * 
 * @param pIndices - indexes, see operator[]
 * @param nValue - The value to set.
 * @note 1 is added to the index to account for Root's use of bin 
 * for underflows.
 */
template <typename T>
void
CSpectrum1D<T>::set(const UInt_t* pIndices, ULong_t nValue)
{
  TH1* pRootSpectrum = getRootSpectrum();
  pRootSpectrum->SetBinContent(
    pRootSpectrum->GetBin(pIndices[0] + 1), static_cast<Double_t>(nValue)
  );

}
/**
 * GetParameterIds
 *    @param rvIds - references the vector to be filled inw with the
 * ids of the parameter we index. Note that prior contents are lost.
 * 
 */
template <typename T>
void
CSpectrum1D<T>::GetParameterIds(std::vector<UInt_t>& rvIds)
{
  
  rvIds.clear();
  rvIds.push_back(m_nParameter);
}
/**
 *  GetResolutions
 *    @param rvResolutions - will be output as a 1 element vector
 *  of the number of channels in the axis.
 */
template <typename T>
void
CSpectrum1D<T>::GetResolutions(std::vector<UInt_t>& rvResolutions) {
  rvResolutions.clear();
  rvResolutions.push_back(m_nChannels);
}

/**
 *  CreateChannels 
 *    Creates the storage associated with the spectrum.
 * @note this depends on the template type and we support
 * - uint32_t 32 bits/per channel
 * - uint16_t 16 bits/per channel.
 * @note m_nChannels has already been adjusted for Root's under and overflow channels.
 */
template <typename T>
void
CSpectrum1D<T>::CreateChannels()
{
  Address_t pStorage;
  if (std::is_same<T, uint32_t>::value) {

    setStorageType(keLong);
    pStorage = new uint32_t[m_nChannels];   //  For root underlow/overflow.
  } else if (std::is_same<T, uint16_t>::value) {
    setStorageType(keWord);
    pStorage = new uint16_t[m_nChannels];
  } else {
    throw std::bad_typeid();
  }
  ReplaceStorage(pStorage);	// Storage now owned by parent.
  Clear();
  createStatArrays(1);
}
/** 
 * setStorage
 *   Set the root storage to a new pointer/size.  Note  that if the
 * prior storage was owned by the base class it must be released.
 * 
 * @param pStorage - new root storage.
 */
template <typename T>
void
CSpectrum1D<T>::setStorage(Address_t pStorage)
{
  // This is type dependent:
  // And the root array pointers have funky signed types rather than
  // unsigned hence thd casts to Int_t* and Short_t*

  TH1* pR = getRootSpectrum();
  if (std::is_same<T, uint32_t>::value)  {
    TH1I* pRootSpectrum = reinterpret_cast<TH1I*>(pR);
    pRootSpectrum->fArray = reinterpret_cast<Int_t*>(pStorage);
    pRootSpectrum->fN     = m_nChannels;            // Number of cells.
  } else if (std::is_same<T, uint16_t>::value) {
    TH1S* pRootSpectrum = reinterpret_cast<TH1S*>(pR);
    pRootSpectrum->fArray = reinterpret_cast<Short_t*>(pStorage);
    pRootSpectrum->fN     = m_nChannels;            // Number of cells.
  } else {
    throw std::bad_typeid();
  }
}
/**
 *  The stroage required for the spectrum:
 */
template <typename T>
Size_t
CSpectrum1D<T>::StorageNeeded() const {
    // Assume the type is ok:

    return m_nChannels * sizeof(T);   // Includes over/underflows.
}

/**
 * Dimension
 *    Return the number of channels on a dimension.
 * @param nDim - which axis 
 * @return Size_t
 * @note This includes the over/underflow channels.
 * @note for nDim other than 0 1 is returned.
 */
template <typename T>
Size_t
CSpectrum1D<T>::Dimension(UInt_t nDim) const
{
    if (nDim == 0) return m_nChannels;
    return 1;
}

/**
 * CreateRootSpectrum
 *    Create the appropriate root spectrum for the unederlying
 * histogram type and do the common book keeping needed to 
 * make it ours.
 * 
 * @param name - name of the spectrum.
 * @param low - low limit of the axis.
 * @param high - high limit of the axis.
 * @param bins - Number of bins in [low, high),
 * 
 */
template <typename T>
void
CSpectrum1D<T>::CreateRootSpectrum(const char* name, Double_t low, Double_t high, UInt_t bins) {
    std::string oldDir = gDirectory->GetPath();
    gDirectory->Cd("/");    //  Make spectra in the root.
    try {
        TH1* pRs;     // Fill this in the type specific parts.
        if (std::is_same<T, uint32_t>::value) {
            TH1I* pRootSpec = new TH1I(
                name, name, bins, low, high 
            );
            pRootSpec->Adopt(0, nullptr);
            pRs = pRootSpec;
        } else if (std::is_same<T, uint16_t>::value) {
            TH1S* pRootSpec = new TH1S(
                name, name, bins, low, high 
            );
            pRootSpec->Adopt(0, nullptr);
            pRs = pRootSpec;
        } else {
            throw std::bad_typeid();
        }
        setRootSpectrum(pRs);
        gDirectory->Cd(oldDir.c_str());
        CreateChannels();
    }
    catch (...) {
        // if we threw, reset the wd:

        gDirectory->Cd(oldDir.c_str());
    }
}
/**
 *  deleteRootSpectrum
 *     Safely delete the root spectrum.   We'll assume the caller will
 *   - Destroy the pointer on return.
 *   - Release the spectrum storage if it's ours.
 * 
 * We need to do this because the 'fArray field only exists in concrete
 * subclasses of TH1
 * 
 * 
 */
template <typename T>
void
CSpectrum1D<T>::deleteRootSpectrum() {
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