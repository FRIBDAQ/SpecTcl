/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#ifndef RINGFORMATHELPERFACTORY_H
#define RINGFORMATHELPERFACTORY_H

#include <map>
#include <stdint.h>

#ifdef major
#undef major
#endif

#ifdef minor
#undef minor
#endif

/**
 * @file RingFormatHelperFectory.h
 * @brief  Definition for format helper creator class.
 */

// Forward class definitions:

class CRingFormatHelper;
class CRingFormatHelperCreator;

/**
 * @class CRingFormatHelperFactory
 *
 * This class is responsible for creating an appropriate
 * ring buffer format helper class given information
 * about the version of the ring format.  Format helpers are, in turn
 * used by the CRingBufferDecoder class to figure out where bits and pieces
 * of a ring item are when the offsets t those bits and pieces are format
 * version dependent.
 *
 *  We are dependent on CRingFormatHelperCreator(s) which must implement:
 *  *   create - Creates a CRingFormatHelper and returns it.
 *  *   copy-construction
 *  *   Sane destruction.
 * 
 */
class CRingFormatHelperFactory {
public:
    // Inner classes:
    
    class MajorMinorVersion {     // Encapsulates a major/minor version level.
        
    private:
        uint32_t m_major;
        uint32_t m_minor;
    public:
        MajorMinorVersion(uint32_t major, uint32_t minor) :
            m_major(major), m_minor(minor) {}
        int operator<( const MajorMinorVersion& rhs) const {
          int result=0;
          if (m_major < rhs.m_major) {
            result = 1;
          } else if (m_major == rhs.m_major) {
            result = (m_minor < rhs.m_minor);
          } else {
            result = 0;
          }
          return result;
        }
        int operator==(const MajorMinorVersion& rhs) const {
            return (m_major == rhs.m_major) && (m_minor == rhs.m_minor);
        }

        uint32_t major() const {return m_major;}
        uint32_t minor() const {return m_minor;}
    };

    // Internal types:
private:    
    typedef std::map<MajorMinorVersion,  CRingFormatHelperCreator*> CreatorMap;
    
    //  Class attributes (member data):
    
private:
    CreatorMap m_Creators;
    
    // Canonicals:
    
public:    
    CRingFormatHelperFactory();
    CRingFormatHelperFactory(const CRingFormatHelperFactory& rhs);
    virtual ~CRingFormatHelperFactory();
    CRingFormatHelperFactory& operator=(const CRingFormatHelperFactory& rhs);
    
private:
    int operator==(const CRingFormatHelperFactory&) const;
    int operator!=(const CRingFormatHelperFactory&) const;
    
    // object operations:
    
public:
    CRingFormatHelper* create(uint32_t major, uint32_t minor);
    CRingFormatHelper* create(void* pFormatItem);
    
    void addCreator(
        uint32_t major, uint32_t minor, const CRingFormatHelperCreator& creator
    );
    void removeCreator(uint32_t major, uint32_t minor);

};

#endif
