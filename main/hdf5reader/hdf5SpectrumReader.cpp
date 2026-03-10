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
 * @file hdf5SpectrumReader.cpp
 * @brief implement a class to read spectrum data from  HDF5
 * @author Ron Fox<rfoxkendo at gmail dot com>
 */
#include "hdf5spectrumReader.h"

#include <stdexcept>

using namespace H5;

 /**
  *  construction:
  * @param path - path to the file to open.
  */
 hdfSpectrumReader::hdfSpectrumReader(const char* path) :
    m_hdf5File(path, H5F_ACC_RDONLY)
 {}

 /**
  *  destructor
  *    Just closes the file:
  */
 hdfSpectrumReader::~hdfSpectrumReader() {
    m_hdf5File.close();
 }


 ////////////////////////////////////////////////////////////////////

 /**
  * listSpectra
  *  @return std::vector<std::string> The names of the spectra in the file.
  *  @note this is just a list of the names of the top level groups.  
  *  @throw If failed, std::runtime_error can be thrown in addition
  * to the exceptions froj H5Cpp.
  */
 std::vector<std::string>
 hdfSpectrumReader::listSpectra() {
    auto root_id = m_hdf5File.getId();
    std::vector<std::string> result;
    hsize_t idx(0);
    auto err = H5Literate(
        root_id, H5_INDEX_NAME, H5_ITER_NATIVE, &idx,
        hdfSpectrumReader::accumulateNames,  &result
    );
    if (err) {
        throw std::runtime_error("Failed to iterate through top level groups");
    }
    return result;
 }

 /////////////////// Private utilities:

 /**
  * accumulateNames - the group iteration callback.
  * See H5Literate for parameters.
  */
 herr_t
 hdfSpectrumReader::accumulateNames(
    hid_t group_id, const char* name, const H5L_info_t*, void* opdata) {
  // Opdata is really a pointer to std::vector<std::string>

  std::vector<std::string>* pAccumulator = 
    reinterpret_cast<std::vector<std::string>*>(opdata);
  pAccumulator->push_back(std::string(name));
  return 0;       // Must return normal.
}
