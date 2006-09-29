/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


static const char* Copyright = "(C) Copyright Michigan State University 2008, All rights reserved";
#include <config.h>
#include "GateMediator.h"
#include <Gate.h>
#include <Cut.h>
#include <Band.h>
#include <Contour.h>
#include <CGammaCut.h>
#include <CGammaBand.h>
#include <CGammaContour.h>

#include "Spectrum.h"
#include <Iostream.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

Bool_t
CGateMediator::operator() ()
{
  string gType = (*m_rGate).Type();
  SpectrumType_t spType = m_pSpec->getSpectrumType();
  UInt_t xParameter, yParameter;
  vector<string> SpecList;
  string SpecName;
  
  // Figure out what kind of spectrum we have, then determine if
  // the gate can be displayed on it.
  switch(spType) {
  case ke1D:
    return mediate1d();
  case ke2D:
    return mediate2d();
  case keG1D:
    return mediateGamma1();
  case keG2D:
    return mediateGamma2();
  case ke2Dm:
    return mediate2dMultiple();
  
    // No other spectrum types can display gates.
  default:
    return kfFALSE;
  }
  return kfFALSE;
}
/**
 *   Mediate the gates for a 1-d spectrum.
 *   1-d spectra can only display gates that are slices
 *   and then, only if the slices are on the parameter displayed
 *   on that gate.
 */
Bool_t
CGateMediator::mediate1d()
{
  string gType = m_rGate->Type();
  if (gType == "s") {
    CCut& rCut = (CCut&)(*m_rGate);
    UInt_t xParameter = rCut.getId();   // the parameter the gate is set on
    if(m_pSpec->UsesParameter(xParameter)) {
      return kfTRUE;
    } else {
      return kfFALSE;
    }
  }
  else {
    return kfFALSE;
  }
}
/**
 *  Determine if a gate is displayable on a 2-d spectrum.
 *  Only band and contour gates are displayable, and then only if both parameters
 *  are involved in the gate.  Presumably if a flip is required it gets done
 *  by the gate entry functions.
 */
Bool_t
CGateMediator::mediate2d()
{
  string gType = m_rGate->Type();
  UInt_t xParameter;
  UInt_t yParameter;
  if((gType == "b") || (gType == "c")) {
    CPointListGate& rPLGate = (CPointListGate&)(*m_rGate);
    xParameter = rPLGate.getxId();
    yParameter = rPLGate.getyId();
    if(m_pSpec->UsesParameter(xParameter) && 
       m_pSpec->UsesParameter(yParameter)) {
      return kfTRUE;
    }
    else {
      return kfFALSE;
    }
    
  }
  else {
    return kfFALSE;
  }
}
Bool_t 
CGateMediator::mediate2dMultiple()
{
  return false;			// for now it is false.. tough tofigure out.
}
/**
 *  Determine if a gate is displayable on 1-d gamma spectrum.
 *  The gate is displayable if the gate is a slice and all of
 *  the parameters in the gate are also in the spectrum.
 *
 */
Bool_t
CGateMediator::mediateGamma1()
{
  string gType = m_rGate->Type();
  if(gType == "gs") {
    CGammaCut& rGCut = (CGammaCut&)(*m_rGate);
    vector<UInt_t> parameters = rGCut.getParameters();
    for (int i=0; i < parameters.size(); i++) {
      if (!m_pSpec->UsesParameter(parameters[i])) {
	return kfFALSE;
      }
    }
    return kfTRUE;
  }
  else {
    return kfFALSE;
  }
}
/**
 * Determine if the gate is displayable on a 2-d gamma spectrum.
 * The gate is displayable if the gate  is a gamma band (gb), or a 
 * gamma contour (gc), and all the parameters are used by the spectrum.
 */
Bool_t
CGateMediator::mediateGamma2()
{
  string gType = m_rGate->Type();
  if ((gType == "gb") || (gType == "gc")) {
    vector<UInt_t>  parameters;
    if(gType == "gb") {
      parameters = ((CGammaBand&)(*m_rGate)).getParameters();
    }
    if(gType == "gc") {
      parameters = ((CGammaContour&)(*m_rGate)).getParameters();
    }
    // There must be at lest 2 params in the gate.

    if(parameters.size() < 2) {
      return kfFALSE;
    }
    // All gate params must be in the spectrum too.

    for (int i =0; i < parameters.size(); i++) {
      if (!m_pSpec->UsesParameter(parameters[i])) {
	return kfFALSE;
      }
      return kfTRUE;
    }
  }
  else {
    return kfFALSE;
  }
  return kfFALSE;
}
