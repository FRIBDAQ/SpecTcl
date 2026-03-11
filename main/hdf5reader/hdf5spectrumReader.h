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
public:
    typedef struct _AxisDefinition {
        float s_low;     // Axis low limit.
        float s_high;    // Axis high limit.
        unsigned s_bins;  // Number of bins on that axis.
    } AxisDefinition;
private:
    H5::H5File  m_hdf5File;

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
    std::string              spectrumType(const char* name);
    std::string              dataType(const char* name);

    // Axis operations:

    bool hasYAxis(const char* name);
    AxisDefinition getXaxis(const char* name);
    AxisDefinition getYaxis(const char* name);
    
    // Gate name:

    std::string getGate(const char* name);

    // Get parameters

    bool hasYparameters(const char* name);   // For e.g. gd and 2dsums.
    std::vector<std::string> getParameters(const char* name); // Note 2d spectra have x then y here.
    std::vector<std::string> getYParameters(const char* name);

    // metadata:

    std::vector<std::pair<std::string, std::string>> getMetadata(const char* name);

    // Contents:

    void* getContents(const char* name);  // Caller must delete -- even btter put in std::unique_ptr.


    // Private methods:

private:
    std::string getStringAttribute(H5::H5Object& parent, const char* name);
    AxisDefinition getAxisAttribute(H5::H5Object& parent, const char* name);
    // Iteration callback:
    static herr_t accumulateNames(
        hid_t group_id, const char* name, 
        const H5L_info_t*, void* opdata
    );
};



#endif
