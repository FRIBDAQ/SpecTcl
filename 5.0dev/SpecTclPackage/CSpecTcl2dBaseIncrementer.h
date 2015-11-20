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

#ifndef _CSPECTCL2DBASEINCRMENTER_H
#define _CSPECTCL2DBASEINCRMENTER_H

#ifndef _CSPECTRUMINCREMENTER_H
#include "CSpectrumIncrementer.h" /* base class */
#endif

/*
 * @file CSpecTcl2dBaseIncrementer.h
 * @brief Common base class for 2d SpecTcl spectra.
 */

/**
 * @class CSpecTcl2dBaseIncrementer
 *
 * Provides shared functionality for all 2-d 
 * SpecTcl Spectra.
 */
#ifndef _CPARAMETERDITIONARY_h
#include "CParameterDictionary.h"
#endif

#ifndef _STD_STRING
#include <string>
#ifndef _STD_STRING
#define _STD_STRING
#endif
#endif

class CSpectrum;

/**
 * @class CSpecTcl2dBaseIncrementer
 *
 *   Class that offers common services for 2d spectra.
 */
class CSpecTcl2dBaseIncrementer : public CSpectrumIncrementer
{
public:
  virtual void clear(CSpectrum& spec);
  virtual uint32_t get(CSpectrum& spec, unsigned x, unsigned y);
  virtual void set(CSpectrum& spec, unsigned x, unsigned y, uint32_t value);

protected:
  CParameterDictionary::ParameterInfo* getParameterInfo(std::string name);
  uint32_t* pChannel(CSpectrum& spec, unsigned x, unsigned y);

  
};
#endif
