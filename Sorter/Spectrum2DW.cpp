//  CSpectrum2DW.cpp
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


#include "Spectrum2DW.h"                               
#include "Parameter.h"
#include "RangeError.h"
#include "Event.h"


static const char* Copyright = 
"CSpectrum1DW.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CSpectrum2DW

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//   CSpectrum1DW(const std::string& rname, UInt_t nId,
//               const CParameter& rXParameter, const CParameter rYParmeter,
//               UInt_t            nxScale, UInt_t nYScale)
// Operation Type:
//   Constructor
//
CSpectrum2DW::CSpectrum2DW(const std::string& rName, UInt_t nId,
			   const CParameter& rXParameter, 
			   const CParameter& rYParameter,
			   UInt_t nXScale, UInt_t nYScale) :
  CSpectrum(rName, nId),
  m_nXScale(nXScale),
  m_nYScale(nYScale),
  m_nXParameter(rXParameter.getNumber()),
  m_nYParameter(rYParameter.getNumber()),
  m_nXScaleDifference((Int_t)rXParameter.getScale() - (Int_t)nXScale),
  m_nYScaleDifference((Int_t)rYParameter.getScale() - (Int_t)nYScale)
{
  // Just need to allocate storage and pass it to our base class for
  // management:

  setStorageType(keWord);

  Size_t nBytes = StorageNeeded();
  UShort_t*      pStorage = new UShort_t[nBytes/sizeof(UShort_t)];

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
CSpectrum2DW::Increment(const CEvent& rE) 
{
// Increments channel number rEvent[m_nParameter] >> m_nScaleDifference
// Formal Parameters:
//
//          const CEvent& rEvent:
//               Event which drives the histogramming
//
  CEvent& rEvent((CEvent&)rE);	// Ok since non const operator[] only on rhs.

  if(rEvent[m_nXParameter].isValid()  && // Require the parameters be in event
     rEvent[m_nYParameter].isValid()) { // size range.
    UInt_t nx = (m_nXScaleDifference >= 0) ? 
		 (UInt_t)(rEvent[m_nXParameter] >> m_nXScaleDifference) :
		 (UInt_t)(rEvent[m_nXParameter] << -(m_nXScaleDifference));
    UInt_t ny = (m_nYScaleDifference >= 0) ?
                   (UInt_t)(rEvent[m_nYParameter] >> m_nYScaleDifference) :
                   (UInt_t)(rEvent[m_nYParameter] << (-m_nYScaleDifference));
    if( (nx < (1 << m_nXScale))  && (ny < (1 << m_nYScale))) {
      UShort_t* pSpec = (UShort_t*)getStorage();
      pSpec[nx + (ny << m_nXScale)]++;
    }
  }

}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Boolt_t UsesParameter (UInt_t nId) const
//  Operation Type:
//     Selector
     
//
Bool_t 
CSpectrum2DW::UsesParameter(UInt_t nId) const
{
// Returns nParameter == m_nParameter
//
// Formal Parameters:
//      UInt_t nParameter:
//        The parameter being checked.

  return (m_nXParameter == nId || 
	  m_nYParameter == nId);

}

//////////////////////////////////////////////////////////////////////
//
// Function:
//   ULong_t operator[](const UInt_t* pIndices) const
// Operation:
//   Selector.
//
ULong_t
CSpectrum2DW::operator[](const UInt_t* pIndices) const
{
  // Provides the value of an element of the spectrum.
  // note:  This is not a 'normal' indexing operation in that
  // references are not returned.
  //
  UShort_t* p = (UShort_t*)getStorage();
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
  return (ULong_t)p[nx + (ny << m_nXScale)];
		      
}
///////////////////////////////////////////////////////////////////////////
//
// Function:
//    virtual void CSpectrum2DW::set(const UInt_t* pIndices, ULong_t nValue)
//
// Operation Type:
//    Mutator.
//
void
CSpectrum2DW::set(const UInt_t* pIndices, ULong_t nValue)
{
  // Provides write access to a channel of the spectrum.
  //
  UShort_t* p = (UShort_t*)getStorage();
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
  p[nx + (ny << m_nXScale)] = (UInt_t)nValue;

  
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//     UInt_t Dimension (UInt_t n) const
// Operation type:
//     Selector.
//
UInt_t 
CSpectrum2DW::Dimension (UInt_t n) const
{
  switch(n) {
  case 0:
    return 1 << m_nXScale;
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
CSpectrum2DW::GetParameterIds(vector<UInt_t>& rvIds)
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
  rvIds.erase(rvIds.begin(), rvIds.end());// Erase the vector.
  rvIds.push_back(m_nXParameter);
  rvIds.push_back(m_nYParameter);
}
////////////////////////////////////////////////////////////////////////
//
// Function:
//    void GetResolutions(vector<UInt_t>&  rvResolutions)
// Operation type:
//    Selector
void
CSpectrum2DW::GetResolutions(vector<UInt_t>&  rvResolutions)
{
  // Returns a vector containing the resolutions of the spectrum.
  // In this case just m_nXScale, m_nYScale
  //
  // Formal Parameters:
  //    vector<UInt_t>&  rvResolutions:
  //          Vector which will contain the resolutions.

  rvResolutions.erase(rvResolutions.begin(), rvResolutions.end());
  rvResolutions.push_back(m_nXScale);
  rvResolutions.push_back(m_nYScale);
}
/////////////////////////////////////////////////////////////////////////
//
// Function:
//    UInt_t getScale(UInt_t nIndex)
// Operation Type:
//   Selector:
//
//
Int_t
CSpectrum2DW::getScale(UInt_t nIndex)
{
  switch(nIndex) {
  case 0:
    return m_nXScaleDifference;
  case 1:
    return m_nYScaleDifference;
  default:
    return 0;
  }

}
