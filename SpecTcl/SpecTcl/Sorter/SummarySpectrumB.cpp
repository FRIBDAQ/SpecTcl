//  CSummarySpectrumB.cpp
// Encapsulates the prototypical 1-d Spectrum.  
// For the purposes of the functional prototype,
// the spectrum is a singly incremented
// 1-d spectrum with longword channel size
//  data are dynamically allocated initially (self owned).
// 
//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//


#include "SummarySpectrumB.h"                               
#include "Parameter.h"
#include "RangeError.h"
#include "Event.h"
#include <algorithm>
#include <assert.h>

static const char* Copyright = 
"CSpectrum1DW.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CSummarySpectrumB

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//   CSummarySpectrumB(const std::string& rname, UInt_t nId,
//                     std::vector<CParameter&>& rrParameters,
//                     UInt_t nYScale)
// Operation Type:
//   Constructor
//
CSummarySpectrumB::CSummarySpectrumB(const std::string& rName, UInt_t nId,
				     vector<CParameter> rrParameters,
				     UInt_t nYScale) :

  CSpectrum(rName, nId),
  m_nYScale(nYScale),
  m_nXChannels(rrParameters.size())
{

  // Fill in the parameter and scale difference vector members:

  for(UInt_t i = 0; i < rrParameters.size(); i++) {
    CParameter& rParam(rrParameters[i]);
    ParameterDef def;
    def.nParameter = rParam.getNumber();
    def.nScale     = rParam.getScale() - m_nYScale;
    m_vParameters.push_back(def);
  }

  // Just need to allocate storage and pass it to our base class for
  // management:

  setStorageType(keByte);

  Size_t nBytes = StorageNeeded();
  UChar_t*      pStorage = new UChar_t[nBytes/sizeof(UChar_t)];

  ReplaceStorage(pStorage);	// Storage now owned by parent.
  Clear();
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void Increment ( const CEvent& rE )
//  Operation Type:
//     mutator
//
void 
CSummarySpectrumB::Increment(const CEvent& rE) 
{
// Increments channel number rEvent[m_nParameter] >> m_nScaleDifference
// Formal Parameters:
//
//          const CEvent& rEvent:
//               Event which drives the histogramming
//

  CEvent& rEvent((CEvent&)rE);
  UInt_t nYChans = 1 << m_nYScale;
  UChar_t* pStorage = (UChar_t*)getStorage();

  for(UInt_t xChan = 0; xChan < m_vParameters.size(); xChan++) {
    if(rEvent[m_vParameters[xChan].nParameter].isValid()) {
      Int_t sd = m_vParameters[xChan].nScale;
      UInt_t y = rEvent[m_vParameters[xChan].nParameter];
      y = (sd > 0) ? y >> sd :
	y << -sd; 
      if(y < (1 << m_nYScale)) { // Ensure scaled param is inside spectrum.
	pStorage[xChan + y*m_nXChannels]++;
      }
    }
  }
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Boolt_t UsesParameter (UInt_t nId)
//  Operation Type:
//     Selector
     
//
Bool_t 
CSummarySpectrumB::UsesParameter(UInt_t nId) const
{
// Returns nParameter == m_nParameter
//
// Formal Parameters:
//      UInt_t nParameter:
//        The parameter being checked.

  for(UInt_t i = 0; i < m_vParameters.size(); i++) {
    if( m_vParameters[i].nParameter == nId)
      return kfTRUE;
  }
  return kfFALSE;

}

//////////////////////////////////////////////////////////////////////
//
// Function:
//   ULong_t operator[](const UInt_t* pIndices) const
// Operation:
//   Selector.
//
ULong_t
CSummarySpectrumB::operator[](const UInt_t* pIndices) const
{
  // Provides the value of an element of the spectrum.
  // note:  This is not a 'normal' indexing operation in that
  // references are not returned.
  //
  UChar_t* p = (UChar_t*)getStorage();
  UInt_t   nx = pIndices[0];
  UInt_t   ny = pIndices[1];
  if(nx >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, nx,
		      std::string("Indexing SummaryW spectrum x axis"));
  }
  if(ny >= Dimension(1)) {
    throw CRangeError(0, Dimension(1)-1, ny,
		      std::string("Indexing SummaryW spectrum y axis"));
  }
  return (ULong_t)p[nx + (ny * Dimension(0))];
		      
}
///////////////////////////////////////////////////////////////////////////
//
// Function:
//    virtual void CSummarySpectrumB::set(const UInt_t* pIndices, ULong_t nValue)
//
// Operation Type:
//    Mutator.
//
void
CSummarySpectrumB::set(const UInt_t* pIndices, ULong_t nValue)
{
  // Provides write access to a channel of the spectrum.
  //
  UChar_t* p = (UChar_t*)getStorage();
  UInt_t   nx = pIndices[0];
  UInt_t   ny = pIndices[1];
  if(nx >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, nx,
		      std::string("Indexing 2DW spectrum x axis"));
  }
  if(ny >= Dimension(1)) {
    throw CRangeError(0, Dimension(1)-1, ny,
		      std::string("Indexing 2DW spectrum y axis"));
  }
  p[nx + (ny * Dimension(0))] = (UInt_t)nValue;

  
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//     UInt_t Dimension (UInt_t n) const
// Operation type:
//     Selector.
//
UInt_t 
CSummarySpectrumB::Dimension (UInt_t n) const
{
  switch(n) {
  case 0:
    return m_nXChannels;
  case 1:
    return 1 << m_nYScale;
  default:
    return 0;
  }
}
///////////////////////////////////////////////////////////////////////////
//
// Function
//   void GetParameterIds(vector<UInt_t>& rvIds)
// Operation type:
//   Selector.
void 
CSummarySpectrumB::GetParameterIds(vector<UInt_t>& rvIds)
{
  // Gets the set of parameter ids which make up this 
  // spectrum.  In this case this is just the x and y
  // parameters.
  // 
  // Formal Parameters:
  //    vector<UInt_t>& rvIds:
  //       vector which will contain the list of parameter ids which go into
  //       this spectrum.
  //
  for(UInt_t i = 0;  i < m_vParameters.size(); i++)
  rvIds.push_back(m_vParameters[i].nParameter);
}
////////////////////////////////////////////////////////////////////////
//
// Function:
//    void GetResolutions(vector<UInt_t>&  rvResolutions)
// Operation type:
//    Selector
void
CSummarySpectrumB::GetResolutions(vector<UInt_t>&  rvResolutions)
{
  // Returns a vector containing the resolutions of the spectrum.
  // In this case just m_nYScale.
  //
  // Formal Parameters:
  //    vector<UInt_t>&  rvResolutions:
  //          Vector which will contain the resolutions.

  rvResolutions.push_back(m_nYScale);
}
////////////////////////////////////////////////////////////////////////
//
// Function:
//   UInt_t getScale(UInt_t index)
// Operation Type:
//   selector.
//
Int_t
CSummarySpectrumB::getScale(UInt_t index)
{
  if(index < m_vParameters.size()) {
    return m_vParameters[index].nScale;
  }
  else {
    return 0;
  }
}
