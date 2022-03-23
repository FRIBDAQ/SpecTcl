/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  RingFormatHelper12Creator.h
 *  @brief: Define the ring format
 */
#ifndef RINGFORMATHELPER12CREATOR_H
#define RINGFORMATHELPER12CREATOR_H

#include "RingFormatHelperCreator.h"

/**
 * @class CRingFormatHelper12Creator
 *   Creates CRingFormatHelper12 objects for the helper factory.
 */
class CRingFormatHelper12Creator : public CRingFormatHelperCreator
{
public:
    CRingFormatHelper12Creator() {}
    CRingFormatHelper12Creator(const CRingFormatHelper12Creator& rhs);
    
public:
    virtual CRingFormatHelperCreator* clone() const;
    virtual CRingFormatHelper*        create();
};

#endif
