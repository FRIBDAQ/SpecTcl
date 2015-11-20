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
#include "CSpecTcl2dIncrementer.h"
#include "CSpectrum.h"
#include "CAxis.h"
#include "CParameterDictionary.h"


#include <string.h>
#include <stdexcept>
#include <sstream>

/**
 * constructor.
 *
 *   - Initialize the data.
 *   - If possible bind the parameters and
 *   - if both bind, set m_bound true.
 *
 * @param xName - name of the x parameter.
 * @param yName - name of the y parameter.
 */
CSpecTcl2dIncrementer::CSpecTcl2dIncrementer(std::string xName, std::string yName) :
  m_xParameterName(xName), m_yParameterName(yName), m_xParameter(0), m_yParameter(0),
  m_bound(false)
{
  // Try to bind the parameters:

  validate();
}
/**
 * destructor
 */
CSpecTcl2dIncrementer::~CSpecTcl2dIncrementer() {}

/**
 * validate
 *
 * Determine if the spectrum can be incremented by checking for the
 * existence of all the parameters needed for the increment.
 *
 * @param spec - reference to the spectrum this is a component of.
 */
void
CSpecTcl2dIncrementer::validate(CSpectrum& spec)
{
  validate();
}

/**
 * operator()
 *
 *   Process an event.
 *   - Must be bound.
 *   - Both parameters must be valid.
 *   - Both parameters must be in range.
 *   - The index is calculated and that cell incremented.
 *
 * @param spec - Reference to our containing spectrum.
 */
void
CSpecTcl2dIncrementer::operator()(CSpectrum& spec)
{
  if (m_bound && 
      m_yParameter->s_value.isvalid() && m_xParameter->s_value.isvalid()) {
    CAxis* xAxis = spec.getAxis(0);
    CAxis* yAxis = spec.getAxis(1);
    
    double x = m_xParameter->s_value;
    double y = m_yParameter->s_value;
    
    unsigned xch = xAxis->toChannels(x);
    unsigned ych = yAxis->toChannels(y);
    
    unsigned offset = xch + ych * xAxis->channels();
    
    uint32_t* p = reinterpret_cast<uint32_t*>(spec.getHandle());
    
    p[offset]++;
  }

}

/*----------------------------------------------------------------
 *  Private utilities.
 */ 

/**
 * validate
 *   Overload that acutally does the validation.
 */
void
CSpecTcl2dIncrementer::validate()
{
  m_xParameter = getParameterInfo(m_xParameterName);
  m_yParameter = getParameterInfo(m_yParameterName);
  m_bound      = m_xParameter && m_yParameter;
}
