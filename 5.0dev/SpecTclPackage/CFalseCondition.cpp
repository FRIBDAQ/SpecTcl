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

#include "CFalseCondition.h"

/**
 * operator()
 *
 *  Evaluate the predicate.
 *
 * @param spectrum - The spectrum the condition is applied to.
 * @return false
 */
bool
CFalseCondition::operator()(CSpectrum& spec) 
{
  return false;
}
/**
 * validate
 *
 *  A no-op for this class.
 */
void
CFalseCondition::validate(CSpectrum& spec)
{
}
