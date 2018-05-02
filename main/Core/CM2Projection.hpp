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

/** @file:  CM2Projection.hpp
 *  @brief: Implementation of the CM2Projection templated class.
 *  @note:  We get included into the header so we don't need to
 *          include the header as most class implementations do.
 */
#define CM2PROJECTION_HPP              // Include guard.

#include "Event.h"
#include "GateContainer.h"
#include "PointlistGate.h"
#include <RangeError.h>
#include <TDirectory.h>

#include <errno.h>
#include <ErrnoException.h>

/**
 * constructor
 *    Only one sort of constructor is defined as the others normally
 *    supplied can be implemented in terms of it:
 *
 * @param name     - Spectrum name.
 * @param nId      - Spectrum id
 * @param xParams  - Vector of x parameter ids.
 * @param yParams  - Vector of y parameter ids (must be same size as xParams).
 * @param roiGates - Set of contours that define the region of interest.
 *                   must be the same length as the xParams as well, all gates
 *                   must be contours and defined on a pair of X/Y parameters.
 * @param nChannels - Number of channels on the (X) axis.
 * @param fLow      - Real coordinate low limits of the (x) axis.
 * @param fHigh     - ReAL coordinate upper limit of the (y) axis.
 * @param units     - Axis parameter units of measure.
 */
template<typename T, typename R>
CM2Projection<T,R>::CM2Projection(
       const std::string name, UInt_t nId,
        const std::vector<UInt_t>& xParams, const std::vector<UInt_t>& yParams,
        const std::vector<CGateContainer*> roiGates,
        UInt_t nChannels, Float_t fLow, Float_t fHigh,
        std::string units, Bool_t x    
) : CSpectrum(name, nId),
  m_nChannels(nChannels+2),            // Root under/overflow channels.
  m_x(x)
{
    AddAxis(nChannels, fLow, fHigh, units);
    
    // Check the size of the y parameter and gate lists are the same.
    
    if (xParams.size() != yParams.size()) {
        throw CRangeError(
            xParams.size(), xParams.size(), yParams.size(),
            "Providing y parameters to M2 projection spectrum"
        );
    }
    if (xParams.size() != roiGates.size()) {
        throw CRangeError(
            xParams.size(), xParams.size(), roiGates.size(),
            "Providing ROI gates to M2 projection spectrum"
        );
    }
    // Marshal the parameter pairs.
    
    for (int i =0; i < xParams.size(); i++) {
        std::pair<UInt_t, UInt_t> xy(xParams[i], yParams[i]);
        m_XYParameterPairs.push_back(xy);
    }
    OrderGateContainers(roiGates);
    // Create the root spectrum:
    
    std::string olddir = gDirectory->GetPath();
    gDirectory->Cd("/");
    
    R* pRootSpectrum = new R(
        name.c_str(), name.c_str(), nChannels,
        static_cast<Double_t>(fLow), static_cast<Double_t>(fHigh)
    );
    pRootSpectrum->Adopt(0, nullptr);              // Release root storage.
    setRootSpectrum(pRootSpectrum);
    gDirectory->Cd(olddir.c_str());
    
    CreateStorage();                              // Allocate actual storage.
    
}
/**
 *  Destructor
 *      Keep root from destroying my storage. Base class will free it if
 *      we're not in Xamine e.g.
 */
template<typename T, typename R>
CM2Projection<T,R>::~CM2Projection()
{
    R* pRootSpectrum = reinterpret_cast<R*>(getRootSpectrum());
    pRootSpectrum->fArray = nullptr;
}
/**
 * Increment
 *    Manage the increment of the spectrum.  Any gate applications on this
 *    spectrum have already been evaluated.
 *    Foreach X/Y parameter pair increment if:
 *    -  Both are valid.
 *    -  Both satisfy their contour
 *
 *   OrderGateContainers has ensured that the m_Gates vector indexing matches
 *   the m_XYParameterPairs indexing.
 */
template<typename T, typename R>
void
CM2Projection<T,R>::Increment(const CEvent& rE)
{
    CEvent rEvent((CEvent&)rE);            // Since CEvent::operator[] isn't const
    
    for (int i = 0; i < m_XYParameterPairs.size(); i++) {
        UInt_t xid = m_XYParameterPairs[i].first;
        UInt_t yid = m_XYParameterPairs[i].second;
        size_t esize = rEvent.size();
        
        if ((xid < esize) && (yid < esize)) {
            if (rEvent[xid].isValid() && rEvent[yid].isValid()) {
                if ((*m_Gates[i])(rEvent)) {
                    CParameterValue& v(rEvent[m_x ? xid : yid]);
                    getRootSpectrum()->Fill(v);
                }
            }
        }
        
    }
}
/**
 * operator[]
 *    Fetch a channel from the spectrum.
 *
 *  @param pIndices - array of indices.  In this case only the first index
 *                    matters.
 *  @return ULong_t  - The channel value.
 */
