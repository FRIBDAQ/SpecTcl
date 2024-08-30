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

#ifndef RINGFORMATHELPER11_H
#define RINGFORMATHELPER11_H


/**
 * @file RingFormatHelper11.h
 * @brief Defines ring format helper for NSCLDAQ verswion 11.x+ data.
 */
#include "RingFormatHelper.h"

namespace ufmt {
    class RingItemFactoryBase;
}
/**
 * @class CRingFormatHelper11
 *
 * Provides a CRingFormatHelper that understands the shape of data for
 * NSCLDAQ11 and later.
 */

class CRingFormatHelper11 : public CRingFormatHelper
{
protected:
    unsigned m_glomSourceId;
    uint64_t m_nLastEventCount;
    ufmt::RingItemFactoryBase* m_Factory;
    // No data implies no need for canonicals.
public:
    CRingFormatHelper11();
    
    virtual bool hasBodyHeader(void* pItem) const;
    virtual void* getBodyPointer(void* pItem);
    virtual void* getBodyHeaderPointer(void* pItem);
    virtual uint32_t getSourceId(void* pItem, BufferTranslator* pTranslator);
  
    // Methods specific to state transitions:
    
    virtual std::string getTitle(void* pItem);
    virtual unsigned    getRunNumber(
        void* pItem, BufferTranslator* pTranslator);
    
    // Methods specific to string lists:
    
    virtual unsigned getStringCount(
        void* pItem, BufferTranslator* pTranslator);
    virtual std::vector<std::string> getStrings(
        void* pItem, BufferTranslator* pTranslator
    );
    
    // Methods specific to scaler items:
    
    virtual unsigned getScalerCount(
        void*  pItem, BufferTranslator* pTranslator);
    virtual std::vector<uint32_t> getScalers(
        void* pItem, BufferTranslator* pTranslator
    );
  virtual uint32_t getScalerOriginalSourceId(void* pItem, BufferTranslator* pTranslator);
  
    // Methods specific to trigger count items
    
    virtual uint64_t getTriggerCount(
        void* pItem, BufferTranslator* pTranslator);
protected:
    bool isStateTransition(void* pItem);
    bool isTextItem(void* pItem);
    bool isScalerItem(void* pItem);
    bool isTriggerCountItem(void* pItem);
};



#endif
