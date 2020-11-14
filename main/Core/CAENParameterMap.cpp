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

/** @file:  CAENParameterMap.cpp
 *  @brief: Implementation of the classes.
 */

#include "CAENParameterMap.h"
#include "CAENModuleHits.h"
#include "CAENHit.h"
#include <TreeParameter.h>   // Both CTreeParameter and the arrays.
#include <assert.h>
#include <stdexcept>

/////////////////////// CAENPHAArrayMapper  ///////////////////////

/**
 * constructor
 *    @param time - base parameter name for time
 *    @param energy - base parameter for energy.
 *    @param extra1 - Base parameter for extra1.
 *    @param extra2 - Base parameter for extra2.
 *
 *    @note if a parameter base name is not supplied no tree parameter
 *    array will be created for it and it will not be unpacked.
 *
 */
CAENPHAArrayMapper::CAENPHAArrayMapper(
    const char* time, const char* energy,
    const char* extra1, const char* extra2

) :
    m_pTimes(nullptr), m_pEnergies(nullptr),
    m_pExtras1(nullptr), m_pExtras2(nullptr)
{
    if (time) {
        m_pTimes = new CTreeParameterArray(time, 16, 0); 
    }
    if (energy) {
        m_pEnergies = new CTreeParameterArray(energy, 16, 16, 0);
    }
    if (extra1) {
        m_pExtras1 = new CTreeParameterArray(extra1, 16, 16, 0);
    }
    if (extra2) {
        m_pExtras2 = new CTreeParameterArray(extra2, 16, 16, 0);
    }
}
/**
 * destrutor destroy the arrays
 */
CAENPHAArrayMapper::~CAENPHAArrayMapper()
{
    delete m_pTimes;
    delete m_pEnergies;
    delete m_pExtras1;
    delete m_pExtras2;
}
/**
 * assignParameters
 *    Assign the parameters from a hit.
 * @param module - Hits from a module.
 */
void
CAENPHAArrayMapper::assignParameters(const CAENModuleHits& module)
{
    const auto& hits = module.getHits();
    for (int i =0; i < hits.size(); i++) {
        // Must be a PHA hit:
        
        const CAENPHAHit& hit(dynamic_cast<const CAENPHAHit&>(*hits[i]));
        unsigned ch = hit.getChannel();
        assert(ch < 16);             // Can't be bigger than 15.
        (*m_pTimes)[ch]    = hit.getTimeTag();
        (*m_pEnergies)[ch] = hit.getEnergy();
        (*m_pExtras1)[ch]  = hit.getExtra1();
        (*m_pExtras2)[ch]  = hit.getExtra2();
    }
}
//////////////////////////////////////////////////////////////////
// CAENPHAParameterMapper implementation.

/**
 * constructor
 *   In the vectors below several things are done to allow for
 *   disconnected, or unsed channnels:
 *   
 *   -   Any empty string implies an unused channel in that slot.
 *   -   If the vector is shorter than 16 elements, all unset channels
 *       are considered unused.
 *
 *  @note that depending on the needs/usage of the data there's
 *        no requirement that missing/unused channels be the same
 *        in all parameters.  This allows, for example,
 *        for a channel who's only purpose is to provide timing.
 *  @param times - names of the time parameters.  A tree parameter
 *        will be made for each with no resolution information.
 *  @param energies - names of the energy parameters.  A tree parameter
 *        with resolution 16 bits will be made for each used parameter.
 *  @param extras1 - names of the extra1 parametres.  A tree parameter
 *        with resolution 16 bits will be made for each used parameter.
 *  @param extras2 - Names of the extra2 parameters.
 *  @throws std::range_error - if any of the name vectors is longer than
 *        16 elements.
 */
