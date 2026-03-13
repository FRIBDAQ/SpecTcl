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
#include "WriteCommand.h"
#include "ReadCommand.h"
#include "SpectrumFormatError.h"
#include "Spectrum.h"
#include "GateContainer.h"
#include "Parameter.h"
#include "SpecTcl.h"
#include "CParameterDictionarySingleton.h"
#include <CInvalidArgumentException.h>
#include <Exception.h>
#include <iostream>
#include <sstream>
#include <map>
#include <hdf5spectrumReader.h>
#include <string.h>
#include <stdint.h>

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
    // Get the file and spectrum index.  Instantiate the reader class and
    // get the name of the spectrum we're restoring:

    std::string filename = ReadCommandInfo::getInstance()->m_filename;
    unsigned    spectrum_index = ReadCommandInfo::getInstance()->m_spectrumIndex;
    hdfSpectrumReader reader(filename.c_str());
    std::vector<std::string> spectrum_names = reader.listSpectra();
    const char* spname = spectrum_names[spectrum_index].c_str();

    // Get the data and spectrum types   in internal form:

    std::stringstream stype(reader.spectrumType(spname));
    std::stringstream dtype(reader.dataType(spname));

    SpectrumType_t spectype;
    stype >> spectype;
    DataType_t     datatype;
    dtype   >> datatype;

    // Make the axis specification vectors:

    std::vector<UInt_t> chanvec;
    std::vector<Float_t> lowvec;
    std::vector<Float_t> hivec;
    
    auto xaxis = reader.getXaxis(spname);
    chanvec.push_back(xaxis.s_bins);
    lowvec.push_back(xaxis.s_low);
    hivec.push_back(xaxis.s_high);

    if (reader.hasYAxis(spname)) {
        // There's a second axi spec:

        auto yaxis = reader.getYaxis(spname);
        chanvec.push_back(yaxis.s_bins);
        lowvec.push_back(yaxis.s_low);
        hivec.push_back(yaxis.s_high);
    }
    // Now the parameter vectors:

    std::vector<std::string> xparam = reader.getParameters(spname);
    CSpectrum* pSpectrum;
    if (reader.hasYparameters(spname)) {
        std::vector<std::string> yparam = reader.getYParameters(spname);
        pSpectrum = SpecTcl::getInstance()->CreateSpectrum(
            spname, spectype, datatype, 
            xparam, yparam,
            chanvec, &lowvec, &hivec
        );
    } else {
        pSpectrum = SpecTcl::getInstance()->CreateSpectrum(
            spname, spectype, datatype, xparam,
            chanvec, &lowvec, &hivec
        );
    }
    // Let's set the metadata and then figure out how to deal with the channels:

    auto metadata = reader.getMetadata(spname);
    for(auto p : metadata) {
        pSpectrum->setMetadata(p.first.c_str(), p.second.c_str());
    }

    // Now the data:

    
    unsigned binsize;   // Number of bytes/bin.
    switch (datatype) {
        case keByte:
            binsize = sizeof(uint8_t);
            break;
        case keWord:
            binsize = sizeof(uint16_t);
            break;
        case keLong:
            binsize = sizeof(uint32_t);
            break;
        default:
            throw CInvalidArgumentException(
                reader.dataType(spname), 
                "Invalid data type specification", 
                "Computing bytes/channel"
            );
    }
    // So how many bytes is this anyway:
    unsigned nBins = chanvec[0];
    if (chanvec.size() == 2) {
        nBins = chanvec[0] * chanvec[1];
    }
    unsigned nBytes = nBins * binsize;

    // Get the contents and copy them into the spectrum:

    void* pContents = reader.getContents(spname);
    memcpy(pSpectrum->getStorage(), pContents, nBytes);

    // This is probably excessive but ... 'correct'.
    switch (datatype) {
        case keByte:
            delete [](reinterpret_cast<uint8_t*>(pContents));
            break;
        case keWord:
            delete [](reinterpret_cast<uint16_t*>(pContents));
            break;
        case keLong:
            delete [](reinterpret_cast<uint32_t*>(pContents));
            break;
    }
    

    return  {spectrum_names[spectrum_index], pSpectrum};

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

    // Get the write context and from that the name of the file and whether 
    // we need to create of just open.

    std::string fname      = WriteCommandInfo::getInstance()->m_filename;
    bool        mustCreate = WriteCommandInfo::getInstance()->m_firstSpectrum;
    const CSpectrum::SpectrumDefinition& description = rSpectrum.GetDefinition();
    // Truncate on open if must create else just open for read/write.

    unsigned int openFlags = mustCreate ? H5F_ACC_TRUNC : H5F_ACC_RDWR;
    try {
        Exception::dontPrint();              // Let's us catch the errors.
        H5File hdf(fname, openFlags);

        // Make a group named after the spectrum:

        Group spectrumGroup = hdf.createGroup(rSpectrum.getName());

        // Add as attributes: "type" and "datatype" which are the
        // stringified spectrum type and ddata type.
        std::stringstream dataTypeStream;
        dataTypeStream << description.eDataType;
        std::string dataType(dataTypeStream.str());

        // The data type has a pad space from operator<< I'm loath to
        // remove because of what else it might affect/why it was put there.
        
        dataType.pop_back();

        std::stringstream spectypeStream;
        spectypeStream << description.eType;
        std::string spectrumType(spectypeStream.str());




        addStringAttribute(spectrumGroup, "spectrumtype", spectrumType.c_str());
        addStringAttribute(spectrumGroup, "datatype", dataType.c_str());


        // Create and fill the contents data set.. for that we need the dimensionality and
        // the dimensions themselves:

        int rank = rSpectrum.Dimensionality();
        hsize_t dimensions[2];    // We only suport  1 and 2 dim sepctra:
        for (int i = 0; i < rank; i++) {
            dimensions[i] = rSpectrum.Dimension(i);
        }
        DataSpace ds(rank, dimensions);

        // To actually create the dataset we need to know the data type.
        // We're going to store in little endian format:
        // But we also need to have data types for the in memory stuff.


        PredType fileDataType(PredType::STD_U32LE);   //  There's no default construtor.
        PredType memoryDataType(PredType::NATIVE_UINT32);
        switch (rSpectrum.StorageType()) {
            case keByte:
                fileDataType = PredType::STD_U8LE;
                memoryDataType = PredType::NATIVE_UINT8;
                break;
            case keWord:
                fileDataType = PredType::STD_U16LE;
                memoryDataType = PredType::NATIVE_UINT16;
                break;
            case keLong:
                fileDataType = PredType::STD_U32LE;
                memoryDataType =PredType::NATIVE_UINT32;
        }
        DataSet contents = spectrumGroup.createDataSet("contents", fileDataType, ds);

        contents.write(rSpectrum.getStorage(), memoryDataType);

        // The contents data set has x and y axis attributs a well as a gate
        // name attribute:

        writeAxisAttribute(
            contents, "Xaxis", 
            description.fLows[0], description.fHighs[0], description.nChannels[0]
        );
        // There might be a y axis
        if (description.fLows.size() == 2) {
            writeAxisAttribute(
                contents, "Yaxis", 
                description.fLows[1], description.fHighs[1], description.nChannels[0]
            );
        }
        std::string gateName = rSpectrum.getGate()->getName();
        addStringAttribute(contents, "gate_name", gateName.c_str());

        // Write the parameter data sets:

        if (description.vParameters.size() > 0) {
            makeParameterDataset(
                spectrumGroup, "xparameters", rDict, description.vParameters
            );
        }
        if (description.vyParameters.size() > 0) {
            makeParameterDataset(
                spectrumGroup, "yparameters", rDict, description.vyParameters
            );
        }

        // write the metadata for the spectrum;

        writeMetadata(
            spectrumGroup, "metadata", rSpectrum.getAllMetadata()
        );


        contents.close();
        spectrumGroup.close();
        hdf.close();
    }
    catch (Exception& e) {
        // COnvert this to a spectrum format error.
        std::string doing = e.getDetailMsg();   
        throw CSpectrumFormatError(CSpectrumFormatError::HDF5Exception, doing.c_str());
    }

}


