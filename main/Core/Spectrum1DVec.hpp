/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2026.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             FRIB
             Michigan State University
             East Lansing, MI 48824-1321

 */
/** @file Spectrum1DVec.hpp
 *  @brief implementations of the 1d spectrumo on parameter vectors.
 */
#include "CAxis.h"
#include "CTreeParameter.h"
#include "Event.h"
#include "Parameter.h"
#include <histotypes.h>
#include <math.h>
#include <type_traits>
#include <typeinfo>

#include <TH1.h>
#include <TDirectory.h>


// Number of bits needed to hold a range
static UInt_t NumBits(const CTreeParameterVector& p) {
    double range = p.high() - p.low();
    return UInt_t(log(range)/log(2.0)) + 1;
}
 /**
  * constructor:
  * 
  * @param name - name of the spectrum.
  * @param id   - Id to assign to the spectsurm.
  * @param params - the parameter vector used to increment the spectrum.
  * @param nChannels - the bins in the spectrum.
  */
 template <typename T>
 CSpectrum1DVec<T>::CSpectrum1DVec(
        const std::string& name, UInt_t id, CTreeParameterVector& params, 
        UInt_t nChannels
) :
CSpectrum(name, id,
	    Axes(1,
		  CAxis((Float_t)0.0, (Float_t)(nChannels-1),
			nChannels,
			CParameterMapping(NumBits(params), params.low(), params.high(), params.units())))),
m_parameters(params), m_nChannels(nChannels)
{
    AddAxis(nChannels, 0.0, Float_t(nChannels), params.units());
    this->CreateRootSpectrum(name.c_str(), 0.0, static_cast<double>(nChannels), nChannels);
}
/**
 *  constructor
 *     Construct with axis specification:
 * 
 * @param name - name of the spectrum.
 * @param id   - Spectrum id.
 * @param params - parameter vector.
 * @param bins - Number of bins.
 * @param low  - Axis low limit.
 * @param high - Axis high limit.
 */
template <typename T>
CSpectrum1DVec<T>::CSpectrum1DVec(
        const std::string& name, UInt_t id, CTreeParameterVector& params,
        UInt_t bins, Float_t low, Float_t high
) :
CSpectrum(name, id,
	    Axes(1, CAxis(low, high , bins,
        CParameterMapping(NumBits(params), params.low(), params.high(), params.units())))),
m_parameters(params), m_nChannels(bins)
{
    AddAxis(bins, low, high, params.units());
    this->CreateRootSpectrum(name.c_str(), low, high, bins);
}

/**
 * destructor:
 *    We must delete the root spectrum... note that
 * the channel storage is deleted by the base class and managed by it exclusively.
 */
template<typename T>
CSpectrum1DVec<T>::~CSpectrum1DVec() {
    // If we are being called after partial construction, the root spectrum might not
    // have been created.
    if (getRootSpectrum()) {
        deleteRootSpectrum();
    }
}
/**
 * Increment
 *    For each of the event vector items in the 
 * parameter vector, fill the spectrum.
 * 
 * @param rEvent - ignored raw event.
 * 
 */
template <typename T>
void
CSpectrum1DVec<T>::Increment(const CEvent& rEvent) {
    for (int i = 0; i < m_parameters.size(); i++) {
        double value = double(m_parameters[i]);
        getRootSpectrum()->Fill(value);
    }
}

/**
 *  operator[]
 *    Index into the spectrum.  Note that root spectra have extra channels fore and aft for the
 *    underflow and overflow statistics...that's why the +1 in the GetBin call.
 * 
 * @param indices - Pointer to the indices - only the first one counts.
 * @return ULongt_t - value of the bin.
 */
template <typename T>
ULong_t
CSpectrum1DVec<T>::operator[](const UInt_t* pIndices) const {
    UInt_t i = *pIndices;
    const TH1* pSpectrum = getRootSpectrum();
    return static_cast<ULong_t>(
        pSpectrum->GetBinContent(
            pSpectrum->GetBin(i+1)
        )
    );
}
/**
 *  set
 *    Set the value of a bin in the spectrum.
 * 
 * @param pIndices - list of indices - only the first one mattters.
 * @param value    - The vlue to set.
 */
