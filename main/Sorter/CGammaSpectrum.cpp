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
//  CGammaSpectrum.cpp
//  Implementation of the Class CGammaSpectrum
//  Created on:      22-Apr-2005 12:55:02 PM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////
#include <config.h>

#include "CGammaSpectrum.h"
#include <assert.h>

#include "GateContainer.h"
#include "CFold.h"
#include "Event.h"
#include "Parameter.h"

#include <histotypes.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/**
 * Destructor.  Must destroy the fold
 */
CGammaSpectrum::~CGammaSpectrum()
{
  delete m_pFold;
}

/**
 * Construct a gamma spectrum base class. 
 *   - The CSpectrum is constructed as directed by the parameter list.
 *   - The fold is defaulted to a fold containing a true gate.
 *   - The parameter vector is initialized from the Parameters argument.
 * @param rName
 *          Name of the spectrum to create.
 * @param nId
 *          Id of the spectrum to create.
 * @param Maps
 *         Set of maps of parameter/axis transformations.
 * @param Parameters
 *         Set of parameters on which the spectrum is defined
 * @param pGate = pDefaultGate
 *         THe gate to initially set on the spectrum.  Defaults to a T gate.
 *
 *
 */
CGammaSpectrum::CGammaSpectrum(const std::string& rName, UInt_t nId,  
			       CSpectrum::Axes Maps,
			       std::vector<CParameter>& Parameters,
			       CGateContainer* pGate) :
  CSpectrum(rName, nId, Maps, pGate),
  m_pFold(new CFold(pDefaultGate))
{
  CreateParameterVector(Parameters);

}
/** * Construct a gamma spectrum base class when no coordinate transforms are
 * specified for the axes:
 * - The CSpectrum is constructed as directe by most of the parameters.
 * - The fold is defaulted to a fold containing a true gate.
 * - The parameter vector is initialized from the Parameters argument.
 *
 * @param rName
 *     The name of the new spectrum.
 * @param nId
 *     The id of the spectrum (not really important in SpecTcl).
 * @param Parameters
 *     The list of parameters on which the spectrum will be incremented.
 * @param pGate
 *     The initial gate on the spectrum, defaults to a true gate.
 *
 */
CGammaSpectrum::CGammaSpectrum(const std::string& rName, UInt_t nId, 
			       std::vector<CParameter>& Parameters,
			       CGateContainer* pGate) :
  CSpectrum(rName, nId, pGate),
  m_pFold(new CFold(pDefaultGate))
{
  CreateParameterVector(Parameters);
}

/*!
  Constructor used by a 2d gamama deluxe spectrum. This spectrum has
  independent x/y parameters.
 * Construct a gamma spectrum base class. 
 *   - The CSpectrum is constructed as directed by the parameter list.
 *   - The fold is defaulted to a fold containing a true gate.
 *   - The parameter vectors are both initialized from the x/y parameters.
 * @param rName
 *          Name of the spectrum to create.
 * @param nId
 *          Id of the spectrum to create.
 * @param Maps
 *         Set of maps of parameter/axis transformations.
 * @param xParameters
 *         Set of parameters on the X axis.
 * @param yParameters
 *         Set of parameters on the Y axis.
 * @param pGate = pDefaultGate
 *         THe gate to initially set on the spectrum.  Defaults to a T gate.
 *
 *
 */
CGammaSpectrum::CGammaSpectrum(const std::string& rName, UInt_t nId,  CSpectrum::Axes Maps,
			       std::vector<CParameter>& xParameters,
			       std::vector<CParameter>& yParameters,
			       CGateContainer* pGate) :
  CSpectrum(rName, nId, Maps, pGate),
  m_pFold(new CFold(pDefaultGate))
{
  CreateParameterVector(xParameters);
  CreateYParameterVector(yParameters);
}

/** * Construct a 2d  gamma deluxe spectrum base class when no coordinate transforms are
 * specified for the axes:
 * - The CSpectrum is constructed as directe by most of the parameters.
 * - The fold is defaulted to a fold containing a true gate.
 * - both  parameter vectors are initialized from the x/y Parameters arguments.
 *
 * @param rName
 *     The name of the new spectrum.
 * @param nId
 *     The id of the spectrum (not really important in SpecTcl).
 * @param xParameters
 *     The list of parameters on the x axis.
 * @param yParameters
 *     The list of parameters on the y axis.
 * @param pGate
 *     The initial gate on the spectrum, defaults to a true gate.
 *
 */
CGammaSpectrum::CGammaSpectrum(const std::string& rName, UInt_t nId,
		 std::vector<CParameter>& xParameters,
		 std::vector<CParameter>& yParameters,
			       CGateContainer* pGate) :
  CSpectrum(rName, nId, pGate),
  m_pFold(new CFold(pDefaultGate))
{
  CreateParameterVector(xParameters);
  CreateYParameterVector(yParameters);
}

