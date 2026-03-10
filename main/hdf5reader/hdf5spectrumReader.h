/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2026.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/**
 * @file hdf5SpectrumReader.h
 * @brief define a class to read spectrum data from  HDF5
 * @author Ron Fox<rfoxkendo at gmail dot com>
 */
#ifndef HDF5SPECTRUMREADER_H   // Include guard.
#define HDF5SPECTRUMREADER_H
#include <vector>
#include <string>
#include <H5Cpp.h>

/**
 *  @class hdf5SpectrumReader
 * 
 * This class provides a SpecTcl independent decoder
 * for data written by SpecTcl using 
 * swrite -format hdf5  
 * 
 * While the formatter supporting sread is built on top
 * of it, the primary purpose of this class is to  be
 * uh...stolen by non SpecTcl programs to decode those files
 * for the nefarious purposes of other consumers of those
 * spectral data.
 * 
 * 
 */
class hdfSpectrumReader {
    H5::H5File  m_hdf5File;
private:

    // Allowed canonicals.
public:
    hdfSpectrumReader(const char* path);
    virtual ~hdfSpectrumReader();
    
    // disallowed canonicals:

private:
    hdfSpectrumReader(const hdfSpectrumReader&);
    hdfSpectrumReader& operator=(const hdfSpectrumReader&);
    int operator==(const hdfSpectrumReader&);
    int operator!=(const hdfSpectrumReader&);

    // Operations on the HDF file:
public:
    std::vector<std::string> listSpectra();

    // Private methods:

private:
    // Iteration callback:
    static herr_t accumulateNames(
        hid_t group_id, const char* name, 
        const H5L_info_t*, void* opdata
    );
};



#endif
