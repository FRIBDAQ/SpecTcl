/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  CFileDrivenParameterMapper.cpp
 *  @brief: Implementation of CFileDrivenParameterMapper class.
 *
 *  @note See CFileDrivenParamteerMapper.h for class overview.
 */
#include "CFileDrivenParameterMapper.h"
#include <CTreeParameter.h>
#include <map>
#include <fstream>
#include <sstream>

/**
 * constructor
 *    Save the filename for parsing, invoke makeMap to parse the map etc.
 *  @param configFile -name of the configuration file.
 */
CFileDrivenParameterMapper::CFileDrivenParameterMapper(const char* configFile) :
    m_configFile(configFile)
{
    makeMap();
}
/**
 * destructor
 *    Deletes the tree parameters that have been made so far.  The
 *    vectors are assumed to know how to kill themselves off:
 */
CFileDrivenParameterMapper::~CFileDrivenParameterMapper()
{
    for (int c = 0; c < m_map.size(); c++) {
        for (int s = 0; s < m_map[c].size(); s++) {
            for (int a = 0; a < m_map[c][s].size(); a++) {
                if (m_map[c][s][a])   {           // Null means a hole in the map.
                    delete m_map[c][s][a];        // non null means dynamic storage to kill off.
                }
            }
        }
    }
}

/**
 * mapToParameter
 *     -  Decodes the hit crate/slot/channels
 *     -  If the resulting channels does not correspond to a mapped item,
 *        report an error for this event.
 *     -  If the resulting channel does correspond to  a mapped item,
 *        unpack the energy into that parameter.
 *
 *  @param hits - reference to a vector of DDAS hits that make up the event
 *               or hits in the crate if the non built version is used.
 *  @param rEvent - reference to the event - ignored for this.
 *  @throw std::string - if the mapping of a hit fails for any reason.
 */

void
CFileDrivenParameterMapper::mapToParameter(
    const std::vector<DDASHit>& hits, CEvent& rEvent
)
{
    for (int h = 0; h < hits.size(); h++) {
        DDASHit& hit(hits[h]);
        
        // Fetch out the stuff we need:
        
        unsigned crate = hit.GetCrateID();
        unsigned slot  = hit.GetSlotID();
        unsigned chan  = hit.GetChannelID();
        unsigned energy= hit.GetEnergy();
        
        // Now probe our way down the vectors to see if there's a mapping to
        // a parameter:
        
        if (
            m_map.size() < crate      &&
            m_map[crate].size < slot  &&
            m_map[crate][slot] < chan &&
            m_map[crate][slot][chan]
        ) {
            *(m[crate][slot][chan]) = energy;
        } else {
            // Mapping failed:
            
            throw formatUnpackError(crate, slot, channel);
        }
        
    }
}

/*------------------------------------------------------------------------------
 *   Private methods for map generation.
 */


/**
 * makeMap
 *    - Open the configuration file.
 *    - Parse lines into the five fields we expect.
 *    - add entries to the maps as requested.
 *
 * @throw std::string - several error conditions are reported includeing:
 *      - File not found
 *      - invalid format of a line in he file.
 *      - Duplicate mapping entry attempted
 *      - Duplicate parameter creation within this method.  No check is done
 *        for duplicates between us and the outside world because that could be
 *        deliberate to allow data to be shared between us and the outside world.
 */
