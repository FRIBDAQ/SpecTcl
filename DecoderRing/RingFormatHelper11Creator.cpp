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
#include "RingFormatHelper11Creator.h"
#include "RingFormatHelper11.h"

/**
 * copy constructor - place holder in case we ever need more.
 *
 * @param rhs - the object that will be copied to create *this.
 */
CRingFormatHelper11Creator::CRingFormatHelper11Creator(const CRingFormatHelper11Creator& rhs)
{}

/**
 * clone
 *    Virtual copy constructor of *this.
 *
 * @return CRingFormatHelperCreator*
 */
CRingFormatHelperCreator*
CRingFormatHelper11Creator::clone() const
{
    return new CRingFormatHelper11Creator(*this);
}
/**
 * create
 *   Create a CRingFormatHelper11 returning a pointer to it:
 *
 * @return CRingFormatHelper11*
 */
CRingFormatHelper*
CRingFormatHelper11Creator::create()
{
    return new CRingFormatHelper11;
}