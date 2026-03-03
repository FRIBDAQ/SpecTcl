/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005-2026 and beyond.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
/** 
 * @file CMetadata.h
 * @author Ron Fox <fox@frib.msu.edu>
 * @brief Defines a generic metadata class to allow attaching name/value pairs to SpecTcl objects.
 */
#ifndef CMETADATA_H
#define CMETADATA_H
#include <string>
#include <map>

/** 
 * @class CMetadata
 *    Metatdata are just name value pairs that allow one to attach arbitrary information
 * to an object. This is for Issue #229.
 * 
*/
class CMetadata {
public:
    typedef std::map<std::string, std::string> Metadata_t;
private:
    Metadata_t  m_metadata;

    // The interface ..normally objects that can have metadata export this interface.

public:
    void set(const char* name, const char* value);
    std::string get(const char* name) const;
    const Metadata_t& get_all() const;

};
#endif