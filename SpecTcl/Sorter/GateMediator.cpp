#include "GateMediator.h"
#include <Gate.h>
#include <Cut.h>
#include <Band.h>
#include <Contour.h>
#include <GammaCut.h>
#include <GammaBand.h>
#include <GammaContour.h>
#include "Spectrum.h"

Bool_t
CGateMediator::operator() ()
{
  string gType = (*m_rGate).Type();
  SpectrumType_t spType = m_pSpec->getSpectrumType();
  UInt_t xParameter, yParameter;
  vector<string> SpecList;
  string SpecName;
  
  //
  // Note the use of curly braces around case statements
  // to avoid 'jump crosses initialization' errors for
  // CGate objects.
  //
  switch(spType) {
  case ke1D:
    {
      if(gType != "s") {
	return kfFALSE;
      }
      CCut& rCut = (CCut&)(*m_rGate);
      xParameter = rCut.getId();   // the parameter the gate is set on
      if(m_pSpec->UsesParameter(xParameter)) 
	return kfTRUE;
      return kfFALSE;
      break;
    }

  case ke2D:
    {
      if(gType != "b" && gType != "c") {
	return kfFALSE;
      }
      CPointListGate& rPLGate = (CPointListGate&)(*m_rGate);
      if(gType == "b")
	rPLGate = (CBand&)rPLGate;
      else
	rPLGate = (CContour&)rPLGate;
      // Now we have the right gate
      xParameter = rPLGate.getxId();
      yParameter = rPLGate.getyId();
      if(m_pSpec->UsesParameter(xParameter) && 
	 m_pSpec->UsesParameter(yParameter))
	return kfTRUE;
      return kfFALSE;
      break;
    }
  
  case keG1D:
    {
      if(gType != "gs") {
	return kfFALSE;
      }
      CGammaCut& rGCut = (CGammaCut&)(*m_rGate);
      SpecList = rGCut.getSpecs();
      SpecName = m_pSpec->getName();
      for(UInt_t i = 0; i < SpecList.size(); i++) {
	if(SpecName == SpecList[i])
	  return kfTRUE;
      }
      return kfFALSE;
      break;
    }
  
  case keG2D:
    {
      if(gType != "gb" && gType != "gc") {
	return kfFALSE;
      }
      CPointListGate& rPLGate = (CPointListGate&)(*m_rGate);
      if(gType == "gb")
	rPLGate = (CGammaBand&)rPLGate;
      else if(gType == "gc")
	rPLGate = (CGammaContour&)rPLGate;
      // Now we have the right gate
      SpecList = rPLGate.getSpecs();
      SpecName = m_pSpec->getName();
      for(UInt_t i = 0; i < SpecList.size(); i++) {
	if(SpecName == SpecList[i])
	  return kfTRUE;
      }
      return kfFALSE;
      break;
    }
  
  default:
    return kfFALSE;
  }
}
