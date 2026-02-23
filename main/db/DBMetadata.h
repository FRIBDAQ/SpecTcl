/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2026.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
        Ron Fox
        FRIB
        Michigan State University
        East Lansing, MI 48824-1321
*/
/**
 *  @file DBMetadata.h
 *  @brief Describe the class that manages metadata. See Issue #299 
 * @author Ron Fox<rfoxkendo@gmail.com>
 */
#ifndef DBMETADATA_H
#define DBMETADATA_H
#include <string>
#include <vector>

class CSqlite;

namespace SpecTclDB {
/**
 * @class DBMetadata
 * This class encapsulates metadata manipulation. Metadata
 * are arbitrary name/value pairs that are associated with
 * parameters, spectra, gates and treevariables.
 * 
 */
    class DBMetadata {
    private:
        CSqlite& m_connection;
        int      m_saveId;
        int      m_fkId;    // Foreign key type.
        std::string m_type; // metadata type.
    public:
        DBMetadata(CSqlite& connection, int saveid);
        ~DBMetadata();

    public:
        // Establish the entity for which we are doing metadata operations:
        void selectParameter(const char* parname);
        void selectSpectrum(const char* specname);
        void selectGate(const char* gatename);
        void selectTreevar(const char*  tvname);

        // Set/get metadata

        void setMetadataItem(const char* name, const char* value);
        std::string getMetadataValue(const char* name);
        std::vector<std::pair<std::string, std::string>> dumpMetadata();

    };
}


#endif