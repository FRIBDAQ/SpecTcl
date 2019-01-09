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

#ifndef RINGFORMATHELPER10_H
#define RINGFORMATHELPER10_H

/**
 * @file RingFormatHelper10.h
 * @brief Defines class CRingFormatHelper10 which understands NSCLDAQ-10.0 ring items.
 */

#include "RingFormatHelper.h"


/**
 * @class CRingFormatHelper10
 *
 * This class is a ring format helper that understands the shape of ring items
 * in NSCLDAQ-10.x
 */
class CRingFormatHelper10 : public CRingFormatHelper
{
    // No member data so non need for the canonicals:
    
public:
    // Generic ring item access methods:
    
    virtual bool hasBodyHeader(void* pItem) const;
    virtual void* getBodyPointer(void* pItem);
    virtual void* getBodyHeaderPointer(void* pItem);
    
    // Specific access methods:
 
    // Methods specific to state transitions:
    
    virtual std::string getTitle(void* pItem);
    virtual unsigned    getRunNumber(
        void* pItem, BufferTranslator* pTranslator);
    
    // Methods specific to string lists:
    
    virtual unsigned getStringCount(
        void* pItem, BufferTranslator* pTranslator);
    
    // Methods specific to scaler items:
    
    virtual unsigned getScalerCount(
        void*  pItem, BufferTranslator* pTranslator);
    
    // Methods specific to trigger count items
    
    virtual uint64_t getTriggerCount(
        void* pItem, BufferTranslator* pTranslator);

private:
     bool isStateTransition(void* pItem);
     bool isTextItem(void* pItem);
     bool isScalerItem(void* pItem);
     bool isTriggerItem(void* pItem);
    
};



#endif