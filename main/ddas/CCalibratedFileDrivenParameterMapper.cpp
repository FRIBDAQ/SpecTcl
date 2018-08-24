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

/** @file:  CCalibratedFileDrivenParameterMapper
 *  @brief: Implement the class.  See the header for more.
 */

#include "CCalibratedFileDrivenParameterMapper.h"

#include <Event.h>
#include <TreeParameter.h>      // includes CTreeParameter.h and CTreeVariable.h
#include <fstream>
#include <sstream>
#include <string>


/**
 * constructor
 *    -   Save the filename (for error messages).
 *    -   Process the configuration file to build up the decode data structs.
 *
 * @param filename - Path to the configuration file.
 */
CCalibratedFileDrivenParameterMapper::CCalibratedFileDrivenParameterMapper(
    const char* filename
) :
    m_configFile(filename)
{
    makeMap();                    // Can throw on errors.
}
/**
 * destructor
 *    Go through the terminal nodes of m_map deleting the
 *    tree parameters and tree variables.  Note that if
 *    a calibration value is constant the m_pVariable pointer will be null
 *    and delete on a null pointer is defined as a no-op according to C++
 */
CCalibratedFileDrivenParameterMapper::~CCalibratedFileDrivenParameterMapper()
{
    for (auto  c = 0; c < m_map.size(); c++) {          // Loop over crates.
        for (auto s = 0; s < m_map[c].size(); s++) {    // Over slots
            for(auto i = 0; i < m_map[c][s].size(); i++) { // over channels.
                if (m_map[c][s][i]) {
                    delete m_map[c][s][i]->s_raw;
                    delete m_map[c][s][i]->s_calibrated;
                    delete m_map[c][s][i]->s_calibration.s_const.s_pVariable;
                    delete m_map[c][s][i]->s_calibration.s_linear.s_pVariable;
                    delete m_map[c][s][i]->s_calibration.s_quadratic.s_pVariable;
                    delete m_map[c][s][i];
                }
            }
        }
    }
    // The vectors bloody well know how to delete themselves now.
}
/**
 * MapToParamters
 *    Given a set of hits, maps them to both raw and calibrated parameters.
 *    Note, in order to deal with future potentialities:
 *    - No raw parameter is set if it's tree parameter pointer is null.
 *    - No calibrated parameter is set if its tree parameter pointer is null.
 *    - Null tree variable pointers are considered to be an indication of a
 *      constant coefficient.
 *    - No mapping for a crate/slot/channel triplet is assumed to be an error.
 *
 *  @param hits - References a vector of hits to analyze.
 *  @param rEvent -References the event (not used) to filll in.
 *                 Not used because we reference it indirectly via treeparams.
 *  @throws std::string - on errors.
 */
void
CCalibratedFileDrivenParameterMapper::mapToParameters(
    const std::vector<DAQ::DDAS::DDASHit>& hits, CEvent& rEvent
)
{
    for (auto h = 0; h < hits.size(); h++) {
        const DAQ::DDAS::DDASHit& hit(hits[h]);
        unsigned c = hit.GetCrateID();
        unsigned s = hit.GetSlotID();
        unsigned ch= hit.GetChannelID();
        
        const ParameterInfo& info(getParameterInfo(c, s, ch));  // Can throw
        unpackRaw(hit, info);
        unpackCalibration(hit, info);
    }
}

/*-----------------------------------------------------------------------------
 *   Private methods for building the unpacking map.
 */

/**
 * makeMap
 *    Create the map that drives parameter unpacking hits into event elements.
 *    To make look -> parameter lookup O(1), the data structures we build are
 *    all made of vectors.  Vectors may be zero length to reflect missing
 *    items but vectors they are.
 *
 * Implicit inputs:
 *   -   m_configFile - Path to the configuration file.  Only needs to be valid
 *                      at construction time.
 */
void
CCalibratedFileDrivenParameterMapper::makeMap()
{
    std::ifstream cfg(m_configFile.c_str());
    if (cfg.fail()) {
        throw formatStandardMessage("Failed to open configuration file");
    }
    
    unsigned lno = 1;
    while (! cfg.eof()) {
        std::string line;
        getline(cfg, line);
        if (!line.empty() && (line[0] != '#')) {
            ParameterInfo pInfo = parseLine(line, lno);
            addParameter(pInfo);
        }
        lno++;
    }
    CTreeParameter::BindParameters();
}
/**
 * parseLine
 *    Turn a line into a parameter Info block.
 *    -  First three fields are numeric and identify the channel.
 *    -  The 4'th and fifth are numeric and string and respectively provide
 *       the channel's resolution and name (used to create a treeparameter).
 *    -  The next three fields (6-8) are either strings or integers.  If
 *       an integer the field represents a calibration coefficient.   If a
 *       string, the tree variable that contains the coefficient.  The
 *       coefficients are in order constant, linear and quadratic coefficients.
 *       Any of them may be constant or tree variables.
 *    -  The next five fields (9-13) provide the low/high/suggested binning
 *       units and name of the resulting calibrated tree parameter.
 *       The low, high binning fields are numeric.  The units and name
 *       are strings units can be specified by an empty string, designated by
 *       a pair of double quotes.
 *
 * @param line - the line to parse.
 * @param linenum - line number - used to format any error strings.
 * @return ParameterInfo - parameter description parsed from the line.
 * @throws std::string - if parse turns up errors.
 *
 */
