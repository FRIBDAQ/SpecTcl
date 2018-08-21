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

/** @file:  CCalibratedFileDrivenParameterMapper.h
 *  @brief: Use a file to define raw and calibrated DDAS parameters.
 */
#ifndef CCALIBRATEDFILEDRIVENPARAMETERMAPPER_H
#define CCALIBRATEDFILEDRIVENPARAMETERMAPPER_H

#include <ParameterMapper.h>
#include <DDASHit.h>
#include <string>
#include <vector>
#include <iostream>


class CEvent;
class CTreeParameter;
class CTreeVariable;

/**
 * @class CCalibratedFileDrivenParameterMapper
 *
 * I know that's a lot to type. Tough.  Users probably only have to type it
 * once.  You can pity me as I'll have to type it for each method implementation
 * and then some...not to mention documentation.
 *
 * Ok this class is very much l ike a CFileDrivenParameterMapper so please
 * have a peek at the comments in that file.  The difference, the
 * software will produce raw and calibrated parameters.  The calibration
 * is a general quadratic calibration but there are methods to make that
 * a linear if that's what you need.  Keep reading.
 *
 * The configuration file you have to prepare to use this class is much like
 * the one  you have to make for CFileDrivenParameterMapper.
 * Same commenting rules same old same old.  There are just
 * some additional fields.  The left 5 fields are the same as before;
 * crate slot channel bits  raw.parameter.name
 *  Additional fields are:
 *  -   constant coefficient
 *  -   linear coefficient
 *  -   quadratic coefficient
 *  -   low
 *  -   high
 *  -   chans
 *  -   units
 *  -   calibrated.parameter.name
 *
 *   Coefficients can be a constant number or a string.  If a string,
 *   the string represents the name of a tree variable from which the
 *   value will come at run time.  Thus you can force linear calibration
 *   by just letting the quadratic coefficient be 0.
 *   The low, high chans units values are used, along with the calibrated.parameter.name
 *   to construct the calibrated tree parameter and its metadata.  Note that
 *   units can be just an empty string ("").  Ok, so here's a few sample lines:
 *
 *   0 2 0 crate0.slot2.raw.chan0 c0.s2.ch0.const c0.s2.ch0.lin 0 0 100 100 MeV crate0.slot2.cal.chan0
 *
 *   This line unpackes the raw value of the energy for crate0 slot2 channel0
 *   into crate0.slot2.raw.chan0.  A linear calibration is done where the
 *   constant and linear coefficients are in the tree variables
 *   crate0.slot2.raw.chan0 c0.s2.ch0.const c0.s2.ch0.lin the zero for the quadratic
 *   coefficient is what forces this to linear.  The tree parameter will be made
 *   with low, high, bins of 0, 100, 100, and units of MeV.  The name of
 *   the calibrated tree parameter will be crate0.slot2.cal.chan0
 *
 *   Similarly:
 *
 *   0 2 0 crate0.slot2.raw.chan0 c0.s2.ch0.const c0.s2.ch0.lin c0.s2.ch0.q 0 100 100 MeV crate0.slot2.cal.chan0
 *
 *   Is the same definition but the quadratic coefficient of the calibration
 *   comes from the tree variable c0.s2.ch0.q
 *
 */
class CCalibratedFileDrivenParameterMapper : public DAQ::DDAS::CParameterMapper
{
    // Data types:
    
public:
    // These types get us to a description of a raw and calibrated parameter
    // and their relationship.

    typedef struct _Coefficient {
        double         s_const;
        CTreeVariable* s_pVariable;
    } Coefficient, *pCoefficient;
    
    
    typedef struct _Calibration {
        Coefficient s_const;
        Coefficient s_linear;
        Coefficient s_quadratic;
    } Calibration, *pCalibration;
    
    typedef struct _ParameterInfo {
        unsigned        s_crate;   //   |
        unsigned        s_slot;    //   > To allow building map from only these
        unsigned        s_channel; //   |
        CTreeParameter* s_raw;
        CTreeParameter* s_calibrated;
        Calibration     s_calibration;
    } ParameterInfo, *pParameterInfo;
    
    // Ok, now the constant time lookup stuff, very much like
    // CFileDrivenParameteMapper but with terminals in ParameterInfo:
    
    typedef std::vector<pParameterInfo>  ChannelMap;   // Channels in a module.
    typedef std::vector<ChannelMap>      SlotMap;      // Modules in a crate
    typedef std::vector<SlotMap>      CrateMap;     // Crates in a system.

    // actual member data:
    
private:
    std::string    m_configFile;            // Keep for error messages.
    CrateMap       m_map;                   // Data struct driving the decode.
    
    // Methods:
    
public:
    CCalibratedFileDrivenParameterMapper(const char* filename);
    virtual ~CCalibratedFileDrivenParameterMapper(); 
    
    virtual void MapToParameters(
        const std::vector<DAQ::DDAS::DDASHit>& hits, CEvent& rEvent
    );
    
    // map creation:

    void makeMap();
    ParameterInfo parseLine(const std::string& line, unsigned linenum);
    Coefficient   parseCoefficient(const std::string& field);
    
    void addParameter(const ParameterInfo& info);
    void addSlotMap(unsigned crate);
    void addChannelMap(unsigned crate, unsigned slot);
    
    // Hit to parameters:
    
    const ParameterInfo& getParameterInfo(
        unsigned crate, unsigned slot, unsigned chan
    );
    void unpackRaw(const DAQ::DDAS::DDASHit& hit, const ParameterInfo& info);
    void unpackCalibration(
        const DAQ::DDAS::DDASHit& hit, const ParameterInfo& info
    );
    double getCoefficient(const Coefficient& coef);
    
    // Error Reporting:
    
    std::string formatUnpackError(
        unsigned crate, unsigned slot, unsigned channel, const char* msg
    );
    std::string formatParseError(
        unsigned lineNum, const std::string& line, const char* msg
    );
    std::string formatStandardMessage(const char* msg);

};

#endif