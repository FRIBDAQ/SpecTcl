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

///////////////////////////////////////////////////////////
//  CFold.cpp
//  Implementation of the Class CFold
//  Created on:      22-Apr-2005 02:40:11 PM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////
#include <config.h>
#include "CFold.h"
#include <GateContainer.h>
#include <Event.h>
#include <CGammaSpectrum.h>
#include <CGammaCut.h>
#include <CGammaBand.h>
#include <CGammaContour.h>


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/**
 * Construct a fold;  Folds are a mechanism to study gamma cascades.
 * they allow you to remove the initial stages of a cascade from the spectrum
 * while conditionalizing further increments to require events in coincidence
 * with the initial stages.
 */
CFold::CFold(CGateContainer* pGate) :
  m_pGate(pGate)
{

}

/**
 * Destructor is a no-op for now
 */
CFold::~CFold()
{

}
/*
 * Copy constructor.  Since in SpecTcl, gate containers are fixed handles
 * to a named gate that never disappear once the gate is created, copy
 * construction can be done via a shallow copy:
 *  
 * @param rhs  
 *     The object from which we are copying to *this.
 *
 */
CFold::CFold(const CFold& rhs) :
  m_pGate(rhs.m_pGate)
{
}
/*
 * Assignment - See the notes under copy construction, once more a shallow
 * copy will do just fine.
 *
 * @param rhs  
 *     The object from which we are copying to *this.
 *
 */
CFold&
CFold::operator=(const CFold& rhs)
{
  if(this != &rhs) {
    m_pGate = rhs.m_pGate;
  }
  return *this;
}
/**
 *  Equivalence comparison. Since, again, gate containers are fixed
 *  items, two gate containers that are equivalent will result in 
 *  equivalent folds, so a shallow comparison is just fine.
 *
 * @param  rhs
 *    The object to which we are comparing *this for equivalence.
 * \return int
 * \retval  0   - not equivalent.
 * \retval  1   - equivalent.
 */
int
CFold::operator==(const CFold& rhs) const
{
  return (m_pGate == rhs.m_pGate);
}
/**
 * Inequivalence comparison.  This is defined as the inverse of equivalence.
 *
 * @param  rhs
 *    The object to which we are comparing *this for equivalence.
 * \return int
 * \retval  1   - not equivalent.
 * \retval  0   - equivalent.
 */
int
CFold::operator!=(const CFold& rhs) const
{
  return !(*this == rhs);
}

/**
 * If the fold points to a gamma gate then the name of that gate is returned. If
 * not, an empty stringis returned indicating there is no active fold on the
 * spectrum.
 *  \return std::string
 *  \retval  The name of the fold, or an empty string if there is no fold,
 *           or if the gate is no fold-worthy (not a gamma gate).
 *           SpecTcl does, in theory, allow empty gate names, but hopefully
 *           only an idiot will create a gate with an empty name and use it as
 *           a fold...
 */
std::string 
CFold::getFoldName()
{

  string type = (*m_pGate)->Type();
  if ((type == "gs") || (type == "gb") || (type == "gc")) {
      return m_pGate->getName();
  } 
  else {
    return string("");
  }
  

}


/**
 * Set the fold's gate container.
 * @param pGate    Pointer to the gate container of our new gate.
 * 
 */
void 
CFold::setGate(CGateContainer* pGate)
{
  
 m_pGate = pGate;
  
  
}


/**
 * Evaluate the fold on the event that's passed in and call back the spectrum's
 * increment member as fold membership is found.   There are three branches to
 * this logic: - The GateContainer is not a gamma gate: The Increment function is
 * called once with a list containing all valid parameters (unfolded behavior). -
 * The GateContainer is a gamma Cut: Each individual parameter is passed into the
 * cut... whenever we have a true value, a vector of all the other valid
 * parameters in the event is constructed and passed in to the increment function.
 * -  The gate is either a GammaBand or GammaContour: The gate is evaluated for
 * each pair of parameters in the event. Whenever a pair of parameters is in the
 * gate, a vector of the remaining valid parameters is formed that is passed to
 * the gamma increment.
 * @param rEvent    
 *      The event that is being histogrammed.
 * @param pSpectrum
 *       Pointer to the gamma spectrum we are folding... we
 *       will callback its special increment function.
 */
