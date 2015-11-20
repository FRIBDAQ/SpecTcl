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

#ifndef _CSPECTCL1DINCREMENTER_H
#define _CSPECTCL1DINCREMENTER_H

/**
 * @file CSpecTcl1dIncrementer.h
 * @brief Definition of spectrum incrementer for plain memory block spectra.
 */

#ifndef _CSPECTCL1DBASEINCREMENTER_H
#include "CSpecTcl1dBaseIncrementer.h"
#endif

#ifndef _CPARAMETERDITIONARY_h
#include "CParameterDictionary.h"
#endif

#ifndef _STD_STRING
#include <string>
#ifndef _STD_STRING
#define _STD_STRING
#endif
#endif

#ifndef _CRT_STDINT_H
#include <stdint.h>
#ifndef _CRT_STDINT_H
#define _CRT_STDINT_H
#endif
#endif

class CSpectrum;

/**
 * @class CSpecTcl1dIncrementer
 *
 *   This is an incrementer in a spectrum that is represented internally
 *   as just a block of memory considered to be a 1-dimensional array
 *   of uint32_t cells.
 */
class CSpecTcl1dIncrementer : public CSpecTcl1dBaseIncrementer
{
private:
  std::string                          m_parameterName;	// The parameter being histogrammed.
  CParameterDictionary::ParameterInfo* m_parameter;     // info/value.

  // Canonicals

public:
  CSpecTcl1dIncrementer(std::string pName);
  virtual ~CSpecTcl1dIncrementer();

  // implementation of the base class methods:

public:

  virtual void validate(CSpectrum& spec);
  virtual void operator()(CSpectrum& spec);

private:
  void validate();
};

#endif
