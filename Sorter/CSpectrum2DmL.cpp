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
#include "CSpectrum2DmL.h"
#include <Event.h>
#include <RangeError.h>

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
CSpectrum2DmL::CSpectrum2DmL(std::string              name,
			     UInt_t                   id,
			     std::vector<CParameter>& parameters,
			     UInt_t                   xscale,
			     UInt_t                   yscale) :
  CSpectrum2Dm(name, id, parameters, xscale, yscale)
{
  CreateChannels();
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
CSpectrum2DmL::CSpectrum2DmL(std::string              name,
			     UInt_t                   id,
			     std::vector<CParameter>& parameters,
			     UInt_t                   xchans,
			     UInt_t                   ychans,
			     Float_t  xlow, Float_t   xhigh,
			     Float_t  ylow, Float_t   yhigh) :
  CSpectrum2Dm(name, id, parameters, xchans, ychans,
	       xlow, xhigh, ylow, yhigh)
{
  CreateChannels();
}

CSpectrum2DmL::~CSpectrum2DmL() {}

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
CSpectrum2DmL::operator[](const UInt_t* pIndices) const
{
  UInt_t* p      = (UInt_t*)getStorage();
  UInt_t  x      = pIndices[0];
  UInt_t  y      = pIndices[1];
  if (x >= Dimension(0)) {
    throw CRangeError(0, Dimension(0) - 1, x,
		      string("Indexing 2m spectrum (x)"));
  }
  if (y >= Dimension(1)) {
    throw CRangeError(0, Dimension(1) - 1, y,
		      string("Indexing 2m Spectrum (y)"));
  }
  return p[coordsToIndex(x,y)];
}

/*!
   Set a channel value.
   \param pIndices : const UInt_t* 
     Array of indices. Caller must ensure there are two indices, x,y in that order.
   \param nValue : ULong_t
     Value to set in the spectrum channel.
*/
void
CSpectrum2DmL::set(const UInt_t* pIndices, ULong_t nValue)
{
  UInt_t*p      = (UInt_t*)getStorage();
  UInt_t x     = pIndices[0];
  UInt_t y     = pIndices[1];

  if (x >= Dimension(0)) {
    throw CRangeError(0, Dimension(0) - 1, x,
		      string("Indexing 2m spectrum (x)"));
  }
  if (y >= Dimension(1)) {
    throw CRangeError(0, Dimension(1) - 1, y,
		      string("Indexing 2m Spectrum (y)"));
  }

  p[coordsToIndex(x,y)] = nValue;

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
CSpectrum2DmL::Increment(const CEvent& rEvent)
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
CSpectrum2DmL::CreateChannels()
{
  setStorageType(keLong);
  UInt_t*  pStorage = new UInt_t[m_xChannels*m_yChannels];
  ReplaceStorage(pStorage);
  Clear();

}

// Increment for one of the parameter pairs:

void
CSpectrum2DmL::IncPair(const CEvent& rEvent, UInt_t nx, UInt_t ny, int i)
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

  Float_t x = const_cast<CEvent&>(rEvent)[nx];
  Float_t y = const_cast<CEvent&>(rEvent)[ny];

  Int_t ix = (Int_t)m_axisMappings[i].MappedParameterToAxis(x);
  if ((ix >= m_xChannels) || (ix < 0)) return;

  Int_t iy = (Int_t)m_axisMappings[i+1].MappedParameterToAxis(y);
  if ((ix < 0) || (iy >= m_yChannels)) return;

  UInt_t* p = static_cast<UInt_t*>(getStorage());
  p[coordsToIndex(ix, iy)]++;
    
}
