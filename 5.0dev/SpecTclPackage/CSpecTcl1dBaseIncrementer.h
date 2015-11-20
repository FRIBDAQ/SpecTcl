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
#ifndef _CSPECTCL1DBASEINCREMENTER_H
#define _CSPECTCL1DBASEINCREMENTER_H

/**
 * @file CSpecTcl1dBaseIncrementer.h
 * @brief Defines a common services base class for SpecTcl 1d incrementers.
 */

#ifndef _CSPECTRUMINCREMENTER_H
#include "CSpectrumIncrementer.h"
#endif

class CSpectrum;


/**
 * @class CSpecTcl1dBaseIncrementer
 *
 *  This class provides common services used by all or at least many
 *  SpecTcl 1d spectrum types.
 */
class CSpecTcl1dBaseIncrementer : public CSpectrumIncrementer
{
public:
  virtual uint32_t get(CSpectrum& spec, unsigned x, unsigned y);
  virtual void set(CSpectrum& spec, unsigned x, unsigned y, uint32_t value);
  virtual void clear(CSpectrum& spec);
protected:
  uint32_t* pChannel(CSpectrum& spec, unsigned x);

};

#endif