void
CFileDrivenParameterMapper::makeMap()
{
    std::map<std::string> existingParams;
    
    // Attempt to open the file
    
    ifstream cfg(m_configFile);
    if (cfg.rdstate() & std::ifstream::failbit) {
        formatStandardError("Open failed on configuration file");
    }
    
    // In order to report the file line for errors,we first read entire lines
    // and then decode from an strstream
    
    unsigned lineNum = 1;
    while (!cfg.eof()) {
        std::string line;
        unsigned crate(0);
        unsigned slot(0);
        unsigned channel(0);
        unsigned bits(0);
        std::string name;
        
        
        getline(cfg, line);
        
        std::stringstream strLine(line);
        strLine >> crate >> slot >> channel >> bits >> name;
        if (strLine.fail()) {
            throw formatParseError(lineNum, line, "Failed to parse this line");
        }
        if (existingParams.count(name) > 0) {
            throw formatParseError(lineNum, line, "This parameter was already defined by us");
        }
        
        // Next see if the parameter mapping entry already exists.. with a non-null
        // pointer.  To save duplication, we're going to add the Subaddress map which
        // expands the mapping as needed
        
        addChannelMap(crate, slot);
        if ((m_map[crate][slot].size >= channel) && (m_map[crate][slot][channel]) {
            throw formatParseError(lineNum, line, "Duplicate crate/slot/channel");
        }
        
        // Now we can add the parameter to the slot map:
        
        existingParams[name] = 1;               // remember we've defined this.
        
        addParameter(crate, slot, channel, bits, name);
        
        lineNum++;
    }
}
/**
 * addParameter
 *    Given that a slot map already exists and that we're not replacing
 *    a duplicate parameter map:
 *     - Create the tree parameter.
 *     - If needed extend the slot map.
 *     - Add the parameter to the slot map.
 */
void
CFileDrivenParameterMapper::addParameter(
    unsigned crate, unsigned slot, unsiged channel, unsigned bits,
    std::string name
)
{
    CTreeParameter* param = new CTreeParameter(name, resolution);
    while (m_map[crate][slot].size() < channel) {
        m_map[crate][slot].push_back(nullptr);
    }
    m_map[crate][slot][channel] = param;
}
/**
 * addSlotMap
 *    Extends the crate map as needed to cover the proposed crate number.
 *
 * @param crate - crate number we need to accommodate
 */
void
CFileDrivenParameterMapper::addSlotMap(unsigned crate)
{
    while (m_map.size() < crate) {
        SlotMap smap;
        m_map.push_back(smap);
    }
}
/**
 * addChannelMap
 *    Add channel maps to crates until there is one for the specified slot.
 *
 *  @param crate - Crate number (note addSlotMap is used to ensure there's a map for the crate)
 *  @param slot  - Slot for which we add a channel map.
 */
void
CFileDrivenParameterMapper::addChannelMap(unsigned crate, unsigned slot)
{
    addSlotMap(crate);
    ChannelMap chmap;                       // Empty channel map.
    while (m_map[crate].size() < slot) {
        m_map[crate].push_back(chmap);
    }
}

/*-----------------------------------------------------------------------------
 *  Private methods to create error strings that can be thrown as exceptions.
 */

/**
 * formatUnpackError
 *     Formats a string that represents an unpack failure.  Typically this
 *     is due to a crate,slot,channel triplet that has no parameter
 *
 * @param crate             - Crate 
 * @param slot              - Slot for which the unpack was attempted.
 * @param channel           - Channel for which the unpack was attempted.
 * @return std::string - an error message.
 */
std::string
CFileDrivenParameterMapper::formatUnpackError(unsigned crate, unsigned slot, unsigned channel)
{
    std::stringstream strMessage;
    
    strMessage << "Unable to find a parameter that maps to crate=" << crate
        << " slot=" << slot << " channel=" << channel;
    return formatStandardError(strMessage.str().c_str());
}
/**
 * formatParseError
 *    Formats an error string that was caught parsing data from the
 *    configuration file.  These error messages have a line number
 *    (in the file) as well as the line of text itself along with
 *    a descriptive error message,.
 *
 *  @param lineNum  - configuration file line number.
 *  @param line     - The line of text that failed.
 *  @param msg      - Descriptive error message.
 *  @return std::string formatted error message.
 */
std::string
CFileDrivenParameterMapper::formatParseError(
    unsigned lineNum, std::string line, const char* msg
)
{
    std::stringstream strMessage;
    strMessage << "Error decoding line: " << linNum << " :  " << msg << std::endl
        << "Line in question: " << line;
    return formatStandardMessage(strMessage.str().c_str());l
}
/**
 * formatStandardMessage
 *    Formats a standard error message.  Standard messages include a header that
 *    has the name of the configuration file followed by the message provided.
 *
 *  @param msg - Error messagse to format:
 *  @return std::string - Formatted error message.
 */
std::string
CFileDrivenParameterMapper::formatStandardMessage(const char* msg)
{
    std::stringstream strMessage;
    strMessage << "Error in file driven unpacker with configuration file "
        << m_configFile << " : " << std::endl << msg;
    return strMessage.str();    
    
}