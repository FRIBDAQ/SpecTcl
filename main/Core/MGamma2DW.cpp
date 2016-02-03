/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321
*/
static const char* Copyright = "(C) Copyright Michigan State University 2008, All rights reserved";
// CGamma1DW.cpp
// Encapsulates the prototypical 2-d Gamma spectrum
// Channel size is byte.
// Data are dynamically allocated.
//
//
// Author:
//    Jason Venema
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:venema@nscl.msu.edu

//
// Header Files:
//
#include <config.h>
#include "MGamma2DW.h"
#include "Parameter.h"
#include "RangeError.h"
#include "Event.h"
#include "GateContainer.h"
#include "Gate.h"
#include "GammaCut.h"
#include "GammaBand.h"
#include "GammaContour.h"
#include <iostream>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Functions for class CMGamma2DW

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//   CMGamma2DW(const std::string& rname, UInt_t nId,
//              vector<CParameter>& rParameters,
//              Float_t nXLow, Float_t nYLow,
//              Float_t nXHigh, Float_t nYHigh,
//              UInt_t nXChans, UInt_t nYChans)
// Operation Type:
//   Constructor
//
CMGamma2DW::CMGamma2DW(const std::string& rName, UInt_t nId,
		       vector<CParameter>& rParameters,
		       Float_t nXLow, Float_t nYLow, Float_t nXHigh,
		       Float_t nYHigh, UInt_t nXChans, UInt_t nYChans) :
  CGamma2DW(rName, nId, rParameters),
  m_nXLow(nXLow),
  m_nYLow(nYLow),
  m_nXHigh(nXHigh),
  m_nYHigh(nYHigh),
  m_nXChannels(nXChans),
  m_nYChannels(nYChans)
{
  setStorageType(keWord);
  for (UInt_t i = 0; i < rParameters.size(); i++) {
    CParameter& rParam(rParameters[i]);
    MParameterDef def;
    def.nParameter = rParam.getNumber();
    def.nScale     = rParam.getScale();
    def.nLow       = rParam.getLow();
    def.nHigh      = rParam.getHigh();
    def.sUnits     = rParam.getUnits();
    m_vParameters.push_back(def);
  }

  Size_t nBytes = StorageNeeded();
  UShort_t* pStorage = new UShort_t[nBytes/sizeof(UShort_t)];
  ReplaceStorage(pStorage);
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
CMGamma2DW::Increment(const CEvent& rE)
{
  string sGateType = getGate()->getGate()->Type();

  //Increment normally if gate is a 'normal' gate
  if(sGateType != "gs" && sGateType != "gb" && sGateType != "gc") {
    CEvent& rEvent((CEvent&)rE);
    UInt_t xChan, yChan;
    
    for (xChan = 0; xChan < m_vParameters.size()-1; xChan++) {
      for( yChan = xChan+1; yChan < m_vParameters.size(); yChan++) {
	if (rEvent[m_vParameters[xChan].nParameter].isValid() && 
	    rEvent[m_vParameters[yChan].nParameter].isValid())  {
	  UInt_t nXRealChan = rEvent[m_vParameters[xChan].nParameter];
	  UInt_t nYRealChan = rEvent[m_vParameters[yChan].nParameter];
	  UInt_t nXChannel = XParamToSpecPoint(nXRealChan, xChan);
	  UInt_t nYChannel = YParamToSpecPoint(nYRealChan, yChan);
	  if((nXChannel < m_nXChannels) && (nYChannel < m_nYChannels)) {
	    UShort_t* pStorage = (UShort_t*)getStorage();
	    assert(pStorage != (UShort_t*) kpNULL);
	    pStorage[nXChannel + (nYChannel * m_nXChannels)]++;
	  }
	}
      }
    }
  }
  
  // otherwise control is passed the GammaGateIncrement
  else if(sGateType == "gs" || sGateType == "gb" || sGateType == "gc") {
    GammaGateIncrement(rE, sGateType);
  }
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void GammaGateIncrement( const CEvent& rEvent, std::string sGT )
//  Operation Type:
//    mutator
//  Purpose:
//    Increment channel number (rEvent[m_vParameters[p1].nParameter] >> 
//                              m_vParameters[p1].nScale) +
//                             (rEvent[m_vParameters[p2].nParameter] >>
//                              m_vParameters[p2].nScale)*Dimension(0);
//    for all parameters, or pairs of parameters, which do not fall 
//    in the gate.
//
void
CMGamma2DW::GammaGateIncrement (const CEvent& rE, std::string sGT)
{
  UChar_t* pStorage = (UChar_t*)getStorage();
  assert(pStorage != (UChar_t*)kpNULL);
  CEvent& rEvent((CEvent&)rE);
  UInt_t xChan, yChan;
  vector<UInt_t> vXP, vYP;

  if(sGT == "gs") {  // Gate is a gamma slice
    CGammaCut* pGate = (CGammaCut*)getGate()->getGate();
    // For all params in the spectrum
    for(UInt_t xChan = 0; xChan < m_vParameters.size(); xChan++) {
      vXP.clear();
      vXP.push_back(m_vParameters[xChan].nParameter);
      if(rEvent[m_vParameters[xChan].nParameter].isValid()) {  // if valid...
	if(pGate->inGate(rEvent, vXP)) {  // and x-param is in the gate...
	  for(UInt_t p1 = 0; p1 < m_vParameters.size()-1; p1++) {
	    for(UInt_t p2 = p1+1; p2 < m_vParameters.size(); p2++) {
	      // Increment for all params not in the pair
	      if(p1 != xChan && p2 != xChan) {
		// Make sure these params are valid too...
		if(rEvent[m_vParameters[p1].nParameter].isValid() &&
		   rEvent[m_vParameters[p2].nParameter].isValid()) {
		  UInt_t nXRealChan = rEvent[m_vParameters[p1].nParameter];
		  UInt_t nYRealChan = rEvent[m_vParameters[p2].nParameter];
		  UInt_t nXChannel = XParamToSpecPoint(nXRealChan, p1);
		  UInt_t nYChannel = YParamToSpecPoint(nYRealChan, p2);
		  if((nXChannel < m_nXChannels) && 
		     (nYChannel < m_nYChannels)) {
		    pStorage[nXChannel + (nYChannel * m_nXChannels)]++;
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }

  else if(sGT == "gb" || sGT == "gc") {  // Gate is a gamma band or contour
    UInt_t mvx, mvy;
    CPointListGate* pGate((CPointListGate*)(getGate()->getGate()));
    if(pGate->Type() == "gb") {
      pGate = (CGammaBand*)pGate;
    }
    else {
      pGate = (CGammaContour*)pGate;
    }

    // For all possible pairs of parameters in the spectrum
    for(xChan = 0; xChan < m_vParameters.size()-1; xChan++) {
      for(yChan = xChan+1; yChan < m_vParameters.size(); yChan++) {
	mvx = m_vParameters[xChan].nParameter;
	mvy = m_vParameters[yChan].nParameter;
	vXP.clear(); vYP.clear();
	vXP.push_back(mvx); vXP.push_back(mvy);
	vYP.push_back(mvy); vYP.push_back(mvx);
	if(rEvent[mvx].isValid() && rEvent[mvy].isValid()) {  // if valid...
	  // and (p1, p2) or (p2, p1) is in the gate...
	  if(pGate->inGate(rEvent, vXP) || pGate->inGate(rEvent, vYP)) {
	    for(UInt_t xParam = 0; xParam < m_vParameters.size()-1; xParam++) {
	      for(UInt_t yParam = xParam+1; yParam < m_vParameters.size(); 
		  yParam++) {
		// Increment for all pairs which do not intersect
		// with (xChan, yChan)
		if((xChan != xParam && xChan != yParam) &&
		   (yChan != xParam && yChan != yParam)) {
		  // Make sure that these params are valid too...
		  if(rEvent[m_vParameters[xParam].nParameter].isValid() &&
		     rEvent[m_vParameters[yParam].nParameter].isValid()) {
		    UInt_t nXReal = rEvent[m_vParameters[xParam].nParameter];
		    UInt_t nYReal = rEvent[m_vParameters[yParam].nParameter];
		    UInt_t nXChannel = XParamToSpecPoint(nXReal, xParam);
		    UInt_t nYChannel = YParamToSpecPoint(nYReal, yParam);
		    if((nXChannel < m_nXChannels) && 
		       (nYChannel < m_nYChannels)) {
		      pStorage[nXChannel + (nYChannel * m_nXChannels)]++;
		    }		      
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////
//
// Function:
//     UInt_t Dimension (UInt_t n) const
// Operation type:
//     Selector.
//
UInt_t
CMGamma2DW::Dimension (UInt_t n) const
{
  switch(n) {
  case 0:
    return m_nXChannels;
  case 1:
    return m_nYChannels;
  default:
    return 0;
  }
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Randomize(Float_t nChannel)
//  Operation Type:
//    operation
//  Purpose:
//    UInt_t CMGamme2DW::Randomize(Float_t nChannel)
//
//    Given a floating point channel value, randomly select an
//    appropriate integer value channel to increment using a weighted
//    randomizer.  Returns the selected channel.
//
UInt_t
CMGamma2DW::Randomize(Float_t nChannel)
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
//    UInt_t GateToPointSpec(UInt_t nPoint)
//  Operation Type:
//    operation
//  Purpose:
//    UInt_t CMGamma2DW::GateToPointSpec(UInt_t nPoint, UInt_t nIndex)
//
//    Given a gate point (x, y), converts it to the
//    appropriate coordinate for the displayer.
//
UInt_t
CMGamma2DW::GatePointToSpec(UInt_t nPoint, UInt_t nIndex)
{
  Float_t scale = (Float_t)nPoint / (1 << m_vParameters[nIndex].nScale);
  return (UInt_t)(scale * (nIndex ? m_nYChannels : m_nXChannels));
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t SpecPointToGate(UInt_t nPoint)
//  Operation Type:
//    operation
//  Purpose:
//    UInt_t CMGamma2DW::GateToPointSpec(UInt_t nPoint, UInt_t nIndex)
//
//    Given a spectrum point (low, high), converts it to the
//    appropriate gate coordinate for the displayer.
//
UInt_t
CMGamma2DW::SpecPointToGate(UInt_t nPoint, UInt_t nIndex)
{
  Float_t scale = (Float_t)(1 << m_vParameters[nIndex].nScale) /
    (nIndex ? m_nYChannels : m_nXChannels);
  return (UInt_t)(scale * nPoint);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t XParamToSpecPoint(UInt_t nParamPoint, UInt_t nChan)
//  Operation Type:
//    conversion
//  Purpose:
//
//    Scales a parameter point to its appropriate value in 
//    spectrum space.
//
UInt_t
CMGamma2DW::XParamToSpecPoint(UInt_t nParamPoint, UInt_t nChan)
{
  // The parameter space step size
  Float_t nParamRange = m_vParameters[nChan].nHigh - m_vParameters[nChan].nLow;
  Float_t nParamStep = (nParamRange) / (1 << m_vParameters[nChan].nScale);

  // The parameter coordinate in mapped parameter space
  Float_t nParamCoord = m_vParameters[nChan].nLow + (nParamPoint * nParamStep);

  // If this is outside of mapped spectrum range, return m_nXChannels+1
  // which will cause no increments
  if((nParamCoord < m_nXLow) || (nParamCoord > m_nXHigh)) {
    return m_nXChannels+1;
  }
  
  // Otherwise, scale this to real spectrum space (i.e. a channel value)
  Float_t nDist = nParamCoord - m_nXLow;          // the distance from the edge
  Float_t nSpecScale = nDist / (m_nXHigh-m_nXLow); // the scale from mapped... 
  if(nSpecScale < 0) nSpecScale *= -1;            // to unmapped spec. space

  // Get the parameter channels to spectrum channels ratio
  Float_t nChannelRatio = (nParamRange / (1 << m_vParameters[nChan].nScale)) /
    ((m_nXHigh-m_nXLow) / m_nXChannels);

  // If the mapped spectrum channel contains one, or more, mapped parameter 
  // channels, randomize based on the mantissa of the floating point channel
  if(nChannelRatio <= 1)
    return Randomize(nSpecScale * m_nXChannels);

  // Otherwise, randomize based on the size of the ratio of parameter channels
  // to spectrum channels.
  else
    return RandomizeToMultipleBins(nSpecScale * m_nXChannels, nChannelRatio);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t YParamToSpecPoint(UInt_t nParamPoint, UInt_t nChan)
//  Operation Type:
//    conversion
//  Purpose:
//
//    Scales a parameter point to its appropriate value in 
//    spectrum space.
//
UInt_t
CMGamma2DW::YParamToSpecPoint(UInt_t nParamPoint, UInt_t nChan)
{
  // The parameter space step size
  Float_t nParamRange = m_vParameters[nChan].nHigh - m_vParameters[nChan].nLow;
  Float_t nParamStep = (nParamRange) / (1 << m_vParameters[nChan].nScale);

  // The parameter coordinate in mapped parameter space
  Float_t nParamCoord = m_vParameters[nChan].nLow + (nParamPoint * nParamStep);

  // If this is outside of mapped spectrum range, return m_nYChannels+1
  // which will cause no increments
  if((nParamCoord < m_nYLow) || (nParamCoord > m_nYHigh)) {
    return m_nYChannels+1;
  }
  
  // Otherwise, scale this to real spectrum space (i.e. a channel value)
  Float_t nDist = nParamCoord - m_nYLow;          // the distance from the edge
  Float_t nSpecScale = nDist / (m_nYHigh-m_nYLow); // the scale from mapped... 
  if(nSpecScale < 0) nSpecScale *= -1;            // to unmapped spec. space

  // Get the parameter channels to spectrum channels ratio
  Float_t nChannelRatio = (nParamRange / (1 << m_vParameters[nChan].nScale)) /
    ((m_nYHigh-m_nYLow) / m_nYChannels);

  // If the mapped spectrum channel contains one, or more, mapped parameter 
  // channels, randomize based on the mantissa of the floating point channel
  if(nChannelRatio <= 1)
    return Randomize(nSpecScale * m_nYChannels);

  // Otherwise, randomize based on the size of the ratio of parameter channels
  // to spectrum channels.
  else
    return RandomizeToMultipleBins(nSpecScale * m_nYChannels, nChannelRatio);
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
CMGamma2DW::RandomizeToMultipleBins(Float_t nChannel, Float_t nRatio)
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
