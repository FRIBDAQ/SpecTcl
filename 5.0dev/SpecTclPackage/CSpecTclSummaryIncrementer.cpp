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
#include "CSpecTclSummaryIncrementer.h"
#include "CAxis.h"
#include "CSpectrum.h"

/**
 * @file CSpecTclSummaryIncrementer.cpp
 * @brief Implement the incrementer for summary spectra.
 */

/**
 * constructor
 *   -  Fill in m_Names
 *   -  Try to fill in m_Params
 *   -  If all m_Params got filled in , set m_bound true
 *
 * @param paramNames - Names of the parameters in the summary spectrum.
 */
CSpecTclSummaryIncrementer::CSpecTclSummaryIncrementer(std::vector<std::string> paramNames) :
  m_Names(paramNames), m_bound(false)
{
  validate();
}
/**
 * destructor
 */
CSpecTclSummaryIncrementer::~CSpecTclSummaryIncrementer()
{}
/**
 * vaidate
 *   Determines if the names required to make the spectrum increment are all
 *   defined parameters.
 *
 * @param spec  the spectrum we are associated with.
 */
void
CSpecTclSummaryIncrementer::validate(CSpectrum& spec)
{
  validate();
}
/**
 * operator()
 *    Handle spectrum increment on an event.
 *
 * @param spec - Reference to the spectrum we are managing.
 */
void
CSpecTclSummaryIncrementer::operator()(CSpectrum& spec)
{
  if (m_bound) {
    CAxis*    yAxis = spec.getAxis(1);
    unsigned  yBins = yAxis->channels();
    
    CAxis*    xAxis = spec.getAxis(0);
    unsigned  xBins = xAxis->channels();
    
    uint32_t* pData = reinterpret_cast<uint32_t*>(spec.getHandle());
    
    for (int i = 0; i < m_Params.size(); i++) {
      if (m_Params[i]->s_value.isvalid()) {
	double x = m_Params[i]->s_value;
	unsigned xChan = yAxis->toChannels(x);
	if (xChan < yBins) {
	  pData[i + xBins*xChan]++;
	}
      }
    }
  }
}

/*-----------------------------------------------------------------------
** Private utilities:
*/

/**
 * validate
 *   Overload that does validation both for construction and the public
 *   validate.
 */
void
CSpecTclSummaryIncrementer::validate()
{
  m_bound = true;
  for (int i = 0; i < m_Names.size(); i++) {
    CParameterDictionary::pParameterInfo pInfo = getParameterInfo(m_Names[i]);
    if (pInfo) {
      m_Params.push_back(pInfo);
    } else{
      m_bound = false;
      break;
    }
  }
}
