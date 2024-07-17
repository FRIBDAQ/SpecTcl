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
 * @file CSpectrum2Dm.hpp
 * @brief - Implement the templated class CSpectrum2Dmx which provides implementations for all
 *  2dm spectra.
 * @note We are included in CSpectrum2Dm.h so we don't include it.
 * 
 */
#include <config.h>
#include <Event.h>
#include <RangeError.h>
#include <TH2I.h>
#include <TDirectory.h>
#include <typeinfo>
#include <type_traits>

//// Canonicals:

/** 
 * constructor
 *  @param name -name of the spectrum.
 *  @param id   - Id of the spectrum.
 *  @param parameters - references the parameteers used by the spectrum.
 *  @param xscale - Number of bins on the x axis which covers [0, xscale)
 *  @param yscale - Number of bins on the y axis which covers [0, yscale)
 */
template <typename T>
CSpectrum2Dmx<T>::CSpectrum2Dmx(std::string              name,
			     UInt_t                   id,
			     std::vector<CParameter>& parameters,
			     UInt_t                   xscale,
			     UInt_t                   yscale) :
  CSpectrum2Dm(name, id, parameters, xscale, yscale)
{
    createRootSpectrum(
        name.c_str(), 0.0, Float_t(xscale), xscale, 0.0, (Float_t)yscale, yscale
    );
  
}
/**
 * construcor
 *  @param name - name of the spectrum.
 *  @param id   - spectrum id.
 *  @param xchans - # bins on the x axis.
 *  @param ychans - # bins on the y axis.
 *  @param xlow, xhigh - extent of the x axis; [xlow, xhigh)
 *  @param ylow, yhigh - Extent of the y axis.
 */
template <typename T>
CSpectrum2Dmx<T>::CSpectrum2Dmx(std::string              name,
			     UInt_t                   id,
			     std::vector<CParameter>& parameters,
			     UInt_t                   xchans,
			     UInt_t                   ychans,
			     Float_t  xlow, Float_t   xhigh,
			     Float_t  ylow, Float_t   yhigh) :
  CSpectrum2Dm(name, id, parameters, xchans, ychans,
	       xlow, xhigh, ylow, yhigh)
{
    createRootSpectrum(name.c_str(), xlow, xhigh, xchans, ylow, yhigh, ychans);
  
}

/** 
 * destructor
 */
template <typename T>
CSpectrum2Dmx<T>::~CSpectrum2Dmx() {
    deleteRootSpectrum();
}

/// Object operations.

/** 
 * operator[] - readonly 
 *   Fetch the value of a bin.
 * @param pIndices - the bin coordinates in pIndices[0] (x) and pIndices[1] (y).
 * @return ULong_t value in that bin
 */
template <typename T>
ULong_t
CSpectrum2Dmx<T>::operator[](const UInt_t* pIndices) const
{
  
  Int_t  x      = pIndices[0];
  Int_t  y      = pIndices[1];
  
  const TH1* pRootSpectrum = getRootSpectrum();
  return static_cast<ULong_t>(pRootSpectrum->GetBinContent(
    pRootSpectrum->GetBin(x + 1, y + 1)             // Alow for underlow bins at x=0, y=0
  ));
}
/**
 *  set 
 *    Set a bin value.
 * 
 * @param pIndices - see operator[].
 * @throw std::string - if indices are out of rang.
 */
template <typename T>
void
CSpectrum2Dmx<T>::set(const UInt_t* pIndices, ULong_t nValue)
{

  Int_t x     = pIndices[0];
  Int_t y     = pIndices[1];

  if (x +2 >= Dimension(0)) {
    throw CRangeError(0, Dimension(0) - 1, x,
		      std::string("Indexing 2m spectrum (x)"));
  }
  if (y +2 >= Dimension(1)) {
    throw CRangeError(0, Dimension(1) - 1, y,
		      std::string("Indexing 2m Spectrum (y)"));
  }
  TH1* pRootSpectrum = getRootSpectrum();
  pRootSpectrum->SetBinContent(
    pRootSpectrum->GetBin(x+1,y+1), static_cast<Double_t>(nValue)
  );

}

/**
 * Increment
 *    Increment the spectrum for an event.  Note that by now the gate has been satisfied.
 * 
 * @param rEvent - refernces the event to increment for.
 */
template<typename T>
void
CSpectrum2Dmx<T>::Increment(const CEvent& rEvent)
{
  for (int i =0; i < m_parameterList.size(); i += 2) {
    IncPair(rEvent, m_parameterList[i], m_parameterList[i+1]);
  }
}

// Utilities (private).

/**
 * CreateChannels
 *    Create and set storage for the spectrum.  This has type specific
 *    code.
 * 
 * @throw std::bad_typeid - if T is not a supported channel type for this spectrum.
 */
template <typename T>
void
CSpectrum2Dmx<T>::CreateChannels()
{
  DataType_t chantype;
  if (std::is_same<T, uint32_t>::value) {
    chantype = (keLong);
  } else if (std::is_same<T, uint16_t>::value) {
    chantype = (keWord);
  } else if (std::is_same<T, uint8_t>::value) {
    chantype = (keByte);
  } else {
    throw std::bad_typeid();
  }

  setStorageType(chantype);
  T*  pStorage = new T[m_xChannels*m_yChannels];
  ReplaceStorage(pStorage);
  Clear();

}

/**
 *  IncPair
 *    Given the event and a pari of parameter numbers, 
 * Increment the spectrum accordingly.  Note that in general this will be called
 * more than once per event.
 * 
 * @param rEvent - event reference
 * @param nx, ny - parameter numbers in the pair.
 */
