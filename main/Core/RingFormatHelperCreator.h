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
#ifndef RINGFORMATHELPERCREATOR_H
#define RINGFORMATHELPERCREATOR_H

/**
 * @file RingFormatHelperCreator.h
 * @brief Abstract base class for creators of ring format helpers.
 */

class CRingFormatHelper;

/**
 * @class CRingFormatHelperCreator
 *
 * Abstract Base Class for all classes that create format helpers for the
 * CFormatHelperFactory.  These are registered with that factory
 * with major/minor version matches and are called upon to create the actual
 * helper
 */
class CRingFormatHelperCreator
{
    // Canonicals.. everyone's going to need to probably implement a
    // copy constructor to support clone
    
public:
    CRingFormatHelperCreator() {}
    CRingFormatHelperCreator(const CRingFormatHelperCreator& rhs);
    
    // Interface supplied to the outside world:

public:    
    virtual CRingFormatHelperCreator* clone() const = 0;
    virtual CRingFormatHelper*        create() = 0;
};


#endif