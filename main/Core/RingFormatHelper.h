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

#ifndef _RINGFORMATHELPER_H
#define _RINGFORMATHELPER_H

#ifndef _STL_STRING
#include <string>
#ifndef _STL_STRING
#define _STL_STRING
#endif
#endif

#ifndef __STDINT_H
#include <stdint.h>
#ifndef __STDINT_H
#define __STDINT_H
#endif
#endif
/**
 * @file RingFormatHelper.h
 * @brief  Definition of the abstract base class for Ring Format helpers
 */

class BufferTranslator;

/**
 * @class CRingFormatHelper
 *
 * This is the abstract base class of a class hierarchy that is intended
 * to help CRingDecoder determine where bits and pieces of a ring item
 * are.  This can vary between ring buffer format levels.  The idea therefore
 * is that a CRingDecoder has the 'right' one of these to help it out and does
 * not need any internal code that is version aware.  That, of course, begs the
 * question of how it gets that helper.
 */
class CRingFormatHelper
{
    // Canonicals - All are defined as public, but subclasses can certainly
    // turn somoe off themselves.
    
public:
    CRingFormatHelper();
    virtual ~CRingFormatHelper();
    CRingFormatHelper(const CRingFormatHelper&);
    CRingFormatHelper& operator=(const CRingFormatHelper&);
    int operator==(const CRingFormatHelper&) const;
    int operator!=(const CRingFormatHelper&) const;
    
    // These methods are what the subclasses need to implement:
    
public:
    // Generic methods:
    
    virtual bool hasBodyHeader(void* pItem) const   = 0;
    virtual void* getBodyPointer(void* pItem)       = 0;
    virtual void* getBodyHeaderPointer(void* pItem) = 0;
    
    // Methods specific to state transitions:
    
    virtual std::string getTitle(void* pItem) = 0;
    virtual unsigned    getRunNumber(
        void* pItem, BufferTranslator* pTranslator) = 0;
    
    // Methods specific to string lists:
    
    virtual unsigned getStringCount(
        void* pItem, BufferTranslator* pTranslator) = 0;
    
    // Methods specific to scaler items:
    
    virtual unsigned getScalerCount(
        void*  pItem, BufferTranslator* pTranslator) = 0;
    
    // Methods specific to trigger count items
    
    virtual uint64_t getTriggerCount(
        void* pItem, BufferTranslator* pTranslator) = 0;

    uint16_t itemType(void* pItem);

};
#endif