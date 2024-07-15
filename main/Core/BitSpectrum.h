/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

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
/**
 * @file BitSpectrum.h 
 * @brief Templated class definition for bit mask spectra.  Replaces BitSpectrum{L,W}.
 */

#ifndef BITSPECTRUM_H
#define BITSPECTRUM_H

#include "Spectrum1d.h"
#include <stdint.h>


/**
 *  @class CBitSpectrum 
 *   This is a templated class that handles bit spectra.  Really a bit spectrum is just a 1-d spectrum
 * that has a special incrementer.  Therefore, unlike the original classes we derive, not from CSpectrum
 * but from the appropriate CSpectrum1D class and just override the increment method.
 */
template <typename T>
class CBitSpectrum : public CSpectrum1D<T> {

public:

			//Constructor(s) with arguments

    CBitSpectrum(const std::string& rName, UInt_t nId,
            const CParameter& rParameter,
            UInt_t nChannels);	// Parameter is unmapped.
    CBitSpectrum(const std::string& rName, UInt_t nId,
            const CParameter& rParameter,
            UInt_t nLow,
            UInt_t nHigh);	// Slice of the space... floats are no good.
		

    virtual  ~ CBitSpectrum( ) ;

    virtual   void Increment (const CEvent& rEvent)  ;
};

typedef CBitSpectrum<uint32_t> CBitSpectrumL;
typedef CBitSpectrum<uint16_t> CBitSpectrumW;

#include "BitSpectrum.hpp"

#endif