void 
CFold::operator()(vector<pair<UInt_t, Float_t> >& rEvent, CGammaSpectrum* pSpectrum)
{
  string gateType = (*m_pGate)->Type();

  // Slice gate (1-d)

  if(gateType == "gs") {
    CGammaCut* pGate = static_cast<CGammaCut*>(m_pGate->getGate());
    for (int i = 0; i < rEvent.size(); i++) {
      if(pGate->inGate(rEvent[i].second)) {
	vector<pair<UInt_t, Float_t> > parameterlist;
	CutDownParameters(parameterlist, rEvent, rEvent[i].first);
	if (parameterlist.size()) {
	  pSpectrum->Increment(parameterlist);
	}
      }
    }
  }

  // Band gate (2-d) - there may be some inheritance games we can play to 
  // collapse these two code branches.
  //

  else if(gateType == "gb") {
    CGammaBand* pBand = static_cast<CGammaBand*>(m_pGate->getGate());
    for (int i =0; i < rEvent.size()-1; i++) {
      for(int j = i+1; j < rEvent.size(); j++) {
	UInt_t   ipar    = rEvent[i].first;
	Float_t  iparval = rEvent[i].second;

	UInt_t   jpar    = rEvent[j].first;
	Float_t  jparval = rEvent[j].second;

	if(pBand->Inside(iparval, jparval) || pBand->Inside(jparval, iparval)) {
	  vector<pair<UInt_t, Float_t> > parameterlist;
	  CutDownParameters(parameterlist, rEvent, ipar, jpar);
	  pSpectrum->Increment(parameterlist);
	}
      }
    }

  // Contour gate (2-d)

  } 
  else if(gateType == "gc") {
    CGammaContour* pContour = static_cast<CGammaContour*>(m_pGate->getGate());
    for (int i =0; i < rEvent.size()-1; i++) {
      for(int j = i+1; j < rEvent.size(); j++) {
	UInt_t   ipar    = rEvent[i].first;
	Float_t  iparval = rEvent[i].second;
	
	UInt_t   jpar    = rEvent[j].first;
	Float_t  jparval = rEvent[j].second;
	
	if(pContour->Inside(iparval, jparval) || pContour->Inside(jparval, iparval)) {
	  vector<pair<UInt_t, Float_t> > parameterlist;
	  CutDownParameters(parameterlist, rEvent, ipar, jpar);
	  pSpectrum->Increment(parameterlist);
	}
      }
    }
  }
  
  // No gate or not a fold-worthy gate.
  
  else {
    pSpectrum->Increment(rEvent);
  }
}

