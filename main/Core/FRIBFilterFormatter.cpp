
/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
             Giordano Cerizza
             Simon Giraud
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/**
 * Implements the FRIBFilterFormatter class.
*/
#include "FRIBFilterFormatter.h"
#include "CTreeVariable.h"
#include "Event.h"

#include <v12/RingItemFactory.h>
#include <v12/CRingItem.h>

#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdexcept>
#include <sstream>
#include <cstdint>
#include <memory>
// For now these definitions have come from the FRIB pipeline AnalysisRingItems.h file:
// TODO:  Figure out a clean way to avoid duplicating them here.

#pragma pack(push, 1)        // We want structs packed tight.
static const unsigned MAX_UNITS_LENGTH(32);
static const unsigned MAX_IDENT(128);
/**
 * Analysis ring items don't have body headers so:
 */


/**
 * This item is a parameter definition:
 *  - sizeof is not useful.
 */
typedef struct _ParameterDefintion {
    std::uint32_t s_parameterNumber;
    char          s_parameterName[0];   // Actually a cz string.
} ParameterDefinition, *pParameterDefinition;

/**
 *  parameter defintion ring item
 *  sizeof  is not useful.
 */
typedef struct  _ParameterDefinitions {
    RingItemHeader s_header;
    std::uint32_t  s_numParameters;
    ParameterDefinition s_parameters [0];
} ParameterDefinitions, *pParameterDefinitions;

/**
 *    This contains the value of one parameter.
 */
typedef struct _ParameterValue {
    std::uint32_t s_number;
    double        s_value;
} ParameterValue, *pParameterValue;

/*
    * Ring item of parameter unpacked data.
    * sizeof is worthless.
    */
typedef struct _ParameterItem {
    RingItemHeader s_header;
    std::uint64_t  s_triggerCount;
    std::uint32_t  s_parameterCount;
    ParameterValue s_parameters[0];
} ParameterItem, *pParameterItem;

/** Variable data is used to document steering parameters:
 *
 */
typedef struct _Variable {
    double s_value;
    char   s_variableUnits[MAX_UNITS_LENGTH];     // Fixed length
    char   s_variableName[0];       // variable length
} Variable, *pVariable;

typedef struct _VariableItem {
    RingItemHeader s_header;
    std::uint32_t  s_numVars;
    Variable       s_variables[0];
    
} VariableItem, *pVariableItem;

/* Ring Item types - these begin at 32768 (0x8000). - the first user type
    * documented in the NSCLDAQ ring item world:
    *
    *  LAST_PASSTHROUGH - ring items with types <=  are just passed through.
    *  
    */

static const std::uint32_t PARAMETER_DEFINITIONS = 32768;
static const std::uint32_t VARIABLE_VALUES       = 32769;
static const std::uint32_t PARAMETER_DATA        = 32770;

#pragma pack(pop)

/**
 *  constructor
 *    We'll construct with fd = -1 as an indicator the file is not open.
 * 
*/
CFRIBFilterFormat::CFRIBFilterFormat() :
    m_fd(-1), m_trigger(0)
{}

/**
 * destructor
 *    If the file is open we'll close it to be nice:
*/
CFRIBFilterFormat::~CFRIBFilterFormat() {
    if (m_fd >= 0) {
        ::close(m_fd);
    }
}


/**
 * open 
 *    Opens the filter file for write.  If there's already an open file we'll be nice
 * and close it though SpecTcl is supposed to ensure that does not happen.
 * 
*/
void
CFRIBFilterFormat::open(std::string filename) {
    if (m_fd > 0) {
        ::close(m_fd);
        m_fd = -1;   // In case the open fails.
    }
    m_fd = creat(filename.c_str(), S_IRUSR | S_IWUSR | S_IRGRP);
    if (m_fd < 0) {
        std::stringstream error_stream;
        error_stream << "Unable to create: " << filename << " : " << strerror(errno);
        std::string error_message = error_stream.str();
        throw std::runtime_error(error_message);
    }
    m_trigger = 0;
    m_ids.clear();
}

/** 
 * close 
 *    Close the filter file.  While it's not supposed to happen, if the file is not open,
 * we don't even call close(2)
*/
void
CFRIBFilterFormat::close() {
    if (m_fd >= 0) {
        ::close(m_fd);
        m_fd = -1;
    }
}

/**
 * DescribeEvent
 *    - Using the information given, we construct and write a PARAMETER_DEFINITIONS ring item.
 *    - Iterate over the tree variable map to construct and write a VARIABLE_VALUES ringitem.
*/

void
CFRIBFilterFormat::DescribeEvent(
    std::vector<std::string> parameterNames, 
    std::vector<UInt_t>      parameterIds
) {
    writeParameterDescriptions(parameterNames, parameterIds);
    writeVariableDefs();
    // Save the ids:

    m_ids = parameterIds;

}