template<typename T, typename R>
ULong_t
CM2Projection<T,R>::operator[](const UInt_t* pIndices) const
{
    // Note the +1 in the get bin is because Root reserves channel 0 for
    // an overflow counter.
    
    const TH1* pRootSpectrum = getRootSpectrum();
    return static_cast<ULong_t>(
        pRootSpectrum->GetBinContent(pRootSpectrum->GetBin(pIndices[0]+1)));
}
/**
 * set
 *     Set a channel of the spectrum to the specified value;
 *  @param pIndices - Pointer to the channel number to set.
 *  @param nValue   - Value to put in that channel.
 */
template<typename T, typename R>
void
CM2Projection<T,R>::set(const UInt_t* pIndices, ULong_t nValue)
{
    TH1* pRootSpectrum = getRootSpectrum();
  pRootSpectrum->SetBinContent(
    pRootSpectrum->GetBin(pIndices[0] + 1), static_cast<Double_t>(nValue)
  );
}
/**
 *  usesParameter
 *     @param id - id of a parameter.
 *     @return bool Bool_t kfTRUE if the spectrum uses the parameter.
 */
template<typename T, typename R>
Bool_t
CM2Projection<T,R>::UsesParameter(UInt_t id) const
{
    for (int i = 0; i < m_XYParameterPairs.size(); i++) {
        const std::pair<UInt_t, UInt_t>& xy(m_XYParameterPairs[i]);
        if ((id == xy.first) || (id == xy.second)) return kfTRUE;
    }
    return kfFALSE;
}
/**
 * GetParameterIds
 *    Make a vector of the parameter ids used by this spectrum.
 *
 * @param ids - Reference to a vector that will be cleared and filled with the
 *              ids.
 */
template<typename T, typename R>
void
CM2Projection<T,R>::GetParameterIds(std::vector<UInt_t>& ids)
{
    ids.clear();
    for (int i = 0; i < m_XYParameterPairs.size(); i++) {
        std::pair<UInt_t, UInt_t>& xy(m_XYParameterPairs[i]);
        ids.push_back(xy.first);
        ids.push_back(xy.second);
    }
}
/**
 * GetResolutions
 *    @param res - this vector will be cleared an the number of channels
 *                 pushed into it.  Note that the number of channels will include
 *                 the extra under/overflow channels Root creates.
 */
template<typename T, typename R>
void
CM2Projection<T,R>::GetResolutions(std::vector<UInt_t>& res)
{
    res.clear();
    res.push_back(m_nChannels);
}
/**
 *  Dimension
 *     @param n - which dimension (only 0, X makes sense).
 *     @return Size_t - Number of channels in that dimension.  For 0,
 *                      the number of axis channels (including roo'ts over/underflow
 *                      channels) are returned.  Otherwise, the constant 1 is
 *                      returned reflecting that a 1-d spectrum looks like a 2-d
 *                      spectrum with only one row of channels.
 */
template<typename T, typename R>
Size_t
CM2Projection<T,R>::Dimension(UInt_t n) const
{
    return (n == 0) ? m_nChannels : 1;
}
/**
 * Dimensionality
 *    @return UInt_t - number of dimensinos in the spectrum (1).
 */
template<typename T, typename R>
UInt_t
CM2Projection<T,R>::Dimensionality() const
{
    return 1;
}
/**
 * GetDefinition
 *     @return CSpectrum::SpectrumDefinition&
 *         Builds and returns a reference to our spectrum definition.
 *         To satsify scoping (in case a ref is received), the definition is
 *         built in m_def.  Note that for this spectrum type the information
 *         in the definition is not the only thing needed to describe the
 *         spectrum.  The x flag is needed as are the list of contours.
 */
template<typename T, typename R>
CSpectrum::SpectrumDefinition&
CM2Projection<T,R>::GetDefinition()
{
    // Clear the vector elements:
    
    
    m_def.vParameters.clear();
    m_def.vyParameters.clear();
    m_def.nChannels.clear();
    m_def.fLows.clear();
    m_def.fHighs.clear();
    
    // Simple stuff:
    
    m_def.sName = getName();
    m_def.nId   = getNumber();
    m_def.eType =  getSpectrumType();
    m_def.eDataType = StorageType();
    
    for (int i = 0; i < m_XYParameterPairs.size(); i++) {
        std::pair<UInt_t, UInt_t>& xy(m_XYParameterPairs[i]);
        m_def.vParameters.push_back(xy.first);
        m_def.vyParameters.push_back(xy.second);
    }
    
    m_def.nChannels.push_back(m_nChannels-2);   // I am (g)root
    m_def.fLows.push_back(GetLow(0));
    m_def.fHighs.push_back(GetHigh(0));
    
    
    return m_def;
}
/**
 * needParameter
 *    @return Bool_t fkFALSE - since there's no single required parameter to
 *               increment us so we have to go in the uncategorized set of
 *               spectra.
 */
template<typename T, typename R>
Bool_t
CM2Projection<T,R>::needParameter() const
{
    return kfFALSE;
}
/**
 * getSpectrumType
 *    @return ke2DmProj - 2dm projection spectrum.
 */
template<typename T, typename R>
SpectrumType_t
CM2Projection<T,R>::getSpectrumType()
{
    return ke2DmProj;
}
/**
 * setStorage
 *    Called when the storage is being changed (e.g. from local to shared
 *    or back)
 *
 *  @param pStorage - pointer to the new spectrum storage.
 */
