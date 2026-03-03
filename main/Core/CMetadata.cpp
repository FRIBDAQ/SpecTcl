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
 * @file CMetadata.cpp
 * @author Ron Fox <fox@frib.msu.edu>
 * @brief Implements the metadata class.
 */

 #include "CMetadata.h"
 #include <CNoSuchObjectException.h>

 /**
  * set
  *   Set/replace metadata value:
  * @param name - name of the metadata item.
  * @param value - (new) value for the item.
  * 
  * @note if the item does not yet exist it is cdreatd.
  */
 void
 CMetadata::set(const char* name, const char* value) {
    std::string n(name);
    std::string v(value);

    m_metadata[n] = v;
 }
 /** 
  * get 
  *    Retrieve the value of a metadata item by name.
  * 
  * @param name
  * @return std::string the value..
  * @throw CNoSuchObjectException - the named metadata item is not defined.
 */
std::string
CMetadata::get(const char* name) const {
    std::string n(name);
    auto p = m_metadata.find(n);
    if (p != m_metadata.end()) {
        return p->second;
    } else {
        throw CNoSuchObjectException("Retrieving metadata", name);
    }
}
/**
 * get_all 
 *    Get a const reference to the metadata map.  This is intended for use in iteration
 * 
 * @return const Metadata_t&
 */
const CMetadata::Metadata_t&
CMetadata::get_all() const {
    return m_metadata;
}