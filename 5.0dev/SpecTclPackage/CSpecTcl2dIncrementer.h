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


#ifndef _CSPECTCL2DINCREMENTER_H
#define _CSPECTCL2DINCREMENTER_H


/**
 * @file CSpecTcl2dIncrementer.h
 * @brief Incrementer for 2d plain memory block spectra.
 */

#ifndef _CSPECTCL2DBASEINCREMENTER_H
#include "CSpecTcl2dBaseIncrementer.h"
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
 * @class CSpecTcl2dIncrementer
 *
 *   This class is an incrementer that understands
 *   how to access ordinary 2d spectra that are represented
 *   as simple blocks of memory that are organized as a 2-d 
 *   x-axis major array of uint32_t cells.
 */
class CSpecTcl2dIncrementer : public CSpecTcl2dBaseIncrementer 
{

private:
  std::string                          m_xParameterName; // The X parameter being histogrammed.
  CParameterDictionary::ParameterInfo* m_xParameter;     // info/value. for x.

  std::string                          m_yParameterName; // The Y parameter being histogrammed
  CParameterDictionary::ParameterInfo* m_yParameter;     // info/value for y

  bool m_bound;			// True if both m_xParameter and m_yParameter are non-null.

  // canonicals:

public:
  CSpecTcl2dIncrementer(std::string xName, std::string yName);
  virtual ~CSpecTcl2dIncrementer();

  // implementation of the base class methods:

public:

  virtual void validate(CSpectrum& spec);
  virtual void operator()(CSpectrum& spec);

private:
  void validate();

 
};

#endif
