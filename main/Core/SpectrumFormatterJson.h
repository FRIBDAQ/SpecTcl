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
// Class: CSpectrumFormatterJson                     //ANSI C++
//
// Base class of spectrum formatters.   Derivations
// of this class are responsible for reading and writing
// spectra from C++ streams.
//
//
// Author:
//     Ron Fox
//     NSCL
//     Michigan State University
//     East Lansing, MI 48824-1321
//     mailto: fox@nscl.msu.edu
//
// (c) Copyright NSCL 2023, All rights reserved SpectrumFormatter.h
//

#ifndef SPECTRUMFORMATTERJSON_H
#define SPECTRUMFORMATTERJSON_H

#include "SpectrumFormatter.h"
#include <vector>
#include <string>
#include <utility>

namespace Json {
    class Value;
}

/**
 *   Provide a spectrum formatter (read/write support)
 * for JSON formatted spectra.  The format used is the
 * same as the JSON format used by Rustogramer providing for
 * spectrum interchange between the two.  At the top level is
 * a JSON rendering that contains two objects: "definition"
 * which describes the spectrum properties and "channels"
 * which contains data for all spectrum channels that are non-zero.
 *
 * The "definition"  object has the following attributes:
 *
 *   *   "name"  - (string) name of the spectrum.
 *   *   "type_string" - (string) spectrum type e.g. "1" for 1d spectrum\
 *   *   "x_parameters" - (array of string) contains parameters on the X axis.
 *   *   "y_parameters" - (array of string) contains parameters on the y axis.
 *   *   "x_axis" - If not null a three element triplet providing
 *                The X axis defintion [low, high, number_of_bins].
 *   *   "y_axis"  - Same as x_axis but describes the y axis if any.
 *
 * "channels" is an array that contains an object for each
 * non-empty channel.  Each channel struct contains the following
 * attributes:
 *
 *   *  chan_type - (string) type of channel this is one of "Bin", "Underflow" or "Overflow"
 *   *  x_coord   - (double) The real coordinate of the left side of the channel.
 *   *  y_coord   - (double) The real coordinate of the bottom side of the channel.
 *   *  x_bin     - (size_t) The xaxis bin of the channel.
 *   *  y_bin     - (size_t) The y axis bin of the channel.
 *   *  value     - (size_t) number of counts in that channel.
 *
 * Here's a sample spectrum file with two spectra that have a spike
 * peak.
 * \verbatim
 * [
    {"definition":
       {"name":"1","type_string":"1",
       "x_parameters":["parameters.05"],
       "y_parameters":[],
       "x_axis":[0.0,1024.0,1026],
       "y_axis":null},
       "channels":[
        {"chan_type":"Bin",
         "x_coord":500.0,"y_coord":0.0,
         "x_bin":501,"y_bin":0,"value":163500}]},
    {"definition":
    {"name":"2","type_string":"2",
    "x_parameters":["parameters.05"],
    "y_parameters":["parameters.06"],
    "x_axis":[0.0,1024.0,1026],
    "y_axis":[0.0,1024.0,1026]},
    "channels":[
        {"chan_type":"Bin",
        "x_coord":500.0,"y_coord":600.0,
        "x_bin":501,"y_bin":602,"value":163500}]}
]
 * \endverbatim
 *
 * ### Notes:
 *   *  Note that the file is an array of spectra.
 *   *  Note that binning and channel number follow Root's channel
 * numbering - each axis has two extra one to the left for underflows,
 * one to the right for overflows so:
 * "x_axis":[0.0,1024.0,1026] means the axis real coordinates are from 0.0 - 1024.0
 *  with a one to one mapping of channels to coordinates (channel 1 is [0.0, 1.0) Channel 
 *  2 is [1.0, 2.0)  etc.
 *   *  Note how the "y_axis" value for the 1d spectrum has the value
 *  null - indicating it is not present.
 *   *  It's going to take some thinking to understand how to
 * get more than the first spectrum from a file given that 
 * the JSON library will read the entire file (the whole array of
 * spectra) and but get instantiated and called on a per sread
 * command basis...with a new istream each  time because
 * multiple spectrum reads are done by e.g.:
 * \verbatim
 *    set fd [open $file r]
 *    while {![eof $fd]} {
 *      sread ?options? $fd  
 *    }
 * \endverbatim
 * 
 * Each sread command constructs a new istream that is connected to 
 * the underlying Tcl_Chan represented by fd.  In JSon the first sread
 * will read the entire file into a vector of spectra and how do we
 * then connect the second ... sreads to that vector?  What state
 * can we hold?
*/
class CSpectrumFormatterJson : public CSpectrumFormatter {
private:
    
public:
    CSpectrumFormatterJson();
    virtual ~CSpectrumFormatterJson();
    CSpectrumFormatterJson(const CSpectrumFormatterJson& rhs);
    const CSpectrumFormatterJson& operator=(const CSpectrumFormatterJson& rhs);

    int operator==(const CSpectrumFormatterJson& rhs) const;
    int operator!=(const CSpectrumFormatterJson& rhs) const;

    // Implementation:

    virtual std::pair<std::string, CSpectrum *> Read(std::istream &rStream,
                                                     ParameterDictionary &rDict) = 0;
    virtual void Write(std::ostream &rStream, CSpectrum &rSpectrum,
                       ParameterDictionary &rDict) = 0;
private:
    // Utilities for generating output:

    Json::Value generateHeader(CSpectrum& rSpectrum, ParameterDictionary& pDict);
    std::pair<std::vector<std::string>, std::vector<std::string>>
        marshallParameters(CSpectrum& rSpectrum, ParameterDictionary& pdict);
    std::vector<std::string> 
        paramIdsToNames(const std::vector<UInt_t>& ids, ParameterDictionary& pdict);
    std::pair<Json::Value, Json::Value> getAxisDefinitions(CSpectrum& rSpectrum);
};


#endif