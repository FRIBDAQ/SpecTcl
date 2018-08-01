/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef RINGFORMATHELPER10CREATOR_H
#define RINGFORMATHELPER10CREATOR_H

#include "RingFormatHelperCreator.h"

/**
 * @file RingFormatHelper10Creator
 * @brief Defines CRingFormatHelper10Creator which makes CRingFormatHelper10's.
 */

class CRingFormatHelper;

/**
 * @class CRingFormatHelper10Creator
 *
 * Creator class for helpers that understand the ring format for Version 10.x
 */
class CRingFormatHelper10Creator : public CRingFormatHelperCreator
{
    // Copy constructor:
  
public:
    CRingFormatHelper10Creator() {}
    CRingFormatHelper10Creator(const CRingFormatHelper10Creator & rhs);
    
    // Virtual methods:
    
    virtual CRingFormatHelperCreator* clone() const;
    virtual CRingFormatHelper*        create();
};

#endif