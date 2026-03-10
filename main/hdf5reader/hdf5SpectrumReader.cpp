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

 /**
  * spectrumType
  * @param name of a spectrum in the file.
  * @return std::string - SpecTcl spectrum type of that spectrum.
  * 
  */
 std::string
 hdfSpectrumReader::spectrumType(const char* name) {
    Group spectrum = m_hdf5File.openGroup(name);

    // The spectrum type is an attribute "spectrumtype: of the group:

    auto result = getStringAttribute(spectrum, "spectrumtype");

    spectrum.close();
    return result;

 }
 /**
  *  dataType
  * @param name = spectrum name 
  * @return std::string - data type from SpecTcl.  This is one of:
  * - "long" - uint32_t
  * - "short" - uint16_t
  * - "byte"  = uint8_t.
  */
 std::string
 hdfSpectrumReader::dataType(const char* name) {
    Group spectrum = m_hdf5File.openGroup(name);
    auto result = getStringAttribute(spectrum, "datatype");
    spectrum.close();
    return result;
 }
 /**
  *  hasYAxis
  * @param name -spectrum name.
  * @return bool - returns true if the spectrum has a y axis specification.
  */
 bool
 hdfSpectrumReader::hasYAxis(const char* name) {
    Group spectrum = m_hdf5File.openGroup(name);
    DataSet contents = spectrum.openDataSet("contents");

    bool result = contents.attrExists("Yaxis");
    contents.close();
    spectrum.close();
    return result;
 }
/**
 * getXaxis
 *    - return the X axis specification of a spectrum.
 * @param name -spectrum name.
 * @return hdfSpectrumReader::AxisDefinition.
 * 
 */
hdfSpectrumReader::AxisDefinition
hdfSpectrumReader::getXaxis(const char* name) {
    Group spectrum = m_hdf5File.openGroup(name);
    DataSet contents = spectrum.openDataSet("contents");
    AxisDefinition result = getAxisAttribute(contents, "Xaxis");
    contents.close();
    spectrum.close();

    return result;
}
/**
 * getYAxis
 *    Return the definition of the Y axis.  If no Y axis exists
 * this will throw an error from hdf5C++.
 * 
 * @param name - name of the spectrum.
 * @return hdfSpectrumReader::AxisDefinition.
 * 
 */
hdfSpectrumReader::AxisDefinition
hdfSpectrumReader::getYaxis(const char* name) {
    Group spectrum = m_hdf5File.openGroup(name);
    DataSet contents = spectrum.openDataSet("contents");
    AxisDefinition result = getAxisAttribute(contents, "Yaxis");
    contents.close();
    spectrum.close();

    return result;
}

 /////////////////// Private utilities:

 /**
  *  Return a named string valued attribute from an object:
  */
std::string
hdfSpectrumReader::getStringAttribute(H5Object& parent, const char* name) {
    auto a = parent.openAttribute(name);
    auto stype = a.getStrType();
    std::string result;
    a.read(stype, result);
    a.close();
    return result;
}
/**
 * getAxisAttribute
 *    Given an axis attribute name, fetch and return
 * the corresponding axis definition.
 * 
 */
hdfSpectrumReader::AxisDefinition
hdfSpectrumReader::getAxisAttribute(H5Object& parent, const char* name) {
    auto a = parent.openAttribute(name);

    // We know the attribute is stored as 3 floats:

    float raw[3];
    a.read(PredType::NATIVE_FLOAT, raw);

    AxisDefinition result = {
        .s_low = raw[0],
        .s_high = raw[1],
        .s_bins = static_cast<unsigned>(raw[2])
    };
    a.close();

    return result;
}

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