template <typename T>
void
CSpectrum1DVec<T>::set(const UInt_t* pIndices, ULong_t value) {
    UInt_t i = *pIndices;
    TH1* pSpectrum = getRootSpectrum();
    pSpectrum->SetBinContent(
        pSpectrum->GetBin(i+1), static_cast<Double_t>(value)
    );

}

/**
 * UsesParametr
 *    Returns true if the spectrum uses the specified parameter.
 * This is determined by iterating over the created parameters and
 * returning true if one has the id requested.
 * 
 * @param UsesParameter - id - the id of the parameter to check.
 * @return Bool_t true if we use it.
 */
template <typename T>
Bool_t
CSpectrum1DVec<T>::UsesParameter(UInt_t id) const {
    for (auto p : m_parameters.createdParams()) {
        if (id == p->getId())
        return kfTRUE;
    }
    return kfFALSE;
}
/**
 * GetParameterIds
 *    Return the currently used parameter ids.
 * These are the parameter ids of the parameters that have been created so far.
 * 
 * @param rvIds - vector of parametr ids that will be filled in.
 * @note the rvIds vector is cleared before we push our ids back.
 */
template <typename T>
void
CSpectrum1DVec<T>::GetParameterIds(std::vector<UInt_t>& rvIds) {
    rvIds.clear();
    for (auto p : m_parameters.createdParams()) {
        rvIds.push_back(p->getId());
    }
}
/**
 * GetResolutions
 * 
 * @param std::vector<UInt_t>& rvResolutions
 * 
 * deprecated fill sin rvResolutions with the number of bits of resolution
 * on each (the only) axis. 
 * 
 * @note rvResolutions is cleared prior to pushing.
 */
template <typename T>
void
CSpectrum1DVec<T>::GetResolutions(std::vector<UInt_t>& rvResolutions) {
    rvResolutions.clear();    // as promised in the note.

    double resolution = log(m_nChannels)/log(2.0) + 1.0;
    rvResolutions.push_back(static_cast<UInt_t>(resolution));
}

/**
 * StorageNeeded
 *    Return the number of bytes of storage required.
 * Note that Root spectra have extra bins on the left and right side for
 * over/underflow statistics.
 * 
 * @return size_t - the number of bytse of storage required:
 */
template <typename T>
Size_t
CSpectrum1DVec<T>::StorageNeeded() const {
    return (m_nChannels + 2)  * sizeof(T);
}

/**
 * Dimension
 *    Retuns the number of bins on an axis.
 * 
 * @param nDimension - the axis on which to return the bin count:
 * @note The return value includes the under/overflow channels.
 * @note For nDimension other than 0, 1 is returned so that
 * multiplications will yield the right number of total bins.
 */
template <typename T>
Size_t
CSpectrum1DVec<T>::Dimension(UInt_t nDimension) const {
    return  nDimension == 0 ? (m_nChannels + 2) : 1;
}
/**
 * needParameter
 *    @return kfFALSE - because this spectrum can't be put into a parameter driven increment list.
 */
template <typename T>
Bool_t
CSpectrum1DVec<T>::needParameter() const {
    return kfFALSE;
}
/**
 * getSpectrumType
 * 
 * @return SpectruType_t  
 * @retval  ke1DVec
 */
template <typename T>
SpectrumType_t
CSpectrum1DVec<T>::getSpectrumType() {
    return ke1DVec;
}

/**
 * getVectorName 
 * 
 *  @return std::strig -name of the tree vector we are defined on:
 */
template <typename T>
std::string
CSpectrum1DVec<T>::getVectorName() const {
    return m_parameters.name();
}
//////////////////////////////// protected utilities.

/**
 * CreateRootSpectrum.
 *    Creates the root spectrum associated with this SpecTcl spectrum.
 * 
 * @param name - name of the spectrum.
 * @param low  - low axis limits in world coordinates.
 * @param high - high axis limits in world coordinates.
 * @param bins - Number of bins on the axis (exclusive of over/underflow) bins.
 * 
 * @note this function performs a call to setRootSpectrum to make it known to the
 * base class.
 * @note the storage for the root spectrum is freed and the base class will fill and adopt it later.
 */
