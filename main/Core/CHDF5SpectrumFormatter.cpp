/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2026

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321
(c) Copyright FRIB/MSU 2026 All rights reserved.
*/

/**
 * @file CHDF5SpectrumFormatter.cpp
 * @brief Implementation for the spectrum formatter to write and read spectra from hdf5 format. 
 * @note See https://github.com/FRIBDAQ/SpecTcl/wiki/hdf5spectra  for information
 * about the organization of the file.
 *     
 */

 #include "CHDF5SpectrumFormatter.h"
 #include <H5Cpp.h>
 #include <Exception.h>

 using namespace H5;

/**
 *  construction
 
 */
CHDF5SpectrumFormatter::CHDF5SpectrumFormatter() :
    CSpectrumFormatter("HDF5 spectrum format", ".h5")
{}
/**
 * Destruction.
 */
CHDF5SpectrumFormatter::~CHDF5SpectrumFormatter() {}

/**
 *  Copy construction:
 * 
 * @param source -the object that will be copy constructed.
 */
CHDF5SpectrumFormatter::CHDF5SpectrumFormatter(const CHDF5SpectrumFormatter& source) :
    CSpectrumFormatter(source)
{

}
/**
 * assignment:
 * @param rhs - the object that will be assigned into us
 */
CHDF5SpectrumFormatter&
CHDF5SpectrumFormatter::operator=(const CHDF5SpectrumFormatter& rhs) {
    // Only assign if it's not of the form
    //    a = a.
    // 
    if (this != &rhs) {
        CSpectrumFormatter::operator=(rhs);
    }
    return *this;
}

/**
 * Read
 *    Read a spetrum from file...note that sread for non-serial files
 * does not support multiple spectrum reads so we'll just read the first group we come
 * across for now.
 * 
 * @param rstream - the stream open on the file.. note that we're going to 
 *   use the filename in the sreadcommand's context, as we can't open hdf5 files on 
 *   c++ streams.
 * @param rDict - Referencdes the parameter dictionary.
 * @return std::pair<string, CSpectrum*> - A pair containing the name of the spectrum and a
 * pointer to the spectrum object atually produced.
 */
std::pair<std::string, CSpectrum*> 
CHDF5SpectrumFormatter::Read(std::istream& rStream, ParameterDictionary& rDict) {
    throw CException("Reading hdf5 files is in progress not supported yet.");
}
/**
 * Write
 *    Write a spectrum to an hdf5 file. We get our information from the
 * context block associated with the swrite command.
 * 
 * @param rstream - tsream open on the file - we use the filename from the context block.
 * @param rSpectrum - references the spectrum to write.
 * @param rDict   - References the parameter dict.
 * 
 */
void
CHDF5SpectrumFormatter::Write (
    std::ostream& rStream, CSpectrum& rSpectrum,
	ParameterDictionary& rDict) {

    throw CException("Writing HDF5 files is  in progresss, not yet supported");
}