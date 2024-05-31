/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  FitHitUnpacker.cpp
 *  @brief: Implements FitHitUnpacker.
 */

#include "FitHitUnpacker.h"
#include "DDASFitHit.h"
#include <DataFormat.h>


#include <string.h>
#include <stdexcept>
#include <FitExtender.h>

/**
 * decode
 *    - Determines the limits of the old style hit.
 *    - Determines where, or if, there's an extension block.
 *    - Unpacks the original hit using DDASHitUnpacker::unpack.
 *    - Sets the extension if there is one.
 *    @param pRingItem - points to the ring item to decode.  For an event built
 *           fragment, this is normally the FragmentInfo's s_itemhdr pointer.
 *           Note the difference from DDASHitUnpacker which expects a pointer to the
 *           body.
 *    @param hit - hit item that we will unpack.
 *    @return Pointer just after the ring item.
 */
const void*
DAQ::DDAS::FitHitUnpacker::decode(
    const void* p, DDASFitHit& hit
)
{
    // Find the ring item body:
    
    const RingItem* pItem = reinterpret_cast<const RingItem*>(p);
    const uint8_t*  pBody;
    uint32_t bodyHeaderSize;                // Number of bytes in the body header.
    if (pItem->s_body.u_noBodyHeader.s_mbz) {
        pBody = reinterpret_cast<const uint8_t*>(
            &(pItem->s_body.u_hasBodyHeader.s_bodyHeader)
        );
        bodyHeaderSize = pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_size;
        pBody += bodyHeaderSize;                      // Works in presence of a body header extension
    } else {
        pBody = reinterpret_cast<const uint8_t*>(pItem->s_body.u_noBodyHeader.s_body);
        bodyHeaderSize = sizeof(uint32_t);
    }
    // Generate a pointer to off the end of the body.
    
    uint32_t bodySize = pItem->s_header.s_size - bodyHeaderSize - sizeof(RingItemHeader);
    const uint8_t* pEnd = pBody + bodySize;
    
    /*
     * The first 32 bits of the body is the number of 16 bit words of
     * DDAS  hit.
     * -   If this works out equivalent to bodySize - there's no extension.
     * -   If this is larger, to accommodate data from ring blockdealer
     *     as well as fitextender, we have the followin cases:
     *     *    The extra data is the size of HitExtension
     *          The extra data is a HitExtension from ringblockdealer.
     *     *    The extra data is sizeof(uint32_t) the extra data is
     *          must contain sizeof(uint32_t) and is a null extension
     *          from the FitExtender.
     *     *    The extra data is sizeof(FitInfo) from FitExtender -
     *          the extra data is FitExtender fit information
     *     *    Anything else - we don't know how to do with  and fail
     *          with an error message.
     */
    
    const uint32_t* pHitSize = reinterpret_cast<const uint32_t*>(pBody);
    uint32_t bodyWords = *pHitSize;
    uint32_t bodyBytes = bodyWords*sizeof(uint16_t);
    

    if (bodyBytes == bodySize) {
        // This is just an ordinary HIT:
        
        unpack(
            reinterpret_cast<const uint32_t*>(pBody),
            reinterpret_cast<const uint32_t*>(pEnd),
            hit
        );
        
    } else if ((bodyBytes + sizeof(::DDAS::HitExtension)) == bodySize) {
        // hit with fits.
        
        pEnd -= sizeof(::DDAS::HitExtension);  // also points to extension.
        unpack(
            reinterpret_cast<const uint32_t*>(pBody),
            reinterpret_cast<const uint32_t*>(pEnd),
            hit
        );
        hit.setExtension(*(reinterpret_cast<const ::DDAS::HitExtension*>(pEnd)));
    } else if (bodyBytes + sizeof(uint32_t) == bodySize) {
        
        // There's no hit extension actually -- it's a FitExtender null extension.
        
        pEnd -= sizeof(uint32_t);
        unpack(
            reinterpret_cast<const uint32_t*>(pBody),
            reinterpret_cast<const uint32_t*>(pEnd),
            hit
        );
                
    } else if (bodyBytes + sizeof(FitInfo)  == bodySize) {
        // FitExtender fit information:
        
        pEnd -= sizeof(FitInfo);
        unpack(
            reinterpret_cast<const uint32_t*>(pBody),
            reinterpret_cast<const uint32_t*>(pEnd),
            hit
        );
        const FitInfo* pExtension = reinterpret_cast<const FitInfo*>(pEnd);
        hit.setExtension(pExtension->s_extension);
    } else {
        throw std::length_error("Inconsistent event size for DDASHit or extended hit");
    }
    
    return pEnd;
}
