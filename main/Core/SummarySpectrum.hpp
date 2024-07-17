/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


/**
 * @file SummarySpectrum.hpp 
 * @brief Implementation of the templated summary spectrum class.
 * @note  We don't include SummarySpectrum.h because it includes _us_
 */
#include "Event.h"
#include <algorithm>
#include <TH1.h>
#include <iostream>  /// debugging.

//////////////////////// Canonicals:

/**
 * constructor
 *    Construct the base class 2d spectrum and just save the parameter ids.
 * 
 * @param rName - name of the spectrum.
 * @param nId - Id of the spectrum.
 * @param rrParameters - Number of parameters (defines the X axis).
 * @param nYScale - number of bins on the Y axis.
 * @note the parameters we pass to the 2d constuctor are meaningless and hidden
 *   via our accessors.
 */
template <typename T>
CSummarySpectrum<T>::CSummarySpectrum(const std::string& rName, UInt_t nId,
		    std::vector<CParameter> rrParameters,
		    UInt_t nYScale) : 
    CSpectrum2D<T>(rName, nId, rrParameters[0], rrParameters[0], rrParameters.size(), nYScale) {
        ParametersToIds(rrParameters);
        computeDefinition();
    }
/**
 *  constructor
 *    COnstruct the base class 2d spectrum but we use world coordinates.  The X axis world coordinates go from
 * 0.0 - rrParameters.size() with that many bins so it's a 1:1 mapping.
 * 
 *   @param rName - name of the spectrum.
 *   @param nId   - Id of the spectrum.
 *   @param rrParameters - Vector of parameters to histogram.
 *   @param nYScale  - Number of y bins.
 *   @param fYLow    - Y axis low limit.
 *   @param fYHIgh   - Y Axis high limit.
 *   @note the parameters we pass to the 2d spectrum base class constructor are meaningless and masked by our
 * accessors.
 */
template <typename T>
CSummarySpectrum<T>::CSummarySpectrum(const std::string& rName, UInt_t nId,
		    std::vector<CParameter> rrParameters,
		    UInt_t nYScale,
		    Float_t fYLow,
		    Float_t fYHigh) :
    CSpectrum2D<T>(
        rName, nId, rrParameters[0], rrParameters[0], 
        rrParameters.size(), 0.0, rrParameters.size(),
        nYScale, fYLow, fYHigh) {
        ParametersToIds(rrParameters);
        computeDefinition();
    }

/**
 *  destructor
 * 
 *    We can let the base class pick up the pieces:
 */
template <typename T>
CSummarySpectrum<T>::~CSummarySpectrum() {}


////// Object operations:

/**
 *  Increment
 *    Increment the spectrum once for every parameter present in the event that we care about:
 * 
 * @param rEvent - the event to increment for.
 * @note by this time the gate on the spectrum should already have been evaluated to True.
 */
template <typename T>
void
CSummarySpectrum<T>::Increment(const CEvent& rEv) {
    TH1* pSpec = this->getRootSpectrum();
    
    CEvent& rEvent((CEvent&)rEv);
    Double_t xchan(0.0);
    for (auto pno : m_vParameters) {
        if (rEvent[pno].isValid()) {
            Double_t value = rEvent[pno];
            std::cerr << "Filling with " << xchan << " " << value << std::endl;
            pSpec->Fill(xchan, value);
        }
        xchan += 1.0;
    }
}
/**
 * UsesParameter
 *    @param nId
 *    @return bool - True if nId is a parameter incremented by the spectrum.
 */
template <typename T>
Bool_t
CSummarySpectrum<T>::UsesParameter(UInt_t nId) const {
    return std::find(m_vParameters.begin(), m_vParameters.end(), nId) != m_vParameters.end();
}
/**
 *  GetParameterIds
 *    @param rvIds - vector that will be loaded with the parameter ids used by this spectrm.
 */
template <typename T>
void
CSummarySpectrum<T>::GetParameterIds(std::vector<UInt_t>& rvIds) {
    rvIds = m_vParameters;
}
/**
 * GetDefinition
 *    Return a definition of the spectrum.  
 *    Since the definition of the spectrum is time invariant, we computed it when we constructed - into
 *    m_defintion  
 * 
 * @return SpectrumDefinition&
 */
template <typename T>
CSpectrum::SpectrumDefinition&
CSummarySpectrum<T>::GetDefinition() {
    return m_definition;
}
/**
 *  nnedParameter
 *     @return Bool_t  - False since the spectrum cannot be placed in any per parameter increment list
 */
template <typename T>
Bool_t
CSummarySpectrum<T>::needParameter() const {
    return kfFALSE;
}


/// Private utlities.


/**
 * ParametersToIds
 *    Given the set of parameter definitions passed in, stoocks m_vParameters
 * 
 * @param pDefs - references the parameter definitons to convert.
 */
template <typename T>
void
CSummarySpectrum<T>::ParametersToIds(const std::vector<CParameter>& pDefs) {
    for(auto& p : pDefs) {
        m_vParameters.push_back(p.getNumber());
    }
}
/**
 *  computeDefinition
 *   Compute the contents of m_defintion.  We assume that Spectrum.h,  by now, can compute the definition.
 * 
 * 
 */
template <typename T>
void
CSummarySpectrum<T>::computeDefinition() {
    m_definition = CSpectrum::GetDefinition();

    // Remove the X axis information and we're done:

    m_definition.nChannels.erase(m_definition.nChannels.begin());
    m_definition.fLows.erase(m_definition.fLows.begin());
    m_definition.fHighs.erase(m_definition.fHighs.begin());
}


