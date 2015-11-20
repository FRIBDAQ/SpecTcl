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

#include "CSpecTclGammaSummaryIncrementer.h"

/**
 * @file CSpecTclGammaSummaryIncrementer.cpp
 * @brief Implementation of a gamma summary spectrum incrementer for SpecTcl spectra.
 */


#include "CSpectrum.h"
#include "CAxis.h"
#include "CParameterDictionary.h"
#include "CParameter.h"


/**
 * constructor:
 *  - Save the parameter names for each channel.
 *  - Check the parameter names for each channel and use that to fill in
 *  - m_bound and m_params.
 *
 * @param names - Vector of vector of parameter names for each channel.
 *
 */
CSpecTclGammaSummaryIncrementer::CSpecTclGammaSummaryIncrementer(
    CSpecTclGammaSummaryIncrementer::SpectrumParameterNames names) :
  m_names(names)
{
 
  // Bind the parameters and set the m_bound vector accordingly.
  
  validate();
}

/**
 * validate
 *
 *  Called usually after changes to the parameter dictionary.  Re-check
 *  the validity of the parameters in the spectrum channels.
 *
 * @param spectrum - Reference to the spectrum that runs this incrementer.
 *                  (ignored).
 */
void 
CSpecTclGammaSummaryIncrementer::validate(CSpectrum& spec)
{
  validate();
}

/**
 * operator()
 *   Increment the spectrum given the current event.
 *
 * @param spec - the spectrum that we are incrementing.
 */
void
CSpecTclGammaSummaryIncrementer::operator()(CSpectrum& spec)
{
  uint32_t* pChans = reinterpret_cast<uint32_t*>(spec.getHandle());
  CAxis*    yAxis  = spec.getAxis(1);
  uint32_t  yChans = yAxis->channels();
  uint32_t  xChans = m_bound.size();


  for (int x = 0; x < xChans; x++) {
    if (m_bound[x]) {
      for (int i = 0; i < m_params[x].size(); i++) {
	if(m_params[x][i]->s_value.isvalid()) {
	  double y = m_params[x][i]->s_value;
	  uint32_t ych = yAxis->toChannels(y);
	  if (ych < yChans) {
	    // All this to know we can increment:

	    pChans[x + xChans*ych]++;
	  }
	}
      }
    }
  }
}

/*-----------------------------------------------------------------------
**
** Private utilties:
*/

/**
 * validate
 *   Given that m_names is filled in, determine m_bound and m_params
 */
void
CSpecTclGammaSummaryIncrementer::validate()
{
  m_bound.clear();
  m_params.clear();

  m_bound.insert(m_bound.begin(), m_names.size(), true); //  optimistic assumption.

  for (int ch = 0; ch < m_names.size(); ch++) {
    ChannelParameters  chParams;
    
    for (int i = 0 ; i < m_names[ch].size(); i++) {
      CParameterDictionary::ParameterInfo* pInfo = getParameterInfo(m_names[ch][i]);
      if (!pInfo) {
	m_bound[ch] = false;	// Missing parameter.
	break;
      } else {
	chParams.push_back(pInfo);
      }
    }
    m_params.push_back(chParams);
    chParams.clear();		// Not needed I think due to scoping.
  }
}

