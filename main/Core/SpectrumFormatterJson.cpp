
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
#include "Parameter.h"
#include <histotypes.h>

// Root definitions:

#include <TH1.h>
#include <TAxis.h>

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

        Json::Value spectrum(Json::objectValue);
        
        Json::Value contents = getSpectrumContents(rSpectrum);

        spectrum["description"] = generateHeader(rSpectrum, rDict);
        spectrum["channels"]   = contents;

        outvec.append(spectrum);


        rStream << outvec;

    }

    ////////////////////////////////////////////////////
    // Utilities:

    /***
     * generateHeader
     *    Generate a Json::Value that contains a spectrum's
     *   description.
     * 
     * @param rSpectrum - References the spectrum.
     * @param pDict  - reference to the parameter dictionary.
     * 
     * @return Json::Value - contains the contents of the 'description'
     *   attribute of the spectrum object:
    */
   Json::Value
   CSpectrumFormatterJson::generateHeader(CSpectrum& rSpectrum, ParameterDictionary& pdict) {
        Json::Value description(Json::objectValue);
        description["name"] = rSpectrum.getName();
        std::stringstream type_name;
        type_name << rSpectrum.getSpectrumType();
        description["type_string"] = type_name.str();

        // How we marshall the set of parameters to x/y parameters
        // all depends on the spectrum type:

        auto xyparams = marshallParameters(rSpectrum,  pdict);
        Json::Value xpnames(Json::arrayValue);
        for (auto name : xyparams.first) {
            xpnames.append(Json::Value(name));
        }
        Json::Value ypnames(Json::arrayValue);
        for (auto name : xyparams.second) {
            ypnames.append(Json::Value(name));
        }
        description["x_parameters"] = xpnames;
        description["y_parameters"] = ypnames;

        auto axes = getAxisDefinitions(rSpectrum);
        description["x_axis"] = axes.first;
        description["y_axis"] = axes.second;


        return description;
   }
   /**
    * marshallParameters 
    * 
    * Produces the vectors of x and y parameter names.
    * How this is done depends on the spectrum type as the base class
    * CSpectrum only provides a method to obtain the parameter ids.
    * Method:
    *    - Get the parameter ids:
    *    - Turn them all into parameter names.
    *    - Depending on the spectrum type turn that ordered vector of parmaeter
    * names into a pair of vectors for x/y parameters.
    * 
    * @param rSpectrum - References the spectrum.
    * @param pdict     - parameter dictionary (lookup id -> name).
    * @return std::pair<std::vector<std::string>, std::vector<std::string>>
    *   where the first vector are the x parameter names and the second vector
    * (can be empty) is are the y parameter names.
    * 
   */
   std::pair<std::vector<std::string>, std::vector<std::string>>
   CSpectrumFormatterJson::marshallParameters(CSpectrum &rSpectrum,  ParameterDictionary &pdict) {
        std::vector<UInt_t> parameterIds;
        rSpectrum.GetParameterIds(parameterIds);
        std::vector<std::string> parameterNames = paramIdsToNames(parameterIds, pdict);
        std::vector<std::string> xp;
        std::vector<std::string> yp;
        switch (rSpectrum.getSpectrumType())
        {
        case ke1D: 
            xp = parameterNames;
            break;
        case ke2D:
            xp.push_back(parameterNames[0]);
            yp.push_back(parameterNames[1]);
            break;
        case keBitmask:
            xp = parameterNames;
            break;
        case keSummary:
            xp = parameterNames;
            break;
        case keG1D:
            xp = parameterNames;
            break;
        case keG2D:
            xp = parameterNames;
            break;
        case keStrip:
            xp.push_back(parameterNames[0]);
            yp.push_back(parameterNames[1]);
            break;
        case ke2Dm:
            for (int i = 0; i < parameterNames.size(); i+=2) {
                xp.push_back(parameterNames[i]);
                yp.push_back(parameterNames[i+1]);
            }
            break;
        case keG2DD:
            {
                size_t offset = parameterNames.size()/2;  // y parameters.
                for (size_t i = 0; i < parameterNames.size()/2; i++) {
                    xp.push_back(parameterNames[i]);
                    yp.push_back(parameterNames[offset]);
                    offset++;
                }
            }
            break;
        case keGSummary:
            xp = parameterNames;
            break;
        case ke2DmProj:
            xp = parameterNames;
            break;
        case keUnknown:
            throw std::string("Unknown spectrum type");
            break;
        default:
            throw std::string("Undefined spectrum type");
            break;
        }
        return std::make_pair(xp, yp);
   }
   /**
    * parameterIdsToNames
    *    Given a vector of parameter ids (e.g. gotten from CSpectrum::getParameterIds()),
    *  returns the names of the parameters.
    * 
    * @param ids - const referencd to the ids to lookup.
    * @param pdict - references the parameter dictionary.
    * @return std::vector<std::string> - the paramter names in order
    * as per ids.
    * @note - if an id fails to lookup, the parameter name *DELETED* is
    *  used for that slot.  This is in keeping with how SpecTcl handles
    *  such cases.
    *
    *   The id lookup is O(n) in the number of parameters sadly
    * as the parameter dictionary is keyed by parameter name.
   */
  std::vector<std::string>
  CSpectrumFormatterJson::paramIdsToNames(const std::vector<UInt_t>& ids, ParameterDictionary& pdict) {

        std::vector<std::string> result;
        for (auto id : ids) {
            auto p = std::find_if(pdict.begin(), pdict.end(), [id] (std::pair<std::string, CParameter> p) {return p.second.getNumber() == id; });
            if (p != pdict.end()) {
                result.push_back(p->second.getName());
            } else {
                result.push_back(std::string("*DELETED*"));
            }
        }

        return result;
  }
  /**
   *  Given a spectrum return the axis definition JSon objects
   *  
   * @param rSpectrum - references the spectrum.
   * @return std::pair<Json::Value, Json::Value> - x, y axis 
   * @note It is legal for e.g. the y axis to be Null.
   * 
  */
 std::pair<Json::Value, Json::Value> 
 CSpectrumFormatterJson::getAxisDefinitions(CSpectrum& rSpec) {
        // There's always an x axis:

        Json::Value xaxis(Json::arrayValue);
        xaxis.append(rSpec.GetLow(0));
        xaxis.append(rSpec.GetHigh(0));
        xaxis.append(rSpec.Dimension(0));  // +2 for over/underflows.

        if (rSpec.Dimensionality() == 1) {
            Json::Value yaxis;
            return std::make_pair(xaxis, yaxis);
        } else {
            // There's a y axis:

            Json::Value yaxis(Json::arrayValue);
            yaxis.append(rSpec.GetLow(1));
            yaxis.append(rSpec.GetHigh(1));
            yaxis.append(rSpec.Dimension(1));

            return std::make_pair(xaxis, yaxis);
        }

 }
 /**
  * getSpectrumContents
  *    Returns a Json Value that is an array which contains
  * all of the non-zero spectrum channel.  See the comment header
  * in SpectrumFormatterJson.h for what each channel looks like.
  * 
  * @param rSpec - CSpectrum& for he spectrum to dump.
  * @return Json::Value - array of nonzero spectrum channel objects.
  * @note This version does not return overflow/underflow bins. 
 */