CAENPHAParameterMapper::CAENPHAParameterMapper(
    const std::vector<std::string>& times,
    const std::vector<std::string>& energies,
    const std::vector<std::string>& extras1,
    const std::vector<std::string>& extras2
)
{
    // Range check the input arrays:
    
    std::string message(
        "There can be no more than 16 names in a parameter name vector"
    );
    if (times.size() > 16) throw std::range_error(message);
    if (energies.size() > 16) throw std::range_error(message);
    if (extras1.size() > 16) throw std::range_error(message);
    if (extras2.size() > 16) throw std::range_error(message);
    
    for (int i =0; i < 16; i++) {
        if((i < times.size() ) && times[i] != "") {
            m_Times.push_back(new CTreeParameter(times[i]));
        } else {
            m_Times.push_back(nullptr);
        }
        
        if ((i < energies.size()) && (energies[i] != "")) {
            m_Energies.push_back(new CTreeParameter(energies[i], 16 ));
        } else {
            m_Energies.push_back(nullptr);
        }
        
        if (i< (extras1.size()) && (extras1[i] != "")) {
            m_Extras1.push_back(new CTreeParameter(extras1[i], 16));
        } else {
            m_Extras1.push_back(nullptr);
        }
        
        if ((i < extras2.size()) && (extras2[i] != "")) {
            m_Extras2.push_back(new CTreeParameter(extras2[i], 16));
        } else {
            m_Extras2.push_back(nullptr);
        }
    }
    // Note that _all_ of the member arrays have 16 elements now.
    // we make use of that to simplify the destructor and
    // range checking later on:
}
/**
 * destructor
 */
CAENPHAParameterMapper::~CAENPHAParameterMapper()
{
    // Use the fact that all vectors have length 16.
    
    for (int i = 0; i < 16; i++) {
        delete m_Times[i];
        delete m_Energies[i];
        delete m_Extras1[i];
        delete m_Extras2[i];
    }
}
/**
 * assignParameters
 *   @param module - hits from a module
 */
void
CAENPHAParameterMapper::assignParameters(const CAENModuleHits& module)
{
    
    const auto& hits(module.getHits());
    for (int i = 0; i < hits.size(); i++) {
        // This throws a bad_cast if these are the wrong hit types:
        
        const CAENPHAHit& hit(dynamic_cast<const CAENPHAHit&>(*hits[i]));
        unsigned ch = hit.getChannel();
        assert(ch < 16);       // Uses that all vectors are fully stuffed.
        
        
        if (m_Times[ch])   *(m_Times[ch])    = hit.getTimeTag();
        if (m_Energies[ch]) *(m_Energies[ch]) = hit.getEnergy();
        if (m_Extras1[ch])  *(m_Extras1[ch] ) = hit.getExtra1();
        if (m_Extras2[ch])  *(m_Extras2[ch])  = hit.getExtra2();
    }
}
///////////////////////////////////////////////////////////////////
// Implemenatation of the CAENPSDArrayMapper.  This is a lot like
// CAENPHAArrayMapper but the parametes a PSD supplies are different.
// Note for time, the assumption is that there's a CFD time which
// provides a fine grained time value.  Users can subclass this
// to pull out the raw timetag instead or as well.
//

/**
 * constructor
 *   @param time - base name of the time tree parameter array.
 *   @param shortGate - base name of the short gate integration parameter array.
 *   @param longGate  - base name of the long gate integration parameter array.
 *   @param baseline  - base name of the baseline value array.
 *   @param pur       - base name of the pile up rejection parameter.
 *
 * @note - the pile up rejection parameter is actually a boolean that
 *         gives the state of pile up rejection (0 if not nonzero if so).
 *         The parameters for PUR will be unpacked so that their values
 *         are 20 for no pile up rejection and 40 if there is pile up rejection.
 *         The parameter range will allow for those channels, in a default
 *         spectrum to be relatively central (e.g. 100 bins 0-99).
 *  @note - if a parameter array is not desired, a null pointer for the
 *          name prevents creation.
 */
CAENPSDArrayMapper::CAENPSDArrayMapper(
     const char* time, const char* shortGate, const char* longGate,
     const char* baseline,
     const char* pur
) :
    m_pShortGate(nullptr), m_pLongGate(nullptr), m_pBaseline(nullptr),
    m_pTime(nullptr), m_pPURFlag(nullptr)
{
    if (time) m_pTime = new CTreeParameterArray(time, 16, 0);
    if (shortGate) m_pShortGate = new CTreeParameterArray(shortGate, 16, 16, 0);
    if (longGate) m_pLongGate = new CTreeParameterArray(longGate, 16, 16, 0);
    if (baseline) m_pBaseline = new CTreeParameterArray(baseline, 16, 16, 0);
    if (pur) m_pPURFlag = new CTreeParameterArray(pur, 100, 0, 99, "", 16, 0);    
}
/**
 * destructor
 */
