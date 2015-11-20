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
#include "CSpecTclSum1dIncrementer.h"

#include "CSpectrum.h"
#include "CAxis.h"

/**
 * @file CSpecTclSum1dIncrementer.cpp
 * @brief Implementation of the 1d sum spectrum incrementer.
 */

/**
 * construtor
 *
 *   - Save the names.
 *   - Bind all the names to parameters that we can.
 *   - If all the names were bound, mark ourselves as completely bound.
 *
 * @param names - Names of the parameters that increment this spectrum.
 */
CSpecTclSum1dIncrementer::CSpecTclSum1dIncrementer(std::vector<std::string> names) :
  m_parameterNames(names),
  m_completelyBound(false),
  m_overflows(0),
  m_underflows(0)
{
  validate();

}

/**
 * destructor
 */
CSpecTclSum1dIncrementer::~CSpecTclSum1dIncrementer() {}

/**
 * validate
 *
 *  Ensure all parameters are defined.  If so, set m_completelyBound to true.
 *  
 * @note it may be wise at some point to permit spectra of this type
 *       to increment for the parameters that _are_ defined rather than require
 *       all parameters to be defined
 */
void 
CSpecTclSum1dIncrementer::validate(CSpectrum& spec)
{
  validate();
}
/**
 * operator()
 *    Process an event.    Sum spectra are multiply incremented, once for each
 *    parameter that has a valid value...but only if the spectrum is
 *    completely bound.
 *
 */
void
CSpecTclSum1dIncrementer::operator()(CSpectrum &spec)
{
  if (m_completelyBound) {
    uint32_t* pChannels = reinterpret_cast<uint32_t*>(spec.getHandle());
    CAxis*    pAxis     = spec.getAxis(0);
    unsigned  bins      = pAxis->channels();
    
    for (int i = 0; i < m_parameters.size(); i++) {
      if (m_parameters[i]->s_value.isvalid()) {
	int chan = pAxis->toChannels(m_parameters[i]->s_value);
	if (chan < 0) {
	  m_underflows++;
	} else if (chan >= bins) {
	  m_overflows++;
	} else {
	  pChannels[chan]++;
	}
      }
    }
  }
}

/*-------------------------------------------------------------------
 * Private utilities.
 */

/**
 * validate
 *
 * Determines if all of the parameters needed for this spectrum exist
 * @note side effects:
 * - m_parameters are set to point to the dictionary entries of existing parameters.
 * - m_completelyBound is true if all parameters exist.
 */

void
CSpecTclSum1dIncrementer::validate()
{
  m_parameters.clear();		// Start over.
  CParameterDictionary* pDict = CParameterDictionary::instance();

  for (int  i = 0; i < m_parameterNames.size(); i++) {
    CParameterDictionary::DictionaryIterator p = pDict->find(m_parameterNames[i]);
    if (p != pDict->end()) {
      m_parameters.push_back(p->second);
    }
  }
  m_completelyBound = (m_parameterNames.size() == m_parameters.size());
}
