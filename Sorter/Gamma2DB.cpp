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
//    mailto:venemaja@msu.edu

//
// Header Files:
//

#include "Gamma2DB.h"
#include "Parameter.h"
#include "RangeError.h"
#include "Event.h"
#include "GateContainer.h"
#include "Gate.h"
#include "GammaCut.h"
#include "GammaBand.h"
#include "GammaContour.h"

// Functions for class CGamma2DB

//////////////////////////////////////////////////////////////////////////
//
//  Function:
//   CGamma2DB(const std::string& rname, UInt_t nId,
//             vector<CParameter>& rParameters,
//             UInt_t nxScale, UInt_t nYScale)
// Operation Type:
//   Constructor
//

CGamma2DB::CGamma2DB(const std::string& rName, UInt_t nId,
		     vector<CParameter>& rParameters,
		     UInt_t nXScale, UInt_t nYScale) :
  CSpectrum(rName, nId),
  m_nXScale(nXScale),
  m_nYScale(nYScale)
{
  setStorageType(keByte);
  for (UInt_t i = 0; i < rParameters.size(); i++) {
    CParameter& rParam(rParameters[i]);
    ParameterDef def;
    def.nParameter = rParam.getNumber();
    def.nScale = rParam.getScale() - m_nXScale;
    m_vParameters.push_back(def);
  }
  
  Size_t nBytes = StorageNeeded();
  UChar_t* pStorage = new UChar_t[nBytes/sizeof(UChar_t)];

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
CGamma2DB::Increment(const CEvent& rE)
{
  string sGateType = getGate()->getGate()->Type();

  //Increment normally if gate is a 'normal' gate
  if(sGateType != "gs" && sGateType != "gb" && sGateType != "gc") {
    CEvent& rEvent((CEvent&)rE);
    UChar_t* pStorage = (UChar_t*)getStorage();
    assert(pStorage != (UChar_t*) kpNULL);
    UInt_t xChan, yChan;
    
    for (xChan = 0; xChan < m_vParameters.size()-1; xChan++) {
      for( yChan = xChan+1; yChan < m_vParameters.size(); yChan++) {
	if (rEvent[m_vParameters[xChan].nParameter].isValid() && 
	    rEvent[m_vParameters[yChan].nParameter].isValid())  {
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
CGamma2DB::GammaGateIncrement (const CEvent& rE, std::string sGT)
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
      if(rEvent[m_vParameters[xChan].nParameter].isValid() &&
	 rEvent[m_vParameters[yChan].nParameter].isValid()) {  // if valid...
	if(pGate->inGate(rEvent, vXP)) {  // and x-param is in the gate...
	  for(UInt_t p1 = 0; p1 < m_vParameters.size()-1; p1++) {
	    for(UInt_t p2 = p1+1; p2 < m_vParameters.size(); p2++) {
	      // Increment for all params not in the pair
	      if(p1 != xChan && p2 != xChan) {
		// Make sure these params are valid too...
		if(rEvent[m_vParameters[p1].nParameter].isValid() &&
		   rEvent[m_vParameters[p2].nParameter].isValid()) {
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
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Boolt_t UsesParameter (UInt_t nId) const
//  Operation Type:
//     Selector

Bool_t
CGamma2DB::UsesParameter(UInt_t nId) const
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
CGamma2DB::operator[] (const UInt_t* pIndices) const
{
  UChar_t* pStorage = (UChar_t*)getStorage();
  UInt_t nx = pIndices[0];
  UInt_t ny = pIndices[1];
  if (nx >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, nx,
		      std::string("Indexing 2DB gamma spectrum x axis"));
  }
  if (ny >= Dimension(1)) {
    throw CRangeError(0, Dimension(1)-1, ny,
		      std::string("Indexing 2DB gamma spectrum y axis"));
  }
  return (ULong_t)pStorage[nx + (ny << m_nXScale)];
}

///////////////////////////////////////////////////////////////////////////
//
// Function:
//    virtual void CGamma2DB::set(const UInt_t* pIndices, ULong_t nValue)
//
// Operation Type:
//    Mutator.
//

void
CGamma2DB::set (const UInt_t* pIndices, ULong_t nValue)
{
  UChar_t* pStorage = (UChar_t*)getStorage();
  UInt_t nx = pIndices[0];
  UInt_t ny = pIndices[1];
  if (nx >= Dimension(0)) {
    throw CRangeError(0, Dimension(0)-1, nx,
		      std::string("Indexing 2DB gamma spectrum x axis"));
  }
  if (ny >= Dimension(1)) {
    throw CRangeError(0, Dimension(1)-1, ny,
		      std::string("Indexing 2DB gamma spectrum y axis"));
  }
  pStorage[nx + (ny << m_nXScale)] = (UInt_t)nValue;
}

///////////////////////////////////////////////////////////////////////////
//
// Function
//   void GetParameterIds(vector<UInt_t>& rvIds)
// Operation type:
//   Selector.

void
CGamma2DB::GetParameterIds (vector<UInt_t>&rvIds)
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
CGamma2DB::Dimension (UInt_t n) const
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

void
CGamma2DB::GetResolutions (vector<UInt_t>& rvResolutions)
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
CGamma2DB::getScale (UInt_t nIndex)
{
  if (nIndex < m_vParameters.size()) {
    return m_vParameters[nIndex].nScale;
  }
  return 0;
}
