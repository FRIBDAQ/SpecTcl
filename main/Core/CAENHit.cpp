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

/** @file:  CAENHit.cpp
 *  @brief: CAEN Hit parsing/encapsulation classes
 */
#include "CAENHit.h"

///////////////////////////// base class CAENHit //////////////////

/**
 * getType
 *    Derived classes, when constructing must set the m_type
 *    member of the base class. This allows clients to know what
 *    type of event has been unpacked
 * @return CAENHit::HitType - type of hit
 */
CAENHit::HitType
CAENHit::getType() const {
    return m_type;
}
/**
 * getTimeTag
 *    Get the time tag.  When CFD's exist and are enabled,
 *    this is the coarse time of the trigger.  Hit types which can
 *    merge this coarse tag with finer timing information from, e.g.
 *    a CFD computation will implement e.g. getTime() to return this
 *    merged time
 * @return uint64_t Coarse trigger time in nanoseconds.
 */
uint64_t
CAENHit::getTimeTag() const
{
    return m_timeTag;
}
/**
 * getChannel
 *    @return uint32_t - the channel number of the hit in the module.
 */
uint32_t
CAENHit::getChannel() const
{
    return m_channel;
}
/**
 * trace1
 *    Each channel hit can store two traces as selected by
 *    the digitzer configuration.
 *    This returns a reference to the first (or only) of those traces.
 *    The vector size is 0 if ther eis no trace1
 * @return const std::vector<uint16_t>&
 */
const std::vector<uint16_t>&
CAENHit::trace1() const
{
    return m_trace1;
}
/**
 * trace2
 *    Each channel hit can store two traces as selected by
 *    the digitzer configuration.
 *    This returns a reference to the second trace if there is one.
 *    The vector size is 0 if there is no trace1
 * @return const std::vector<uint16_t>&
 */
const std::vector<uint16_t>&
CAENHit::trace2() const
{
    return m_trace2;
}

/**
 * unpackTraces
 *   GIven a pointer to the number of trace samples,
 *   unpacks the one or two traces.  Note that if the number
 *   of samples is 0, no traces are unpacked.
 * @param pData - pointer to the number of samples in the trace data.
 *                at that point the format of PSD and PHA data are
 *                identical allowing this be factored out.
 */
void
CAENHit::unpackTraces(void* pData)
{
    uint32_t* p32 = static_cast<uint32_t*>(pData);
    uint32_t nSamples = *p32++;
    m_trace1.clear();
    m_trace2.clear();
    
    if (nSamples) {

        bool dual;
        uint16_t* pTrace;
        if (m_type == PSD) {
            uint8_t* pDual = reinterpret_cast<uint8_t*>(p32);
            dual = (*pDual++) != 0;
            pDual++;                // Skip trace selector.
            pTrace = reinterpret_cast<uint16_t*>(pDual);
        } else {
            uint16_t* pDual = reinterpret_cast<uint16_t*>(p32);
            dual = (*pDual++) != 0;
            pTrace = pDual;
        }
        

        
        // First trace:
        
        for (int i =0; i < nSamples; i++) {
            m_trace1.push_back(*pTrace++);    
        }
        if (dual) {
            for (int i =0; i < nSamples; i++) {
                m_trace2.push_back(*pTrace++);
            }
        }
    }
}
/////////////////////////////  CAENPHAHit - PHA hits ///////////

/**
 * constructor
 *    Set the hit type.
 */
CAENPHAHit::CAENPHAHit(int mult) :
    CAENHit(CAENHit::PHA),
    m_fineTimeMult(mult)
{}
/**
 *  getEnergy
 *    Return the processed energy.
 * @return uin16_t - DPP energy.
 */
uint16_t
CAENPHAHit::getEnergy() const
{
    return m_energy;
}
/**
 * getExtra1
 *    @return uint16_t - extra1 value from the unpacked event.
 */
uint16_t
CAENPHAHit::getExtra1() const
{
    return m_extra1;
}
/**
 * getExtra2
 *   @return uint16_t - extra2 value from the unpacked event.
 */
uint16_t
CAENPHAHit::getExtra2() const
{
    return m_extra2;
}
/**
 * getTime
 *    Compute the high resolution time assuming that extras2 bottom 16
 *    bits is the time interpolation value.
 * @return double - high resolution time.
 */
double
CAENPHAHit::getTime() const
{
    uint64_t coarseTime = getTimeTag();
    double interpolation = (double)(m_fineTimeMult) * 1000.0 *
                            (double)(m_extra2 & 0xffff)/double(65536);   // Interpolation in ps.
    double result = (double)(coarseTime) + interpolation/1000.0; /// back in ns.
    return result;
}
/**
 * unpack
 *    Given a pointer to the hit information unpacks
 *    the event into the member data
 * @param void *  - pointer to the event.
 */