/////////////////////// private utilities:

/**
 * addStringAttribute
 *     Add a string valued attribute to an HDF5 object:
 * 
 * @param object - the object to add the attributes to.
 * @param name   - name of the attribute.
 * @param value  - value of the attribute.
 * @note error handling is determined by the caller, that is either
 *   the program fails with an error or must catch and exception.
 */
void
CHDF5SpectrumFormatter::addStringAttribute(
    H5Object& object, const char* name, const char* value
) {
    H5std_string strName(name);
    H5std_string strValue(value);
    StrType atr_type(PredType::C_S1, strValue.size());
    auto desc_ds = DataSpace(H5S_SCALAR);

    auto attr = object.createAttribute(strName, atr_type, desc_ds);
    attr.write(atr_type, strValue);
    attr.close();
}
/**
 *  addAxisAttribute
 *    Given the data for an axis, writes it as an attribute for the
 *  the object (usually contents) given.  Note that the attribute
 *  is written as three floats, (the bins is converted to a float).
 *  for simplicity.
 * 
 * @param object - the HDF5 objet to which the attribute will be attached.
 * @param name   - attribute name.
 * @param low    - Axis low limit.
 * @param high   - Axis high limit.
 * @param bins   - Integer bin count.
 * @note the caller determine the error handling.
 */