/**
 * Increment called by sorter (or rather the base class spectrum) Takes a CEvent
 * as a parameter.
 * @param rEvent
 *    Reference to the current event.
 * 
 */
void 
CGammaSpectrum::Increment(const CEvent& rEvent)
{

  assert(m_pFold);                  // There must always be a fold.
  
  vector<pair<UInt_t, Float_t> >   parameterList;
  CreateParameterList(parameterList, rEvent);

  // Need to handle the deluxe guy differently:

  if (getSpectrumType() == keG2DD) {
    vector<pair<UInt_t, Float_t> > yParameterList;
    CreateYParameterList(yParameterList, rEvent);
    (*m_pFold)(parameterList, yParameterList, this);
  }
  else {
    (*m_pFold)(parameterList, this); // The fold will take care of  incrementing us.
  }
}

/**
 * True if the gamma spectrum has a fold.
 * This will be the case if there is a non-null fold pointer and the fold it
 * points to has a 'suitable gate'... tested by asking the fold to return it's name.  Note
 * that there should never be a null fold pointer as we'll construct with a fold pointer to
 * True.
 */
Bool_t 
CGammaSpectrum::haveFold()
{
  
  if(m_pFold) {
    if (m_pFold->getFoldName() != "") {
      return kfTRUE;
    }
  }
  return kfFALSE;
  
  
}


/**
 * Adds/replaces a fold for the spectrum.
 * @param pGate
 *    A pointer to the gate container that will be used as the new
 *    fold for this spectrum.
 * 
 */
void 
CGammaSpectrum::Fold(CGateContainer* pGate)
{
  
  if(!m_pFold) {
    m_pFold = new CFold(pGate);
  }
  m_pFold->setGate(pGate);
  
  
}


/**
 * Return the gamma spectrum's current fold.
 */
CFold* 
CGammaSpectrum::getFold()
{
  
  if(haveFold()) {
    return m_pFold;
  }
  else {
    return (CFold*)kpNULL;
  }
  
  
}





/**
 * Take an event and produce a parameter list.
 * @param outList
 *    The output list to create.
 * @param rEvent
 *    The event to create it from.
 * 
 */
void 
CGammaSpectrum::CreateParameterList(vector<pair<UInt_t, Float_t> >& outList, 
				    const CEvent& rrEvent)
{
  
  CEvent& rEvent((CEvent&)rrEvent);
  int nSize = rEvent.size();
  int pSize = m_Parameters.size();
  for(int i = 0; i < pSize; i++) {
    UInt_t pnum = m_Parameters[i];
    if (pnum < nSize) {
      if (rEvent[pnum].isValid()) {
	outList.push_back(pair<UInt_t,Float_t>(pnum,(Float_t)(rEvent[pnum])));

      }
    }
  }
}
//  Same but for the y parameters.

void
CGammaSpectrum::CreateYParameterList(vector<pair<UInt_t, Float_t> >& outList,
				     const CEvent& rrEvent)
{
  CEvent& rEvent((CEvent&)rrEvent);         // Lose the const.
  int nSize = rEvent.size();
  int pSize = m_yParameters.size();
  for (int i = 0; i < pSize; i++) {
    UInt_t pnum = m_yParameters[i];
    if (pnum < nSize) {
      if (rEvent[pnum].isValid()) {
	outList.push_back(pair<UInt_t, Float_t>(pnum, (Float_t)(rEvent[pnum])));
      }
    }
  }
}

//
void
CGammaSpectrum::CreateParameterVector(vector<CParameter>& Parameters) 
{
  for (int i =0; i < Parameters.size(); i++) {
    m_Parameters.push_back(Parameters[i].getNumber());
  }
}
// Same for the Y parameters:

void
CGammaSpectrum::CreateYParameterVector(vector<CParameter>& Parameters)
{
  for (int i=0; i < Parameters.size(); i++) {
    m_yParameters.push_back(Parameters[i].getNumber());
  }
}

/*!
   Returna list of the parameter id's in use by this spectrum.
   - May not be uniquified.
*/
void
CGammaSpectrum::GetParameterIds(std::vector<UInt_t>& rvIds)
{
  for (UInt_t i = 0; i < m_Parameters.size(); i++)
    rvIds.push_back(m_Parameters[i]);
  for (UInt_t i = 0; i < m_yParameters.size(); i++) {
    rvIds.push_back(m_yParameters[i]);
  }
}


/*!
   Return true if the specified parameter is used by the spectrum.
*/

Bool_t
CGammaSpectrum::UsesParameter(UInt_t nId) const
{
  for (UInt_t i = 0; i < m_Parameters.size(); i++) {
    if (m_Parameters[i] == nId)
      return kfTRUE;
  }
  for (UInt_t i = 0; i < m_yParameters.size(); i++) {
    if (m_yParameters[i] == nId)
      return kfTRUE;
  }
  return kfFALSE;
}
/*!
  Gamma spectra don't actually need a specific parameter.
*/
Bool_t
CGammaSpectrum::needParameter() const
{
  return kfFALSE;
}
