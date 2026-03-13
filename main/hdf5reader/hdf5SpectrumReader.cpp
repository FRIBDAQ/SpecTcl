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

#include <stdint.h>
#include <stdexcept>
#include <memory>

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
/** 
 * getGate
 *    Return the name of the  gate that's applied to the spectrum.
 * Note: all spectra have gates.  The name of an ungated spectrum
 * will be "-TRUE-" whichis a SpecTcl pre-defined gate that is
 * a true type gate.
 * 
 * @param name - spectrum name.
 * @return std::string - namme of the applied gate.
 */
std::string
hdfSpectrumReader::getGate(const char* name) {
    Group spectrum = m_hdf5File.openGroup(name); 

    // The gate name is an attribute of the contents data set:

    DataSet s = spectrum.openDataSet("contents");

    std::string result = getStringAttribute(s, "gate_name");
    s.close();
    spectrum.close();

    return result;
}
/**
 * hasYparameters
 *   This one is a bit tricky since it might return false when
 *  you least expect it.  For spectra that have lists of X _and_
 * lists of y spectra, this will return true. Examples I can think of
 * are "gd" and "2dsum".  You might think that "2" would return true
 * but a single list of parameters gives an unambiguous definition
 * (first x, then y), similarly g2 spectra only need a singe list of
 * parameters to define the spactra as do s.
 * 
 * @param name - spectrum name.
 * @return bool - if there is a list of Y parameters.
 * 
 */
bool 
hdfSpectrumReader::hasYparameters(const char* name)  {
    Group spectrum = m_hdf5File.openGroup(name);

    // Try the .exists method see if it works:

    bool result = spectrum.exists("yparameters");
    spectrum.close();

    return result;
}
/**
 * getParameters
 *    Gets the list of parameter names for spectra that have an unambiguous single
 * parameter list.  Note. This includes 2d spectra which have, in order, the X axis
 * parameter followed by the Y axis parameter.
 * 
 * @param name - Spectrum name.
 * @return std::vector<std::string> - the parameter names.
 */
std::vector<std::string>
hdfSpectrumReader::getParameters(const char* name) {
    std::vector<std::string> result;
    
    Group spectrum = m_hdf5File.openGroup(name);
    DataSet params = spectrum.openDataSet("xparameters");
    result = getStringListDataSet(params);

    params.close();
    spectrum.close();

    return result;
}
/** getYParameters
 *    Get the list of parameter names for the y axis if
 * it's not uniquely defined by a single list.
 * One should ensure this exists by calling hasYparameters
 * first.
 * 
 * @param name - spectrum name.
 * @return std::vector<std::string> - list of names.
 */
std::vector<std::string>
hdfSpectrumReader::getYParameters(const char* name) {
    std::vector<std::string> result;

    Group spectrum = m_hdf5File.openGroup(name);
    DataSet params = spectrum.openDataSet("yparameters");
    result = getStringListDataSet(params);

    params.close();
    spectrum.close();

    return result;
}

/**
 *  getMetadata:
 *   @param name -name of the spectrum.
 *   @return std::vector<std::pair<std::string, std::string>>  the metadata associated with
 * the spectrum.   THe first item of each pair is the name of the metadata item, the second,
 * its value.
 * 
 */
std::vector<std::pair<std::string, std::string>>
hdfSpectrumReader::getMetadata(const char* name) {
    Group spectrum = m_hdf5File.openGroup(name);
    DataSet metadata = spectrum.openDataSet("metadata");
    std::vector<std::string> stringlist = getStringListDataSet(metadata);

    // Now pull the pairs from the flat list:
    // It's an error not to have an even number of strings:
    if ((stringlist.size() % 2) != 0) {
        throw std::runtime_error("Metadata string list is not even length!!!");
    }
    std::vector<std::pair<std::string, std::string>> result;
    for (int i =0; i < stringlist.size(); i+=2) {
        result.push_back({stringlist[i], stringlist[i+1]});
    }

    return result;
}

/**
 * getContents
 *   This gets the contents of the spectrum.  The contents are gotten in the 
 * appropriate native data type. It's up to the caller to:
 * - Interpret the data in that time and dimensionality (see get{X,Yaxis).
 * - delete (not free delete) the data returned when done with it.  You might
 * think about using one of the smart pointer types to ensure this happens.
 * 
 * @param name -spectrumname.
 * @returns void* pointer to the data that was read in.  When no longer needed,
 * the caller should delete []ptr  that pointer.
 * 
 */
void*
hdfSpectrumReader::getContents(const char* name) {
    Group spectrum = m_hdf5File.openGroup(name);
    DataSet contents = spectrum.openDataSet("contents");
    DataSpace ds     = contents.getSpace();

    // In order to know our desired native type, 
    // we need to know the memory size and number of storage units.
    // the size of one storage unit detemines whic of the native int types we need to
    // use.

    hsize_t totalBytes = contents.getStorageSize();
    hsize_t nUnits     = ds.getSimpleExtentNpoints();
    hsize_t unitSize   = totalBytes/nUnits;

    PredType memType = PredType::NATIVE_UINT32;  // null construction not allowed so...
    switch (unitSize) {
    case sizeof(uint8_t):
        memType = PredType::NATIVE_UINT8;
        break;
    case sizeof(uint16_t):
        memType = PredType::NATIVE_UINT16;
        break;
    case sizeof(uint32_t):
        memType = PredType::NATIVE_UINT32;
        break;
    default:
        //No such type:

        throw std::runtime_error("Unrecognized spectrum conents storage size.");
    }

    uint8_t* p = new uint8_t[totalBytes];    // Now I have storage to read into:
    contents.read(p, memType);

    return p;

}
/**
 *  getSpectrumDimensions
 *    Get the dataspace dimensions from a spectrum.
 * 
 * @param name -name of the spectrum.
 * @return std::vector<size_t> - dimensions (x first if there are two) of the dataspace.
 * @note Given spectrum storage is in root, dimensions will, in general, be larger by
 * nbins by 2 for the under/overflow channels.
 */
std::vector<size_t>
hdfSpectrumReader::getSpectrumDimensions(const char* name) {
    Group spectrum = m_hdf5File.openGroup(name);
    DataSet contents = spectrum.openDataSet("contents");
    DataSpace ds     = contents.getSpace();

    int ndims = ds.getSimpleExtentNdims();    // Number of dimensions.
    std::vector<hsize_t> hresult;
    hresult.resize(ndims, 0);

    ds.getSimpleExtentDims(hresult.data());
    std::vector<size_t> result;
    for (auto n : hresult) {
        result.push_back(n);
    }

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

/**
 * retrieves the contents of a data set that is a list of strings.
 * 
 * @param ds - data set.
 * @return std::vector<std::string>
 */
std::vector<std::string>
hdfSpectrumReader::getStringListDataSet(DataSet& ds) {
    // How many strings do I have and allocate memory for them:

    hsize_t dims[1];
    ds.getSpace().getSimpleExtentDims(dims, nullptr);
    StrType datatype = ds.getStrType();

    std::unique_ptr<char*[]> strings(new char*[dims[0]]);   // Allocate auto freed storage.

    ds.read(strings.get(), datatype);

    // marshall the strings from the vector read into the result vector

    std::vector<std::string> result;
    for (int i =0; i < dims[0]; i++) {
        result.push_back(std::string(strings[i]));
        H5free_memory(strings[i]);    // Free dynamic memory.S

    }

    return result;
}