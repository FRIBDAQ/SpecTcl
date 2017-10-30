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

// Implement the longword multi incremented 2d spectrum.

#include <config.h>
#include "CSpectrum2DmB.h"
#include <Event.h>
#include <RangeError.h>
#include <TH2C.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

//////////////////////////////////////////////////////////////////
///////////////////// Canonicals //////////////////////////////////
//////////////////////////////////////////////////////////////////

/*!
   Constructor:  The main work is done by the base class.. we just
   need to set the storage type so that we get the appropriate
   amount of storage...and get the storage in the first place.
  Construct the spectrum.
  \param name   : std::string
     Name of the spectrum.
  \param id     : UInt_T
     unique id of the spect5rum.
  \param parameters : std::vector<CParameter>
     Set of parameters that make up the spectrums parameters...
     the caller must ensure that there are an even number of these.
  \param xscale, yscale : UInt_t
     Number of channels on the X axis and y axis respectively.
*/
CSpectrum2DmB::CSpectrum2DmB(std::string              name,
			     UInt_t                   id,
			     std::vector<CParameter>& parameters,
			     UInt_t                   xscale,
			     UInt_t                   yscale) :
  CSpectrum2Dm(name, id, parameters, xscale + 2, yscale + 2)
{
  m_pRootSpectrum = new TH2C(
    name.c_str(), name.c_str(),
    xscale, 0.0, static_cast<Double_t>(xscale),
    yscale, 0.0, static_cast<Double_t>(yscale)
  );
  m_pRootSpectrum->Adopt(0, nullptr);
  CreateChannels();
  setStorageType(keByte);
}

/*!
   Overloaded constructor:
    \param name : std::string
       Name of the spectrum.
    \param id   : UInt_t
       Unique spectrum id.
    \param parameters :std::vector<CParameter>& parameters
       Parameter definitions for the histogram.
    \param xchans : UInt_t
      Channels on x axis.
    \param ychans : UInt_t ychans
      Channels on y axis.
    \param xlow,xhigh : Float_t
      Range covered by x axis.
    \param ylow,yhigh : Float_t
      Range covered by y axis.

*/
CSpectrum2DmB::CSpectrum2DmB(std::string              name,
			     UInt_t                   id,
			     std::vector<CParameter>& parameters,
			     UInt_t                   xchans,
			     UInt_t                   ychans,
			     Float_t  xlow, Float_t   xhigh,
			     Float_t  ylow, Float_t   yhigh) :
  CSpectrum2Dm(name, id, parameters, xchans + 2, ychans + 2,
	       xlow, xhigh, ylow, yhigh)
{
  m_pRootSpectrum = new TH2C(
    name.c_str(), name.c_str(),
    xchans, static_cast<Double_t>(xlow), static_cast<Double_t>(xhigh),
    ychans, static_cast<Double_t>(ylow), static_cast<Double_t>(yhigh)
  );
  m_pRootSpectrum->Adopt(0, nullptr);
  setStorageType(keByte);
  CreateChannels();
}


CSpectrum2DmB::~CSpectrum2DmB() 
{
  m_pRootSpectrum->fArray = nullptr;
  delete m_pRootSpectrum;
}

///////////////////////////////////////////////////////////////////
///////////////// Virtual function overrides //////////////////////
///////////////////////////////////////////////////////////////////

/*!
   Fetches a value from the spectrum.  The parameters
   are a bit awkward in order to be able to accomodate
   all spectrum types and shapes.

   \param pIndices : const UInt_t*
      Pointer to an array of indices.  It is the caller's responsibility
      to ensure that there are 2 indices.

   \return ULong_t
   \retval  the channel value at the x/y coordinates passed in.
 
   \throws CRangeError - in the event one or more of the indices is out of
           range.
*/
ULong_t
CSpectrum2DmB::operator[](const UInt_t* pIndices) const
{
  Int_t  x      = pIndices[0];
  Int_t  y      = pIndices[1];
  if (x >= Dimension(0)) {
    throw CRangeError(0, Dimension(0) - 1, x,
		      string("Indexing 2m spectrum (x)"));
  }
  if (y >= Dimension(1)) {
    throw CRangeError(0, Dimension(1) - 1, y,
		      string("Indexing 2m Spectrum (y)"));
  }
  return static_cast<ULong_t>(
    m_pRootSpectrum->GetBinContent(m_pRootSpectrum->GetBin(x, y))
  );
}

/*!
   Set a channel value.
   \param pIndices : const UInt_t* 
     Array of indices. Caller must ensure there are two indices, x,y in that order.
   \param nValue : ULong_t
     Value to set in the spectrum channel.
*/
void
CSpectrum2DmB::set(const UInt_t* pIndices, ULong_t nValue)
{
  
  Int_t x     = pIndices[0];
  Int_t y     = pIndices[1];

  if (x >= Dimension(0)) {
    throw CRangeError(0, Dimension(0) - 1, x,
		      string("Indexing 2m spectrum (x)"));
  }
  if (y >= Dimension(1)) {
    throw CRangeError(0, Dimension(1) - 1, y,
		      string("Indexing 2m Spectrum (y)"));
  }

  m_pRootSpectrum->SetBinContent(
    m_pRootSpectrum->GetBin(x,y), static_cast<Double_t>(nValue)
  );

}

/*!
   Increment the spectrum.  By now the spectrum gate  has been checked
   and we will unconditionally increment the spectrum.
   For each consecutive pair in the parameter array we'll increment if both
   indices scale to in range.
   
   \param rEvent : const CEvent&

      Reference to the event 'array'.
*/
void 
CSpectrum2DmB::Increment(const CEvent& rEvent)
{
  for (int i =0; i < m_parameterList.size(); i += 2) {
    IncPair(rEvent, m_parameterList[i], m_parameterList[i+1], i);
  }
}

/////////////////////////////////////////////////////////////////////////
//////////////////////// Private utilities //////////////////////////////
/////////////////////////////////////////////////////////////////////////

// Create and set channel storage (also data type).

void
CSpectrum2DmB::CreateChannels()
{
  setStorageType(keByte);
  UChar_t*  pStorage = new UChar_t[m_xChannels*m_yChannels];
  ReplaceStorage(pStorage);
  Clear();

}

// Increment for one of the parameter pairs:

void
CSpectrum2DmB::IncPair(const CEvent& rEvent, UInt_t nx, UInt_t ny, int i)
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
  m_pRootSpectrum->Fill(x, y);
  
     
}
/**
 * setStorage
 *    Replaces the root spectrum storage with a new block of storage.
 * @param pStorage - pointer to the new  block of storage.
 */
void
CSpectrum2DmB::setStorage(Address_t pStorage)
{
  m_pRootSpectrum->fArray = reinterpret_cast<Char_t*>(pStorage);
  m_pRootSpectrum->fN     = Dimension(0) * Dimension(1);
}
/**
 *  StorageNeeded
 *
 *  @return Size_t - number of bytes of spectrum storage required.
 */
Size_t
CSpectrum2DmB::StorageNeeded() const
{
  return static_cast<Size_t>(Dimension(0) * Dimension(1) * sizeof(Char_t));
}