CAENPSDArrayMapper::~CAENPSDArrayMapper()
{
    delete m_pTime;
    delete m_pShortGate;
    delete m_pLongGate;
    delete m_pBaseline;
    delete m_pPURFlag;
}
/**
 * assignParameter
 *   @param module - container of module hits.
 */
void
CAENPSDArrayMapper::assignParameters(const CAENModuleHits& module)
{
    const auto& hits = module.getHits();
    for (int i =0; i < hits.size(); i++) {
        // Hits must be PSDhits:
        
        const CAENPSDHit& hit(dynamic_cast<const CAENPSDHit&>(*hits[i]));
        unsigned ch = hit.getChannel();
        assert (ch < 16);
        if (m_pShortGate) (*m_pShortGate)[ch] = hit.getShortCharge();
        if (m_pLongGate)  (*m_pLongGate)[ch]  = hit.getLongCharge();
        if (m_pBaseline)  (*m_pBaseline)[ch]  = hit.getBaseline();
        if (m_pPURFlag)   {
            (*m_pPURFlag)[ch]   = (hit.getPURFlag() ? 20 : 40);
        }
        if (m_pTime) (*m_pTime)[ch] = hit.getTime();
        
    }
}
////////////////////////////////////////////////////////////////
// Implemente CAENPSDParameterMapper
//  Note the same rules for leaving out parameters hold as for
//  CAENPHAParameterMapper.
//

/**
 * constructor
 *    @param shortGates -names of short gate parameters.
 *    @param longGates  -names of the long gate parameters.
 *    @param baselines  -Names of the baseline parameters
 *    @param times      -Names of the full time parameters.
 *    @param pur        -Names of the pile up rejection flag parameters.
 */
CAENPSDParameterMapper::CAENPSDParameterMapper(
    const std::vector<std::string>& shortGates,
    const std::vector<std::string>& longGates,
    const std::vector<std::string>& baselines,
    const std::vector<std::string>& times,
    const std::vector<std::string>& pur
) : m_shortGate(16, nullptr), m_longGate(16, nullptr),
    m_baseline(16, nullptr), m_time(16, nullptr),
    m_PUR(16, nullptr)
    
{
    // The vectors were construted to contain exactly 16 nulls.
    // that makes this loop simpler.
    
    for (int i = 0; i < 16; i++) {
        if ((i < shortGates.size()) && (shortGates[i] != "")) {
            m_shortGate[i] = new CTreeParameter(shortGates[i], 16);
        }
        if ((i < longGates.size()) && (longGates[i] != "")) {
            m_longGate[i] = new CTreeParameter(longGates[i], 16);
        }
        if ((i < baselines.size()) && (baselines[i] != "")) {
            m_baseline[i] = new CTreeParameter(baselines[i], 16);
        }
        if ((i < times.size()) && (times[i] != "")) {
            m_time[i] = new CTreeParameter(times[i]);   // No range suggestion
        }
        if ((i < pur.size()) && (pur[i] != "")) {
            m_PUR[i] = new CTreeParameter(pur[i], 16);
        }
    }
}
/**
 * destructor
 *   Just delete all the tree parameters.
 */
CAENPSDParameterMapper::~CAENPSDParameterMapper()
{
    for (int i =0; i < 16; i++) {
        delete m_shortGate[i];
        delete m_longGate[i];
        delete m_baseline[i];
        delete m_time[i];
        delete m_PUR[i];
    }
}
/**
 * assignParameters
 *    Given a set of raw hits, assigns them to the appropriate parameters.
 *  @param module - container for the hits.
 *  @throw std::invalid_cast if the hits are not PSD hits
 */
void
CAENPSDParameterMapper::assignParameters(const CAENModuleHits& module)
{
    const auto& hits = module.getHits();
    for (int i =0; i < hits.size(); i++) {
        const CAENPSDHit& hit = dynamic_cast<const CAENPSDHit&>(*hits[i]);
        
        int c = hit.getChannel();
        if (m_shortGate[c]) *(m_shortGate[c]) = hit.getShortCharge();
        if (m_longGate[c])  *(m_longGate[c])  = hit.getLongCharge();
        if (m_baseline[c])  *(m_baseline[c])  = hit.getBaseline();
        if (m_time[c])      *(m_time[c])      = hit.getTime();
        if (m_PUR[c])       *(m_PUR[c])       = (hit.getPURFlag() ? 20: 40);
    }
}