void
CHDF5SpectrumFormatter::writeAxisAttribute(
     H5::H5Object& object, const char* name,
    Float_t low, Float_t high, UInt_t bins
) {
    Float_t data[3] = {low, high, static_cast<Float_t>(bins)};  // HDF5 data array.
    hsize_t dims[1] = {3};                                        // 1-d 3 element data set.

    DataSpace attrds = DataSpace(1, dims);
    Attribute attr = object.createAttribute(name, PredType::IEEE_F32LE, attrds);  // Store as IEEE32 bit little endian.
    attr.write(PredType::NATIVE_FLOAT, data);  // But it comes from the native float format.
    attr.close();
}
/**
 * Convert a vector of parameter ids into a vector of strings.
 * 
 * @param rDict - the parameter dictionary.
 * @param ids   - References a vector of ids.
 * 
 * Note that we must preserve the order of the parameters in the output.  Therefore:
 * 1. We precreate the output vector with ids.size() strings with the value "-DELETED-"
 * 2. We map a map keyed by id with values the index into the ids. vector.
 * 3. We iterate the dictionary, for each id in the dict that's in the map,
 * we can fill in the element of the result array indexed by the value of that map entry
 * in the output vector.  The code might be clearer than this explanation but
 * Suppose I have an id j and its map entry looks like {j, k} WHen j is found in
 * the dictionary, the name is stored in result[k].  That results in an order preserved
 * output.
 */
std::vector<std::string>
CHDF5SpectrumFormatter::parameterIdsToNames(
    ParameterDictionary& rDict, const std::vector<UInt_t>& ids
) {
    // Make the id -> index map and initialized result.

    std::map<UInt_t, UInt_t> idmap;
    std::vector<std::string> result;
    for (int i =0; i < ids.size(); i++) {
        idmap[ids[i]] = i;
        result.push_back("--DELETED--");
    }
    
    // Now iterate over the parameter definitions:

    for (const auto& p : rDict) {
        auto f = idmap.find(p.second.getNumber());    // Search for the index.
        if (f != idmap.end()) {
            // Found fill in the parameter name in the result.
            result[f->second] = p.first;
        }
    }

    return result;
}
/**
 * makeParameterDataSet
 *    Given the parent group, the name of a data set to create,
 *    the parameter dictionary and ids of the parameters,
 *    Makes a new dataset containing the names of the
 *    parameteres that belong in that data set.
 * 
 * @param parent - the group in which to create the dataset.
 * @param name   - name of the data set.
 * @param rDict  - reference to the paramter dictionary.
 * @param paramIds - Ids of the paramters that belong in the dataset.
 * @note the caller determines error handlingh.
 */
void
CHDF5SpectrumFormatter::makeParameterDataset(
    Group& parent, const char* name,
    ParameterDictionary& rDict, const std::vector<UInt_t>& paramIds
) {
    // Create the vector of names:

    std::vector<std::string> paramNames = parameterIdsToNames(rDict, paramIds);


    // Create a vector of pointers to the strings... that's what we
    // need to write:

    
    std::vector<const char*> params;   // we'll write .data() of this vector.
    for (auto& s : paramNames) {       // Can't copy due to c_str scope issues.
        params.push_back(s.c_str());
    }


    // create our dataspace and a data set for variable strings:

    StrType memType(PredType::C_S1, H5T_VARIABLE);   // shorter strings get padded nulls.
    
    
    hsize_t dims[1] = {params.size()};  //1d as many strings as we have.
    DataSpace space(1, dims);
    DataSet pset = parent.createDataSet(name, memType, space);
    pset.write(params.data(), memType);

    pset.close();
}
/**
 *  writeMetadata
 *   Given the spectrum group, the name of a data set and the metadata for the specturm,
 *   writes the metadata.  The metadata are writtena as a 2xn array of strings
 *   of variable len gth where [0][n] is the name of a metadata item and
 *   [1][n] is the value of that metadata item.
 * 
 * @param parent - the group in which our dataset is put.
 * @param name   - name of the data set (probably something like "metadata").
 * @param metadata - the metadata to write.
 */
void
CHDF5SpectrumFormatter::writeMetadata(
    Group& parent, const char *name,
    const CMetadata::Metadata_t& metadata
) {
    hsize_t dims[1] = {metadata.size()*2};
    auto pData = new const char*[dims[0]*2];
    // marshall the metadata into the pData:

    int i = 0;
    for (const auto& p : metadata) {
        pData[i*2] = p.first.c_str();
        pData[i*2+1] = p.second.c_str();
        i++;
    }

    // Define the memory an file data spaces:

    StrType memType(PredType::C_S1, H5T_VARIABLE);
    DataSpace fSpace(1, dims);

    DataSet mset = parent.createDataSet(name, memType, fSpace);
    mset.write(pData, memType);

    delete pData;
    mset.close();
   

}