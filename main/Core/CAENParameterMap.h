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

/** @file:  CAENParameterMap.h
 *  @brief: Map hits in a specific CAEN digitizer into parameters.
 *
 */
#ifndef CAENPARAMETERMAP_H
#define CAENPARAMETERMAP_H
#include <vector>
#include <string>

class CTreeParameter;
class CTreeParameterArray;
class CAENModuleHits;

/**
 * @class CAENParameterMap
 *    This is an abstract base class for the unpackers
 *    for digitizers.  The concrete subclasses specialize for
 *    the digitizer firmwares, each of which provides different
 *    parameters.
 *
 *    This class simply establishes an interface for assigning parameters
 *    from a CAENModuleHits object.    That interface must be implemented
 *    by each concrete subclass.  Polymophism then allows us to put these
 *    into some container that unpacks and assigns events to all
 *    modules present in an event built event (think about combining e.g.
 *    CAENParser and a collection of these).
 *
 *    If one needs to produce computed parameters I can suggest one
 *    of two approaches:
 *
 *    1.  An additional event processor registered to run
 *        after the event processors containing these classes
 *        this is essentially necessary if the computed parameters
 *        span module boundaries.
 *    2.  A derived class whose assignParameters method first invokes
 *        the base class assignParameters method and then performs
 *        whatever computations are required.  This is suitable
 *        for cases where the computed parameters don't cross
 *        module boundaries.
 *    3.  A container class that has more than one of these internally
 *       (this is actually  how the stock unpacker works).  Once the results
 *       of CAENParser are applied to all module parameter mappers,
 *       the computed parameters are created.  This also allows the
 *       computed parameters to span module boundaries and is functionally
 *       quite similar to 1. above.
 *
 *    In all cases remember that if two tree parameters have the same
 *    name, they will refer to the same SpecTcl parameter.
 *
 *    There is, of course, nothing to stop you from deriving your own
 *    application specific parameter map class if none of the
 *    provided ones are suitable.
 */
class CAENParameterMap
{
public:
    virtual ~CAENParameterMap() {}           // Supports polymorhphic destructors.
    
    virtual void assignParameters(const CAENModuleHits& module) = 0;
};

/**
 * This seems senseless but it's a mechanism to support
 * determining if a mapper is for PHA firmware.  We have to supply
 * at least one virtual method *implementation* for dynamic cast to
 * work.
 */

class CAENPHAMapper : public CAENParameterMap
{
public:
    virtual void assignParameters(const CAENModuleHits& module) {}
};

/**
 * @class CAENPHAArrayMapper
 *   This pre-built parameter mapper takes four parameter names 
 *   and builds four tree parameter arrays for them indexed by
 *   the channel number in the module.  The parameter names unpack
 *   to the timestamp, the energy, extra1, and extra2.
 *
 *  @note  - just because these parameters are unpacked does not mean
 *           you need to histogram them -- after all you may not
 *           actually have enabled extra1 and extra2.
 */
class CAENPHAArrayMapper : public CAENPHAMapper
{
protected:
    CTreeParameterArray* m_pTimes;
    CTreeParameterArray* m_pEnergies;
    CTreeParameterArray* m_pExtras1;
    CTreeParameterArray* m_pExtras2;
    CTreeParameterArray* m_pHpTimes;
public:
    CAENPHAArrayMapper(
        const char* time, const char* energy,
        const char* extra1, const char* extra2,
        const char* hptime
    );
    virtual ~CAENPHAArrayMapper();
    virtual void assignParameters(const CAENModuleHits& module);
};

/**
 * @class CAENPHAParameterMapper
 *    Generic parameter mapper.  The user supplies parameter names
 *    for every time, energy, extra1, exta2 channel and
 *    we unpack into those.  The only thing we control is the
 *    range/binning of the energies, and extras.  No range information
 *    is used for the times.
 *  @note  There's no obligation to provide all parameters:
 *       - Too few parameters and we ignore hits in the channels
 *         beyond what's supplied.
 *       - Empty parameter names indicate an unused or uninteresting
 *         value.
 */
class CAENPHAParameterMapper : public CAENPHAMapper
{
protected:
    std::vector<CTreeParameter*> m_Times;
    std::vector<CTreeParameter*> m_Energies;
    std::vector<CTreeParameter*> m_Extras1;
    std::vector<CTreeParameter*> m_Extras2;
    std::vector<CTreeParameter*> m_hpTimes;
public:
    CAENPHAParameterMapper(
        const std::vector<std::string>& times,
        const std::vector<std::string>& energies,
        const std::vector<std::string>& extras1,
        const std::vector<std::string>& extras2,
        const std::vector<std::string>& hptimes
    );
    virtual ~CAENPHAParameterMapper();
    
    virtual void assignParameters(const CAENModuleHits& module);
};

// Similarly this base class allows us to determin if a an object
// is a mapper for PSD

class CAENPSDMapper : public CAENParameterMap
{
public:
    virtual void assignParameters(const CAENModuleHits& module) {}
};

/**
 * @class CAENPSDArrayMapper
 *    Same as for CAENPHAArrayMapper but we're unpacking
 *    PSD modules. This class has parameters for the
 *    short get, long gate, baseline, full fine time and
 *    optionally the pileup rejection flag.  See the
 *    internal implementation details about how the
 *    PUR flag parameter is handled.
 */
class CAENPSDArrayMapper : public CAENPSDMapper
{
protected:
    CTreeParameterArray* m_pShortGate;
    CTreeParameterArray* m_pLongGate;
    CTreeParameterArray* m_pBaseline;
    CTreeParameterArray* m_pTime;
    CTreeParameterArray* m_pPURFlag;
public:
    CAENPSDArrayMapper(
        const char* time, const char* shortGate, const char* longGate,
        const char* baseline,
        const char* pur = 0     // Default is not defined.
    );
    virtual ~CAENPSDArrayMapper();
    
    virtual void assignParameters(const CAENModuleHits& module);
};
/**
 * CAENPSDParameterMapper
 *    As with CENPHAParamterMapper, this class allows individual
 *    parameter names to be assigned to individual parameters.
 *    See CAENPHAParamterMapper for information about how to
 *    keep parameters from being assigned values by
 *    the mappers.
 */
class CAENPSDParameterMapper : public CAENPSDMapper
{
protected:
    std::vector<CTreeParameter*> m_shortGate;
    std::vector<CTreeParameter*> m_longGate;
    std::vector<CTreeParameter*> m_baseline;
    std::vector<CTreeParameter*> m_time;
    std::vector<CTreeParameter*> m_PUR;
public:
    CAENPSDParameterMapper(
        const std::vector<std::string>& shortGates,
        const std::vector<std::string>& longGates,
        const std::vector<std::string>& baselines,
        const std::vector<std::string>& times,
        const std::vector<std::string>& pur
    );
    virtual ~CAENPSDParameterMapper();
    
    virtual void assignParameters(const CAENModuleHits& module);
};
#endif