// CGamma1DW.cpp
// Encapsulates the prototypical 2-d Gamma spectrum
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
// Header Files:
//

#include "Gamma2DW.h"
#include "Parameter.h"
#include "RangeError.h"
#include "Event.h"
#include "GateContainer.h"
#include "Gate.h"
#include "GammaCut.h"
#include "GammaBand.h"
#include "GammaContour.h"

// Functions for class CGamma2DW

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//   CGammaDW(const std::string& rname, UInt_t nId,
//            vector<CParameter>& rParameters,
//            UInt_t nxScale, UInt_t nYScale)
// Operation Type:
//   Constructor
//

CGamma2DW::CGamma2DW(const std::string& rName, UInt_t nId,
		     vector<CParameter>& rParameters,
		     UInt_t nXScale, UInt_t nYScale) :
  CSpectrum(rName, nId),
  m_nXScale(nXScale),
  m_nYScale(nYScale)
{
  setStorageType(keWord);
  for (UInt_t i = 0; i < rParameters.size(); i++) {
    CParameter& rParam(rParameters[i]);
    ParameterDef def;
    def.nParameter = rParam.getNumber();
    def.nScale = rParam.getScale() - m_nXScale;
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
CGamma2DW::Increment(const CEvent& rE)
{
  string sGateType = getGate()->getGate()->Type();

  // Increment normally if gate is a 'normal' gate
  if(sGateType[0] != 'g') {
    CEvent& rEvent((CEvent&)rE);
    UShort_t* pStorage = (UShort_t*)getStorage();
    assert(pStorage != (UShort_t) kpNULL);
    UInt_t xChan, yChan;
    
    for (xChan = 0; xChan < m_vParameters.size()-1; xChan++) {
      for( yChan = xChan+1; yChan < m_vParameters.size(); yChan++) {
	if (rEvent[m_vParameters[xChan].nParameter].isValid() && 
	    rEvent[m_vParameters[yChan].nParameter].isValid()) {
	  Int_t xsd = m_vParameters[xChan].nScale;
	  Int_t ysd = m_vParameters[yChan].nScale;
	  UInt_t px = rEvent[m_vParameters[xChan].nParameter];
	  UInt_t py = rEvent[m_vParameters[yChan].nParameter];
	  px = (xsd > 0) ? px >> xsd : px << -xsd;
	  py = (ysd > 0) ? py >> ysd : py << -ysd;
	  if (px < (1 << m_nXScale) && py < (1 << m_nYScale )) {
	    pStorage[px + py*Dimension(0)]++;
	  }
	}
      }
    }
  }

  // Otherwise control is passed to GammaGateIncrement
  else if(sGateType[0] == 'g') {
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
CGamma2DW::GammaGateIncrement (const CEvent& rE, std::string sGT)
{
  UShort_t* pStorage = (UShort_t*)getStorage();
  assert(pStorage != (UShort_t*)kpNULL);
  CEvent& rEvent((CEvent&)rE);
  UInt_t xChan, yChan;
  vector<UInt_t> vXP, vYP;

  if(sGT == "gs") {  // Gate is a gamma slice
    CGammaCut* pGate = (CGammaCut*)getGate()->getGate();
    // For all parameters in the spectrum
    for(UInt_t xChan = 0; xChan < m_vParameters.size(); xChan++) {
      vXP.clear();
      vXP.push_back(m_vParameters[xChan].nParameter);
      if(rEvent[m_vParameters[xChan].nParameter].isValid()) { // if valid...
	if(pGate->inGate(rEvent, vXP)) {  // and X-param is in gate...
	  for(UInt_t p1 = 0; p1 < m_vParameters.size()-1; p1++) {
	    for(UInt_t p2 = p1+1; p2 < m_vParameters.size(); p2++) {
	      // Increment for all pairs not containing the param
	      if(p1 != xChan && p2 != xChan) {
		Int_t xsd = m_vParameters[p1].nScale;
		Int_t ysd = m_vParameters[p2].nScale;
		UInt_t px = rEvent[m_vParameters[p1].nParameter];
		UInt_t py = rEvent[m_vParameters[p2].nParameter];
		px = (xsd > 0) ? px >> xsd : px << -xsd;
		py = (ysd > 0) ? py >> ysd : py << -ysd;
		if (px < (1 << m_nXScale) && py < (1 << m_nYScale )) {
		  pStorage[px + py*Dimension(0)]++;
		}
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
    UInt_t mvx, mvy;
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
		if(((xChan != xParam && xChan != yParam)) &&
		   (yChan != xParam && yChan != yParam)) {
		  // Increment for all pairs which do not intersect
		  // with (xChan, yChan)
		  Int_t xsd = m_vParameters[xParam].nScale;
		  Int_t ysd = m_vParameters[yParam].nScale;
		  UInt_t px = rEvent[m_vParameters[xParam].nParameter];
		  UInt_t py = rEvent[m_vParameters[yParam].nParameter];
		  px = (xsd > 0) ? px >> xsd : px << -xsd;
		  py = (ysd > 0) ? py >> ysd : py << -ysd;
		  if (px < (1 << m_nXScale) && py < (1 << m_nYScale )) {
		    pStorage[px + py*Dimension(0)]++;
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
//  Function:   
//    Boolt_t UsesParameter (UInt_t nId) const
//  Operation Type:
//     Selector

Bool_t
CGamma2DW::UsesParameter(UInt_t nId) const
{
  for (UInt_t I = 0; I < m_vParameters.size(); I++) {
    if (m_vParameters[I].nParameter == nId)
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
CGamma2DW::operator[] (const UInt_t* pIndices) const
{
  UShort_t* pStorage = (UShort_t*)getStorage();
  UInt_t nx = pIndices[0];
  UInt_t ny = pIndices[1];
  if (nx >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, nx,
		      std::string("Indexing 2DW gamma spectrum x axis"));
  }
  if (ny >= Dimension(1)) {
    throw CRangeError(0, Dimension(1)-1, ny,
		      std::string("Indexing 2DW gamma spectrum y axis"));
  }
  return (ULong_t)pStorage[nx + (ny << m_nXScale)];
}

///////////////////////////////////////////////////////////////////////////
//
// Function:
//    virtual void CGamma2DW::set(const UInt_t* pIndices, ULong_t nValue)
//
// Operation Type:
//    Mutator.
//

void
CGamma2DW::set (const UInt_t* pIndices, ULong_t nValue)
{
  UShort_t* pStorage = (UShort_t*)getStorage();
  UInt_t nx = pIndices[0];
  UInt_t ny = pIndices[1];
  if (nx >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, nx,
		      std::string("Indexing 2DW gamma spectrum x axis"));
  }
  if (ny >= Dimension(1)) {
    throw CRangeError(0, Dimension(1)-1, ny,
		      std::string("Indexing 2DW gamma spectrum y axis"));
  }
  pStorage[nx + (ny << m_nXScale)] = (UInt_t)nValue;
}

///////////////////////////////////////////////////////////////////////////
//
// Function
//   void GetParameterIds(vector<UInt_t>& rvIds)
// Operation type:
//   Selector.
//

void
CGamma2DW::GetParameterIds (vector<UInt_t>&rvIds)
{
  for (UInt_t I = 0; I < m_vParameters.size(); I++) {
    rvIds.push_back(m_vParameters[I].nParameter);
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
CGamma2DW::Dimension (UInt_t n) const
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

////////////////////////////////////////////////////////////////////////
//
// Function:
//    void GetResolutions(vector<UInt_t>&  rvResolutions)
// Operation type:
//    Selector
//

void
CGamma2DW::GetResolutions (vector<UInt_t>& rvResolutions)
{
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

Int_t
CGamma2DW::getScale (UInt_t nIndex)
{
  if (nIndex < m_vParameters.size()) {
    return m_vParameters[nIndex].nScale;
  }
  return 0;
}
