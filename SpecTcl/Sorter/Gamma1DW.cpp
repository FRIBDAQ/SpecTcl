// CGamma1DW.cpp
// Encapsulates the prototypical 1-d Gamma spectrum
// Channel size is word.
// Data are dynamically allocated.
//
//
// Author:
//    Jason Venema
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:venemaja@msu.edu

//
//  Header files:
//

#include "Gamma1DW.h"                       
#include "Parameter.h"
#include "RangeError.h"
#include "Event.h"
#include "GateContainer.h"
#include "Gate.h"
#include "GammaCut.h"
#include "GammaBand.h"
#include "GammaContour.h"

// Functions for class CGamma1DW

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//   CGamma1DW(const std::string& rname, UInt_t nId,
//             vector<CParameter> rrParameters,
//             UInt_t nScale)
// Operation Type:
//   Constructor
//

CGamma1DW::CGamma1DW(const std::string& rName, UInt_t nId,
		     vector<CParameter> rrParameters,
		     UInt_t nScale) :
  CSpectrum(rName, nId),
  m_nScale(nScale)
{
  setStorageType(keWord);
  for(UInt_t i = 0; i < rrParameters.size(); i++) {
    CParameter& rParam(rrParameters[i]);
    ParameterDef def;
    def.nParameter = rParam.getNumber();
    def.nScale = rParam.getScale() - m_nScale;
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
//    void Increment( const CEvent& rEvent )
//  Operation Type:
//    mutator
//  Purpose:
//    Increment channel number rEvent[m_vParameters[xChan].nParameter] >> 
//                             m_vParameters[xChan].nScale;
//    If gate is a gamma, control is passed to GammaGateIncrement
//
void CGamma1DW::Increment(const CEvent& rE)
{
  string sGateType = getGate()->getGate()->Type();

  // Increment normally if gate is a 'normal' gate...
  if (sGateType != "gs" && sGateType != "gb" && sGateType != "gc") {
    UShort_t* pStorage = (UShort_t*)getStorage();
    assert(pStorage != (UShort_t*)kpNULL);
    CEvent& rEvent((CEvent&)rE);
    UInt_t xChan;
    
    for (xChan = 0; xChan < m_vParameters.size(); xChan++) {
      if(rEvent[m_vParameters[xChan].nParameter].isValid()) {
	Int_t sd = m_vParameters[xChan].nScale;
	UInt_t y = rEvent[m_vParameters[xChan].nParameter];
	y = (sd > 0) ? y >> sd : y << -sd;
	if (y < (1 << m_nScale)) {
	  pStorage[y]++;
	}
      }
    }
  }

  // otherwise pass control to GammaGateIncrement
  else if (sGateType == "gs" || sGateType == "gb" || sGateType == "gc") {
    GammaGateIncrement (rE, sGateType);
  }
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    void GammaGateIncrement( const CEvent& rEvent, std::string sGT )
//  Operation Type:
//    mutator
//  Purpose:
//    Increment channel number rEvent[m_vParameters[xChan].nParameter] >> 
//                             m_vParameters[xChan].nScale;
//    for all parameters, or pairs of parameters, which do not fall 
//    in the gate.
//
void
CGamma1DW::GammaGateIncrement (const CEvent& rE, std::string sGT)
{
  UShort_t* pStorage = (UShort_t*)getStorage();
  assert(pStorage != (UShort_t*)kpNULL);
  CEvent& rEvent((CEvent&)rE);
  UInt_t xChan, mvx, mvy;
  vector<UInt_t> vXP, vYP;

  if(sGT == "gs") {    // Gate is a gamma slice
    CGammaCut* pGate = (CGammaCut*)getGate()->getGate();
    for(xChan = 0; xChan < m_vParameters.size(); xChan++) {  // for all params
      vXP.clear();
      vXP.push_back(m_vParameters[xChan].nParameter);
      if(rEvent[m_vParameters[xChan].nParameter].isValid()) {  // if valid
	if(pGate->inGate(rEvent, vXP)) {  // and parameter is in bounds
	  for(UInt_t Param = 0; Param < m_vParameters.size(); Param++) {
	    if(Param != xChan) {  // Increment for all other params
	      Int_t sd = m_vParameters[Param].nScale;
	      UInt_t y = rEvent[m_vParameters[Param].nParameter];
	      y = (sd > 0) ? y >> sd : y << -sd;
	      if (y < (1 << m_nScale)) {
		pStorage[y]++;
	      }
	    }
	  }
	}
      }
    }
  }
  
  else if(sGT == "gb" || sGT == "gc") {  // Gate is a gamma band or contour
    CPointListGate* pGate((CPointListGate*)(getGate()->getGate()));
    if(pGate->Type() == "gb") {
      pGate = (CGammaBand*)pGate;
    }
    else {
      pGate = (CGammaContour*)pGate;
    }
    // for all possible pairs of parameters in the spectrum
    for(UInt_t XPar = 0; XPar < m_vParameters.size()-1; XPar++) {
      for(UInt_t YPar = XPar+1; YPar < m_vParameters.size(); YPar++) {
	if(rEvent[m_vParameters[XPar].nParameter].isValid()) { // if valid...
	  mvx = m_vParameters[XPar].nParameter;
	  mvy = m_vParameters[YPar].nParameter;
	  vXP.clear(); vYP.clear();
	  vXP.push_back(mvx); vXP.push_back(mvy);
	  vYP.push_back(mvy); vYP.push_back(mvx);
	  // and if (p1, p2) or (p2, p1) is in gate... 
	  if(pGate->inGate(rEvent, vXP) || pGate->inGate(rEvent, vYP)) {
	    for(UInt_t Param = 0; Param < m_vParameters.size(); Param++) {
	      // Increment for all params not in (p1, p2)
	      if(Param != XPar && Param != YPar) {
		Int_t sd = m_vParameters[Param].nScale;
		UInt_t y = rEvent[m_vParameters[Param].nParameter];
		y = (sd > 0) ? y >> sd : y << -sd;
		if (y < (1 << m_nScale)) {
		  pStorage[y]++;
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
//  Function:   
//    Boolt_t UsesParameter (UInt_t nId) const
//  Operation Type:
//     Selector   
//

Bool_t
CGamma1DW::UsesParameter(UInt_t nId) const
{
  for (UInt_t I = 0; I < m_vParameters.size(); I++) {
    if (m_vParameters[I].nParameter == nId);
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
CGamma1DW::operator[] (const UInt_t* pIndices) const
{
  UShort_t* pStorage = (UShort_t*)getStorage();
  UInt_t n = pIndices[0];
  if (n >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, n, 
		      std::string("Indexing 1DW gamma spectrum"));
  }
  return (ULong_t)pStorage[n];
}

///////////////////////////////////////////////////////////////////////////
//
// Function:
//    virtual void CGamma1DW::set(const UInt_t* pIndices, ULong_t nValue)
//
// Operation Type:
//    Mutator.
//

void
CGamma1DW::set (const UInt_t* pIndices, ULong_t nValue)
{
  UShort_t* pStorage = (UShort_t*)getStorage();
  UInt_t n = pIndices[0];
  if (n >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, n,
		      std::string("Indexing 1DW gamma spectrum"));
  }
  pStorage[n] = (UInt_t)nValue;
}

/////////////////////////////////////////////////////////////////////
//
//  Function:
//    void GetParameterIds(vector<UInt_t>& rvIds)
//  Operation Type:
//    Selector.
//

void
CGamma1DW::GetParameterIds (vector<UInt_t>& rvIds)
{
  for (UInt_t I = 0; I < m_vParameters.size(); I++)
    rvIds.push_back(m_vParameters[I].nParameter);
}

///////////////////////////////////////////////////////////////////////
//
//
// Function:
//    void GetResolutions(vector<UInt_t>&  rvResolutions)
// Operation type:
//    Selector
//

void
CGamma1DW::GetResolutions (vector<UInt_t>& rvResolutions)
{
  rvResolutions.push_back (m_nScale);
}

////////////////////////////////////////////////////////////////////////
//  
// Function:
//    UInt_t getScale(UInt_t nIndex)
// Operation Type:
//    Selector.
//

Int_t
CGamma1DW::getScale (UInt_t nIndex)
{
  if (nIndex < m_vParameters.size()) {
    return m_vParameters[nIndex].nScale;
  }
  else {
    return 0;
  }
}
