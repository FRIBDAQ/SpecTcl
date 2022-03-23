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

/** @file:  RingFormatHelper12.h
 *  @brief: Defines the format helper for NSCLDAQ-12.0
 */
#ifndef RINGFORMATHELPER12_H
#define RINGFORMATHELPER12_H
#include "RingFormatHelper11.h"
/**
 * @class CRingFormatHelper12
 *    This class is derived from the version 11 helper because many of the
 *    operations are identical  In V12, we added an original source id field
 *    to several items and only those methdods that look past that
 *    field in those items need to be overridden from the v11 helper.
 */
class CRingFormatHelper12 : public CRingFormatHelper11
{
public:  
    CRingFormatHelper12();
    virtual ~CRingFormatHelper12();
    
    // State change items.
    
    virtual std::string getTitle(void* pItem);
    
    // Text Items:
    
    virtual std::vector<std::string> getStrings(
        void* pItem, BufferTranslator* pTranslator
    );
    // Scaler items:
    
    virtual std::vector<uint32_t> getScalers(
        void* pItem, BufferTranslator* pTranslator
    );
    
    // trigger count items:
    
    virtual uint64_t getTriggerCount(
        void* pItem, BufferTranslator* pTranslator
    );
    
};


#endif