template <typename T>
void
CSpectrum1DVec<T>::CreateRootSpectrum(const char* name, Double_t low, Double_t high, UInt_t bins) {
    std::string oldDir = gDirectory->GetPath();
    gDirectory->Cd("/");    //  Make spectra in the root.
    try {
        TH1* pRs;     // Fill this in the type specific parts.

        // Sadly the 'Adopt' method isn't virtual in the TH1 base class, since it's
        // actually a method of the TArray class the specific spectrum classes inherit
        // from.
        if (std::is_same<T, uint32_t>::value) {     // longword
            TH1I* pRootSpec = new TH1I(
                name, name, bins, low, high 
            );
            pRootSpec->Adopt(0, nullptr);
            pRs = pRootSpec;
        } else if (std::is_same<T, uint16_t>::value) { // short.
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
        CreateChannels();               // THis will create and Adopt the storage for the 
    }
    catch (...) {
        // if we threw, reset the wd:

        gDirectory->Cd(oldDir.c_str());
    }
}
/**
 * deleteRootSpectrum
 *    We have to do a bit extra to ensure that we manage storage for the spectrum.
 * Specifically, if the spectrum is bound to the display, the storage cannot
 * be deleted storage management is handled by the base class and deletion was done by now
 * if appropriate.
 */
template <typename T>
void
CSpectrum1DVec<T>::deleteRootSpectrum() {
    auto rs = getRootSpectrum();
    if (std::is_same<T, uint32_t>::value) {             // uint32_t (long)
        TH1I* rootSpec = reinterpret_cast<TH1I*>(rs);
        rootSpec->fArray = nullptr;     // So root won't delete that.
    } else if (std::is_same<T, uint16_t>::value) {      // uint16_t (short)
        TH1S* rootSpec = reinterpret_cast<TH1S*>(rs);
        rootSpec->fArray = nullptr;     // So root won't delete that.
    } else {
        throw std::bad_typeid();
    }
    delete rs;    
}
/**
 * CreateChannels
 * 
 *    This is called after the root spectrum has been created
 * in order to allocate local storage (prior to any display binding)
 * for the bins.  In reading this, require that there are 2 extra bins, for
 * over and underflow channels.
 */
template <typename T>
void
CSpectrum1DVec<T>::CreateChannels() {
    Address_t pStorage;
    if (std::is_same<T, uint32_t>::value) {

        setStorageType(keLong);
        pStorage = new uint32_t[m_nChannels+2];   //  For root underlow/overflow.
    } else if (std::is_same<T, uint16_t>::value) {
        setStorageType(keWord);
        pStorage = new uint16_t[m_nChannels+2];
    } else {
        throw std::bad_typeid();
    }
    ReplaceStorage(pStorage);	// Storage now owned by parent.
    Clear();
    createStatArrays(1);
}

/**
 * setStorage
 *     Called to set the root spectrum storage to a specific pointer.  This
 * can be called after CreateChannels() or when the spetrum is bound
 * @param pStorage - new spectrum storage.
 * @note that root spectra have 2 extra channels for under/overflow statistics.
 * 
 */
template<typename T>
void
CSpectrum1DVec<T>::setStorage(Address_t pStorage) {
    // This is type dependent because TH1 doesn't inherit from array which has
    // the stuff we need, only the type specific spetrum classes.
    // Note that the assumption is that the caller will have
    // deleted any storage it needs to delete, if appropriate.
    TH1* pR = getRootSpectrum();
    if (std::is_same<T, uint32_t>::value)  {
        TH1I* pRootSpectrum = reinterpret_cast<TH1I*>(pR);
        pRootSpectrum->fArray = reinterpret_cast<Int_t*>(pStorage);
        pRootSpectrum->fN     = m_nChannels+2;            // Number of cells.
    } else if (std::is_same<T, uint16_t>::value) {
        TH1S* pRootSpectrum = reinterpret_cast<TH1S*>(pR);
        pRootSpectrum->fArray = reinterpret_cast<Short_t*>(pStorage);
        pRootSpectrum->fN     = m_nChannels+2;            // Number of cells.
    } else {
        throw std::bad_typeid();
    }
}