CCalibratedFileDrivenParameterMapper::ParameterInfo
CCalibratedFileDrivenParameterMapper::parseLine(
    const std::string& line, unsigned linenum
)
{
    unsigned crate, slot, channel;    // Identification.
    
    unsigned resolution;              // Raw parameter
    std::string rawName;
    
    std::string constant, linear, quadratic;
    double low, high;                // Calibrated parameter.
    unsigned bins;
    std::string units, calibratedName;
    
    
    ParameterInfo result;
    
    // First try to do the overall line decode:
    
    std::stringstream strLine(line);
    strLine >> crate >> slot >> channel
        >> resolution >> rawName
        >> constant >> linear >> quadratic
        >> low >> high >>bins >> units >> calibratedName;
    
    if (strLine.fail()) {
        throw formatParseError(
            linenum, line, "Failed to decode the fields from the line"
        );
    }
    
    try {
        // Fill in what we know so far (including creating the tree params)
        
        result.s_crate   = crate;
        result.s_slot    = slot;
        result.s_channel = channel;
        result.s_raw     = new CTreeParameter(rawName, resolution);
        result.s_calibrated =
            new CTreeParameter(calibratedName, bins, low, high, units);
        
        // last we need to convert each of the coefficient strings;
        
        result.s_calibration.s_const     = parseCoefficient(constant);
        result.s_calibration.s_linear    = parseCoefficient(linear);
        result.s_calibration.s_quadratic = parseCoefficient(quadratic);
        
        
    } catch(std::string msg) {
        throw formatParseError(linenum, line, msg.c_str());
    }
    
    return result;
}
/**
 * parseCoefficent
 *    Parses a coefficient value into a Coefficient struct.  If the
 *    value parses as a double fine...it's a constant.  Otherwise
 *    we need to create a treevariable with that name and initial value 0.0
 *    with empty units string..and store that in the result.
 *
 * @param field - value of the coefficient field in the configuration line
 *                being parsed.
 * @return Coefficient - filled in coefficient descriptor.
 */
CCalibratedFileDrivenParameterMapper::Coefficient
CCalibratedFileDrivenParameterMapper::parseCoefficient(const std::string& field)
{
    Coefficient result;
    result.s_const     = 0.0;
    result.s_pVariable = nullptr;
    // Try treating this like a double;
    
    char* endptr(0);
    double c = strtod(field.c_str(), &endptr);
    if ((c == 0.0) && (endptr == field.c_str())) {
        result.s_pVariable = new CTreeVariable(field, 0.0, "");
    } else {
        result.s_const = c;
    }
    
    return result;
}

/**
 * addParameter
 *    Given a new ParameterInfo struct, adds it to the data structures.
 *    If necessary the crate vector is expanded as needed to accomodate.
 *    If necessary that crate's slot map is expanded.
 *    If there's already a parameter info for that crate/slot/channel triplet,
 *    that's considered an error and we throw.  Otherwise we allocate a new
 *    parameter info item, fill it in and shove it into the map.
 */
void
CCalibratedFileDrivenParameterMapper::addParameter(const ParameterInfo& info)
{
    unsigned crate = info.s_crate;
    unsigned slot  = info.s_slot;
    unsigned chan  = info.s_channel;
    
    addSlotMap(crate);
    addChannelMap(crate, slot);
    
    // Expand the channel map as needed as well.
    
    while (m_map[crate][slot].size() <= chan) {
        m_map[crate][slot].push_back(nullptr);
    }
    
    // If there's already a mapping that's an error.
    
    if (m_map[crate][slot][chan]) {
        throw std::string("Duplicate channel mapping attempted");
    }
    // Make the new item and add it to the map:
    
    pParameterInfo pNewInfo = new ParameterInfo;
    pNewInfo->s_crate  = crate;
    pNewInfo->s_slot   = slot;
    pNewInfo->s_channel= chan;
    pNewInfo->s_raw    = info.s_raw;
    pNewInfo->s_calibrated = info.s_calibrated;
    pNewInfo->s_calibration = info.s_calibration;
    
    
    
    m_map[crate][slot][chan] = pNewInfo;
}
/**
 * addSlotMap
 *    Ensures there are enough crate entries to hold the slot map
 *    for the specified crate;
 *
 * @param c - Create number we need a map for.
 */
void
CCalibratedFileDrivenParameterMapper::addSlotMap(unsigned crate)
{
    SlotMap newMap;
    while(m_map.size() <= crate) {
        m_map.push_back(newMap);
    }
}
/**
 * addChannelMap
 *    Ensures a crate has sufficient slots to create a new channel map
 *    for that slot.
 *
 * @param c - crate number
 * @param s - slot number.
 */
