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
 * @file BitSpectrum.hpp
 * @brief Implementation of the bit spectrum class.
 */

// Note that this is included from BitSpectrum.h so we don't need to
// include our header... This is a consequencde of being a template
// class

/**
 * constructor:
 *    @param rName - name of the spectrum.
 *    @param nId   - Spectrum id.
 *    @param rParameter - references the paramter definition.
 *    @param nChannels - Number of channels in the spectrum.
 * 
 * We let the CSpectrum parent constructor do all the work:
 * 
 */
template <typename T>
CBitSpectrum<T>::CBitSpectrum(const std::string& rName, UInt_t nId,
            const CParameter& rParameter,
            UInt_t nChannels) :
    CSpectrum1D<T>(rName, nId, rParameter, nChannels) {}

/**
 *  constructor
 *     Over a range of bits:
 *    @param rName - name of the spectrum.
 *    @param nId   - Spectrum id.
 *    @param rParameter - references the paramter definition.
 *    @param nLow - low bit number
 *    @param nHigh- High bit number exclusive (I think).
 * 
 */
template <typename T>
CBitSpectrum<T>:: CBitSpectrum(const std::string& rName, UInt_t nId,
            const CParameter& rParameter,
            UInt_t nLow,
            UInt_t nHigh) :
    CSpectrum1D<T>(
        rName, nId, rParameter,
        (nHigh - nLow),
        (Float_t)nLow, (Float_t)(nHigh)
    ) {}

/**
 *  destructor:
 */
template <typename T>
CBitSpectrum<T>::~CBitSpectrum() {}


/**
 * Increment
 *   The only difference between the 1d spectrum and the bit spectrum is
 * how we increment.
 *    @param rEvent - references the event to increment over.
 * 
 */
template  <typename T>
void
CBitSpectrum<T>::Increment(const CEvent& rEvent) {
    UInt_t myPar = this->getParameter();
    CEvent& rE((CEvent&)rEvent);
    if (myPar < rE.size() && rE[myPar].isValid()) {
        UInt_t nParam = rE[myPar];
        Double_t bitnum = 0.0;
        UInt_t bit = 1;
        while(nParam) {
            if (bit & nParam) {
                
                nParam &= ~bit;
                this->getRootSpectrum()->Fill(bitnum);
            }
            bit = bit << 1;
            bitnum += 1.0;
        }
    }
}