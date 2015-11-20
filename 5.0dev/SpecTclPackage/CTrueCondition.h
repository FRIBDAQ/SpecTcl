/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef _CTRUECONDITION_H
#define _CTRUECONDITION_H

/**
 * @file CTrueCondition.h
 * @brief Defines a condition predicate that is always true.
 */

#ifndef _CCONDITION_H
#include <CCondition.h>
#endif


/**
 *  @class CTrueCondition
 *
 *   The predicate always returns true and is never invalidated.
 */
class CTrueCondition : public CCondition
{
public:
  virtual void validate(CSpectrum& spectrum);
  virtual bool operator()(CSpectrum& spectrum);
};
#endif