void
CCalibratedFileDrivenParameterMapper::addChannelMap(unsigned c, unsigned s)
{
    addSlotMap(c);
    ChannelMap newMap;
    while(m_map[c].size() <= s) {
        m_map[c].push_back(newMap);
    }
}
/*----------------------------------------------------------------------------
 *  Private methods connected with event processing.
 */

/**
 * getParameterInfo
 *    Given a crate, slot channel, return a reference to a parameter info
 *    object for that channel specification.  Throws if there's no match.
 *
 * @param crate - crate number of the hit.
 * @param slot  - Slot in the crate the hit occured in.
 * @param chan  - Channel of the slot that detected the hit.
 * @return const ParameterInfo& - references the parameter info record for that
 *               channel specification.
 */
const CCalibratedFileDrivenParameterMapper::ParameterInfo&
CCalibratedFileDrivenParameterMapper::getParameterInfo(
    unsigned crate, unsigned slot, unsigned channel
)
{
    if ((crate < m_map.size())      &&
        (slot < m_map[crate].size()) &&
        (channel < m_map[crate][slot].size()) ) {
        
        pParameterInfo pInfo = m_map[crate][slot][channel];
        if (pInfo) {
            return *pInfo;    
        } else {
            throw formatUnpackError(crate, slot, channel, "No parameter map defined");
        }
    } else {
        throw formatUnpackError(crate, slot, channel, "No parametr map defined.");
    }
}
/**
 * unpackRaw
 *    Unpacks the raw parameter from the hit given the mapping information.
 *
 * @param hit - DDAS hit to set a raw parameter for.
 * @param info - Const reference for the parameter mapping information.
 */
void
CCalibratedFileDrivenParameterMapper::unpackRaw(
    const DAQ::DDAS::DDASHit& hit, const ParameterInfo& info
)
{
    if (info.s_raw) {
        *(info.s_raw) = hit.GetEnergy();
    }
}
/**
 * unpackCalibration
 *    Sets the calibrated parameter associated with a hit.
 * @param hit - references the hit object.
 * @param info - references the parameter mapping information.
 */
void
CCalibratedFileDrivenParameterMapper::unpackCalibration(
    const DAQ::DDAS::DDASHit& hit, const ParameterInfo& info
)
{
    if (info.s_calibrated) {
        double c = getCoefficient(info.s_calibration.s_const);
        double l = getCoefficient(info.s_calibration.s_linear);
        double q = getCoefficient(info.s_calibration.s_quadratic);
        
        double e = hit.GetEnergy();
        double calib = c + e*(l + q * e);  // Avoids squaring etc.
        *(info.s_calibrated) = calib;
    }
}
/**
 * getCoefficcient
 *   Given a coefficient description, returns the current coefficient value.
 *
 * @param coef - coefficient description.
 * @return double
 */
double
CCalibratedFileDrivenParameterMapper::getCoefficient(
    const CCalibratedFileDrivenParameterMapper::Coefficient& coef
)
{
    double result = coef.s_const;
    if (coef.s_pVariable) {
        result = *(coef.s_pVariable);
    }
    return result;
}

/*------------------------------------------------------------------------------
 *  Error reporting private methods.
 */

/**
 * formatUnpackError
 *    Formats an error string for a problem noticed while unpacking data.
 *
 * @param crate       - Crate of the hit
 * @param slot        - Slot of the hit.
 * @param channel     - Channel of the hit.
 * @param msg         - Base error message we embellish on.
 * @return std::string
 */
std::string
CCalibratedFileDrivenParameterMapper::formatUnpackError(
    unsigned crate, unsigned slot, unsigned channel,
    const char* msg
)
{
    std::stringstream result;
    result << "Error decoding hit for config file hit in crate: " << crate
        << " slot " << slot << " channel " << channel << " : "
        << msg << std::endl;
        
    return formatStandardMessage(result.str().c_str());
}
/**
 * formatParseError
 *    Format a configuration file parse error string.
 *
 * @param linenum -  line in the configuration file where the error was detected.
 * @param line    -  Text of the line in which the error was detected.
 * @param msg     - Description of the message.
 * @return std::string
 */
std::string
CCalibratedFileDrivenParameterMapper::formatParseError(
    unsigned lineNum, const std::string& line, const char* msg
)
{
    std::stringstream result;
    
    result << "Error parsing configuration file: " << msg
        << " Found in line " << lineNum << std::endl
        << " Line: "  << line << std::endl;
    return formatStandardMessage(result.str().c_str());
}

/**
 * formatStandardError
 *    Formats a standard error message.
 *
 *  @param msg - base error message.
 *  @return std::string
 */
std::string
CCalibratedFileDrivenParameterMapper::formatStandardMessage(const char* msg)
{
    std::stringstream result;
    
    result << "Error in instance of DDAS Calibrated unpacker for config file: "
        << m_configFile << std::endl << msg;
    
    return result.str();
}