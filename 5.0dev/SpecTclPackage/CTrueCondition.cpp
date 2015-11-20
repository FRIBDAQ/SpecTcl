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

#include "CTrueCondition.h"

/**
 * operator()
 *
 *  Evaluate the predicate.
 *
 * @param spectrum - The spectrum the condition is applied to.
 * @return true
 */
bool
CTrueCondition::operator()(CSpectrum& spec) 
{
  return true;
}
/**
 * validate
 *
 *  A no-op for this class.
 */
void
CTrueCondition::validate(CSpectrum& spec)
{
}
