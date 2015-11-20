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
 * @file CSpecTcl1dBaseIncrementer.cpp
 * @brief Provide common services for all SpecTcl 1d spectrum incrementers.
 */

#include "CSpecTcl1dBaseIncrementer.h"
#include "CAxis.h"
#include "CSpectrum.h"

#include <string.h>

/**
 * get
 *   Return the contents of a channel.
 *
 * @param spec - reference to the containing spectrum.
 * @param x    - X channel coordinate.
 * @param y    - Y channel coordinate.
 *
 * @return uint32_t - channel value
 * @throws std::out_of_range - if the channel number is bad.
 */
uint32_t
CSpecTcl1dBaseIncrementer::get(CSpectrum& spec, unsigned x, unsigned y)
{
  uint32_t* p = pChannel(spec, x);
  return *p;
}
/**
 * set
 *   Set the contents of a channel of the spectrum to a specific
 *   value.
 *
 * @param spec - reference to the spectrum containing us.
 * @param x    - X channel
 * @param y    - Y channel (ignored)
 * @param value - New value to set in the channel.
 *
 * @throw std::out_of_range if the channel number is bad.
 */
void
CSpecTcl1dBaseIncrementer::set(CSpectrum& spec, unsigned x, unsigned y,  uint32_t value)
{
  uint32_t* p = pChannel(spec, x);
  *p = value;

}
/**
 * clear
 *   clears the spectrum channels:
 *   - Determine the number of storage cells from the axis definitions.
 *   - memset them all to zero.
 *
 * @param spec - reference to the spectrum of which this is a component.
 */
void
CSpecTcl1dBaseIncrementer::clear(CSpectrum& spec)
{
  size_t xdim, ydim;
  CAxis* xAxis = spec.getAxis(0); // must exist.
  CAxis* yAxis = spec.getAxis(1); // could be null.

  xdim = xAxis->channels();
  ydim = 1;
  if (yAxis) {
    ydim = yAxis->channels();
  }

  memset(spec.getHandle(), 0, xdim * ydim * sizeof(uint32_t));

}
/*---------------------------------------------------------
 * Private utiltities:
 */

/**
 * pChannel
 *   Return a pointer to the specified channel, throwing an error if
 *   the channel is out of range.
 *
 * @param spec - reference to the spectrum
 * @param x    - X Channel number.
 *
 * @return uint32_t*
 */
uint32_t*
CSpecTcl1dBaseIncrementer::pChannel(CSpectrum& spec, unsigned x)
{
  CAxis* X = spec.getAxis(0);
  if (x < X->channels()) {	// unsigned x.
    uint32_t* p = reinterpret_cast<uint32_t*>(spec.getHandle());
    return p +x;
  } else {
    badChannel(x, X->channels());
  }
}
