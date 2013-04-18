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

#ifndef _RINGFORMATHELPER11CREATOR_H
#define _RINGFORMATHELPER11CREATOR_H

/**
 * @file RingFormatHelper11Creator.h
 * @brief Define CRingFormatHelper11Creator which creates CRingFormatHelper11's.
 */

#ifndef _RINGFORMATHELPERCREATOR_H
#include "RingFormatHelperCreator.h"
#endif


class CRingFormatHelper;

/**
 * @class CRingFormatHelper11Creator
 *   Helper creational class that normally is registered with a
 *   CRingFormatHelperFactory to produce CRingFormatHelper11's when the
 *   ring format version warrants it.
 */
class CRingFormatHelper11Creator : public CRingFormatHelperCreator
{
    // Copy constructor - placeholder for clone().
    
public:
    CRingFormatHelper11Creator() {}
    CRingFormatHelper11Creator(const CRingFormatHelper11Creator& rhs);
    
    // Virtual method declarations:
    
    virtual CRingFormatHelperCreator* clone() const;
    virtual CRingFormatHelper*        create();
};

#endif