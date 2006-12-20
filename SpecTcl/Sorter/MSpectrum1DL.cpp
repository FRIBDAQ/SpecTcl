//  CMSpectrum1DL.cpp
// Encapsulates the mapped (coordinate transformed) 1-d Spectrum.  
// The spectrum is a singly incremented
// 1-d spectrum with long channel size, and
// data are dynamically allocated initially (self owned).
// 
//
//   Author:
//      Jason Venema
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:venema@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//

#include <config.h>
#include <stdlib.h>
#include <Iostream.h>
#include <limits.h>
#include "MSpectrum1DL.h"
#include "Parameter.h"                               
#include "RangeError.h"
#include "Event.h"


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Functions for class CMSpectrum1DW

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//   CSpectrum1DW(const std::string& rname, UInt_t nId,
//               const CParameter& rParameter,
//               UInt_t            nScale)
// Operation Type:
//   Constructor
//
CMSpectrum1DL::CMSpectrum1DL(const std::string& rName, UInt_t nId,
			     const CParameter& rParameter,
			     Float_t nLow, Float_t nHigh,
			     UInt_t nChannels) :
  CSpectrum1DL(rName, nId, rParameter),
  m_nLow(nLow),
  m_nHigh(nHigh),
  m_nChannels(nChannels)
{
  // Get the parameter attributes while we can. These are used for scaling.
  m_Parameter.nScale = rParameter.getScale();
  m_Parameter.nLow   = rParameter.getLow();
  m_Parameter.nHigh  = rParameter.getHigh();
  m_Parameter.sUnits = rParameter.getUnits();

  // Just need to allocate storage and pass it to our base class for
  // management:
  setStorageType(keLong);
  UInt_t* pStorage = new UInt_t[nChannels];
  ReplaceStorage(pStorage);	// Storage now owned by parent.
  Clear();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Increment ( const CEvent& rEvent )
//  Operation Type:
//     mutator
//
void 
CMSpectrum1DL::Increment(const CEvent& rE) 
{
  CEvent& rEvent((CEvent&)rE);

  UInt_t ParamId = getParameter();
  if(rEvent[ParamId].isValid()) {  // Only increment if param present.
    UInt_t nChannel = ParamToSpecPoint(rEvent[ParamId]);
    if(nChannel < m_nChannels) {             // Only increment if in range. 
      UInt_t* p = (UInt_t*)getStorage();
      assert(p != (UInt_t*)kpNULL);          // Spectrum storage must exist!!
      p[nChannel]++;		             // Increment the histogram.
    }
  }
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t Randomize(Float_t nChannel)
//  Operation Type:
//    operation
//  Purpose:
//    UInt_t CMSpectrum1DL::Randomize(Float_t nChannel)
//
//    Given a floating point channel value, randomly select an
//    appropriate integer value channel to increment using a weighted
//    randomizer.  Returns the selected channel.
//
UInt_t
CMSpectrum1DL::Randomize(Float_t nChannel)
{
  Float_t nWeight = (1 - (nChannel - (UInt_t)nChannel));
  if(nWeight == 0.)
    return (UInt_t)nChannel;

  Float_t nRandNum = ((Float_t)rand() / (RAND_MAX+1.0));

  if(nWeight <= nRandNum)
    return ((UInt_t)nChannel+1);
  else
    return (UInt_t)nChannel;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t SpecToParamPoint(UInt_t nSpecPoint)
//  Operation Type:
//    conversion
//  Purpose:
//
//    Scales a spectrum point to its appropriate value in 
//    parameter space.
//
UInt_t
CMSpectrum1DL::SpecToParamPoint(UInt_t nSpecPoint) 
{
  Float_t nParamRange = (m_Parameter.nHigh - m_Parameter.nLow);
  Float_t nParamStep  = nParamRange / (1 << m_Parameter.nScale);
  Float_t nParamToSpecScale = (m_nChannels / nParamRange);
  Float_t nParamCoord = (Float_t)nSpecPoint / nParamToSpecScale;
  UInt_t  nRealChan   = (UInt_t)(nParamCoord / nParamStep);
  return nRealChan;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t ParamToSpecPoint(UInt_t nParamPoint)
//  Operation Type:
//    conversion
//  Purpose:
//
//    Scales a parameter point to its appropriate value in 
//    spectrum space.
//
UInt_t
CMSpectrum1DL::ParamToSpecPoint(UInt_t nParamPoint)
{
  // The parameter space step size
  Float_t nParamRange = m_Parameter.nHigh - m_Parameter.nLow;
  Float_t nParamStep = (nParamRange) / (1 << m_Parameter.nScale);

  // The parameter coordinate in mapped parameter space
  Float_t nParamCoord = m_Parameter.nLow + (nParamPoint * nParamStep);

  // If this is outside of mapped spectrum range, return m_nChannels+1
  // which will cause no increments
  if((nParamCoord < m_nLow) || (nParamCoord > m_nHigh)) {
    return m_nChannels+1;
  }
  
  // Otherwise, scale this to real spectrum space (i.e. a channel value)

  Float_t nDist = nParamCoord - m_nLow;           // the distance from the edge
  Float_t nSpecScale = nDist / (m_nHigh-m_nLow);  // the scale from mapped... 
  if(nSpecScale < 0) nSpecScale *= -1;            // to unmapped spec. space

  // Get the parameter channels to spectrum channels ratio
  Float_t nChannelRatio = (nParamRange / (1 << m_Parameter.nScale)) /
    ((m_nHigh-m_nLow) / m_nChannels);

  // If the mapped spectrum channel contains one, or more, mapped parameter 
  // channels, randomize based on the mantissa of the floating point channel
  if(nChannelRatio <= 1)
    return Randomize(nSpecScale * m_nChannels);

  // Otherwise, randomize based on the size of the ratio of parameter channels
  // to spectrum channels.
  else
    return RandomizeToMultipleBins(nSpecScale * m_nChannels, nChannelRatio);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t RandomizeToMultipleBins(Float_t nChannel, Float_t nRatio)
//  Operation Type:
//    operation
//  Purpose:
//
//    Receives a floating point channel in mapped spectrum space which
//    can fall into one of (UInt_t)(nChannel+nRatio) bins. The way to
//    determine which bin it actually DOES fall into is to weight each
//    bin according to its "in-ness", generate a random number and see
//    into which bin it falls.
//
UInt_t
CMSpectrum1DL::RandomizeToMultipleBins(Float_t nChannel, Float_t nRatio)
{
  // First we generate our random number between 0 and 1, being sure
  // to look at the high-end bits rather than the low end.
  Float_t nRandNum  = ((Float_t)rand() / (RAND_MAX+1.0));

  // We need to determine the number of bins this channel has the possibility
  // of falling into. Then we weight each bin, according to the amount that it
  // is in the range. To do this, we use the following variables:
  //    nHighLim - the upper limit channel number that this count could incr.
  //    nLowLim  - the lower limit channel number that this count could incr.
  //    nLowMantissa - The fractional part of nChannel
  UInt_t nHighLim = (UInt_t)(nChannel + nRatio);
  UInt_t nLowLim  = (UInt_t)nChannel;
  Float_t nLowMantissa  = nChannel - nLowLim;

  // Determine the weight given to the lowest possible bin, (UInt_t)nChannel.
  // We do this by dividing the fractional part of the channel passed to us
  // by the ratio of spectrum channels to parameter channels (nRatio).
  // If our random number falls in the range from 0 to this number, then
  // increment this channel.
  Float_t nLowChanProb = (1-nLowMantissa) / nRatio;
  if(nLowChanProb >= nRandNum)
    return nLowLim;

  // Otherwise, there are a number of "whole" spectrum channels covered by
  // this parameter channel that we need to examine. Weight each "whole"
  // spectrum channel the same, and see if the random number falls within
  // the specified range.
  else {
    for(int i = 1; i < (nHighLim - nLowLim); i++) {
      if((1 / nRatio)+nLowChanProb >= nRandNum)
	return nLowLim + i;
      else 
	nLowChanProb += (1 / nRatio);
    }
  }

  // If it didn't fall into any of the "whole" spectrum channels fully covered
  // by this parameter channel, then it has to fall into the highest possible
  // channel that this parameter channel only partially covers.
  return nHighLim;
}
