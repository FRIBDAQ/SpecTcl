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
 * @file CHDF5SpectrumFormatter.h
 * @brief Header for the spectrum formatter to write and read spectra from hdf5 format. 
 * @note See https://github.com/FRIBDAQ/SpecTcl/wiki/hdf5spectra  for information
 * about the organization of the file.
 *     
 */

#ifndef CHDF5SPECTRUMFORMATTER_H
#define CHDF5SPECTRUMFORMATTER_H
#include "SpectrumFormatter.h"   // Base class.

/**
 * @class CHDFSpectrumFormatter 
 *    Provides a spectrum formatter for HDF5 files.
 * For information about the internal structure
 * of spectrum files, see:
 *   https://github.com/FRIBDAQ/SpecTcl/wiki/hdf5spectra 
 */
class CHDF5SpectrumFormatter : public CSpectrumFormatter {

    // Canonical methods.
public:
    CHDF5SpectrumFormatter();
    virtual ~CHDF5SpectrumFormatter();
    
    CHDF5SpectrumFormatter(const CHDF5SpectrumFormatter& source);
    CHDF5SpectrumFormatter& operator=(const CHDF5SpectrumFormatter& rhs);

    // disallowed canonicals:

private:
    int operator==(const CHDF5SpectrumFormatter& rhs) const;
    int operator!=(const CHDF5SpectrumFormatter& rhs) const;

    // Operations that implement the interface:
public:
    virtual   std::pair<std::string, CSpectrum*> Read (std::istream& rStream, 
			    ParameterDictionary& rDict);
    virtual   void Write (std::ostream& rStream, CSpectrum& rSpectrum,
		       ParameterDictionary& rDict);
 

};
#endif