//  CMSpectrum2DW.cpp
// Encapsulates the mapped 2-d Spectrum.  
// The spectrum is a doubly incremented
// 2-d spectrum with word channel size
// Data are dynamically allocated initially (self owned).
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


#include "MSpectrum2DW.h"                               
#include "Parameter.h"
#include "RangeError.h"
#include "Event.h"


// Functions for class CMSpectrum2DW

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//   CMSpectrum2DW(const std::string& rname, UInt_t nId,
//                 const CParameter& rXParameter, const CParameter rYParmeter,
//                 UInt_t            nxScale, UInt_t nYScale)
// Operation Type:
//   Constructor
//
CMSpectrum2DW::CMSpectrum2DW(const std::string& rName, UInt_t nId,
			     const CParameter& rXParameter, 
			     const CParameter& rYParameter,
			     Float_t nXLow, Float_t nYLow, Float_t nXHigh,
			     Float_t nYHigh, UInt_t XChans, UInt_t YChans) :
  CSpectrum2DW(rName, nId, rXParameter, rYParameter),
  m_nXLow(nXLow),
  m_nYLow(nYLow),
  m_nXHigh(nXHigh),
  m_nYHigh(nYHigh),
  m_nXChannels(XChans),
  m_nYChannels(YChans)
{
  // Set up the parameter definition
  m_XParameter.nScale = rXParameter.getScale();
  m_YParameter.nScale = rYParameter.getScale();
  m_XParameter.nLow   = rXParameter.getLow();
  m_XParameter.nHigh  = rXParameter.getHigh();
  m_YParameter.nLow   = rYParameter.getLow();
  m_YParameter.nHigh  = rYParameter.getHigh();
  m_XParameter.sUnits = rXParameter.getUnits();
  m_YParameter.sUnits = rYParameter.getUnits();

  // Just need to allocate storage and pass it to our base class for
  // management:

  setStorageType(keWord);

  //Size_t nBytes = StorageNeeded();
  UShort_t* pStorage = new UShort_t[(XChans*YChans)];

  ReplaceStorage(pStorage);	// Storage now owned by parent.
  Clear();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Increment( const CEvent& rEvent )
//  Operation Type:
//     mutator
//
void 
CMSpectrum2DW::Increment(const CEvent& rE) 
{
  CEvent& rEvent((CEvent&)rE);
  UInt_t nXParam = getXParameter();
  UInt_t nYParam = getYParameter();
  if(rEvent[nXParam].isValid() &&  // Require the parameters be in event
     rEvent[nYParam].isValid()) {
    // The channels in mapped spectrum coordinates
    UInt_t nXChannel = XParamToSpecPoint(rEvent[nXParam]);
    UInt_t nYChannel = YParamToSpecPoint(rEvent[nYParam]);
    if((nXChannel < m_nXChannels) && (nYChannel < m_nYChannels)) {
      UShort_t* pSpec = (UShort_t*)getStorage();
      pSpec[nXChannel + (nYChannel * m_nXChannels)]++;
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
CMSpectrum2DW::Dimension (UInt_t n) const
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
//    UInt_t Randomize(Float_t nChannel)
//  Operation Type:
//    operation
//  Purpose:
//    UInt_t CMSpectrum2DW::Randomize(Float_t nChannel)
//
//    Given a floating point channel value, randomly select an
//    appropriate integer value channel to increment using a weighted
//    randomizer.  Returns the selected channel.
//
UInt_t
CMSpectrum2DW::Randomize(Float_t nChannel)
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
//    UInt_t SpecToXParamPoint(UInt_t nSpecPoint)
//  Operation Type:
//    conversion
//  Purpose:
//
//    Scales a spectrum point to its appropriate value in 
//    x-parameter space.
//
UInt_t
CMSpectrum2DW::SpecToXParamPoint(UInt_t nSpecPoint)
{
  Float_t nParamRange = (m_XParameter.nHigh - m_XParameter.nLow);
  Float_t nParamStep  = nParamRange / (1 << m_XParameter.nScale);
  Float_t nParamToSpecScale = (m_nXChannels / nParamRange);
  Float_t nParamCoord = (Float_t)nSpecPoint / nParamToSpecScale;
  return (UInt_t)(nParamCoord / nParamStep);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t SpecToYParamPoint(UInt_t nSpecPoint)
//  Operation Type:
//    conversion
//  Purpose:
//
//    Scales a spectrum point to its appropriate value in 
//    y-parameter space.
//
UInt_t
CMSpectrum2DW::SpecToYParamPoint(UInt_t nSpecPoint)
{
  Float_t nParamRange = (m_YParameter.nHigh - m_YParameter.nLow);
  Float_t nParamStep  = nParamRange / (1 << m_YParameter.nScale);
  Float_t nParamToSpecScale = (m_nYChannels / nParamRange);
  Float_t nParamCoord = (Float_t)nSpecPoint / nParamToSpecScale;
  return (UInt_t)(nParamCoord / nParamStep);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t XParamToSpecPoint(UInt_t nParamPoint)
//  Operation Type:
//    conversion
//  Purpose:
//
//    Scales a parameter point to its appropriate value in 
//    x-spectrum space.
//
UInt_t
CMSpectrum2DW::XParamToSpecPoint(UInt_t nParamPoint)
{
  // The parameter range space step size
  Float_t nParamStep = (m_XParameter.nHigh - m_XParameter.nLow) /
    (1 << m_XParameter.nScale);

  // The parameter coordinate in mapped parameter space
  Float_t nParamCoord = (nParamPoint * nParamStep);
  
  // The parameter to spectrum space scaling
  Float_t nParamToSpecScale = ((m_nXChannels) / 
			       (m_XParameter.nHigh - m_XParameter.nLow));
  
  // The channel in mapped spectrum coordinates
  return Randomize(nParamCoord * nParamToSpecScale);
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    UInt_t YParamToSpecPoint(UInt_t nParamPoint)
//  Operation Type:
//    conversion
//  Purpose:
//
//    Scales a parameter point to its appropriate value in 
//    y-spectrum space.
//
UInt_t
CMSpectrum2DW::YParamToSpecPoint(UInt_t nParamPoint)
{
  // The parameter range space step size
  Float_t nParamStep = (m_YParameter.nHigh - m_YParameter.nLow) /
    (1 << m_YParameter.nScale);

  // The parameter coordinate in mapped parameter space
  Float_t nParamCoord = (nParamPoint * nParamStep);
  
  // The parameter to spectrum space scaling
  Float_t nParamToSpecScale = ((m_nYChannels) / 
			       (m_YParameter.nHigh - m_YParameter.nLow));
  
  // The channel in mapped spectrum coordinates
  return Randomize(nParamCoord * nParamToSpecScale);
}
