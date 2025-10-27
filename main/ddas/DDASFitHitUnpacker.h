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
 * @file  DDASFitHitUnpacker.h
 * @brief Defines a class to unpack DDAS data with fit extensions.
 * @note DDASFitHitUnpacker is in the ddastoys namespace.
 */

#ifndef DDASFITHITUNPACKER_H
#define DDASFITHITUNPACKER_H

#include <DDASHitUnpacker.h>

namespace ddastoys {
    
    class DDASFitHit;

    /** 
     * @addtogroup unpacker libDDASFitHitUnpacker.so
     * @brief Unpacker for DDAS events possibly containing fit extension
     * data.
     * @details
     * If no fit extension data is present, the rest of the event is 
     * unpacked similar to the "normal" DDAS unpacker from the DDASFormat 
     * library.
     * @{
     */
	
    /**
     * @class DDASFitHitUnpacker
     * @brief Unpack raw hit data from DDAS event files.
     *
     * @details
     * The DDASHitUnpacker is capable of unpacking raw hits from NSCLDAQ event 
     * data which include hit extension data. Hit extensions are data 
     * structures which contain additional information appended to each hit 
     * which cannot be obtained from the digitizer module, for example from 
     * post-processing traces. A typical trace analysis may involve fitting
     * traces and performing further analysis with the fit output (event 
     * classification, measuring physics observables, etc.). This class 
     * is an extension of ddasfmt::DDASHitUnpacker and retains all the 
     * funcitonality of the base class.
     */
    
    class DDASFitHitUnpacker : public ddasfmt::DDASHitUnpacker
    {
    public:
	/**
	 * @brief Decode the current event and unpack it into a DDASFitHit.
	 * @param p Pointer to the ring item to decode. For an event built
	 *   fragment, this is normally the FragmentInfo's s_itemhdr 
	 *   pointer. Note the difference from DDASHitUnpacker which 
	 *   expects a pointer to the body.
	 * @param hit Hit item that we will unpack data into.
	 * @throw std::length_error An unexpected hit or extension size is 
	 *   encountered.
	 * @return A pointer just after the ring item.
	 */
	const void* decode(const void* p, DDASFitHit& hit);
    };
	
    /** @} */
	
}

#endif
