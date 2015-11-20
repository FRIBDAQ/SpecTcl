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

/**
 * @file CSpecTclCombo2dIncrementer.cpp
 * @brief Implements the incrementer behind the 2d combo gamma
 *        spectrum.
 */
#include "CSpecTclCombo2dIncrementer.h"
#include "CSpectrum.h"
#include "CAxis.h"

/**
 * constructor
 *   - Initialize the internal data
 *   - Determine if all of the parameters are defined, filling in the
 *     sets as well and setting m_fullyBound if appropriate.
 */
CSpecTclCombo2dIncrementer::CSpecTclCombo2dIncrementer(std::vector<std::string> xParams, 
						       std::vector<std::string> yParams) :
  m_xNames(xParams), m_yNames(yParams), m_fullyBound(false)
{
  validate();
}
/**
 * destructor
 */
CSpecTclCombo2dIncrementer::~CSpecTclCombo2dIncrementer()
{}

/**
 * validate
 *
 *  - Populate the m_xParams and m_yParams in accordance with the
 *    set of defined parameters in the m_xNames and m_yNames vectors.
 *  - If all x and y parameters are defined, sets m_fullyBound.
 *
 * @param spec - The  spectrum that is using us.
 */
void 
CSpecTclCombo2dIncrementer::validate(CSpectrum& spec)
{
  validate();
}
/**
 * operator()
 *
 *   Process an event.  
 *   - Fold the parameters if needed.
 *   - Increment for each pair of x/y parameters.
 *
 * @param spec - Reference to the spectrum that is using us.
 */
void
CSpecTclCombo2dIncrementer::operator()(CSpectrum& spec)
{
  if (m_fullyBound) {
    uint32_t* pData = reinterpret_cast<uint32_t*>(spec.getHandle());
    CAxis*    pX    = spec.getAxis(0);
    CAxis*    pY    = spec.getAxis(1);
    unsigned  xmax  = pX->channels();
    unsigned  ymax  = pY->channels();
    
    std::set<CParameterDictionary::pParameterInfo>::iterator x;
    std::set<CParameterDictionary::pParameterInfo>::iterator y;
    
    for (y = m_yParams.begin(); y != m_yParams.end(); y++) {
      if ((*y)->s_value.isvalid()) {
	unsigned Y = pY->toChannels((*y)->s_value);
	if (Y < ymax) {
	  for (x = m_xParams.begin(); x != m_xParams.end(); x++) {
	    if ((*x)->s_value.isvalid()) {
	      unsigned X = pX->toChannels((*x)->s_value);
	      if (X < xmax) {
		pData[X + Y*xmax]++;
	      }
	    }
	  }
	}
      }
    }
  }
}

/*------------------------------------------------------------------------------
**  Utility methods:
*/

/**
 * validate
 *  One of many overloads.  This version returns a set containing the
 *  pointers to parameter info for names that are defined.
 *
 * @param names - vector of parameter names.
 * 
 * @return std::set<CParameterDictionary::pParameterInfo> - set of pointers.
 */
std::set<CParameterDictionary::pParameterInfo> 
CSpecTclCombo2dIncrementer::validate(std::vector<std::string> names)
{
  CParameterDictionary* pDict = CParameterDictionary::instance();
  std::set<CParameterDictionary::pParameterInfo> result;
  for(int i = 0; i < names.size(); i++) {
    CParameterDictionary::pParameterInfo p = getParameterInfo(names[i]);
    if (p) {
      result.insert(p);
    }

  }

  return result;
}
/**
 * validate
 *    sets the m_xParams, m_yParams and m_fullyBound from the
 *    parameters in  m_xNames, m_yNames
 */
void
CSpecTclCombo2dIncrementer::validate()
{
  m_xParams = validate(m_xNames);
  m_yParams = validate(m_yNames);

  m_fullyBound = 
    (m_xParams.size() == m_xNames.size()) && (m_yParams.size() == m_yNames.size());  
}