template <typename T>
void
CSpectrum2Dmx<T>::IncPair(const CEvent& rEvent, UInt_t nx, UInt_t ny)
{
  // The parameters must both be in the range of the rEvent vector:

  UInt_t nParams = rEvent.size();
  if (!((nx < nParams) && (ny < nParams))) {
    return;			// One or both out of range of the vector.
  }
  // The parameters must both be defined:

  if (!(const_cast<CEvent&>(rEvent)[nx].isValid() && 
      const_cast<CEvent&>(rEvent)[ny].isValid())) {
    return;			// One or both invalid for this event.
  }
  // The parameters must both be in range for the spectrum after scaling:

  Double_t x = const_cast<CEvent&>(rEvent)[nx];
  Double_t y = const_cast<CEvent&>(rEvent)[ny];
  
  getRootSpectrum()->Fill(x,y);
  
}
/**
 *  setStorage
 *    Given a pointer to storage for the histogram, sets it in the root spectrum.
 * Note that if this is replacing local dynamic storage, that storage must be freed somewhere
 * 
 * @param pStorage - pointe4r to the storage.
 * @throw std::bad_typeid - if T is not a valid channel type for 2m spectra.
 */
template <typename T>
void
CSpectrum2Dmx<T>::setStorage(Address_t pStorage)
{
    TH2I* pRootSpectrum = reinterpret_cast<TH2I*>(getRootSpectrum());

    if (std::is_same<T, uint32_t>::value) {
        TH2I* pRootSpectrum = reinterpret_cast<TH2I*>(getRootSpectrum());
        pRootSpectrum->fArray = reinterpret_cast<Int_t*>(pStorage);
        pRootSpectrum->fN     = Dimension(0) * Dimension(1);
    } else if (std::is_same<T, uint16_t>::value) {
        TH2S* pRootSpectrum = reinterpret_cast<TH2S*>(getRootSpectrum());
        pRootSpectrum->fArray = reinterpret_cast<Short_t*>(pStorage);
        pRootSpectrum->fN     = Dimension(0) * Dimension(1);
    } else if (std::is_same<T, uint8_t>::value) {
        TH2C* pRootSpectrum = reinterpret_cast<TH2C*>(getRootSpectrum());
        pRootSpectrum->fArray = reinterpret_cast<Char_t*>(pStorage);
        pRootSpectrum->fN     = Dimension(0) * Dimension(1);
    } else {
        throw std::bad_typeid();
    }

  
}
/**
 * StorageNeeded
 *   Return the number of bytes of storage required for the speectrum:
 * 
 * @return Size_t
 */
template <typename T>
Size_t
CSpectrum2Dmx<T>::StorageNeeded() const
{
  return static_cast<Size_t>(Dimension(0) * Dimension(1) * sizeof(T));
}


/**
 *  createRootSpectrum
 * 
 *   Do all the book keeping associated with creating a root spectrum for this histogram.
 * This is type dependent code.
 * 
 * @param name -name of the spectrum.
 * @param xlow, xhigh, xbins - describe the x axis of the spectrum.
 * @param ylow, yhigh, ybins - describe the y axis of the spectrum.
 * @throw std::bad_typeid - T is not a valid data type for channels in this spectrum.
 */

template<typename T>
void
CSpectrum2Dmx<T>::createRootSpectrum(
    const char* name,
    Float_t xlow, Float_t xhigh, UInt_t xbins,
    Float_t ylow, Float_t yhigh, UInt_t ybins
  ) {
    std::string olddir = gDirectory->GetPath();
    gDirectory->Cd("/");              // Our spectra live in /

    // Type dependent code is wrapped in a Try/Catch block so we cna restore the wd from olddir

    TH1* pSpectrum(0);          // The type dependent code will set this.
    try {
        if (std::is_same<T, uint32_t>::value) { 
            TH2I* p = new TH2I( name, name,
                xbins, xlow, xhigh,
                ybins, ylow, yhigh
            );
            p->Adopt(0,  nullptr);
            pSpectrum = p;
        } else if (std::is_same<T, uint16_t>::value) {
            TH2S* p = new TH2S( name, name,
                xbins, xlow, xhigh,
                ybins, ylow, yhigh
            );
            p->Adopt(0,  nullptr);
            pSpectrum = p;
        } else if (std::is_same<T, uint8_t>::value) {
            TH2C* p = new TH2C( name, name,
                xbins, xlow, xhigh,
                ybins, ylow, yhigh
            );
            p->Adopt(0,  nullptr);
            pSpectrum = p;
        } else {
            throw std::bad_typeid();
        }
    } catch (...) {
        gDirectory->Cd(olddir.c_str());
        throw;
    }

    
    setRootSpectrum(pSpectrum);
    CreateChannels();            // Sets the storage type.
    gDirectory->Cd(olddir.c_str());

  }
  /** 
   * deleteRootSpectrum.
   *   Type dependent code to safely delete the root spectrum that we use.  This entails
   * getting the specific histogram type and setting fArray to nullptr so that we manage
   * the storage on delete...then deleting the actual histogram.
   * 
   * @throw std::bad_typeid if T is not a valid channel type.
   */
  template <typename T>
  void
  CSpectrum2Dmx<T>::deleteRootSpectrum() {
    TH2* pRaw = reinterpret_cast<TH2*>(getRootSpectrum());

    if (std::is_same<T, uint32_t>::value) {
        TH2I* p = reinterpret_cast<TH2I*>(pRaw);
        p->fArray = nullptr;
    } else if (std::is_same<T, uint16_t>::value) {
        TH2S* p = reinterpret_cast<TH2S*>(pRaw);
        p->fArray = nullptr;
    } else if (std::is_same<T, uint8_t>::value) {
        TH2C* p = reinterpret_cast<TH2C*>(pRaw);
        p->fArray = nullptr;
    } else {
        throw std::bad_typeid();
    }


    delete pRaw;
  }