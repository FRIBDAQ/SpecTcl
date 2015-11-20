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
#include "CAxis.h"
#include <math.h>

/**
 * constructor
 *    Just needs to fill in member data.
 *
 * @param name - Name/title for the axis.
 * @param bins - Number of bins on the axis.
 * @param low  - Parameter coordinate that corresponds to bin0 left edge.
 * @param high - Parameter coordinate that correspondes to bins bin right edge.
 */
CAxis::CAxis(const char* name, unsigned bins, double low, double high) :
  m_name(name), m_channels(bins), m_low(low), m_high(high) {}

/**
 * name
 *   Selector for m_name.
 *
 * @return std::string.
 */
std::string
CAxis::name() const
{
  return m_name;
}
/**
 * channels
 *   Selector for the m_channels member.
 *
 * @return unsigned
 */
unsigned
CAxis::channels() const {
  return m_channels;
}
/**
 * low
 *   Selector for m_low
 *
 * @return double
 */
double
CAxis::low() const
{
  return m_low;
}
/**
 * high
 * selector for m_high
 *
 * @return double
 */
double
CAxis::high() const
{
  return m_high;
}
/**
 * toChannels
 *  Compute the bin into which the axis coordinate given falls.
 *
 * @param value - The axis coordinate value to transform.
 *
 * @return unsigned - the bin number.
 *
 * @note - no range checking is done so it's possible to get
 *         channels off the end of the axis.
 */
int
CAxis::toChannels(double value) const
{
  return lround((value - m_low)*m_channels/(m_high - m_low)); // rounded.
  
}
/**
 * toAxis
 *   Compute the axis coordinate into which a bin falls.
 * 
 * @param value - bin number to tranform.
 *
 * @return double.
 *
 */
double
CAxis::toAxis(int value)
{
  return value * (m_high-m_low)/m_channels + m_low;
}
