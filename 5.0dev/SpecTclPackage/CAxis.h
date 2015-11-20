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
#ifndef _CAXIS_H
#define _CAXIS_H

/**
 * @file CAxis.h
 * @brief Define a class that describes an axis and performs its scaling
 */

#ifndef _STD_STRING
#include <string>
#ifndef _STD_STRING
#define _STD_STRING
#endif
#endif

/**
 * @class CAxis
 *
 *   Defines an axis.  An axis is an object that defines a mapping between
 *   some window in a parameter coordinate space to/from bins that subdivide
 *   the axis.  The most common use of an axis is to describe
 *   a histogram coordinate system and how it maps to and from parameter space.
 */
class CAxis {
private:
  std::string   m_name;
  unsigned      m_channels;
  double        m_low;
  double        m_high;

  // Canonicals:
public:
  CAxis(const char* name, unsigned bins, double low, double high);
  
  // Selectors:

public:
  std::string name()     const;
  unsigned    channels() const;
  double      low()      const;
  double      high()     const;

  // operations:

  int  toChannels(double value) const;
  double    toAxis(int value);
};

#endif
