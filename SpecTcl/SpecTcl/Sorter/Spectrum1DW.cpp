//  CSpectrum1DW.cpp
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


#include "Spectrum1DW.h"                               
#include "Parameter.h"
#include "RangeError.h"
#include "Event.h"


static const char* Copyright = 
"CSpectrum1DW.cpp: Copyright 1999 NSCL, All rights reserved\n";

// Functions for class CSpectrum1DW

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//   CSpectrum1DW(const std::string& rname, UInt_t nId,
//               const CParameter& rParameter,
//               UInt_t            nScale)
// Operation Type:
//   Constructor
//
CSpectrum1DW::CSpectrum1DW(const std::string& rName, UInt_t nId,
			    const CParameter& rParameter,
			    UInt_t            nScale) :
  CSpectrum(rName, nId),
  m_nScale(nScale),
  m_nParameter(rParameter.getNumber()),
  m_nScaleDifference((Int_t)rParameter.getScale() - (Int_t)nScale)
{
  // Just need to allocate storage and pass it to our base class for
  // management:

  setStorageType(keWord);
  UShort_t* pStorage = new UShort_t[(1 << m_nScale)];
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
CSpectrum1DW::Increment(const CEvent& rE) 
{
// Increments channel number rEvent[m_nParameter] >> m_nScaleDifference
// Formal Parameters:
//
//          const CEvent& rEvent:
//               Event which drives the histogramming
//
  CEvent& rEvent((CEvent&)rE);
  if(rEvent[m_nParameter].isValid()) {  // Only increment if param present.
    UInt_t nChannel = (m_nScaleDifference > 0) ?
      (UInt_t)(rEvent[m_nParameter] >> m_nScaleDifference) :
      (UInt_t)(rEvent[m_nParameter] << (-m_nScaleDifference));
    if(nChannel < (1 << m_nScale)) {  // Only increment if in range. 
      UShort_t* p = (UShort_t*)getStorage();
      assert(p != (UShort_t*)kpNULL);    // Spectrum storage must exist!!
      p[nChannel]++;		      // Increment the histogram.
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
CSpectrum1DW::UsesParameter(UInt_t nId) const
{
// Returns nParameter == m_nParameter
//
// Formal Parameters:
//      UInt_t nParameter:
//        The parameter being checked.

  return (m_nParameter == nId);

}

//////////////////////////////////////////////////////////////////////
//
// Function:
//   ULong_t operator[](const UInt_t* pIndices) const
// Operation:
//   Selector.
//
ULong_t
CSpectrum1DW::operator[](const UInt_t* pIndices) const
{
  // Provides the value of an element of the spectrum.
  // note:  This is not a 'normal' indexing operation in that
  // references are not returned.
  //
  UShort_t* p = (UShort_t*)getStorage();
  UInt_t   n = pIndices[0];
  if(n >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, n,
		      std::string("Indexing 1DW spectrum"));
  }
  return (ULong_t)p[n];
		      
}
///////////////////////////////////////////////////////////////////////////
//
// Function:
//    virtual void CSpectrum1DW::set(const UInt_t* pIndices, ULong_t nValue)
//
// Operation Type:
//    Mutator.
//
void
CSpectrum1DW::set(const UInt_t* pIndices, ULong_t nValue)
{
  // Provides write access to a channel of the spectrum.
  //
  UShort_t* p = (UShort_t*)getStorage();
  UInt_t   n = pIndices[0];
  if(n >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, n,
		      std::string("Indexing 1DW spectrum"));
  }
  p[n] = (UInt_t)nValue;

  
}
/////////////////////////////////////////////////////////////////////
//
//  Function:
//    void GetParameterIds(vector<UInt_t>& rvIds)
//  Operation Type:
//    Selector.
//
void
CSpectrum1DW::GetParameterIds(vector<UInt_t>& rvIds)
{
  // Returns a vector containing the set of parameter Ids which are
  // used to increment this spectrum.  In our case, there's just one
  // id, the single parameter histogrammed.
  // 
  // Formal Parameters:
  //    vector<UInt_t>& rvIds:
  //       Refers to the vector to be returned.
  //
  rvIds.erase(rvIds.begin(), rvIds.end());// Clear the vector.
  rvIds.push_back(m_nParameter);
}
///////////////////////////////////////////////////////////////////////
//
//
// Function:
//    void GetResolutions(vector<UInt_t>&  rvResolutions)

// Operation type:
//    Selector
void
CSpectrum1DW::GetResolutions(vector<UInt_t>&  rvResolutions)
{
  // Returns a vector containing the set of spectrum resolutions.
  // In this case it's just the single resolution.
  //
  rvResolutions.erase(rvResolutions.begin(), rvResolutions.end());
  rvResolutions.push_back(m_nScale);
}
////////////////////////////////////////////////////////////////////////
//  
// Function:
//    UInt_t getScale(UInt_t nIndex)
// Operation Type:
//    Selector.
//
Int_t
CSpectrum1DW::getScale(UInt_t nIndex)
{
  return ((nIndex == 0) ? m_nScaleDifference : 0);
}
