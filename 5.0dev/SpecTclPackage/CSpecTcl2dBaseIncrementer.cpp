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
#include "CSpecTcl2dBaseIncrementer.h"
#include "CSpectrum.h"
#include "CAxis.h"

#include <string.h>

/**
 * clear
 *
 *   Cleare the spectrum.
 *
 * @param spec - reference to the spectrum of which we are a component.
 */
void
CSpecTcl2dBaseIncrementer::clear(CSpectrum& spec)
{
  CAxis* x = spec.getAxis(0);
  CAxis* y = spec.getAxis(1);

  size_t nBytes = x->channels() * y->channels() * sizeof(uint32_t);
  memset(spec.getHandle(), 0, nBytes);
}

/**
 * set
 *    Set a channel to a specified value:
 *
 * @param spec - Reference to the spectrum object in which we live.
 * @param x    - X channel coordinate.
 * @param y    - Y channel coordinate.
 * @param value - Value to set channel x,y.
 */
void
CSpecTcl2dBaseIncrementer::set(CSpectrum& spec, unsigned x, unsigned y, uint32_t value)
{

  uint32_t* p = pChannel(spec, x, y);
  *p = value;

}
/**
 * get
 *   Return the contents of a channel:
 *
 * @param spec - the spectrum using this.
 * @param x    - Xcoordinate of the spectrum.
 * @param y    - Y channel of the spectrum.
 *
 * @return uint32_t the value at the selected channel.
 */
uint32_t
CSpecTcl2dBaseIncrementer::get(CSpectrum& spec, unsigned x, unsigned y)
{
  uint32_t* p = pChannel(spec, x, y);

  return *p;
}

/**
 * getParameterInfo
 *
 *  Return the parameter info pointer for a parameter given its name.
 *
 * @param name - Name of the parameter.
 *
 * @return CParameterDictionary::ParameterInfo* - Pointer to the info or
 *                                                NULL if not defined.
 */
CParameterDictionary::ParameterInfo*
CSpecTcl2dBaseIncrementer::getParameterInfo(std::string name)
{
  CParameterDictionary* pDict = CParameterDictionary::instance();
  CParameterDictionary::DictionaryIterator p = pDict->find(name);
  if (p != pDict->end()) {
    return p->second;
  }
  return 0;
}

/**
 * pChannel
 *
 *   Generate a pointer to the specified channel.
 *
 * @param spec - reference to the spectrum.
 * @param x    - X channel number.
 * @param y    - Y channel number.
 *
 * @return uint32_t*
 */
uint32_t*
CSpecTcl2dBaseIncrementer::pChannel(CSpectrum& spec, unsigned x, unsigned y)
{
  CAxis* xAxis = spec.getAxis(0);
  CAxis* yAxis = spec.getAxis(1);
  
  unsigned nx = xAxis->channels();
  unsigned ny = yAxis->channels();

  if (x > nx) {
    badChannel(x, nx);
  }
  if (y > ny) {
    badChannel(y, ny);
  }

  uint32_t* p = reinterpret_cast<uint32_t*>(spec.getHandle());
  return p + (x + y*nx);
}
