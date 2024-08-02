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
#ifndef FITHITUNPACKER_H
#define FITHITUNPACKER_H
/** @file:  FitHitUnpacker.h
 *  @brief: Unpack DDAS data where the traces _may_ have a HitExtension
 *          that contains one and two pulse fits.
 */




#include <DDASHitUnpacker.h>




// Let's put this sob in the same namespace as the DDASHitUnpacker method.

namespace DAQ {
    namespace DDAS {
        class DDASFitHit;
/**
 *  @class FitHitUnpacker
 *  
 * DAQ::DDAS::DDASHitUnpacker is capable of unpacking raw hits from DDAS
 * files. Typical trace analysis may involve fitting traces to one or two pulses.
 * This class extends the DDASHitUnpacker class to support access to the results
 * of the fit which have been tacked on the back end of a hit by
 * CDDASAnalyzer.cpp.
 */

 class FitHitUnpacker : public DAQ::DDAS::DDASHitUnpacker
 {
 public:
    const void* decode(const void* p, DDASFitHit& hit
    );
 };
 
 ////////////////////
    }                                   // namespace DDAS
}                                       // namespace DAQ


#endif