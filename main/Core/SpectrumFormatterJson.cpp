
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

#include "SpectrumFormatterJson.h"
#include "Spectrum.h"
#include "SpectrumFormatError.h"

#include <string>
#include <vector>
#include <sstream>
#include <json/writer.h>
#include <json/reader.h>

/**
 * Construct the formatter.  For the most part
 * we just let the base class do its work.
*/
CSpectrumFormatterJson::CSpectrumFormatterJson() :
    CSpectrumFormatter("JavaScript Object notation (json)", ".json")
 {}
 /**
  *  Destruct the formatter.  Again the base class can do all
  * the work:
  * 
 */
CSpectrumFormatterJson::~CSpectrumFormatterJson() {}

/** 
 * Copy constructor - again the base class handles that (for now)
*/
CSpectrumFormatterJson::CSpectrumFormatterJson(const CSpectrumFormatterJson& rhs) :
    CSpectrumFormatter(rhs) {}

/**
 *  assignment - again as of this point handled by the base class
*/
const CSpectrumFormatterJson& 
CSpectrumFormatterJson::operator=(const CSpectrumFormatterJson& rhs) {
    CSpectrumFormatter::operator=(rhs);
    return *this;
}
/**
 * Equality comparison:
*/
int 
CSpectrumFormatterJson::operator==(const CSpectrumFormatterJson& rhs) const {
    return CSpectrumFormatter::operator==(rhs);
}
/**
 *  Inequality comparison
 * 
*/
int 
CSpectrumFormatterJson::operator!=(const CSpectrumFormatterJson& rhs) const {
    return CSpectrumFormatter::operator!=(rhs);
}

/**
 * Read
 *    Reads a spectrum from file.  
 * 
 * @param  rStream - refernce to the stream containing spectra in Json
 * @param  rDict   - The SpecTcl parameter dictionary.
 * @return std::pair<std::string, CSpectrum *> - Pair containing the final name 
 * of the read in spectrum and a dynamically created anonymous spectrum.
 * See CNSCLAsciiSpectrumFormatter::Read for the reason
 * this is anonymous.
 * @throw CSpectrumFormatError if the JSON cannot be properly read
 *   or does not contain all the stuff we need.
 * 
*/
std::pair<std::string, CSpectrum *>
CSpectrumFormatterJson::Read(
    std::istream &rStream,
    ParameterDictionary &rDict) {

    throw CSpectrumFormatError(CSpectrumFormatError::IncompatibleFormat, "Json read not yet implemented");
}
/**
 *  Write the specified spectrum to file
 * 
 * @param rStream - references the stream to which to write the
 * spectrum.  
 * @param rSpectrum - References the spectrum to write.
 * 
*/
void
CSpectrumFormatterJson:: Write(
    std::ostream &rStream, CSpectrum &rSpectrum,
    ParameterDictionary &rDict) {

        Json::Value outvec(Json::arrayValue);

        Json::Value spectrum;
        Json::Value description;
        description["name"] = rSpectrum.getName();
        std::stringstream type_name;
        type_name << rSpectrum.getSpectrumType();
        description["type_string"] = type_name.str();
        Json::Value contents;

        spectrum["description"] = description;
        spectrum["channels"]   = contents;

        outvec.append(spectrum);


        rStream << outvec;

    }