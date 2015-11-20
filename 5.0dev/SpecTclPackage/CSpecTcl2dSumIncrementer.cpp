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

#include "CSpecTcl2dSumIncrementer.h"


/**
 * @file CSpecTcl2dSumIncrementer.cpp
 * @brief Implement the incrementer for 2d sum spectra.
 */


#include "CSpectrum.h"
#include "CAxis.h"
#include "CParameterDictionary.h"

#include <stdexcept>


/**
 * Constructor
 *  - Fill in the name arrays.
 *  - From them fill in the parameter arrays.
 *  - If the parameter arrays contain all parameters set m_bound true.
 *
 * @param xNames - names of the x parameters.
 * @param yNames - Names of the y parametrers
 *
 * @throw std::length_error - the lengths of the xNames and yNames
 *                            parameters don't match.
 */
CSpecTcl2dSumIncrementer::CSpecTcl2dSumIncrementer(
 std::vector<std::string> xNames, std::vector<std::string> yNames ) :
  m_xNames(xNames), m_yNames(yNames), m_bound(false)
{
  if (m_xNames.size() != m_yNames.size()) {
    throw std::length_error("CSpecTcl2dSumIncrementer: Must be same number of x and y parameters");
  } else {
    validate();
  }
}
/**
 * Destructor
 */
CSpecTcl2dSumIncrementer::~CSpecTcl2dSumIncrementer()
{
}

  
/**
 * validate
 *
 *   Fill in the m_xParams, m_yParams and the m_bound members.
 *
 * @param spec - References the spectrum we are being used by.
 */
void
CSpecTcl2dSumIncrementer::validate(CSpectrum& spec)
{
  validate();
}
/**
 * operator()
 *    Increment the spectrum based on the current event.
 *
 * @param spec - Reference to the spectrum.
 */
void
CSpecTcl2dSumIncrementer::operator()(CSpectrum& spec)
{
  if (m_bound) {
    uint32_t* pData = reinterpret_cast<uint32_t*>(spec.getHandle());
    CAxis&    xAxis(*(spec.getAxis(0)));
    CAxis&    yAxis(*(spec.getAxis(1)));

    unsigned xc = xAxis.channels();
    unsigned yc = yAxis.channels();

    for (int i =0; i < m_xParams.size(); i++) {
      if (m_xParams[i]->s_value.isvalid() && m_yParams[i]->s_value.isvalid()) {
	double x = m_xParams[i]->s_value;
	double y = m_yParams[i]->s_value;

	int ix = xAxis.toChannels(x);
	int iy = yAxis.toChannels(y);

	if ((ix >= 0) && (iy >= 0) && (ix < xc) && (iy < yc)) {
	  pData[ix + iy*xc]++;
	}
      }
    }
  }
}

/*---------------------------------------------------------------------------------
 *
 * Private utilitye methods.
 */

/**
 * validate
 *
 *   Common validation code between the public validate and the constructor.
 */
void 
CSpecTcl2dSumIncrementer::validate()
{
  m_xParams.clear();
  m_yParams.clear();
  m_bound = false;
  for (int i =0; i < m_xNames.size(); i++) {
    CParameterDictionary::pParameterInfo p = getParameterInfo(m_xNames[i]);
    if (!p) return;
    m_xParams.push_back(p);

    p = getParameterInfo(m_yNames[i]);
    if (!p) return;
    m_yParams.push_back(p);
  }
  m_bound = true;
  
}
