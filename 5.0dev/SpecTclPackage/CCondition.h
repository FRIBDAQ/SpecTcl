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
#ifndef _CCONDITION_H
#define _CCONDITION_H

/**
 * @file CCondition.h
 * @brief Define the abstract base class for conditions.
 */

class CSpectrum;

/**
 * @class CCondition
 *
 *   Conditions are predicates that are defined over the current event.
 *   and analysis state.   Conditions must be able to determine two
 *   things:
 *      * Are they still able to evaulate sanely?
 *      * Are they true for the current event.
 *
 *  If the first condition is not the case, the predicate event evaluation
 *  should return something sensible (usually either true or false).
 *
 *  Conditions are associated with spectra and their operations are given
 *  access to spectrum they conditionalize.
 *
 *  This is an abstract base class.
 */
class CCondition
{
public:
  virtual void validate(CSpectrum& spectrum) = 0;
  virtual bool operator()(CSpectrum& spectrum) = 0;
};
#endif
