//  BitSpectrumW.cpp:
//     Implements the non trivial functions of the CBitSpectrumW class.
//     This class maintains bit mask spectra.  For each bit set in the
//     parameter represented by this spectrum in each event, the corresponding
//     channel  is incremented.  The scale (spectrum size), simply determined
//     how many bits are represented by the spectrum.
// Author:
//   Ron Fox
//   NSCL
//   Michigan State Univesrity
//   East Lansing, MI 48824-1321
//
#include "BitSpectrumW.h"
#include "Parameter.h"
#include "Event.h"
#include "RangeError.h"
#include <histotypes.h>
#include <assert.h>
#include <math.h>

static const char* pCopyRight =
"BitSpectrumW.cpp - (c) Copyright NSCL 1999, All rights reserved.\n";


/////////////////////////////////////////////////////////////////////////////

// Functions for class CBitSpectrumW:

////////////////////////////////////////////////////////////////////////////
//
// Function:
//      CBitSpectrumW(const std::string& rName, UInt_t nId,
//                    const CParameter& rParameter,
//                    UInt_t            nScale)
// Operation Type:
//      Constructor.
//
CBitSpectrumW::CBitSpectrumW(const std::string& rName, UInt_t nId,
			     const CParameter& rParameter,
			     UInt_t            nScale) :
  CSpectrum(rName, nId),
  m_nChannels(1 << nScale),
  m_nParameter(rParameter.getNumber())
  
{
  setStorageType(keWord);
  UShort_t* pStorage = new UShort_t[m_nChannels];
  ReplaceStorage(pStorage);	// Submit storage and ownership to parent class
  Clear();
}

///////////////////////////////////////////////////////////////////////
// Function:
//    void Increment(const CEvent& rEvent)
// Operation Type:
//   Functionalizer.
//
void 
CBitSpectrumW::Increment(const CEvent& rE)
{
  // Increment the histogram as dictated by the event.
  // 
  // Formal Paramters:
  //    const CEvent& rEvent:
  //      Refers to the event being histogrammed at this instant.
  //
  CEvent& rEvent((CEvent&)rE);
  if(rEvent[m_nParameter].isValid()) {
    UShort_t* p = (UShort_t*)getStorage();
    assert(p != (UShort_t*)kpNULL);
    UInt_t nParam(rEvent[m_nParameter]);
    UInt_t nBit   = 1;
    UInt_t nChan  = 0;
    //
    //  Increment a channel in p for every bit set in 
    //  nParam:
    //
    while((nParam != 0) && (nChan < m_nChannels)) {
      if(nBit & nParam) {
	p[nChan]++;
	nParam &= ~nBit;
      }
      nBit = nBit << 1;
      nChan++;
    }
  }
}
///////////////////////////////////////////////////////////////////////////
//
// Function:
//    Bool_t  UsesParameter (UInt_t nId) const
//  Operation Type:
//     Selector
//
Bool_t
CBitSpectrumW::UsesParameter (UInt_t nId) const
{
  return (m_nParameter == nId);
}
/////////////////////////////////////////////////////////////////////////
//
// Function:
//     ULong_t operator[] (const UInt_t* pIndices) const
// Operation Type:
//     Selector.
//
ULong_t
CBitSpectrumW::operator[](const UInt_t* pIndices) const
{
  // Provides the value of an element of hte spectrum.
  // Note: this is not a 'normal' indexing operator in that it only
  // produces rvalues.  This, however allows a uniform call/return 
  // for all types of spectra.
  //
  UShort_t* p = (UShort_t*)getStorage();
  UInt_t    n = pIndices[0];
  if(n > Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, n,
		      std::string("Indexing BitW spectrum"));
  }
  return (ULong_t)p[n];
}
///////////////////////////////////////////////////////////////////////
//
// Function:
//     void set(const UInt_t* pIndices, ULong_t nValue)
// Operation Type:
//     Mutator.
//
void
CBitSpectrumW::set(const UInt_t* pIndices, ULong_t nValue)
{
  // Provides write access to a spectrum channel.
  // 
  // Formal Parameters:
  //    const UInt_t* pIndices:
  //        Set of array indices for the spectrum (only [0] is used).
  //    ULong_t nValue:
  //        Value to set.
  //
 
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
CBitSpectrumW::GetParameterIds(vector<UInt_t>& rvIds)
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
CBitSpectrumW::GetResolutions(vector<UInt_t>&  rvResolutions)
{
  // Returns a vector containing the set of spectrum resolutions.
  // In this case it's just the single resolution.
  //
  rvResolutions.erase(rvResolutions.begin(), rvResolutions.end());

  DFloat_t scale = log((DFloat_t)m_nChannels)/log(2.0);
  rvResolutions.push_back((UInt_t)(scale + 0.5));
}
////////////////////////////////////////////////////////////////////////
//
// Function:
//     virtual Int_t getScale(UInt_t index)
// Operation type:
//     Selector.
//
Int_t 
CBitSpectrumW::getScale(UInt_t index)
{
  // Returns the index'th scale factor.. this is
  // just  0

  return 0;
}