/*!
   Apply a fold to a 2-d gamma deluxe spectrum.  
   This is the same effectively as 1-d but the cut down is for all
   the parameters in both the x/y vectors.
*/
void
CFold::operator()(std::vector<std::pair<UInt_t, Float_t> >& xParams,
		  std::vector<std::pair<UInt_t, Float_t> >& yParams,
		  CGammaSpectrum* pSpectrum)
{
  string gateType = (*m_pGate)->Type();

  // Slice gate (1-d)

  if(gateType == "gs") {
    CGammaCut* pGate = static_cast<CGammaCut*>(m_pGate->getGate());
    for (int i = 0; i < xParams.size(); i++) {
      if(pGate->inGate(xParams[i].second)) {
	vector<pair<UInt_t, Float_t> > xparameterlist;
	vector<pair<UInt_t, Float_t> > yparameterlist;
	CutDownParameters(xparameterlist, xParams, xParams[i].first);
	CutDownParameters(yparameterlist, yParams, xParams[i].first);
	if (xparameterlist.size() && yparameterlist.size()) {
	  pSpectrum->Increment(xparameterlist,yparameterlist);
	}
      }
    }
    for (int i = 0; i < yParams.size(); i++) {
      if (pGate->inGate(yParams[i].second)) {
	vector<pair<UInt_t, Float_t> > xparameterlist;
	vector<pair<UInt_t, Float_t> > yparameterlist;
	CutDownParameters(xparameterlist, xParams, xParams[i].first);
	CutDownParameters(yparameterlist, yParams, xParams[i].first);
	if (xparameterlist.size() && yparameterlist.size()) {
	  pSpectrum->Increment(xparameterlist,yparameterlist);
	}
      }
    }
  }

  // Band gate (2-d) - there may be some inheritance games we can play to 
  // collapse these two code branches.
  //

  else if(gateType == "gb") {
    CGammaBand* pBand = static_cast<CGammaBand*>(m_pGate->getGate());
    for (int i =0; i < xParams.size(); i++) {
      for(int j = 0; j < yParams.size(); j++) {
	UInt_t   ipar    = xParams[i].first;
	Float_t  iparval = xParams[i].second;

	UInt_t   jpar    = yParams[j].first;
	Float_t  jparval = yParams[j].second;

	if(pBand->Inside(iparval, jparval)) {
	  vector<pair<UInt_t, Float_t> > xparameterlist;
	  vector<pair<UInt_t, Float_t> > yparameterlist;
	  CutDownParameters(xparameterlist, xParams, ipar);
	  CutDownParameters(yparameterlist, yParams, jpar);
	  pSpectrum->Increment(xparameterlist, yparameterlist);
	}
      }
    }

  // Contour gate (2-d)

  } 
  else if(gateType == "gc") {
    CGammaContour* pContour = static_cast<CGammaContour*>(m_pGate->getGate());
    for (int i =0; i < xParams.size(); i++) {
      for(int j = 0; j < yParams.size(); j++) {
	UInt_t   ipar    = xParams[i].first;
	Float_t  iparval = xParams[i].second;
	
	UInt_t   jpar    = yParams[j].first;
	Float_t  jparval = yParams[j].second;
	
	if(pContour->Inside(iparval, jparval)) {
	  vector<pair<UInt_t, Float_t> > xparameterlist;
	  vector<pair<UInt_t, Float_t> > yparameterlist;
	  CutDownParameters(xparameterlist, xParams, ipar);
	  CutDownParameters(yparameterlist, yParams, jpar);
	  pSpectrum->Increment(xparameterlist, yparameterlist);
	}
      }
    }
    // No gate or not a fold-worthy gate.
  }
  else {
    pSpectrum->Increment(xParams, yParams);
  }
    
}

/**
 * Cut down a parameter list by removing a single parmater by id.
 * @param rNew    The new parameter list we are creating.
 * 
 * @param rOld    The original vector of parameters.
 * @param removeMe    The id of the parameter to remove.
 * 
 */
void 
CFold::CutDownParameters(vector<pair<UInt_t, Float_t> >& rNew, 
			   vector<pair<UInt_t, Float_t> >& rOld, 
			   UInt_t removeMe)
{
  
  for(int i =0; i < rOld.size(); i++) {
    if (rOld[i].first  != removeMe) {
      rNew.push_back(rOld[i]);
    }
  }
}

/**
 * Cut down a parameter list by a pair of parmaeter ids.
 * I know, we could compose two cutdowns, but we are in the
 * innermost innermost loop so this is going to perform better.
 * @param rNew    Resulting parameter list.
 * @param rOld    The list to cut down.
 * @param remove1    First parameter id to remove.
 * @param remove2    Second id to remove.
 * 
 */
void 
CFold::CutDownParameters(vector<pair<UInt_t, Float_t> >& rNew, 
			vector<pair<UInt_t, Float_t> > & rOld, 
			UInt_t remove1, UInt_t remove2)
{
  
  for(int i =0; i < rOld.size(); i++) {
    UInt_t id = rOld[i].first;
    if ((id != remove1) && (id != remove2)) {
      rNew.push_back(rOld[i]);
    }
    
    
    
  }
}