template<typename T, typename R>
void
CM2Projection<T,R>::setStorage(Address_t pStorage)
{
    R* pRootSpectrum = reinterpret_cast<R*>(getRootSpectrum());
    pRootSpectrum->fArray = reinterpret_cast<T*>(pStorage);
    pRootSpectrum->fN     = m_nChannels;
}
/**
 * StorageNeeded
 *     
 *
 *  @return Size_t - The number of bytes of storage required for the spectrtum.
 */
template<typename T, typename R>
Size_t
CM2Projection<T,R>::StorageNeeded() const
{
    return sizeof(T)*m_nChannels;     // m_nChannels already has the +2 root adjust.
}
/**
 * isXprojection
 *     @return Bool_t - kfTRUE if this is an X projection else kfFALSE
 */
template<typename T, typename R>
Bool_t
CM2Projection<T,R>::isXprojection() const
{
    return m_x;
}
/**
 * getRoiGates
 *    @return std::vector<CGateContainer*> - vector of gates that determine
 *                                           if parameter pairs increment.
 *    @note - these gates are not guaranteed to be returned in the order in which
 *            they were passed to the constructor.
 */
template<typename T, typename R>
std::vector<CGateContainer*>
CM2Projection<T,R>::getRoiGates() const
{
    return m_Gates;
}

/*-----------------------------------------------------------------------------
 *   Private method implementations:
 */

/**
 * CreateStorage
 *    Creates appropriate storage and sets the storage type.
 *    Note that this requires a switch on the type.  If storage types
 *    other than those in the typedef (well actually those in the switch)
 *    are used this method must be modified to handle that.
 */
template<typename T, typename R>
void
CM2Projection<T,R>::CreateStorage()
{
     // Figure out the storage type:
     switch (sizeof(T)) {
        case sizeof(UChar_t):
            setStorageType(keByte);
            break;
        case sizeof(UShort_t):
            setStorageType(keWord);
            break;
        case sizeof(Int_t):
            setStorageType(keLong);
            break;
        default:
            setStorageType(keUnknown_dt);
     }
     
     T* pStorage = new T[m_nChannels];
     ReplaceStorage(pStorage);
     Clear();
     createStatArrays(1);            // Actually obsolete but...
}
/**
 * ORderGateContainers
 *   This method assumes that the m_XYParameterPairs array has been setup.
 *   -  An exception is thrown if the gate vector is not the same size as
 *      m_XYParametersPairs.
 *   - An exception is thrown if any gate is not a contour.
 *   -  For each parameter pair in m_XYParametersPairs;
 *      *  The corresponding gate is located and pushed back into the m_Gates vector.
 *      *  If no correponding gateis located, an exception is thrown.
 *
 *  The end result of this is that m_Gates is ordered so that for any index
 *  in m_XYParameterPairs, the corresponding index in m_Gates has the
 *  gate container to determine if that parameter is to be incremented.
 *
 *  @param gates - the gates to reorder.
 */
template<typename T, typename R>
void
CM2Projection<T,R>::OrderGateContainers(const std::vector<CGateContainer*>& gates)
{
    // Ensure gate vector is the right size:
    
    if (gates.size() != m_XYParameterPairs.size()) {
        throw CRangeError(
            m_XYParameterPairs.size(), m_XYParameterPairs.size(), gates.size(),
            "Checking gates for M2Projectionspectrum"
        );  
    }
    // Ensure the gates are all contours:
    
    for(int i =0; i < gates.size(); i++) {
        if ((*gates[i])->Type() != "c") {
            std::string doing("Checking that gate ");
            doing += gates[i]->getName();
            doing += " was a contour";
            errno = EINVAL;
            throw CErrnoException(doing);
        }
    }
    
    /* Now we can re-order the gates as they're all legal -- assuming
       we have one for each parameter pair that is.  This code is O(n^2)
       But it's not so clear that there's anything simpler/better and, since
       it only runs at construction time, it's not really that critical.
       Note as well ther'es a vulnerability of some sort...or maybe a feature.
       Once the containers have been set as the gates, the user could modify
       the underlying gates so that they're not only  no longer contours,
       but don't even specifically depend on the parmaeter pairs.  This
       could allow either for confusion or for a very flexible gating
       of the individual parameter pair increments depending on your point of
       view and on the intentionality of the operation that changed the gates.
      
        Enough talk.  Finally some action
    */
    for (int p = 0; p < m_XYParameterPairs.size(); p++) {
        UInt_t pXid = m_XYParameterPairs[p].first;
        UInt_t pYid = m_XYParameterPairs[p].second;
        for (int g=0; g < gates.size(); g++) {
            CPointListGate* pGate =
                reinterpret_cast<CPointListGate*>(gates[g]->getGate());
            UInt_t gXid = pGate->getxId();
            UInt_t gYid = pGate->getyId();
            
            if (((gXid == pXid) && (gYid == pYid)) ||
                ((gXid == pYid) && (gYid == pXid))) {
                m_Gates.push_back(gates[g]);
                break;
            }
        }
    }
}