/**
 *  operator()
 *    Writes an event:
 *     Figure out how many of the parameters are valid and allocate an appopriately sized
 *   PARAMETER_DATA item.
*/
void 
CFRIBFilterFormat::operator()(CEvent& event) {
    std::vector<unsigned> present;    // The parameters present:

    for (auto p: m_ids) {
        if (event[p].isValid()) {
            present.push_back(p);
        }
    }
    // Use that to size the ring Item:


    size_t bodySize = sizeof(ParameterItem) + present.size()*(sizeof(ParameterValue));
    v12::RingItemFactory fact; 

    // Allocate the item with my usual chicken size:

    std::unique_ptr<CRingItem> pItem(fact.makeRingItem(PARAMETER_DATA, bodySize + 100));

    union cursor {
        std::uint64_t* p64;
        std::uint32_t* p32;
        double*        pv;
    } pt;
    pt.p64 = reinterpret_cast<std::uint64_t*>(pItem->getBodyCursor());

    //trigger and count:

    *pt.p64 = m_trigger++; 
    pt.p64++;
    *pt.p32 = present.size();
    pt.p32++;

    // the values:
    for (auto p : present) {
        *pt.p32 = p;
        pt.p32++;

        *pt.pv = event[p];
        pt.pv++;
    }

    // Update the size and put it to file:

    pItem->setBodyCursor(pt.p32);
    pItem->updateSize();

    fact.putRingItem(pItem.get(), m_fd);

    // unique_ptr deallocateds the ring item here.
}

/**
 *  Document the type of the filter formatter:
*/
std::string
CFRIBFilterFormat::type() {
    return std::string("FRIBPIpe");
}




/////////////////////////////////////////////////////////////////////////////////
// Private utilities.

/**
 * writeParameterDescriptions
 *    - Figure out how large a parameter description item we need.
 *    - Get a ring item big enough for that.
 *    - Fill in the body.
 *    - Get the factory to write it to file.
*/
void
CFRIBFilterFormat::writeParameterDescriptions(
    const std::vector<std::string>& names, const std::vector<UInt_t> ids
) {
    // Figure out the body size:

    size_t bodySize(sizeof(ParameterDefinitions));
    for (auto s : names) {
        bodySize += sizeof(std::uint32_t);    // the id.
        bodySize += s.size() + 1;              // name and null terminator.
    }
    // Have the factory give me a ring item I'm chicken on the size.:

    v12::RingItemFactory fact;
    std::unique_ptr<CRingItem> pItem(fact.makeRingItem(PARAMETER_DEFINITIONS, bodySize + 100));

    // Get my body cursor and have at it:

    union cursor {
        std::uint32_t* p32;
        std::uint8_t*  p8;
        char*          pc;
    } p;
    p.p8 = reinterpret_cast<std::uint8_t*>(pItem->getBodyCursor());
    *p.p32 = static_cast<std::uint32_t>(names.size());   // Num parameters.
    p.p32++;

    // Now the parameters:

    for (int i =0; i < names.size(); i++) {
        *p.p32 = ids[i];
        p.p32++;
        strcpy(p.pc, names[i].c_str());
        p.p8 += names[i].size() + 1;
    }
    // Set the size and get the factory to write the item:

    pItem->setBodyCursor(p.p8);
    pItem->updateSize();

    fact.putRingItem(pItem.get(), m_fd);

    //std::unique_ptr releases the ring item storage here.
}
/** 
 * writeVariableDefs
 *    - Figure out how big a body we need.
 *    - Get the factory to alocate the item.
 *    - Fill in the body
 *    - Get the factory to write the item.
*/
void 
CFRIBFilterFormat::writeVariableDefs() {
    // Compute the body size:

    size_t bodySize(sizeof(VariableItem));

    for(auto p = CTreeVariable::begin(); p != CTreeVariable::end(); ++p) {
        // The iterator is a pair and the first element is its name.

        bodySize += sizeof(Variable);
        bodySize += p->first.size() + 1;
    }

    // Allocate the ring item and fill inthe body:
    // I'm chicken on my size computation:

    v12::RingItemFactory fact;
    std::unique_ptr<CRingItem> pItem(fact.makeRingItem(VARIABLE_VALUES, bodySize + 100));

    union cursor {
        std::uint32_t* p32;
        double*        pv;
        char*          pc;
        std::uint8_t*  p8;
    } pt;
    pt.p8 = reinterpret_cast<std::uint8_t*>(pItem->getBodyCursor());

    // Number of items:

    *pt.p32 = CTreeVariable::size(); 
    pt.p32++;

    // Now the items:

    for (auto p = CTreeVariable::begin(); p != CTreeVariable::end(); ++p) {
        // Value:
        CTreeVariableProperties* props = p->second;
        *pt.pv = (double)(*props);
        pt.pv++;

        // Units:

        strncpy(pt.pc, props->getUnits().c_str(), MAX_UNITS_LENGTH);
        pt.p8 += MAX_UNITS_LENGTH;

        // Name:
        strcpy(pt.pc, p->first.c_str());
        pt.p8 += p->first.size() + 1;
    }
    // Update the size and write the beast:

    pItem->setBodyCursor(pt.p8);
    pItem->updateSize();

    fact.putRingItem(pItem.get(), m_fd);
    
    // std::unique_ptr deallocates the tiem here.
}