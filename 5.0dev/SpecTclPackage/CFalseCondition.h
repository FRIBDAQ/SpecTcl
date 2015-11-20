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

#ifndef _CFALSECONDITION_H
#define _CFALSECONDITION_H

/**
 * @file CFalseCondition.h
 * @brief Defines a condition predicate that is always false.
 */

#ifndef _CCONDITION_H
#include <CCondition.h>
#endif


/**
 *  @class CFalseCondition
 *
 *   The predicate always returns false and is never invalidated.
 */
class CFalseCondition : public CCondition
{
public:
  virtual void validate(CSpectrum& spectrum) ;
  virtual bool operator()(CSpectrum& spectrum);
};
#endif
