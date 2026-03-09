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
#include "SpectrumFormatError.h"
#include "Spectrum.h"
#include "GateContainer.h"
#include <Exception.h>
#include <iostream>
#include <sstream>

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

}