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

/** @file:  CRingFormatHelper12Creator.cpp
 *  @brief:  Create CRingFormatHelper12 objects for the helper factory.
 */
#include "RingFormatHelper12Creator.h"
#include "RingFormatHelper12.h"

/**
 * copy constructor
 */
CRingFormatHelper12Creator::CRingFormatHelper12Creator(
    const CRingFormatHelper12Creator& rhs
) {}

/**
 * clone virtual copy constructor of *this
 */

CRingFormatHelperCreator*
CRingFormatHelper12Creator::clone() const
{
    return new CRingFormatHelper12Creator(*this);
}
/**
 * create
 *   Create a new helper.
 */
CRingFormatHelper*
CRingFormatHelper12Creator::create()
{
    return new CRingFormatHelper12;
}