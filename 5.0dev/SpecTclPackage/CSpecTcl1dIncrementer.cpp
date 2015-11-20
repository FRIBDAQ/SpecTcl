/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2013.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#include "CSpecTcl1dIncrementer.h"
#include "CSpectrum.h"
#include "CParameter.h"
#include "CParameterDictionary.h"
#include "CAxis.h"

#include <string.h>
#include <stdexcept>
#include <sstream>

/**
 * @file CSpecTcl1dIncrementer.cpp
 * @brief Implements the spectrum incrementer for 'just a chunk of memory' spectra
 */


/**
 * constructor
 *   Create the incrementer. The spectrum is not connected statically
 *   but is passed into action methods.  The constructor is just intended
 *   to make an intial connection to the parameter.
 *
 * @param pName - Name of the parameter to histogram.
 *
 * @note - no policy decision is made about handling parameters that don't
 *         exist.  In that case we're just going to create a spectrum with
 *         no parameter binding.  Any error conditions will need to be 
 *         flagged by our client rather than us.
 */
CSpecTcl1dIncrementer::CSpecTcl1dIncrementer(std::string pName) :
  m_parameterName(pName), m_parameter(0)
{
  validate();
}
/**
 * destructor.
 */
CSpecTcl1dIncrementer::~CSpecTcl1dIncrementer()
{}

/**
 * validate
 *
 *   Looks up our parameter.  If we find it, m_parameter is set to point to 
 *   it's information, otherise it's set to 0 indicating we are unbound.
 *
 * @param spec  - (unused) reference to the spectrum we are part of.
 */
void
CSpecTcl1dIncrementer::validate(CSpectrum& spec)
{
  validate();
}


/**
 * operator()
 *
 *   Called to process an event.  At this point, any condition
 *   on the spectrum has already been verified to be made
 *
 * @param spec - the spectrum we are a component of.
 */
void
CSpecTcl1dIncrementer::operator()(CSpectrum& spec)
{
  if (m_parameter) {		    // Ensure we're bound.
    CAxis* pAxis = spec.getAxis(0); // x axis should be the only one.
    double value = m_parameter->s_value;
    int    chno  = pAxis->toChannels(value);
    uint32_t* pS = reinterpret_cast<uint32_t*>(spec.getHandle());
    if ((chno >= 0) && (chno < pAxis->channels())) {
      pS[chno]++;
    }
  }
}

/*---------------------------------------------------------------------
 * Private utilities:
 */

/*
 * validate (overload)
 *
 *  Does the work of validation. 
 *  Looks up our parameter.  If we find it, m_parameter is set to point to 
 *  it's information, otherise it's set to 0 indicating we are unbound.
 *  This method allows us to factor out validations between the public
 *  validate and the constructor which does not get a spectrum to work with.
 */
void
CSpecTcl1dIncrementer::validate()
{
  CParameterDictionary* pDict = CParameterDictionary::instance();
  CParameterDictionary::DictionaryIterator pIter = pDict->find(m_parameterName);

  if (pIter != pDict->end()) {
    m_parameter = pIter->second;
  } else {
    m_parameter = 0;
  }
}