void
CAENPHAHit::unpack(void* pData)
{
    uint32_t* p32 = static_cast<uint32_t*>(pData);
    p32++;                            // Skip the size in words
    p32++;                            // and in bytes as well.
    m_channel  = *p32++;
    uint64_t* p64 = reinterpret_cast<uint64_t*>(p32);
    m_timeTag = *p64++;
    uint16_t* p16 = reinterpret_cast<uint16_t*>(p64);
    m_energy = *p16++;
    m_extra1 = *p16++;
    p32      = reinterpret_cast<uint32_t*>(p16);
    m_extra2 = *p32++;                    // extra2 is 32 bits.
    p16      = reinterpret_cast<uint16_t*>(p32);
    
    unpackTraces(p16);
}
///////////////////////CAENPSDHIt ////////////////////////////

/**
 * constructor
 *    Set the hit type.
 *  @param cfdMultiplier the multiplier that gives the number of ns
 *            Between which the CFD interpolates.  In most cases this is
 *            2 for a 500MHz module and 4 for a 250MHz module, however
 *            the values of bits 10:11 in the CFD settings register can change
 *            This. See the table below:
 *            
 *            |  Bits 11:10 |   Mult for 250MHz | Mult for 500Mhz |
 *            |=============|===================|=================|
 *            |   00        |    4              |       2         |
 *            |   01        |    16             |       8         |
 *            |   10        |    24             |       12        |
 *            |   11        |    32             |       16        |
 *
 *      Because these bits control the number of samples across which the
 *      CFD interpolates and the interpolation is 1024 bins withinh that time range.
 *        
 */
CAENPSDHit::CAENPSDHit(int cfdMultiplier) :
    CAENHit(CAENHit::PSD),
    m_cfdMultiplier(cfdMultiplier)
{}

/**
 * getShortCharge
 *   @return uint16_t - charge integrated from short gate.
 */
uint16_t
CAENPSDHit::getShortCharge() const
{
    return m_shortGateCharge;
}
/**
 * getLongCharge
 *   @return uint16_t charge integrated from the long gate.
 */
uint16_t
CAENPSDHit::getLongCharge() const
{
    return m_longGateCharge;
}
/**
 * getBaseline
 *   @return uint16_t - baseline measurement for this hit
 */
uint16_t
CAENPSDHit::getBaseline() const
{
    return m_baseline;
}
/**
 * getPURFlag
 *   @return uint16_t Pile up rejection flag.
 */
uint16_t
CAENPSDHit::getPURFlag() const
{
    return m_purFlag;
}
/**
 * getCFDTime
 *    @return uint16_t - the 11 bits of time from the extras
 *              word of the data.
 *    @note the correct extras selection must have been made
 *           for this to make sense.
 */
uint16_t
CAENPSDHit::getCFDTime() const
{
    return m_CFDTime;
}
/**
 * getTime
 *    Merges the timestamp and the CFD Time to compute a timestamp
 *    with subnanosecond resolution
 * @return double - time in nanoseconds.
 * @note the correct extras selection must have been made for this
 *     to make sense.
 */
double
CAENPSDHit::getTime() const
{
    double result = getTimeTag();      // Alread in ns.
    double fine   = getCFDTime();
    fine = (fine * m_cfdMultiplier*1000)/1024.0 ; // fine time in ps.
    result = result + fine/1000;     // Fine time in ns. not ps.
    return result;                 // STUB until Pierluigi tells me how.
}
/**
 * unpack
 *    Unpacks the raw hit data from the event into the
 */
void
CAENPSDHit::unpack(void* pData) 
{
    uint32_t* p32 = static_cast<uint32_t*>(pData);
    
    p32 += 2;           // Size in words and size in bytes.
    
    uint64_t* p64 = reinterpret_cast<uint64_t*>(p32);
    m_timeTag = *p64++;
    
    uint16_t* p16 = reinterpret_cast<uint16_t*>(p64);
    m_channel = *p16++;
    
    m_shortGateCharge = *p16++;
    m_longGateCharge  = *p16++;
    m_baseline        = *p16++;
    m_purFlag         = *p16++;
    p32               = reinterpret_cast<uint32_t*>(p16);
    m_CFDTime         = (*p32++) & 0xff;
    
    if (*p32++ > sizeof(uint32_t)) {
        unpackTraces(p32);
    }
    
}