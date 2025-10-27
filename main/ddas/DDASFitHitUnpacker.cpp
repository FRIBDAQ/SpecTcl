/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     Aaron Chester
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** 
 * @file DDASFitHitUnpacker.cpp
 * @brief Implements DDASFitHitUnpacker class to unpack hits with fit 
 * extension data.
 */

#include "DDASFitHitUnpacker.h"

#include <string>
#include <stdexcept>
#include <iostream>

#include <DataFormat.h>

#include "DDASFitHit.h"

using namespace ddastoys; // Fit extensions, DDASFitHit, etc.
using namespace ufmt;

/**
 * @details
 * The decode function:
 * - Determines the limits of the hit.
 * - Determines where, or if, there's an extension block.
 * - Unpacks the original hit using `DAQ::DDAS::DDASHitUnpacker::unpack()`.
 * - Sets the extension if there is one.
 *
 * The first 32 bits of the body contain the number of 16-bit words in the 
 * hit data.
 * - If this works out equivalent to bodySize - there's no extension.
 * - If this is larger, to accommodate data from ringblockdealer or the 
 *   editor fitting framework, we have the following cases:
 *     1) The extra data is the size of HitExtensionLegacy - the extra data
 *        is an old-style hit extension e.g., from ringblockdealer.
 *     2) The extra data is sizeof(uint32_t) - the extra data is a null 
 *        extension from the editor fitting framework.
 *     3) The extra data is sizeof(FitInfo) from fit_extensions.h - the 
 *        extra data is a hit extension from the editor fitting framework.
 *     4) Anything else - we don't know how to do with and fail with an
 *        error message.
 */
const void*
ddastoys::DDASFitHitUnpacker::decode(const void* p, DDASFitHit& hit)
{  
    // Find the ring item body:
    
    const RingItem* pItem = reinterpret_cast<const RingItem*>(p);
    const uint8_t*  pBody;
    uint32_t bodyHeaderSize; // Number of bytes in the body header.
    if (pItem->s_body.u_noBodyHeader.s_mbz) {
	pBody = reinterpret_cast<const uint8_t*>(
	    pItem->s_body.u_hasBodyHeader.s_body
	    );
	bodyHeaderSize = pItem->s_body.u_hasBodyHeader.s_bodyHeader.s_size;
    } else {
	pBody = reinterpret_cast<const uint8_t*>(
	    pItem->s_body.u_noBodyHeader.s_body
	    );
	bodyHeaderSize = sizeof(uint32_t);
    }
  
    // Generate a pointer to off the end of the body. Note bodySize is bytes:
    
    uint32_t bodySize
	= pItem->s_header.s_size - bodyHeaderSize - sizeof(RingItemHeader);
    const uint8_t* pEnd = pBody + bodySize;

    // Get the hit data and decide what to do with it:
       
    const uint32_t* pHitSize = reinterpret_cast<const uint32_t*>(pBody);
    uint32_t bodyWords = *pHitSize;
    uint32_t bodyBytes = bodyWords*sizeof(uint16_t);
 
    if (bodyBytes == bodySize) {    
	// This is just an ordinary hit:
	unpack(
	    reinterpret_cast<const uint32_t*>(pBody),
	    reinterpret_cast<const uint32_t*>(pEnd),
	    hit
	    );        
    } else if ((bodyBytes + sizeof(HitExtensionLegacy)) == bodySize) {    
	// Hit with old-style fits:
	pEnd -= sizeof(HitExtensionLegacy); // Also points to extension.
	unpack(
	    reinterpret_cast<const uint32_t*>(pBody),
	    reinterpret_cast<const uint32_t*>(pEnd),
	    hit
	    );
	// Convert to modern extension and set it:
	HitExtension ext(*(reinterpret_cast<const HitExtensionLegacy*>(pEnd)));
	hit.setExtension(ext);
    } else if (bodyBytes + sizeof(uint32_t) == bodySize) {        
	// There's no hit extension actually -- it's a null extension:        
	pEnd -= sizeof(uint32_t);
	unpack(
	    reinterpret_cast<const uint32_t*>(pBody),
	    reinterpret_cast<const uint32_t*>(pEnd),
	    hit
	    );                
    } else if (bodyBytes + sizeof(FitInfo) == bodySize) {    
	// Fit information from this DDASToys:  
	pEnd -= sizeof(FitInfo);
	unpack(
	    reinterpret_cast<const uint32_t*>(pBody),
	    reinterpret_cast<const uint32_t*>(pEnd),
	    hit
	    );
	hit.setExtension(reinterpret_cast<const FitInfo*>(pEnd)->s_extension);
    } else {
	throw std::length_error(
	    "Inconsistent event size for DAQ::DDAS::DDASHit or extended hit"
	    );
    }
    
    return nullptr; // Should not get here.
    
}