Json::Value
CSpectrumFormatterJson::getSpectrumContents(CSpectrum& rspec) {
    Json::Value result(Json::arrayValue);
    TH1* pSpec = rspec.getRootSpectrum();
    if (rspec.Dimensionality() == 1) {
        // 1d spectrum.

        TAxis* xAxis = pSpec->GetXaxis();
        UInt_t nBins = rspec.Dimension(0) -2;
        // The +1's below are because bin 0 in the root spectrum
        // is the overflow channel.
        for (int ix = 0; ix < nBins; ix++) {
            auto value = pSpec->GetBinContent(ix+1);
            if (value > 0.0) {
                Json::Value chan(Json::objectValue);
                chan["chan_type"] = Json::Value("Bin");
                chan["x_coord"] = Json::Value(xAxis->GetBinCenter(ix+1));
                chan["y_coord"] = Json::Value(0.0);
                chan["x_bin"] = Json::Value(ix+1);
                chan["y_bin"] = Json::Value(0);
                chan["value"] = Json::Value(value);

                result.append(chan);
            }
        }

    } else {
        // 2d spectrum.

        TAxis* xAxis = pSpec->GetXaxis();
        TAxis* yAxis = pSpec->GetYaxis();

        UInt_t nx = rspec.Dimension(0) -2;
        UInt_t ny = rspec.Dimension(1) -2;
        // Again the +1's below have to do with the fact that
        // bins #0 are overflows.

        for (int ix = 0; ix < nx; ix++) {
            for (int iy = 0; iy < ny; iy++) {
                auto value = pSpec->GetBinContent(ix+1, iy+1);
                if (value > 0.0) {
                    Json::Value chan(Json::objectValue);
                    chan["chan_type"] = Json::Value("Bin");
                    chan["x_coord"] = Json::Value(xAxis->GetBinCenter(ix+1));
                    chan["y_coord"] = Json::Value(yAxis->GetBinCenter(iy+1));
                    chan["x_bin"]  = Json::Value(ix+1);
                    chan["y_bin"]  = Json::Value(iy+1);
                    chan["value"] = Json::Value(value);

                    result.append(chan);
                }
            }
        }
    }

    return result;
}



