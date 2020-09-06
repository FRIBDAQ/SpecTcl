/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  CCAENHit.h
 *  @brief: Base class for a CAEN Hit.
 */
#ifndef CAENHIT_H
#define CAENHIT_H
#include <stdint.h>
#include <vector>
/**
 * @class CAENHit
 *    This is a base class for the CAEN Hit type.
 *    There are currently two subtypes:
 *    CAENPHAHit and CAENPSDHit  These are also defined
 *    in this file.  The hits include the mechanics to unpack
 *    themselves from a pointer to event data.
 */
class CAENHit {
public:
    typedef enum _HitType {PSD, PHA} HitType;
protected:
    HitType   m_type;
    uint64_t  m_timeTag;
    uint32_t  m_channel;
    std::vector<uint16_t> m_trace1;
    std::vector<uint16_t> m_trace2;
    
    
    // All the other bits ad pieces are type dependent.

public:
    CAENHit(HitType type) : m_type(type) {}
    HitType  getType() const;
    uint64_t getTimeTag() const;
    uint32_t getChannel() const;
    const std::vector<uint16_t>& trace1() const;
    const std::vector<uint16_t>& trace2() const;
 
    
    /** Required interfaces:  */

public:
    virtual void unpack(void* pData) = 0;
    
protected:
    void unpackTraces(void* pTrace);
};

/**
 * @class CAENPHAHit
 *   PHA Hit.
 */
class CAENPHAHit : public CAENHit
{
private:
    uint16_t m_energy;
    uint16_t m_extra1;
    uint16_t m_extra2;
public:
    CAENPHAHit();
    
    uint16_t getEnergy() const;
    uint16_t getExtra1() const;
    uint16_t getExtra2() const;
    
    virtual void unpack(void* pData) ;
};
/**
 * @class CAENPSDHit
 *    PSD hit.
 */
class CAENPSDHit : public CAENHit
{
private:
    uint16_t m_shortGateCharge;
    uint16_t m_longGateCharge;
    uint16_t m_baseline;
    uint16_t m_purFlag;
    uint16_t m_CFDTime;
    int      m_cfdMultiplier;
    
public:
    CAENPSDHit(int cfdMultiplier);
    
    uint16_t getShortCharge() const;
    uint16_t getLongCharge()  const;
    uint16_t getBaseline()    const;
    uint16_t getPURFlag()     const;
    uint16_t getCFDTime()     const;
    double   getTime()        const;
  
    virtual void unpack(void* pData);
};


#endif