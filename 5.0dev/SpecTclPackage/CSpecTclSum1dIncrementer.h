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

#ifndef _CSPECTCLSUM1DINCREMENTER_H
#define _CSPECTCLSUM1DINCREMENTER_H

/**
 * @file CSpecTclSum1dIncrementer.h
 * @brief Definition of 1d sum spectrum incrementer on memory block spectra.
 */
#ifndef _CSPECTCL1DBASEINCREMENTER_H
#include "CSpecTcl1dBaseIncrementer.h"
#endif


#ifndef _STD_VECTOR
#include <vector>
#ifndef _STD_VECTOR
#define _STD_VECTOR
#endif
#endif

#ifndef _STD_STRING
#include <string>
#ifndef _STD_STRING
#define _STD_STRING
#endif
#endif

#ifndef _CPARAMETERDICTIONARY_H
#include "CParameterDictionary.h"
#endif

/**
 * @class CSpecTclSum1dIncrementer
 *
 *    Understands how to increment 1d sum spectra.
 *    1d sum spectra have multiple parameters associated with them.
 *    each parameter, if defined for an event, will increment the spectrum.
 *    Thus Sum1D spectra would look like a spectrum that is the sum of
 *    the individual spectra of all parameters on which it is defined.
 */
class CSpecTclSum1dIncrementer : public CSpecTcl1dBaseIncrementer
{
private:
  std::vector<std::string>                          m_parameterNames;
  std::vector<CParameterDictionary::pParameterInfo> m_parameters;
  bool                                              m_completelyBound;
  unsigned                                          m_overflows; 
  unsigned                                          m_underflows;

  // Canonicals:

public:
  CSpecTclSum1dIncrementer(std::vector<std::string> parameterNames);
  virtual ~CSpecTclSum1dIncrementer();

  // incrementer interface:

public:
  virtual void validate(CSpectrum& spec);
  virtual void operator()(CSpectrum& spec);


  // Private utilities:
private:
  void validate();
};
#endif
