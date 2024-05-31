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

/** @file:  CFileDrivenParameterMapper.h
 *  @brief: Do parameter mapping for DDASBuiltUnpacker and DDSUnpacker.
 */
#ifndef CFILEDRIVENPARAMETERMAPPER_H
#define CFILEDRIVENPARAMETERMAPPER_H

#include "ParameterMapper.h"
#include <string>

class CTreeParameter;

/**
 * @class  CFileDrivenParameterMapper
 *
 *   Provides a parameter mapper for unpacking a hit into a set of parameters.
 *   The unpacking is defined by a file which describes for a crate/slot/channel
 *   the name of a parameter into which the energy parameter of the hit is
 *   unpacked.  The format of the file is pretty simple:
 *   Each line contains five whitespace separated fields.  These are in order
 *   the Crate number, the slot number the channel number, the number of bits
 *   in the channel (used to set tree parameter limits) and the parameter name.
 *   Note that tree parameter arrays are just parameters with a base name, a
 *   . and an element number.  E.g. a.b.c.00 is offset 0 in a tree parameter array.
 *   Note that some care must be taken to ensure the proper number of digits
 *   are supplied if this is to be mapped to someone else's tree parameter array.
 */
class CFileDrivenParameterMapper : public DAQ::DDAS::CParameterMapper {
private:
    typedef std::vector<CTreeParameter*>   ChannelMap;   // indices are channels
    typedef std::vector<ChannelMap>        SlotMap;         // indices are slots
    typedef std::vector<SlotMap>     CrateMap;        // indices are crates.
private:
    std::string m_configFile;                              // Held until we can use it.
    CrateMap    m_map;                                     // Mapping built here.
    
public:        // API.
    
    CFileDrivenParameterMapper(const char* configFile);
    virtual ~CFileDrivenParameterMapper();
    virtual void mapToParameters(
        const std::vector<DAQ::DDAS::DDASHit>& hits, CEvent& rEvent
    );
    
private:
    
    // Map creation.
    
    void makeMap();
    void addParameter(
        unsigned crate, unsigned slot, unsigned channel, unsigned bits,
        std::string parameterName
    );
    void addSlotMap(unsigned crate);
    void addChannelMap(unsigned crate, unsigned slot);
    
    // Error reporting.
    
    std::string formatUnpackError(unsigned crate, unsigned slot, unsigned channel);
    std::string formatParseError(unsigned lineNum, std::string line, const char* msg);
    std::string formatStandardMessage(const char* msg);

};



#endif