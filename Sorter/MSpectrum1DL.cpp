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


#include <stdlib.h>
#include "MSpectrum1DL.h"
#include "Parameter.h"                               
#include "RangeError.h"
#include "Event.h"


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
  Float_t nWeight = (1 - (nChannel - (UInt_t)nChannel)) * 100;
  UInt_t nRandNum = rand() % 100;

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
  // The parameter range space step size
  Float_t nParamStep = (m_Parameter.nHigh - m_Parameter.nLow) /
    (1 << m_Parameter.nScale);

  // The parameter coordinate in mapped parameter space
  Float_t nParamCoord = (nParamPoint * nParamStep);
  
  // The parameter to spectrum space scaling
  Float_t nParamToSpecScale = ((m_nChannels) / 
			       (m_Parameter.nHigh - m_Parameter.nLow));
  
  // The channel in mapped spectrum coordinates
  UInt_t nChannel = Randomize(nParamCoord * nParamToSpecScale);
  return nChannel;
}
