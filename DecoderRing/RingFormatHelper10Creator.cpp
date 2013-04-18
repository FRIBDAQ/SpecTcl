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

/**
 * @file RingFormatHelper10Creator.cpp
 * @brief implemenet CRingFormatHelepr10Creator, creator of CRingFormatHelper10's.
 */

#include "RingFormatHelper10Creator.h"
#include "RingFormatHelper10.h"

/**
 * Copy constructor -- placeholder for now
 */
CRingFormatHelper10Creator::CRingFormatHelper10Creator(constructorCRingFormatHelper10Creator& rhs)
{}

/**
 * clone
 *
 * Virtual copy constructor.
 *
 * @return CRingFormatHelperCreator - pointer to a new'd instance of *this.
 */
CRingFormatHelperCreator*
CRingFormatHelper10Creator::clone() const
{
    return new CRingFormatHelper10Creator(*this);
}
/**
 * create
 *
 *    Create a RingFormatHelper(10) and return it.
 *
 *    @return CRingFormatHelper - (version 10.)
 */
CRingFormatHelper*
CRingFormatHelper10Creator::create()
{
    return new CRingFormatHelper10;
}